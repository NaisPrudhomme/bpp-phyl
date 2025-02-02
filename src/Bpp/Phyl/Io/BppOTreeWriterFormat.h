//
// File: BppOTreeWriterFormat.h
// Authors:
//   Julien Dutheil
// Created: 2021-06-19 13:14:00
//

/*
  Copyright or ÃÂ© or Copr. Bio++ Development Team, (2016)
  
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

#ifndef BPP_PHYL_IO_BPPOTREEWRITERFORMAT_H
#define BPP_PHYL_IO_BPPOTREEWRITERFORMAT_H


#include "IoTreeFactory.h"

namespace bpp
{
/**
 * @brief Tree I/O in BppO format.
 *
 * Creates a new OTree object according to
 * distribution description syntax (see the Bio++ Program Suite
 * manual for a detailed description of this syntax).
 *
 */
class BppOTreeWriterFormat :
  public virtual IOFormat
{
private:
  std::map<std::string, std::string> unparsedArguments_;
  int warningLevel_;

public:
  BppOTreeWriterFormat(int warningLevel) :
    unparsedArguments_(), warningLevel_(warningLevel) {}

  virtual ~BppOTreeWriterFormat() {}

public:
  const std::string getFormatName() const { return "BppO"; }

  const std::string getFormatDescription() const { return "Bpp Options format."; }

  const std::string getDataType() const { return "Tree writer"; }

  /**
   * @brief Read a OTree object from a string.
   *
   * @param description A string describing the reader in the keyval syntax.
   * @return A new OTree object according to options specified.
   * @throw Exception if an error occured.
   */
  OTree* readOTree(const std::string& description);

  /**
   * @return The arguments and their unparsed values from the last call of the read function, if there are any.
   */
  virtual const std::map<std::string, std::string>& getUnparsedArguments() const { return unparsedArguments_; }
};
} // end of namespace bpp.
#endif // BPP_PHYL_IO_BPPOTREEWRITERFORMAT_H
