//
// File: OneJumpSubstitutionCount.cpp
// Authors:
//   Julien Dutheil
// Created: 2009-11-24 17:00:00
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


#include "OneJumpSubstitutionCount.h"

using namespace bpp;

Matrix<double>* OneJumpSubstitutionCount::getAllNumbersOfSubstitutions(double length, size_t type) const
{
  if (!model_)
    throw Exception("OneJumpSubstitutionCount::getAllNumberOfSubstitutions: model not defined.");

  tmp_ = model_->getPij_t(length);
  size_t n = model_->getNumberOfStates();
  Matrix<double>* probs = new LinearMatrix<double>(n, n);
  for (size_t i = 0; i < n; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      (*probs)(i, j) = (i == j ? 1. - tmp_(i, j) : 1.);
    }
  }
  return probs;
}

void OneJumpSubstitutionCount::storeAllNumbersOfSubstitutions(double length, size_t type, Eigen::MatrixXd& mat) const
{
  if (!model_)
    throw Exception("OneJumpSubstitutionCount::storeNumberOfSubstitutions: model not defined.");

  tmp_ = model_->getPij_t(length);
  auto n = Eigen::Index(model_->getNumberOfStates());

  mat.resize(n, n);
  for (auto i = 0; i < n; i++)
  {
    for (auto j = 0; j < n; j++)
    {
      mat(i, j) = (i == j ? 1. - tmp_(size_t(i), size_t(j)) : 1.);
    }
  }
}
