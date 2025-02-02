//
// File: CompoundTransitionModel.cpp
// Authors:
//   Anaïs Prud'homme
// Date :
//   Vendredi 3 décembre 2021 à 11h30
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

#include <Bpp/Exceptions.h>
#include <Bpp/Numeric/NumConstants.h>
#include <Bpp/Numeric/VectorTools.h>
#include <string>

#include "CompoundTransitionModel.h"

using namespace bpp;
using namespace std;


CompoundTransitionModel::CompoundTransitionModel(
  const Alphabet* alpha,
  vector<std::shared_ptr<TransitionModel> > vpModel) :
  AbstractParameterAliasable("Compound."),
  AbstractTransitionModel(alpha, vpModel.size() ? vpModel[0]->shareStateMap() : 0, "Compound."),
  modelsContainer_(),
  vProbas_()
{
  size_t i, nbmod = vpModel.size();

  for (i = 0; i < nbmod; i++)
  {
    if (!vpModel[i])
      throw Exception("Empty model number " + TextTools::toString(i) + " in CompoundTransitionModel constructor");
    for (size_t j = i + 1; j < nbmod; j++)
    {
      if (vpModel[i] == vpModel[j])
        throw Exception("Same model at positions " + TextTools::toString(i) + " and " +
                        TextTools::toString(j) + " in CompoundTransitionModel constructor");
    }
  }
  // Initialization of modelsContainer_.

  for (i = 0; i < nbmod; i++)
  {
    modelsContainer_.push_back(vpModel[i]);
    vProbas_.push_back(1.0 / static_cast<double>(nbmod));
  }

  // Initialization of parameters_.

  // relative rates and probas
  for (i = 0; i < nbmod - 1; i++)
  {
    addParameter_(new Parameter("Compound.relproba" + TextTools::toString(i + 1), 1.0 / static_cast<double>(nbmod - i), Parameter::PROP_CONSTRAINT_EX));
  }

  // models parameters

  for (i = 0; i < nbmod; i++)
  {
    modelsContainer_[i]->setNamespace("Compound." + TextTools::toString(i + 1) + "_" + vpModel[i]->getNamespace());
    addParameters_(vpModel[i]->getParameters());
  }

  updateMatrices();
}


CompoundTransitionModel::CompoundTransitionModel(
  const Alphabet* alpha,
  vector<std::shared_ptr<TransitionModel> > vpModel,
  Vdouble& vproba) :
  AbstractParameterAliasable("Compound."),
  AbstractTransitionModel(alpha, vpModel.size() ? vpModel[0]->shareStateMap() : 0, "Compound.")
{
  size_t i, nbmod = vpModel.size();

  for (i = 0; i < nbmod; i++)
  {
    if (!vpModel[i])
      throw Exception("Empty model number " + TextTools::toString(i) + " in CompoundTransitionModel constructor");
    for (size_t j = i + 1; j < nbmod; j++)
    {
      if (vpModel[i] == vpModel[j])
        throw Exception("Same model at positions " + TextTools::toString(i) + " and " +
                        TextTools::toString(j) + " in CompoundTransitionModel constructor");
    }
  }

  double x = 0;

  for (i = 0; i < nbmod; i++)
  {
    if (vproba[i] <= 0)
      throw Exception("Non positive probability: " + TextTools::toString(vproba[i]) + " in CompoundTransitionModel constructor.");
    x += vproba[i];
  }

  if (fabs(1. - x) > NumConstants::SMALL())
    throw Exception("Probabilities must equal 1 (sum = " + TextTools::toString(x) + ").");


  // Initialization of modelsContainer_.

  for (i = 0; i < nbmod; i++)
  {
    modelsContainer_.push_back(vpModel[i]);
  }

  // rates & probas

  vProbas_.resize(nbmod);

  // Initialization of parameters_.

  // relative rates and probas
  x = 0;

  for (i = 0; i < nbmod - 1; i++)
  {
    addParameter_(new Parameter("Compound.relproba" + TextTools::toString(i + 1), vproba[i] / (1 - x), Parameter::PROP_CONSTRAINT_EX));
    x += vproba[i];
  }

  // models parameters

  for (i = 0; i < nbmod; i++)
  {
    modelsContainer_[i]->setNamespace("Compound." + TextTools::toString(i + 1) + "_" + vpModel[i]->getNamespace());
    addParameters_(vpModel[i]->getParameters());
  }

  updateMatrices();
}

