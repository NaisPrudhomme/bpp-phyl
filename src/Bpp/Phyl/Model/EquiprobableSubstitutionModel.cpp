//
// File: EquiprobableSubstitutionModel.cpp
// Authors:
//   Laurent GuÃÂ©guen
// Created: mercredi 13 juillet 2016, ÃÂ  08h 55
//

/*
  Copyright or ÃÂ© or Copr. Bio++ Development Team, (November 16, 2004)
  
  This software is a computer program whose purpose is to provide classes
  for phylogenetic data analysis.
  
  This software is governed by the CeCILL license under French law and
  abiding by the rules of distribution of free software. You can use,
  modify and/ or redistribute the software under the terms of the CeCILL
  license as circulated by CEA, CNRS and INRIA at the following URL
  "http://www.cecill.info".
  
  As a counterpart to the access to the source code and rights to copy,
  modify and redistribute granted by the license, users are provided only
  with a limited warranty and the software's author, the holder of the
  economic rights, and the successive licensors have only limited
  liability.
  
  In this respect, the user's attention is drawn to the risks associated
  with loading, using, modifying and/or developing or reproducing the
  software by the user in light of its specific status of free software,
  that may mean that it is complicated to manipulate, and that also
  therefore means that it is reserved for developers and experienced
  professionals having in-depth computer knowledge. Users are therefore
  encouraged to load and test the software's suitability as regards their
  requirements in conditions enabling the security of their systems and/or
  data to be ensured and, more generally, to use and operate it in the
  same conditions as regards security.
  
  The fact that you are presently reading this means that you have had
  knowledge of the CeCILL license and that you accept its terms.
*/


#include "EquiprobableSubstitutionModel.h"

// From bpp-seq:
#include <Bpp/Seq/Container/SequenceContainerTools.h>

using namespace bpp;

#include <cmath>
#include <map>

using namespace std;

/******************************************************************************/

EquiprobableSubstitutionModel::EquiprobableSubstitutionModel(const Alphabet* alpha) :
  AbstractParameterAliasable("Equi."),
  AbstractReversibleSubstitutionModel(alpha, std::shared_ptr<StateMap>(new CanonicalStateMap(alpha, false)), "Equi."),
  exp_(), p_(size_, size_), freqSet_(0)
{
  freqSet_ = std::make_shared<FixedFrequencySet>(shareStateMap(), freq_);
  updateMatrices();
}

EquiprobableSubstitutionModel::EquiprobableSubstitutionModel(const Alphabet* alpha, FrequencySet* freqSet, bool initFreqs) :
  AbstractParameterAliasable("Equi+F."),
  AbstractReversibleSubstitutionModel(alpha, std::shared_ptr<StateMap>(new CanonicalStateMap(alpha, false)), "Equi+F."),
  exp_(), p_(size_, size_), freqSet_(freqSet)
{
  freqSet_->setNamespace("Equi+F." + freqSet_->getNamespace());
  if (initFreqs)
    freqSet_->setFrequencies(freq_);
  else
    freq_ = freqSet_->getFrequencies();
  addParameters_(freqSet_->getParameters());
  updateMatrices();
}


/******************************************************************************/

