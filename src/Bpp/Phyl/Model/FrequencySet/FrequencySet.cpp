//
// File: FrequencySet.cpp
// Authors:
//   Bastien Boussau
//   Julien Dutheil
// Created: 2007-08-21 00:00:00
//

/*
  Copyright or (c) or Copr. Bio++ Development Team, (November 16, 2004)
  
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

#include <Bpp/Numeric/NumConstants.h>
#include <Bpp/Numeric/Prob/Simplex.h>

#include "FrequencySet.h"

// From bpp-phyl
#include "../SubstitutionModel.h"
#include "../AbstractBiblioSubstitutionModel.h"


using namespace bpp;

#include <cmath>
using namespace std;

std::shared_ptr<IntervalConstraint> FrequencySet::FREQUENCE_CONSTRAINT_MILLI(new IntervalConstraint(NumConstants::MILLI(), 1 - NumConstants::MILLI(), false, false));
std::shared_ptr<IntervalConstraint> FrequencySet::FREQUENCE_CONSTRAINT_SMALL(new IntervalConstraint(NumConstants::SMALL(), 1 - NumConstants::SMALL(), false, false));

// ///////////////////////////////////////
// AbstractFrequencySet


void AbstractFrequencySet::setFrequenciesFromAlphabetStatesFrequencies(const map<int, double>& frequencies)
{
  size_t s = stateMap_->getNumberOfModelStates();
  vector<double> freq(s);
  double x = 0;
  for (size_t i = 0; i < s; ++i)
  {
    map<int, double>::const_iterator it = frequencies.find(stateMap_->getAlphabetStateAsInt(i));
    if (it != frequencies.end())
      freq[i] = it->second;
    else
      freq[i] = 0;
    x += freq[i];
  }

  if (x != 0)
    for (size_t i = 0; i < s; ++i)
    {
      freq[i] /= x;
    }
  else
    for (size_t i = 0; i < s; ++i)
    {
      freq[i] = 1.0 / (double)s;
    }

  setFrequencies(freq);
}

const std::map<int, double> AbstractFrequencySet::getAlphabetStatesFrequencies() const
{
  map<int, double> fmap;
  for (size_t i = 0; i < stateMap_->getNumberOfModelStates(); ++i)
  {
    fmap[stateMap_->getAlphabetStateAsInt(i)] += freq_[i];
  }
  return fmap;
}

// ////////////////////////////
// FullFrequencySet

FullFrequencySet::FullFrequencySet(std::shared_ptr<const StateMap> stateMap, bool allowNullFreqs, unsigned short method, const string& name) :
  AbstractFrequencySet(stateMap, "Full.", name),
  sFreq_(stateMap->getNumberOfModelStates(), method, allowNullFreqs, "Full.")
{
  vector<double> vd;
  double r = 1. / static_cast<double>(stateMap->getNumberOfModelStates());

  for (size_t i = 0; i < stateMap->getNumberOfModelStates(); i++)
  {
    vd.push_back(r);
  }

  sFreq_.setFrequencies(vd);
  addParameters_(sFreq_.getParameters());
  updateFreq_();
}

FullFrequencySet::FullFrequencySet(std::shared_ptr<const StateMap> stateMap, const vector<double>& initFreqs, bool allowNullFreqs, unsigned short method, const string& name) :
  AbstractFrequencySet(stateMap, "Full.", name),
  sFreq_(stateMap->getNumberOfModelStates(), method, allowNullFreqs, "Full.")
{
  sFreq_.setFrequencies(initFreqs);
  addParameters_(sFreq_.getParameters());
  updateFreq_();
}

void FullFrequencySet::setFrequencies(const vector<double>& frequencies)
{
  sFreq_.setFrequencies(frequencies);
  setParametersValues(sFreq_.getParameters());

  updateFreq_();
}

void FullFrequencySet::setNamespace(const std::string& nameSpace)
{
  sFreq_.setNamespace(nameSpace);
  AbstractFrequencySet::setNamespace(nameSpace);
}

void FullFrequencySet::fireParameterChanged(const ParameterList& parameters)
{
  sFreq_.matchParametersValues(parameters);
  updateFreq_();
}

void FullFrequencySet::updateFreq_()
{
  for (size_t i = 0; i < getAlphabet()->getSize(); i++)
  {
    getFreq_(i) = sFreq_.prob(i);
  }
}

// ///////////////////////////////////////////
// / FixedFrequencySet

FixedFrequencySet::FixedFrequencySet(std::shared_ptr<const StateMap> stateMap, const vector<double>& initFreqs, const string& name) :
  AbstractFrequencySet(stateMap, "Fixed.", name)
{
  if (stateMap->getNumberOfModelStates() != initFreqs.size())
    throw Exception("FixedFrequencySet::constructor. size of init vector does not match the number of states in the model.");
  setFrequencies(initFreqs);
}

FixedFrequencySet::FixedFrequencySet(std::shared_ptr<const StateMap> stateMap, const string& name) :
  AbstractFrequencySet(stateMap, "Fixed.", name)
{
  size_t n = stateMap->getNumberOfModelStates();
  setFrequencies_(std::vector<double>(n , 1./(double)n));
}

void FixedFrequencySet::setFrequencies(const vector<double>& frequencies)
{
  if (frequencies.size() != getNumberOfFrequencies())
    throw DimensionException("FixedFrequencySet::setFrequencies", frequencies.size(), getNumberOfFrequencies());
  double sum = 0.0;
  for (size_t i = 0; i < frequencies.size(); i++)
  {
    sum += frequencies[i];
  }
  if (fabs(1. - sum) > 0.00001)
    throw Exception("FixedFrequencySet::setFrequencies. Frequencies sum must equal 1 (sum = " + TextTools::toString(sum) + ").");
  setFrequencies_(frequencies);
}

MarkovModulatedFrequencySet::MarkovModulatedFrequencySet(std::shared_ptr<FrequencySet> freqSet, const std::vector<double>& rateFreqs) :
  AbstractFrequencySet(std::shared_ptr<const StateMap>(new MarkovModulatedStateMap(freqSet->getStateMap(), static_cast<unsigned int>(rateFreqs.size()))), "MarkovModulated.", "MarkovModulated." + freqSet->getName()),
  freqSet_(freqSet),
  rateFreqs_(rateFreqs)
{
  freqSet_->setNamespace(std::string("MarkovModulated.") + freqSet_->getNamespace());
  addParameters_(freqSet_->getParameters());
  setFrequencies_(VectorTools::kroneckerMult(rateFreqs, freqSet_->getFrequencies()));
}

//////////////////////////////////
/// From Model


FromModelFrequencySet::FromModelFrequencySet(const FromModelFrequencySet& fmfs) :
  AbstractFrequencySet(fmfs),
  model_(fmfs.model_->clone())
{}

FromModelFrequencySet& FromModelFrequencySet::operator=(const FromModelFrequencySet& fmfs)
{
  AbstractFrequencySet::operator=(fmfs);
  model_ = fmfs.model_->clone();
  return *this;
}

FromModelFrequencySet::~FromModelFrequencySet()
{
  delete model_;
}

FromModelFrequencySet::FromModelFrequencySet(TransitionModel* model) :
  AbstractFrequencySet(model->shareStateMap(), "FromModel." + (model ? model->getNamespace() : ""), "FromModel"),
  model_(model)
{
  model_->setNamespace(getNamespace());
  addParameters_(model_->getParameters());
  setFrequencies_(model_->getFrequencies());
}


void FromModelFrequencySet::setNamespace(const std::string& name)
{
  AbstractParameterAliasable::setNamespace(name);
  model_->setNamespace(name);
}


void FromModelFrequencySet::setFrequencies(const std::vector<double>& frequencies)
{
  std::map<int, double> freq;
  for (size_t i = 0; i < getNumberOfFrequencies(); ++i)
  {
    freq[getStateMap().getAlphabetStateAsInt(i)] += frequencies[i];
  }
  model_->setFreq(freq);
  matchParametersValues(model_->getParameters());
}

void FromModelFrequencySet::fireParameterChanged(const ParameterList& pl)
{
  model_->matchParametersValues(pl);
  setFrequencies_(model_->getFrequencies());
}


//////////////////////////////////
/// User

UserFrequencySet::UserFrequencySet(std::shared_ptr<const StateMap> stateMap, const std::string& path, size_t nCol) :
  AbstractFrequencySet(stateMap, "Empirical.", "Empirical"),
  path_(path),
  nCol_(nCol)
{
  readFromFile_();
}

UserFrequencySet::UserFrequencySet(const UserFrequencySet& fmfs) :
  AbstractFrequencySet(fmfs),
  path_(fmfs.path_),
  nCol_(fmfs.nCol_)
{}

UserFrequencySet& UserFrequencySet::operator=(const UserFrequencySet& fmfs)
{
  AbstractFrequencySet::operator=(fmfs);
  path_ = fmfs.path_;
  nCol_ = fmfs.nCol_;
  return *this;
}

void UserFrequencySet::readFromFile_()
{
  if (!FileTools::fileExists(path_.c_str()))
    throw Exception("UserFrequencySet::readFromFile. Frequencies file not found : " +  path_);

  ifstream in(path_.c_str(), ios::in);

  // Read profile:

  for (unsigned int i = 0; i < getAlphabet()->getSize(); i++)
  {
    if (!in)
      throw Exception("UserFrequencySet::readFromFile. Missing frequencies in file : " +  path_);

    string line = FileTools::getNextLine(in);
    StringTokenizer st(line);
    double s(0);
    for (unsigned int j = 0; j < nCol_; j++)
    {
      if (!st.hasMoreToken())
        throw Exception("UserFrequencySet::readFromFile. Missing frequencies for column " + TextTools::toString(nCol_) + " in line " + TextTools::toString(i));
      s = TextTools::toDouble(st.nextToken());
    }
    getFreq_(i) = s;
  }

  double sf = VectorTools::sum(getFrequencies_());
  if (fabs(sf - 1) > 0.000001)
  {
    ApplicationTools::displayMessage("WARNING!!! Frequencies sum to " + TextTools::toString(sf) + ", frequencies have been scaled.");
    sf *= 1. / sf;
  }

  // Closing stream:
  in.close();
}

void UserFrequencySet::setFrequencies(const vector<double>& frequencies)
{
  if (frequencies.size() != getNumberOfFrequencies())
    throw DimensionException("UserFrequencySet::setFrequencies", frequencies.size(), getNumberOfFrequencies());
  double sum = 0.0;
  for (size_t i = 0; i < frequencies.size(); i++)
  {
    sum += frequencies[i];
  }
  if (fabs(1. - sum) > 0.00001)
    throw Exception("FixedFrequencySet::setFrequencies. Frequencies sum must equal 1 (sum = " + TextTools::toString(sum) + ").");
  setFrequencies_(frequencies);
}
