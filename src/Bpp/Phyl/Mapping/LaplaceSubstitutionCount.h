//
// File: LaplaceSubstitutionCount.h
// Authors:
//   Julien Dutheil
// Created: 2006-04-05 11:21:00
//

/*
  Copyright or ÃÂ© or Copr. Bio++ Development Team, (November 16, 2004, 2005, 2006)
  
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

#ifndef BPP_PHYL_MAPPING_LAPLACESUBSTITUTIONCOUNT_H
#define BPP_PHYL_MAPPING_LAPLACESUBSTITUTIONCOUNT_H


#include "../Model/SubstitutionModel.h"
#include "SubstitutionCount.h"

namespace bpp
{
/**
 * @brief Laplace estimate of the substitution count.
 *
 * This method uses Laplace transforms, as described in
 * Dutheil J, Pupko T, Jean-Marie A, Galtier N.
 * A model-based approach for detecting coevolving positions in a molecule.
 * Mol Biol Evol. 2005 Sep;22(9):1919-28.
 *
 * @see UniformizationSubstitutionCount
 * @see DecompositionSubstitutionCount
 * @author Julien Dutheil
 */
class LaplaceSubstitutionCount :
  public AbstractSubstitutionCount
{
private:
  const SubstitutionModel* model_;
  size_t cutOff_;
  mutable double currentLength_;
  mutable RowMatrix<double> m_;

public:
  LaplaceSubstitutionCount(const SubstitutionModel* model, size_t cutOff) :
    AbstractSubstitutionCount(new TotalSubstitutionRegister(model->getStateMap())),
    model_        (model),
    cutOff_       (cutOff),
    currentLength_(0),
    m_            (model->getNumberOfStates(), model->getNumberOfStates())
  {}

  LaplaceSubstitutionCount(const StateMap& statemap, size_t cutOff) :
    AbstractSubstitutionCount(new TotalSubstitutionRegister(statemap)),
    model_        (0),
    cutOff_       (cutOff),
    currentLength_(0),
    m_            (statemap.getNumberOfModelStates(), statemap.getNumberOfModelStates())
  {}

  LaplaceSubstitutionCount(const LaplaceSubstitutionCount& asc) :
    AbstractSubstitutionCount(asc),
    model_        (asc.model_),
    cutOff_       (asc.cutOff_),
    currentLength_(asc.currentLength_),
    m_            (asc.m_)
  {}

  LaplaceSubstitutionCount& operator=(const LaplaceSubstitutionCount& asc)
  {
    AbstractSubstitutionCount::operator=(asc);
    model_         = asc.model_;
    cutOff_        = asc.cutOff_;
    currentLength_ = asc.currentLength_;
    m_             = asc.m_;
    return *this;
  }

  virtual ~LaplaceSubstitutionCount() {}

  LaplaceSubstitutionCount* clone() const { return new LaplaceSubstitutionCount(*this); }

public:
  double getNumberOfSubstitutions(size_t initialState, size_t finalState, double length, size_t type = 1) const;

  Matrix<double>* getAllNumbersOfSubstitutions(double length, size_t type = 1) const;
  void storeAllNumbersOfSubstitutions(double length, size_t type, Eigen::MatrixXd& mat) const;

  std::vector<double> getNumberOfSubstitutionsPerType(size_t initialState, size_t finalState, double length) const
  {
    std::vector<double> v(0);
    v[0] = getNumberOfSubstitutions(initialState, finalState, length, 0);
    return v;
  }

  void setSubstitutionModel(const SubstitutionModel* model);

  /*
   *@param reg pointer to a SubstitutionRegister
   *
   */
  void setSubstitutionRegister(SubstitutionRegister* reg)
  {
    throw Exception("LaplaceSubstitutionCount::setSubstitutionRegister. This SubstitutionsCount only works with a TotalSubstitutionRegister.");
  }

protected:
  void computeCounts(double length) const;
  void substitutionRegisterHasChanged() {}
};
} // end of namespace bpp.
#endif // BPP_PHYL_MAPPING_LAPLACESUBSTITUTIONCOUNT_H
