//
// File: DRHomogeneousMixedTreeLikelihood.h
// Authors:
//   Laurent Gueguen
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

#ifndef BPP_PHYL_LEGACY_LIKELIHOOD_DRHOMOGENEOUSMIXEDTREELIKELIHOOD_H
#define BPP_PHYL_LEGACY_LIKELIHOOD_DRHOMOGENEOUSMIXEDTREELIKELIHOOD_H

#include <Bpp/Numeric/Prob/DiscreteDistribution.h>
#include <Bpp/Numeric/VectorTools.h>

#include "../../Model/MixedTransitionModel.h"
#include "../../Model/SubstitutionModel.h"
#include "DRHomogeneousTreeLikelihood.h"

namespace bpp
{
/**
 * @brief A class to compute the average of several
 * DRHomogeneousTreeLikelihood defined from a Mixed Substitution
 * Model.
 *
 * In all computations, the average of the likelihoods, probabilities
 * are computed.
 **/
class DRHomogeneousMixedTreeLikelihood :
  public DRHomogeneousTreeLikelihood
{
private:
  std::vector<DRHomogeneousTreeLikelihood*> treeLikelihoodsContainer_;
  std::vector<double> probas_;

  // true if the root Array should be computed (for ancestral
  // reconstruction)

  bool rootArray_;

public:
  /**
   * @brief Build a new DRHomogeneousMixedTreeLikelihood object without
   * data.
   *
   * This constructor only initialize the parameters. To compute a
   * likelihood, you will need to call the setData() and the
   * computeTreeLikelihood() methods.
   *
   * @param tree The tree to use.
   * @param model The mixed substitution model to use.
   * @param rDist The rate across sites distribution to use.
   * @param checkRooted Tell if we have to check for the tree to be unrooted.
   * If true, any rooted tree will be unrooted before likelihood computation.
   * @param verbose Should I display some info?
   * @param rootArray is true if the array of the likelihoods at the root
   *    should be computed (useful for ancestral reconstruction).
   * @throw Exception in an error occured.
   */
  DRHomogeneousMixedTreeLikelihood(
    const Tree& tree,
    TransitionModel* model,
    DiscreteDistribution* rDist,
    bool checkRooted = true,
    bool verbose = true,
    bool rootArray = false);

  /**
   * @brief Build a new DRHomogeneousMixedTreeLikelihood object with data.
   *
   * This constructor initializes all parameters, data, and likelihood arrays.
   *
   * @param tree The tree to use.
   * @param data Sequences to use.
   * @param model The mixed substitution model to use.
   * @param rDist The rate across sites distribution to use.
   * @param checkRooted Tell if we have to check for the tree to be unrooted.
   * If true, any rooted tree will be unrooted before likelihood computation.
   * @param verbose Should I display some info?
   * @param rootArray is true if the array of the likelihoods at the root
   *    should be computed (useful for ancestral reconstruction).
   * @throw Exception in an error occured.
   */
  DRHomogeneousMixedTreeLikelihood(
    const Tree& tree,
    const AlignedValuesContainer& data,
    TransitionModel* model,
    DiscreteDistribution* rDist,
    bool checkRooted = true,
    bool verbose = true,
    bool rootArray = false);

  DRHomogeneousMixedTreeLikelihood(const DRHomogeneousMixedTreeLikelihood& lik);

  DRHomogeneousMixedTreeLikelihood& operator=(const DRHomogeneousMixedTreeLikelihood& lik);

  virtual ~DRHomogeneousMixedTreeLikelihood();

  DRHomogeneousMixedTreeLikelihood* clone() const { return new DRHomogeneousMixedTreeLikelihood(*this); }

public:
  /**
   * @name The TreeLikelihood interface.
   *
   * Other methods are implemented in the DRHomogeneousTreeLikelihood class.
   *
   * @{
   */
  double getLikelihood() const;
  double getLogLikelihood() const;

  void setData(const AlignedValuesContainer& sites);
  double getLikelihoodForASite (size_t site) const;
  double getLogLikelihoodForASite(size_t site) const;
  /** @} */


  /**
   * @name The DiscreteRatesAcrossSites interface implementation:
   *
   * @{
   */
  double getLikelihoodForASiteForARateClass(size_t site, size_t rateClass) const;
  double getLogLikelihoodForASiteForARateClass(size_t site, size_t rateClass) const;
  double getLikelihoodForASiteForARateClassForAState(size_t site, size_t rateClass, int state) const;
  double getLogLikelihoodForASiteForARateClassForAState(size_t site, size_t rateClass, int state) const;
  /** @} */

  /**
   * @name DerivableFirstOrder interface.
   *
   * @{
   */
  double getFirstOrderDerivative(const std::string& variable) const;
  /** @} */

  /**
   * @name DerivableSecondOrder interface.
   *
   * @{
   */
  double getSecondOrderDerivative(const std::string& variable) const;
  double getSecondOrderDerivative(const std::string& variable1, const std::string& variable2) const { return 0; } // Not implemented for now.
  /** @} */

public:
  // Specific methods:
  void initialize();

  void fireParameterChanged(const ParameterList& params);

  void computeTreeLikelihood();

  virtual void computeTreeDLikelihoods();

protected:
  virtual void computeLikelihoodAtNode_(const Node* node, VVVdouble& likelihoodArray, const Node* sonNode = 0) const;

  /**
   * @brief Compute the likelihood for a subtree defined by the Tree::Node <i>node</i>.
   *
   * @param node The root of the subtree.
   */
  virtual void computeSubtreeLikelihoodPostfix(const Node* node);

  virtual void computeSubtreeLikelihoodPrefix(const Node* node);

  virtual void computeRootLikelihood();

  virtual void computeTreeD2LikelihoodAtNode(const Node*);
  virtual void computeTreeD2Likelihoods();

  void resetLikelihoodArrays(const Node* node);

  /**
   * @brief This method is mainly for debugging purpose.
   *
   * @param node The node at which likelihood values must be displayed.
   */
  virtual void displayLikelihood(const Node* node);
};
} // end of namespace bpp.
#endif // BPP_PHYL_LEGACY_LIKELIHOOD_DRHOMOGENEOUSMIXEDTREELIKELIHOOD_H
