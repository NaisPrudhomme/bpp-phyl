//
// File: HmmPhyloEmissionProbabilities.cpp
// Authors:
//   Laurent GuÃÂ©guen
// Created: mardi 24 septembre 2013, ÃÂ  10h 09
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


#include "HmmPhyloEmissionProbabilities.h"

using namespace bpp;
using namespace std;

HmmPhyloEmissionProbabilities::HmmPhyloEmissionProbabilities(std::shared_ptr<HmmPhyloAlphabet> alphabet) :
  AbstractParametrizable(""),
  context_(alphabet->getContext()),
  phylAlph_(alphabet),
  emProb_(),
  nbSites_(alphabet->getNumberOfSites())
{
  setHmmStateAlphabet(alphabet);
}


void HmmPhyloEmissionProbabilities::setHmmStateAlphabet(std::shared_ptr<HmmStateAlphabet> stateAlphabet)
{
  if (stateAlphabet == NULL)
    throw HmmUnvalidAlphabetException("Null alphabet in HmmPhyloEmissionProbabilities::setHmmStateAlphabet");
  if (dynamic_cast<const HmmPhyloAlphabet*>(stateAlphabet.get()) == NULL)
    throw HmmUnvalidAlphabetException("Non PhyloLikelihood alphabet in HmmPhyloEmissionProbabilities::setHmmStateAlphabet");

  phylAlph_ = dynamic_pointer_cast<HmmPhyloAlphabet>(stateAlphabet);

  std::vector<std::shared_ptr<Node_DF> > vEM;

  auto nbStates = phylAlph_->getNumberOfStates();

  for (size_t i = 0; i < nbStates; i++)
  {
    auto tmp = phylAlph_->getPhyloLikelihood(i).getAlignedLikelihoodCalculation()->getSiteLikelihoods(false);
    vEM.push_back(tmp);
  }

  // Compound to put site log lik of different processes in a matrix

  emProb_ = EmissionLogk::create(context_, std::move(vEM), Dimension<MatrixLik>(Eigen::Index(nbStates), Eigen::Index(nbSites_)));
}