CompoundTransitionModel::CompoundTransitionModel(const CompoundTransitionModel& msm) :
  AbstractParameterAliasable(msm),
  AbstractTransitionModel(msm),
  from_(msm.from_),
  to_(msm.to_),
  modelsContainer_(),
  vProbas_(msm.vProbas_)
{
  for (unsigned int i = 0; i < msm.modelsContainer_.size(); i++)
  {
    modelsContainer_.push_back(std::shared_ptr<TransitionModel>(msm.modelsContainer_[i]->clone()));
  }

}

CompoundTransitionModel& CompoundTransitionModel::operator=(const CompoundTransitionModel& msm)
{
  AbstractParameterAliasable::operator=(msm);
  from_ = msm.from_;
  to_ = msm.to_;

  // Clear existing containers:
  modelsContainer_.clear();
  vProbas_.clear();

  for (unsigned int i = 0; i < msm.modelsContainer_.size(); i++)
  {
    modelsContainer_.push_back(std::shared_ptr<TransitionModel>(msm.modelsContainer_[i]->clone()));
    vProbas_.push_back(msm.vProbas_[i]);
  }
  return *this;
}


void CompoundTransitionModel::updateMatrices()
{
  size_t i, j, nbmod = modelsContainer_.size();

  double x, y;
  x = 1.0;

  for (i = 0; i < nbmod - 1; i++)
  {
    y = getParameterValue("relproba" + TextTools::toString(i + 1));
    vProbas_[i] = x * y;
    x *= 1 - y;
  }
  vProbas_[nbmod - 1] = x;

  // models

  for (i = 0; i < nbmod; i++)
  {
    modelsContainer_[i]->matchParametersValues(getParameters());
  }

  // / freq_

  for (i = 0; i < getNumberOfStates(); i++)
  {
    freq_[i] = 0;
    for (j = 0; j < modelsContainer_.size(); j++)
    {
      freq_[i] += vProbas_[j] * modelsContainer_[j]->freq(i);
    }
  }
}


const TransitionModel* CompoundTransitionModel::getModel(const std::string& name) const
{
  size_t nbmod = getNumberOfModels();

  for (size_t i = 0; i < nbmod; i++)
  {
    if (getModel(i)->getName() == name)
      return getModel(i);
  }

  return NULL;
}

size_t CompoundTransitionModel::getNumberOfStates() const
{
  return modelsContainer_[0]->getNumberOfStates();
}

const Matrix<double>& CompoundTransitionModel::getPij_t(double t) const
{
  vector<const Matrix<double>* > vM;
  double sP = 0;
  for (unsigned int n = 0; n < modelsContainer_.size(); n++)
  {
    vM.push_back(&modelsContainer_[n]->getPij_t(t));
    sP += vProbas_[n];
  }

  for (unsigned int i = 0; i < getNumberOfStates(); i++)
  {
    for (unsigned int j = 0; j < getNumberOfStates(); j++)
    {
      double x = 0;
      for (unsigned int n = 0; n < modelsContainer_.size(); n++)
      {
        x += (*vM[n])(i, j) * vProbas_[n];
      }
      pijt_(i, j) = x / sP;
    }
  }
  return pijt_;
}


const Matrix<double>& CompoundTransitionModel::getdPij_dt(double t) const
{
  vector<const Matrix<double>* > vM;
  double sP = 0;
  for (unsigned int n = 0; n < modelsContainer_.size(); n++)
  {
    vM.push_back(&modelsContainer_[n]->getdPij_dt(t));
    sP += vProbas_[n];
  }

  for (unsigned int i = 0; i < getNumberOfStates(); i++)
  {
    for (unsigned int j = 0; j < getNumberOfStates(); j++)
    {
      double x = 0;
      for (unsigned int n = 0; n < modelsContainer_.size(); n++)
      {
        x += (*vM[n])(i, j) * vProbas_[n];
      }
      dpijt_(i, j) = x / sP;
    }
  }
  return dpijt_;
}


const Matrix<double>& CompoundTransitionModel::getd2Pij_dt2(double t) const
{
  vector<const Matrix<double>* > vM;
  double sP = 0;
  for (unsigned int n = 0; n < modelsContainer_.size(); n++)
  {
    vM.push_back(&modelsContainer_[n]->getd2Pij_dt2(t));
    sP += vProbas_[n];
  }

  for (unsigned int i = 0; i < getNumberOfStates(); i++)
  {
    for (unsigned int j = 0; j < getNumberOfStates(); j++)
    {
      double x = 0;
      for (unsigned int n = 0; n < modelsContainer_.size(); n++)
      {
        x += (*vM[n])(i, j) * vProbas_[n];
      }
      d2pijt_(i, j) = x / sP;
    }
  }
  return d2pijt_;
}
