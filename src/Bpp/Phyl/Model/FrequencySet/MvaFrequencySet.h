//
// File: MvaFrequencySet.h
// Authors:
//   Mathieu Groussin
// Created: 2013-01-12 00:00:00
//

/*
  Copyright or (c) or Copr. Bio++ Development Team, (November 16, 2004)
  
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

#ifndef BPP_PHYL_MODEL_FREQUENCYSET_MVAFREQUENCYSET_H
#define BPP_PHYL_MODEL_FREQUENCYSET_MVAFREQUENCYSET_H


#include "../Protein/Coala.h"
#include "ProteinFrequencySet.h"

namespace bpp
{
/**
 * @brief A frequencies set used to estimate frequencies at the root with the COaLA model.
 * Frequencies at the root are optimized in the same way than the equlibrium frequencies on branches.
 * Hyperparameters are used, which represent positions along the principal axes obtained from a preliminary Correspondence Analysis.
 * From the optimized positions, the 20 frequencies are calculated.
 * @author Mathieu Groussin
 */

class MvaFrequencySet :
  public virtual ProteinFrequencySet,
  public AbstractFrequencySet
{
public:
  /**
   * @brief Constructor
   */
  MvaFrequencySet(const ProteicAlphabet* alpha);

  MvaFrequencySet* clone() const { return new MvaFrequencySet(*this); }

  /*
     MvaFrequencySet& operator=(const MvaFrequencySet& mfs)
     {
      AbstractFrequencySet::operator=(mfs);
      tPpalAxes_ = mfs.tPpalAxes_;
      rowCoords_ = mfs.rowCoords_;
      nbrOfAxes_ = mfs.nbrOfAxes_;
      model_ = mfs.model_;
      columnWeights_ = mfs.columnWeights_;
      paramValues_ = mfs.paramValues_;
      return *this;
     }
   */

protected:
  RowMatrix<double> tPpalAxes_;
  RowMatrix<double> rowCoords_;
  size_t nbrOfAxes_;
  std::string model_;
  std::vector<double> columnWeights_;
  std::map<std::string, std::string> paramValues_;

public:
  const ProteicAlphabet* getAlphabet() const { return dynamic_cast<const ProteicAlphabet*>(AbstractFrequencySet::getAlphabet()); }

  void setTransposeMatrixOfPpalAxes(const RowMatrix<double>& matrix) { tPpalAxes_ = matrix; }
  void setMatrixOfRowCoords(const RowMatrix<double>& matrix) { rowCoords_ = matrix; }
  void setNbrOfAxes(const size_t& nAxes) { nbrOfAxes_ = nAxes; }
  void setModelName(const std::string& modelName) { model_ = modelName; }
  void setVectorOfColumnWeights(const std::vector<double>& cw) { columnWeights_ = cw; }
  void setParamValues(std::map<std::string, std::string>& valuesSettings) {paramValues_ = valuesSettings;}

  void setFrequencies(const std::vector<double>& frequencies);

  void defineParameters();
  void fireParameterChanged(const ParameterList& parameters);
  void updateFrequencies();

  void initSet(CoalaCore* coala);

  void computeReversePCA(const std::vector<double>& positions, std::vector<double>& tmpFreqs);
  void computeCoordsFirstSpacePCA(std::vector<double>& tmpFreqs, std::vector<double>& freqs);
  void computeReverseCOA(const std::vector<double>& positions, std::vector<double>& tmpFreqs);
  void computeCoordsFirstSpaceCOA(std::vector<double>& tmpFreqs, std::vector<double>& freqs);
};
} // end of namespace bpp.
#endif // BPP_PHYL_MODEL_FREQUENCYSET_MVAFREQUENCYSET_H
