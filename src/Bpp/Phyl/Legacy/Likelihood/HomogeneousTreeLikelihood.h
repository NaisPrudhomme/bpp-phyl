//
// File: HomogeneousTreeLikelihood.h
// Authors:
//   Julien Dutheil
// Created: 2007-10-09 16:03:00
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

#ifndef BPP_PHYL_LEGACY_LIKELIHOOD_HOMOGENEOUSTREELIKELIHOOD_H
#define BPP_PHYL_LEGACY_LIKELIHOOD_HOMOGENEOUSTREELIKELIHOOD_H


#include "../../Model/SubstitutionModel.h"
#include "TreeLikelihood.h"

namespace bpp
{
/**
 * @brief Specialization of the TreeLikelihood interface for the Homogeneous case.
 *
 * Homogeneous models assume a unique substitution model along the tree.
 * This interface further assumes that  the substitution model is the same for all sites.
 * For likelihood functions with different model per sites, see SitePartitionHomogeneousTreeLikelihood.
 *
 * @see SubstitutionModel, SitePartitionHomogeneousTreeLikelihood.
 */
class HomogeneousTreeLikelihood :
  public virtual TreeLikelihood
{
public:
  HomogeneousTreeLikelihood* clone() const = 0;

public:
  const TransitionModel* getModelForSite(int nodeId, size_t siteIndex) const
  {
    return getModel();
  }

  TransitionModel* getModelForSite(int nodeId, size_t siteIndex)
  {
    return getModel();
  }

  /**
   * @return The substitution model attached to this instance.
   */
  virtual const TransitionModel* getModel() const = 0;

  /**
   * @return The substitution model attached to this instance.
   */
  virtual TransitionModel* getModel() = 0;

  /**
   * @return Set the substitution model for this instance.
   * @throw Exception If the model could not be set (for instance, because of a wrong alphabet type).
   */
  virtual void setModel(TransitionModel* model) = 0;

  /**
   * @brief Get a SubstitutionModel pointer toward the model associated to this instance, if possible.
   *
   * Performs a cast operation on the pointer. Return NULL if cast failed.
   * @return A SubstitutionModel pointer toward the model associated to this instance.
   */
  virtual const SubstitutionModel* getSubstitutionModel() const = 0;

  /**
   * @brief Get a SubstitutionModel pointer toward the model associated to this instance, if possible.
   *
   * Performs a cast operation on the pointer. Return NULL if cast failed.
   * @return A SubstitutionModel pointer toward the model associated to this instance.
   *
   * @param nodeId Id of the node
   * @param siteIndex Position of the site
   */
  virtual const SubstitutionModel* getSubstitutionModel(int nodeId, size_t siteIndex) const = 0;
};
} // end of namespace bpp.
#endif // BPP_PHYL_LEGACY_LIKELIHOOD_HOMOGENEOUSTREELIKELIHOOD_H
