//
// File: UserProteinSubstitutionModel.h
// Authors:
//   Julien Dutheil
// Created: 2005-08-26 16:27:00
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

#ifndef BPP_PHYL_MODEL_PROTEIN_USERPROTEINSUBSTITUTIONMODEL_H
#define BPP_PHYL_MODEL_PROTEIN_USERPROTEINSUBSTITUTIONMODEL_H


#include "../AbstractSubstitutionModel.h"
#include "../FrequencySet/ProteinFrequencySet.h"
#include "ProteinSubstitutionModel.h"

// From SeqLib:
#include <Bpp/Seq/Alphabet/ProteicAlphabet.h>

// From the STL:
#include <string>

namespace bpp
{
/**
 * @brief Build an empirical protein substitution model from a file.
 *
 * The file must follow PAML's format, and contain the exchangeabilities components (\f$S_{i,j}\f$)
 * and all equilibrium frequencies (\f$\pi_{i}\f$).
 * The generator is build so that \f$Q_{i,j} = \pi_i . S_{i,j}\f$, and is normalized
 * so that \f$\sum_i Q_{i,i} \times \pi_i = -1\f$.
 */
class UserProteinSubstitutionModel :
  public AbstractReversibleProteinSubstitutionModel
{
private:
  std::string path_;
  std::shared_ptr<ProteinFrequencySet> freqSet_;

public:
  /**
   * @brief Build a protein model from a PAML file, with original equilibrium frequencies.
   *
   * @param alpha A proteic alphabet.
   * @param path The path toward the file to parse.
   * @param prefix The parameter namespace to use.
   */
  UserProteinSubstitutionModel(
    const ProteicAlphabet* alpha,
    const std::string& path,
    const std::string& prefix);

  /**
   * @brief Build a protein model from a PAML file, with special equilibrium frequencies.
   *
   * @param alpha A proteic alphabet.
   * @param freqSet A pointer toward a protein frequencies set, which will be owned by this instance.
   * @param path The path toward the file to parse.
   * @param prefix The parameter namespace to use.
   * @param initFreqs Tell if the frequency set should be initialized with the original JTT92 values.
   * Otherwise, the values of the set will be used.
   */
  UserProteinSubstitutionModel(
    const ProteicAlphabet* alpha,
    const std::string& path,
    std::shared_ptr<ProteinFrequencySet> freqSet,
    const std::string& prefix,
    bool initFreqs = false
    );

  UserProteinSubstitutionModel(const UserProteinSubstitutionModel& model) :
    AbstractParameterAliasable(model),
    AbstractReversibleProteinSubstitutionModel(model),
    path_(model.path_),
    freqSet_(dynamic_cast<ProteinFrequencySet*>(model.freqSet_->clone()))
  {}

  UserProteinSubstitutionModel& operator=(const UserProteinSubstitutionModel& model)
  {
    AbstractParameterAliasable::operator=(model);
    AbstractReversibleProteinSubstitutionModel::operator=(model);
    path_ = model.path_;
    freqSet_ = std::shared_ptr<ProteinFrequencySet>(dynamic_cast<ProteinFrequencySet*>(model.freqSet_->clone()));
    return *this;
  }

  virtual ~UserProteinSubstitutionModel() {}

  UserProteinSubstitutionModel* clone() const { return new UserProteinSubstitutionModel(*this); }

public:
  std::string getName() const;
  const std::string& getPath() const { return path_; }

  void fireParameterChanged(const ParameterList& parameters)
  {
    freqSet_->matchParametersValues(parameters);
    freq_ = freqSet_->getFrequencies();
    AbstractReversibleSubstitutionModel::fireParameterChanged(parameters);
  }

  void setNamespace(const std::string& prefix)
  {
    AbstractParameterAliasable::setNamespace(prefix);
    freqSet_->setNamespace(prefix + freqSet_->getName() + ".");
  }


  void setFrequencySet(const ProteinFrequencySet& freqSet)
  {
    freqSet_.reset(dynamic_cast<ProteinFrequencySet*>(freqSet.clone()));
    resetParameters_();
    addParameters_(freqSet_->getParameters());
  }

  const std::shared_ptr<FrequencySet> getFrequencySet() const { return freqSet_; }

  void setFreqFromData(const SequencedValuesContainer& data, double pseudoCount = 0);

protected:
  void readFromFile();
};
} // end of namespace bpp.
#endif // BPP_PHYL_MODEL_PROTEIN_USERPROTEINSUBSTITUTIONMODEL_H
