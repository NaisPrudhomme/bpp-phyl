//
// File: JTT92.h
// Authors:
//   Julien Dutheil
// Created: 2004-01-21 14:09:43
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

#ifndef BPP_PHYL_MODEL_PROTEIN_JTT92_H
#define BPP_PHYL_MODEL_PROTEIN_JTT92_H


#include "../AbstractSubstitutionModel.h"
#include "../FrequencySet/ProteinFrequencySet.h"
#include "ProteinSubstitutionModel.h"

// From bpp-seq:
#include <Bpp/Seq/Alphabet/ProteicAlphabet.h>

namespace bpp
{
/**
 * @brief The Jones, Taylor and Thornton substitution model for proteins.
 *
 * Exchangeabilities have been computed using the DCMut method of Kosiol and Goldman.
 * The exchangability matrix is normalized so that \f$Q = S . \pi\f$ and \f$\sum_i Q_{i,i}\pi_i = -1\f$.
 * The original frequencies can be used, or alternatively a parametrized version, corresponding to the
 * so-called JTT92+F model.
 * Eigen values and vectors are obtained numerically.
 *
 * References:
 * - Jones DT, Taylor WR and Thornton JM (1992), _Computer Applications In The Biosciences_, 8(3) 275-82.
 * - Kosiol C and Goldman N (2005), _Molecular Biology And Evolution_ 22(2) 193-9.
 */
class JTT92 :
  public AbstractReversibleProteinSubstitutionModel
{
private:
  std::shared_ptr<ProteinFrequencySet> freqSet_;

public:
  /**
   * @brief Build a simple JTT92 model, with original equilibrium frequencies.
   *
   * @param alpha A proteic alphabet.
   */
  JTT92(const ProteicAlphabet* alpha);

  /**
   * @brief Build a JTT92 model with special equilibrium frequencies.
   *
   * @param alpha A proteic alphabet.
   * @param freqSet A pointer toward a protein frequencies set, which will be owned by this instance.
   * @param initFreqs Tell if the frequency set should be initialized with the original JTT92 values.
   * Otherwise, the values of the set will be used.
   */
  JTT92(const ProteicAlphabet* alpha, std::shared_ptr<ProteinFrequencySet> freqSet, bool initFreqs = false);

  JTT92(const JTT92& model) :
    AbstractParameterAliasable(model),
    AbstractReversibleProteinSubstitutionModel(model),
    freqSet_(dynamic_cast<ProteinFrequencySet*>(model.freqSet_->clone()))
  {}

  JTT92& operator=(const JTT92& model)
  {
    AbstractParameterAliasable::operator=(model);
    AbstractReversibleProteinSubstitutionModel::operator=(model);
    freqSet_.reset(dynamic_cast<ProteinFrequencySet*>(model.freqSet_->clone()));
    return *this;
  }

  virtual ~JTT92() {}

  JTT92* clone() const { return new JTT92(*this); }

public:
  std::string getName() const
  {
    if (freqSet_->getNamespace().find("JTT92+F.") != std::string::npos)
      return "JTT92+F";
    else
      return "JTT92";
  }

  void setNamespace(const std::string& prefix)
  {
    AbstractParameterAliasable::setNamespace(prefix);
    freqSet_->setNamespace(prefix + freqSet_->getName() + ".");
  }


  void fireParameterChanged(const ParameterList& parameters)
  {
    freqSet_->matchParametersValues(parameters);
    freq_ = freqSet_->getFrequencies();
    AbstractReversibleSubstitutionModel::fireParameterChanged(parameters);
  }

  void setFrequencySet(const ProteinFrequencySet& freqSet)
  {
    freqSet_.reset(dynamic_cast<ProteinFrequencySet*>(freqSet.clone()));
    resetParameters_();
    addParameters_(freqSet_->getParameters());
  }

  const std::shared_ptr<FrequencySet> getFrequencySet() const { return freqSet_; }

  void setFreqFromData(const SequencedValuesContainer& data, double pseudoCount = 0);
};
} // end of namespace bpp.
#endif // BPP_PHYL_MODEL_PROTEIN_JTT92_H
