//
// File: DSO78.cpp
// Authors:
//   Julien Dutheil
// Created: 2004-10-05 18:48:19
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


#include "DSO78.h"

// From bpp-seq:
#include <Bpp/Seq/Container/SequenceContainerTools.h>

using namespace bpp;

#include <map>

using namespace std;

/******************************************************************************/

DSO78::DSO78(const ProteicAlphabet* alpha) :
  AbstractParameterAliasable("DSO78."),
  AbstractReversibleProteinSubstitutionModel(alpha, std::shared_ptr<const StateMap>(new CanonicalStateMap(alpha, false)), "DSO78."),
  freqSet_(0)
{
  #include "__DSO78ExchangeabilityCode"
  #include "__DSO78FrequenciesCode"
  freqSet_.reset(new FixedProteinFrequencySet(alpha, freq_));
  updateMatrices();
}

DSO78::DSO78(const ProteicAlphabet* alpha, std::shared_ptr<ProteinFrequencySet> freqSet, bool initFreqs) :
  AbstractParameterAliasable("DSO78+F."),
  AbstractReversibleProteinSubstitutionModel(alpha, std::shared_ptr<const StateMap>(new CanonicalStateMap(alpha, false)), "DSO78+F."),
  freqSet_(freqSet)
{
  #include "__DSO78ExchangeabilityCode"
  #include "__DSO78FrequenciesCode"
  freqSet_->setNamespace("DSO78+F." + freqSet_->getNamespace());
  if (initFreqs)
    freqSet_->setFrequencies(freq_);
  else
    freq_ = freqSet_->getFrequencies();
  addParameters_(freqSet_->getParameters());
  updateMatrices();
}

/******************************************************************************/

void DSO78::setFreqFromData(const SequencedValuesContainer& data, double pseudoCount)
{
  map<int, double> counts;
  SequenceContainerTools::getFrequencies(data, counts, pseudoCount);
  for (auto i : counts)
  {
    freq_[(size_t)i.first] = i.second;
  }

  freqSet_->setFrequencies(freq_);
  // Update parameters and re-compute generator and eigen values:
  matchParametersValues(freqSet_->getParameters());
}

/******************************************************************************/
