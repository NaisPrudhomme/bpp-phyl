//
// File: YN98.cpp
// Authors:
//   Laurent Gueguen
// Created: 2009-07-08 00:00:00
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

#include <Bpp/Numeric/NumConstants.h>

#include "../FrequencySet/CodonFrequencySet.h"
#include "../Nucleotide/K80.h"
#include "YN98.h"

using namespace bpp;

using namespace std;

/******************************************************************************/

YN98::YN98(const GeneticCode* gc, std::shared_ptr<FrequencySet> codonFreqs) :
  AbstractBiblioTransitionModel("YN98."),
  AbstractBiblioSubstitutionModel("YN98."),
  pmodel_(new CodonDistanceFrequenciesSubstitutionModel(gc, new K80(dynamic_cast<const CodonAlphabet*>(gc->getSourceAlphabet())->getNucleicAlphabet()), codonFreqs))
{
  computeFrequencies(false);

  addParameter_(new Parameter("YN98.kappa", 1, Parameter::R_PLUS_STAR));
  addParameter_(new Parameter("YN98.omega", 1, std::make_shared<IntervalConstraint>(0.001, 999, true, true)));

  pmodel_->setNamespace("YN98.");
  addParameters_(codonFreqs->getParameters());

  lParPmodel_.addParameters(pmodel_->getParameters());

  vector<std::string> v = pmodel_->getFrequencySet()->getParameters().getParameterNames();

  for (size_t i = 0; i < v.size(); i++)
  {
    mapParNamesFromPmodel_[v[i]] = getParameterNameWithoutNamespace(v[i]);
  }
  mapParNamesFromPmodel_["YN98.123_K80.kappa"] = "kappa";
  mapParNamesFromPmodel_["YN98.beta"] = "omega";

  updateMatrices();
}


YN98::YN98(const YN98& yn98) :
  AbstractBiblioTransitionModel(yn98),
  AbstractBiblioSubstitutionModel(yn98),
  pmodel_(new CodonDistanceFrequenciesSubstitutionModel(*yn98.pmodel_))
{}

YN98& YN98::operator=(const YN98& yn98)
{
  AbstractBiblioSubstitutionModel::operator=(yn98);
  pmodel_.reset(new CodonDistanceFrequenciesSubstitutionModel(*yn98.pmodel_));
  return *this;
}
