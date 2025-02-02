//
// File: PhylipDistanceMatrixFormat.h
// Authors:
//   Julien Dutheil
// Created: 2005-06-08 15:57:00
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

#ifndef BPP_PHYL_IO_PHYLIPDISTANCEMATRIXFORMAT_H
#define BPP_PHYL_IO_PHYLIPDISTANCEMATRIXFORMAT_H


#include "IoDistanceMatrix.h"

namespace bpp
{
/**
 * @brief Distance matrix I/O in Phylip format.
 *
 * Entry names must be 10 characters long. If 'extended' is set to true, then
 * entry names can be of any size, and should be separated from the data by at least two spaces.
 * Names should therefor not contian more than one consecutive space.
 */
class PhylipDistanceMatrixFormat :
  public AbstractIDistanceMatrix,
  public AbstractODistanceMatrix
{
private:
  bool extended_;

public:
  PhylipDistanceMatrixFormat(bool extended = false) : extended_(extended) {}
  virtual ~PhylipDistanceMatrixFormat() {}

public:
  const std::string getFormatName() const { return "Phylip"; }

  const std::string getFormatDescription() const {  return "Multiline space-delimited columns."; }
  DistanceMatrix* readDistanceMatrix(const std::string& path) const
  {
    return AbstractIDistanceMatrix::readDistanceMatrix(path);
  }
  DistanceMatrix* readDistanceMatrix(std::istream& in) const;

  void writeDistanceMatrix(const DistanceMatrix& dist, const std::string& path, bool overwrite = true) const
  {
    AbstractODistanceMatrix::writeDistanceMatrix(dist, path, overwrite);
  }
  void writeDistanceMatrix(const DistanceMatrix& dist, std::ostream& out) const;
};
} // end of namespace bpp.
#endif // BPP_PHYL_IO_PHYLIPDISTANCEMATRIXFORMAT_H