void EquiprobableSubstitutionModel::updateMatrices()
{
  // Frequencies:
  for (unsigned int i = 0; i < size_; i++)
  {
    freq_[i] = 1. / (float)size_;
  }

  // Generator:
  for (unsigned int i = 0; i < size_; i++)
  {
    for (unsigned int j = 0; j < size_; j++)
    {
      generator_(i, j) = (i == j) ? -1. : 1. / (float)(size_ - 1);
      exchangeability_(i, j) = generator_(i, j) * (float)size_;
    }
  }

  // Eigen values:
  eigenValues_[0] = 0;
  for (unsigned int i = 1; i < size_; i++)
  {
    eigenValues_[i] = -(float)size_ / (float)(size_ - 1);
  }

  // Eigen vectors:
  for (unsigned int i = 0; i < size_; i++)
  {
    leftEigenVectors_(0, i) = 1. / (float)size_;
  }
  for (unsigned int i = 1; i < size_; i++)
  {
    for (unsigned int j = 0; j < size_; j++)
    {
      leftEigenVectors_(i, j) = -1. / (float)size_;
    }
  }
  for (unsigned int i = 0; i < (size_ - 1); i++)
  {
    leftEigenVectors_((size_ - 1) - i, i) = (float)(size_ - 1) / (float)size_;
  }

  for (unsigned int i = 0; i < size_; i++)
  {
    rightEigenVectors_(i, 0) = 1.;
  }
  for (unsigned int i = 1; i < size_; i++)
  {
    rightEigenVectors_((size_ - 1), i) = -1.;
  }
  for (unsigned int i = 0; i < (size_ - 1); i++)
  {
    for (unsigned int j = 1; j < size_; j++)
    {
      rightEigenVectors_(i, j) = 0.;
    }
  }
  for (unsigned int i = 1; i < size_; i++)
  {
    rightEigenVectors_((size_ - 1) - i, i) = 1.;
  }
}

/******************************************************************************/

double EquiprobableSubstitutionModel::Pij_t(size_t i, size_t j, double d) const
{
  if (i == j)
    return 1. / (float)size_ + (float)(size_ - 1) / (float)size_ * exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
  else
    return 1. / (float)size_ -  1. / (float)size_ * exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
}

/******************************************************************************/

double EquiprobableSubstitutionModel::dPij_dt(size_t i, size_t j, double d) const
{
  if (i == j)
    return -rate_*        exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
  else
    return rate_ * 1. / (float)(size_ - 1) * exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
}

/******************************************************************************/

double EquiprobableSubstitutionModel::d2Pij_dt2(size_t i, size_t j, double d) const
{
  if (i == j)
    return rate_ *  rate_ * (float)size_ / (float)(size_ - 1)  * exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
  else
    return -rate_ *  rate_ * (float)size_ / pow((float)(size_ - 1), 2) * exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
}

/******************************************************************************/

const Matrix<double>& EquiprobableSubstitutionModel::getPij_t(double d) const
{
  exp_ = exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
  for (unsigned int i = 0; i < size_; i++)
  {
    for (unsigned int j = 0; j < size_; j++)
    {
      p_(i, j) = (i == j) ? 1. / (float)size_ + (float)(size_ - 1) / (float)size_ * exp_ : 1. / (float)size_ - 1. / (float)size_ * exp_;
    }
  }
  return p_;
}

const Matrix<double>& EquiprobableSubstitutionModel::getdPij_dt(double d) const
{
  exp_ = exp(-rate_ * (float)size_ / (float)(size_ - 1) * d);
  for (unsigned int i = 0; i < size_; i++)
  {
    for (unsigned int j = 0; j < size_; j++)
    {
      p_(i, j) =  rate_ * ((i == j) ? -exp_ : 1. / (float)(size_ - 1) * exp_);
    }
  }
  return p_;
}

const Matrix<double>& EquiprobableSubstitutionModel::getd2Pij_dt2(double d) const
{
  exp_ = exp( rate_ * -(float)size_ / (float)(size_ - 1) * d);
  for (unsigned int i = 0; i < size_; i++)
  {
    for (unsigned int j = 0; j < size_; j++)
    {
      p_(i, j) =  rate_ *  rate_ * ((i == j) ? (float)size_ / (float)(size_ - 1) * exp_ : -(float)size_ / pow((float)(size_ - 1), 2) * exp_);
    }
  }
  return p_;
}

/******************************************************************************/

void EquiprobableSubstitutionModel::setFreq(std::map<int, double>& freqs)
{
  for (auto i : freqs)
  {
    freq_[(size_t)i.first] = i.second;
  }

  freqSet_->setFrequencies(freq_);
  // Update parameters and re-compute generator and eigen values:
  matchParametersValues(freqSet_->getParameters());
}

/******************************************************************************/
