//
// File: HmmOfAlignedPhyloLikelihood.h
// Authors:
//   Laurent GuÃÂ©guen
// Created: mercredi 28 octobre 2015, ÃÂ  23h 06
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

#ifndef BPP_PHYL_LIKELIHOOD_PHYLOLIKELIHOODS_HMMOFALIGNEDPHYLOLIKELIHOOD_H
#define BPP_PHYL_LIKELIHOOD_PHYLOLIKELIHOODS_HMMOFALIGNEDPHYLOLIKELIHOOD_H


#include "HmmLikelihood_DF.h"
#include "SetOfAlignedPhyloLikelihood.h"

// From Numeric
#include <Bpp/Numeric/Hmm/FullHmmTransitionMatrix.h>

#include "HmmPhyloAlphabet.h"
#include "HmmPhyloEmissionProbabilities.h"


namespace bpp
{
/**
 * @brief Likelihood framework based on a hmm of simple likelihoods
 *
 * The resulting likelihood is the likelihood of the given Hmm with
 * the site emission probabilities proportional to the computed
 * likelihoods of the process.
 *
 *
 */
class HmmOfAlignedPhyloLikelihood :
  public SetOfAlignedPhyloLikelihood
{
private:
  std::shared_ptr<HmmPhyloAlphabet> hma_;

  std::shared_ptr<FullHmmTransitionMatrix> htm_;

  std::shared_ptr<HmmPhyloEmissionProbabilities> hpep_;

  mutable std::shared_ptr<HmmLikelihood_DF> hmm_;

public:
  HmmOfAlignedPhyloLikelihood(Context& context, std::shared_ptr<PhyloLikelihoodContainer> pC, const std::vector<size_t>& nPhylo, bool inCollection = true);

  HmmOfAlignedPhyloLikelihood(const HmmOfAlignedPhyloLikelihood& mlc) :
    AbstractPhyloLikelihood(mlc),
    AbstractAlignedPhyloLikelihood(mlc),
    SetOfAlignedPhyloLikelihood(mlc),
    hma_(std::shared_ptr<HmmPhyloAlphabet>(mlc.hma_->clone())),
    htm_(std::shared_ptr<FullHmmTransitionMatrix>(mlc.htm_->clone())),
    hpep_(std::shared_ptr<HmmPhyloEmissionProbabilities>(mlc.hpep_->clone())),
    hmm_(std::shared_ptr<HmmLikelihood_DF>(mlc.hmm_->clone()))
  {}

  virtual ~HmmOfAlignedPhyloLikelihood() {}

  HmmOfAlignedPhyloLikelihood* clone() const { return new HmmOfAlignedPhyloLikelihood(*this); }

public:
  void setNamespace(const std::string& nameSpace);

  void fireParameterChanged(const ParameterList& parameters);

  const HmmPhyloAlphabet* getHmmStateAlphabet() const
  {
    return hma_.get();
  }

  /**
   * @name The likelihood functions.
   *
   * @{
   */
  std::shared_ptr<LikelihoodCalculation> getLikelihoodCalculation () const
  {
    return hmm_;
  }

  std::shared_ptr<AlignedLikelihoodCalculation> getAlignedLikelihoodCalculation () const
  {
    return hmm_;
  }

  /*
   *@brief return the posterior probabilities of subprocess on each site.
   *
   *@return MatrixXd sites x states
   */
  VVdouble getPosteriorProbabilitiesPerSitePerAligned() const
  {
    VVdouble pp;
    auto mat = hmm_->getHiddenStatesPosteriorProbabilities().transpose();
    copyEigenToBpp(mat, pp);
    return pp;
  }

  Vdouble getPosteriorProbabilitiesForASitePerAligned(size_t site) const
  {
    Vdouble pp;
    auto mat = hmm_->getHiddenStatesPosteriorProbabilitiesForASite(site);
    copyEigenToBpp(mat, pp);
    return pp;
  }

  const Eigen::MatrixXd& getHmmTransitionMatrix() const
  {
    return hmm_->getHmmTransitionMatrix();
  }
};
} // end of namespace bpp.
#endif // BPP_PHYL_LIKELIHOOD_PHYLOLIKELIHOODS_HMMOFALIGNEDPHYLOLIKELIHOOD_H
