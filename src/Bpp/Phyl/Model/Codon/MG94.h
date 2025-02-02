//
// File: MG94.h
// Authors:
//   Laurent Gueguen
// Created: 2009-07-08 00:00:00
//

/*
  Copyright or ÃÂ© or Copr. CNRS, (November 16, 2004)
  
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

#ifndef BPP_PHYL_MODEL_CODON_MG94_H
#define BPP_PHYL_MODEL_CODON_MG94_H


#include "../AbstractBiblioSubstitutionModel.h"
#include "CodonDistancePhaseFrequenciesSubstitutionModel.h"

namespace bpp
{
/**
 * @brief The Muse and Gaut (1994) substitution model for codons.
 * @author Laurent GuÃÂ©guen
 *
 * This model has one ratio @f$\rho@f$ of synonymous substitution rate
 * over non-synonymous substitution rate. It allows distinct
 * equilibrium frequencies between nucleotides.
 *
 * For codons @f$i=i_1i_2i_3@f$ and @f$j=j_1j_2j_3@f$, the generator term
 * @f$Q_{ij} (i \neq j)@f$ is:
 *
 * 0 if 2 or 3 of the pair @f$(i_1,j_1)(i_2,j_2) (i_3,j_3) @f$ are different.
 *
 * @f$\mu \rho \pi_{j_k} @f$  if exactly 1 of the pairs
 * @f$(i_1,j_1) (i_2,j_2) (i_3,j_3) @f$ is different (@f$k@f$), and that
 * difference is non-synonymous.
 *
 * @f$\mu \pi_{j_k} @f$  if exactly 1 of the pairs
 * @f$(i_1,j_1) (i_2,j_2) (i_3,j_3) @f$ is different (@f$k@f$), and that
 * difference is synonymous.
 *
 * @f$\rho@f$ corresponds to ratio @f$\frac{\beta}{\alpha}@f$ in
 * original publication.
 *
 * @f$\mu@f$ is a normalization factor.
 *
 * This model includes one parameter (@f$\rho@f$). The codon
 * frequencies @f$\pi_j@f$ are either observed or infered.
 *
 * Reference:
 * - Muse S.V. and Gaut B.S. (1994), Molecular_ Biology And Evolution_ 11(5) 715--724.
 */

class MG94 :
  public AbstractBiblioSubstitutionModel,
  virtual public CodonReversibleSubstitutionModel
{
private:
  std::unique_ptr<CodonDistancePhaseFrequenciesSubstitutionModel> pmodel_;

public:
  MG94(const GeneticCode* gc, std::shared_ptr<FrequencySet> codonFreqs);

  MG94(const MG94& mg94);

  MG94& operator=(const MG94& mg94);

  ~MG94();

  MG94* clone() const { return new MG94(*this); }

public:
  std::string getName() const { return "MG94"; }

  const SubstitutionModel& getSubstitutionModel() const { return *pmodel_.get(); }

  const GeneticCode* getGeneticCode() const { return pmodel_->getGeneticCode(); }

  double getCodonsMulRate(size_t i, size_t j) const { return pmodel_->getCodonsMulRate(i, j); }

  const std::shared_ptr<FrequencySet> getFrequencySet() const { return pmodel_->getFrequencySet();}

  void setFreq(std::map<int, double>& frequencies)
  {
    AbstractBiblioSubstitutionModel::setFreq(frequencies);
  }

protected:
  SubstitutionModel& getSubstitutionModel() { return *pmodel_.get(); }
};
} // end of namespace bpp.
#endif // BPP_PHYL_MODEL_CODON_MG94_H
