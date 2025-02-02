//
// File: IoFrequencySet.h
// Authors:
//   Laurent GuÃÂ©guen
// Created: lundi 9 juillet 2012, ÃÂ  12h 49
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

#ifndef BPP_PHYL_IO_IOFREQUENCYSET_H
#define BPP_PHYL_IO_IOFREQUENCYSET_H


#include "../Model/FrequencySet/FrequencySet.h"

// From bpp-core:
#include <Bpp/Exceptions.h>
#include <Bpp/Io/IoFormat.h>
#include <Bpp/Io/OutputStream.h>

// From bpp-seq:
#include <Bpp/Seq/Container/SiteContainer.h>

namespace bpp
{
class FrequencySet;

/**
 * @brief General interface for model I/O.
 */
class IoFrequencySet :
  public virtual IOFormat
{
public:
  IoFrequencySet() {}
  virtual ~IoFrequencySet() {}

public:
  virtual const std::string getDataType() const { return "Frequencies Set"; }
};

/**
 * @brief General interface for distance matrix readers.
 */
class IFrequencySet :
  public virtual IoFrequencySet
{
public:
  IFrequencySet() {}
  virtual ~IFrequencySet() {}

public:
  /**
   * @brief Read a frequencies set from a string.
   *
   * @param alphabet         The alpabet to use in the model.
   * @param freqDescription  A string describing the frequencies set.
   * @param data             A SiteContainer with the data to use to initialize fequency parameters. Can be set to 0.
   * @param parseArguments   Attempt to parse function arguments. If not, only store them and use default values instead.
   * @return A new FrequencySet object according to options specified.
   * @throw Exception if an error occured.
   */
  virtual std::shared_ptr<FrequencySet> readFrequencySet(
    const Alphabet* alphabet,
    const std::string& freqDescription,
    const AlignedValuesContainer* data,
    bool parseArguments = true) = 0;

  /**
   * @return The arguments and their unparsed values from the last call of the read function, if there are any.
   */
  virtual const std::map<std::string, std::string>& getUnparsedArguments() const = 0;
};

/**
 * @brief General interface for distance matrix writers.
 */
class OFrequencySet :
  public virtual IoFrequencySet
{
public:
  OFrequencySet() {}
  virtual ~OFrequencySet() {}

public:
  /**
   * @brief Write a substitution model to a stream.
   *
   * @param pfreqset A pointer towards a frequencies set object;
   * @param out The output stream;
   * @param globalAliases parameters linked to global alias. The
   * output will be "name=alias_name";
   * @param writtenNames is the vector of the written
   * parameters so far [in, out];
   */
  virtual void writeFrequencySet(
    const FrequencySet* pfreqset,
    OutputStream& out,
    std::map<std::string, std::string>& globalAliases,
    std::vector<std::string>& writtenNames) const = 0;
};
} // end of namespace bpp.
#endif // BPP_PHYL_IO_IOFREQUENCYSET_H
