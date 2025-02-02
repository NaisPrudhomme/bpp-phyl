//
// File: JCprot.h
// Authors:
//   Julien Dutheil
// Created: 2003-05-25 16:04:36
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

#ifndef BPP_PHYL_MODEL_PROTEIN_JCPROT_H
#define BPP_PHYL_MODEL_PROTEIN_JCPROT_H

#include <Bpp/Seq/Alphabet/ProteicAlphabet.h>

#include "../AbstractSubstitutionModel.h"
#include "../FrequencySet/ProteinFrequencySet.h"
#include "ProteinSubstitutionModel.h"

namespace bpp
{
/**
 * @brief The Jukes-Cantor substitution model for proteins.
 *
 * All rates equal:
 * \f[
 * \begin{pmatrix}
 * \ddots & r      & \ldots & r \\
 * r      & \ddots & \ddots & \vdots \\
 * \vdots & \ddots & \ddots & r \\
 * r      & \ldots & r      & \ddots \\
 * \end{pmatrix}
 * \f]
 * \f[
 * \pi = diag\left(\frac{1}{20}, \ldots, \frac{1}{20}\right)
 * \f]
 * Normalization: \f$r\f$ is set so that \f$\sum_i Q_{i,i}\pi_i = -1\f$:
 * \f[
 * S = \begin{pmatrix}
 * -20           & \frac{20}{19} & \ldots        & \frac{20}{19} \\
 * \frac{20}{19} &           -20 & \ddots        & \vdots \\
 * \vdots        & \ddots        & \ddots        & \frac{20}{19} \\
 * \frac{20}{19} & \ldots        & \frac{20}{19} & -20 \\
 * \end{pmatrix}
 * \f]
 * The normalized generator is obtained by taking the dot product of \f$S\f$ and \f$pi\f$:
 * \f[
 * Q = S . \pi = \begin{pmatrix}
 * -1 & \frac{1}{19}     & \ldots & \frac{1}{19} \\
 * \frac{1}{19} & -1     & \ddots & \vdots \\
 * \vdots       & \ddots & \ddots & \frac{1}{19} \\
 * \frac{1}{19} & \ldots & \frac{1}{19} & -1 \\
 * \end{pmatrix}
 * \f]
 *
 * The eigen values are \f$\left(0, -\frac{20}{19}, \ldots, -\frac{20}{19}\right)\f$,
 * the left eigen vectors are, by row:
 * \f[
 * U = \begin{pmatrix}
 *   \frac{1}{20} &        \ldots &  \frac{1}{20} &   \frac{1}{20} &  \frac{1}{20} \\
 *  -\frac{1}{20} &        \ldots & -\frac{1}{20} &  \frac{19}{20} & -\frac{1}{20} \\
 *         \vdots &        \ddots & \frac{19}{20} & -\frac{1}{20}  & -\frac{1}{20} \\
 *  -\frac{1}{20} &        \ddots &        \ddots &         \vdots &        \vdots \\
 *  \frac{19}{20} & -\frac{1}{20} &        \ldots &  -\frac{1}{20} & -\frac{1}{20} \\
 * \end{pmatrix}
 * \f]
 * and the right eigen vectors are, by column:
 * \f[
 * U^-1 = \begin{pmatrix}
 *      1 &      0 &  \ldots &      0 &  1 \\
 * \vdots & \vdots &  \ddots & \ddots &  0 \\
 *      1 &      0 &       1 & \ddots & \vdots \\
 *      1 &      1 &       0 & \ldots &  0 \\
 *      1 &     -1 &      -1 & \ldots & -1 \\
 * \end{pmatrix}
 * \f]
 *
 * In addition, a rate_ factor defines the mean rate of the model.
 *
 * The probabilities of changes are computed analytically using the formulas:
 * \f[
 * P_{i,j}(t) = \begin{cases}
 * \frac{1}{20} + \frac{19}{20}e^{- rate\_ * \frac{20}{19}t}& \text{if $i=j$}, \\
 * \frac{1}{20} - \frac{1}{20}e^{- rate\_ * \frac{20}{19}t} & \text{otherwise}.
 * \end{cases}
 * \f]
 *
 * First and second order derivatives are also computed analytically using the formulas:
 * \f[
 * \frac{\partial P_{i,j}(t)}{\partial t} = rate\_ * \begin{cases}
 * -e^{- rate\_ * \frac{20}{19}t}           & \text{if $i=j$}, \\
 * \frac{1}{19}e^{- rate\_ * \frac{20}{19}t} & \text{otherwise}.
 * \end{cases}
 * \f]
 * \f[
 * \frac{\partial^2 P_{i,j}(t)}{\partial t^2} =  rate\_^2 * \begin{cases}
 * \frac{20}{19}e^{- rate\_ * \frac{20}{19}t}  & \text{if $i=j$}, \\
 * -\frac{20}{361}e^{- rate\_ * \frac{20}{19}t} & \text{otherwise}.
 * \end{cases}
 * \f]
 *
 * Reference:
 * - Jukes TH and Cantor CR (1969), Evolution_ of proteins molecules_, 121-123, in Mammalian_ protein metabolism_.
 */
class JCprot :
  public AbstractReversibleProteinSubstitutionModel
{
private:
  mutable double exp_;
  mutable RowMatrix<double> p_;
  std::shared_ptr<ProteinFrequencySet> freqSet_;
  bool withFreq_;

public:
  /**
   * @brief Build a simple JC69 model, with original equilibrium frequencies.
   *
   * @param alpha A proteic alphabet.
   */
  JCprot(const ProteicAlphabet* alpha);

  /**
   * @brief Build a JC69 model with special equilibrium frequencies.
   *
   * @param alpha A proteic alphabet.
   * @param freqSet A pointer toward a protein frequencies set, which will be owned by this instance.
   * @param initFreqs Tell if the frequency set should be initialized with the original JTT92 values.
   * Otherwise, the values of the set will be used.
   */
  JCprot(const ProteicAlphabet* alpha, std::shared_ptr<ProteinFrequencySet> freqSet, bool initFreqs = false);

  JCprot(const JCprot& model) :
    AbstractParameterAliasable(model),
    AbstractReversibleProteinSubstitutionModel(model),
    exp_(model.exp_),
    p_(model.p_),
    freqSet_(dynamic_cast<ProteinFrequencySet*>(model.freqSet_->clone())),
    withFreq_(model.withFreq_)
  {}

  JCprot& operator=(const JCprot& model)
  {
    AbstractParameterAliasable::operator=(model);
    AbstractReversibleProteinSubstitutionModel::operator=(model);
    exp_ = model.exp_;
    p_   = model.p_;
    freqSet_.reset(dynamic_cast<ProteinFrequencySet*>(model.freqSet_->clone()));
    withFreq_ = model.withFreq_;
    return *this;
  }

  virtual ~JCprot() {}

  JCprot* clone() const { return new JCprot(*this); }

public:
  double Pij_t    (size_t i, size_t j, double d) const;
  double dPij_dt  (size_t i, size_t j, double d) const;
  double d2Pij_dt2(size_t i, size_t j, double d) const;
  const Matrix<double>& getPij_t    (double d) const;
  const Matrix<double>& getdPij_dt  (double d) const;
  const Matrix<double>& getd2Pij_dt2(double d) const;

  std::string getName() const
  {
    return withFreq_ ? "JC69+F" : "JC69";
  }

  void fireParameterChanged(const ParameterList& parameters)
  {
    freqSet_->matchParametersValues(parameters);
    freq_ = freqSet_->getFrequencies();
    AbstractReversibleSubstitutionModel::fireParameterChanged(parameters);
  }

  void setFrequencySet(const ProteinFrequencySet& freqSet)
  {
    freqSet_.reset(dynamic_cast<ProteinFrequencySet*>(freqSet.clone()));
    resetParameters_();
    addParameters_(freqSet_->getParameters());
  }

  const std::shared_ptr<FrequencySet> getFrequencySet() const { return freqSet_; }

  void setNamespace(const std::string& prefix)
  {
    AbstractParameterAliasable::setNamespace(prefix);
    freqSet_->setNamespace(prefix + freqSet_->getName() + ".");
  }

  void setFreqFromData(const SequencedValuesContainer& data, double pseudoCount = 0);

protected:
  /**
   * In the case of the model of Jukes & Cantor, this method is useless since
   * the generator is fixed! No matrice can be changed... This method is only
   * used in the constructor of the class.
   */
  void updateMatrices();
};
} // end of namespace bpp.
#endif // BPP_PHYL_MODEL_PROTEIN_JCPROT_H
