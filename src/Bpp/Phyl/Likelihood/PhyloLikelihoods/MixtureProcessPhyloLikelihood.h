//
// File: MixtureProcessPhyloLikelihood.h
// Authors:
//   Laurent GuÃÂ©guen
// Created: jeudi 11 juillet 2013, ÃÂ  14h 05
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

#ifndef BPP_PHYL_LIKELIHOOD_PHYLOLIKELIHOODS_MIXTUREPROCESSPHYLOLIKELIHOOD_H
#define BPP_PHYL_LIKELIHOOD_PHYLOLIKELIHOODS_MIXTUREPROCESSPHYLOLIKELIHOOD_H


#include "../DataFlow/Simplex_DF.h"
#include "../MixtureSequenceEvolution.h"
#include "MultiProcessSequencePhyloLikelihood.h"

// From SeqLib:
#include <Bpp/Seq/Container/AlignedValuesContainer.h>


namespace bpp
{
/**
 * @brief Likelihood framework based on a mixture of simple likelihoods
 *
 * The resulting likelihood is the mean value of
 * the SinglePhyloLikelihoods, ponderated with parametrized probabilities
 * (through a Simplex).
 *
 * @see MultiProcessSequencePhyloLikelihood
 */

class MixtureProcessPhyloLikelihood :
  public MultiProcessSequencePhyloLikelihood
{
private:
  /**
   * @brief to avoid the dynamic casts
   *
   */

  MixtureSequenceEvolution& mSeqEvol_;

  /**
   * DF simplex for computation
   *
   */

  std::shared_ptr<ConfiguredSimplex> simplex_;

  /**
   * Aligned LikelihoodCalculation to store DF nodes
   */

  mutable std::shared_ptr<AlignedLikelihoodCalculation> likCal_;

public:
  MixtureProcessPhyloLikelihood(
    const AlignedValuesContainer& data,
    MixtureSequenceEvolution& processSeqEvol,
    CollectionNodes& collNodes,
    size_t nSeqEvol = 0,
    size_t nData = 0);

  MixtureProcessPhyloLikelihood(const MixtureProcessPhyloLikelihood& mlc) :
    AbstractPhyloLikelihood(mlc),
    AbstractAlignedPhyloLikelihood(mlc),
    MultiProcessSequencePhyloLikelihood(mlc),
    mSeqEvol_(mlc.mSeqEvol_),
    likCal_(mlc.likCal_)
  {}

  MixtureProcessPhyloLikelihood* clone() const { return new MixtureProcessPhyloLikelihood(*this); }

public:
  /**
   * @brief return the probability of a  subprocess
   *
   * @param i the index of the subprocess
   */
  double getSubProcessProb(size_t i) const
  {
    return simplex_->getTargetValue()->prob(i);
  }

  /**
   * @name The likelihood functions.
   *
   * @{
   */
  std::shared_ptr<LikelihoodCalculation> getLikelihoodCalculation () const
  {
    return likCal_;
  }

  std::shared_ptr<AlignedLikelihoodCalculation> getAlignedLikelihoodCalculation () const
  {
    return likCal_;
  }

  /*
   *@brief return the posterior probabilities of subprocess on each site.
   *
   *@return 2D-vector sites x states
   */

  VVdouble getPosteriorProbabilitiesPerSitePerProcess() const;

  /*
   * @}
   */
};
} // end of namespace bpp.
#endif // BPP_PHYL_LIKELIHOOD_PHYLOLIKELIHOODS_MIXTUREPROCESSPHYLOLIKELIHOOD_H
