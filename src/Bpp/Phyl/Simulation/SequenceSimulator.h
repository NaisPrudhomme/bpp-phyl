//
// File: SequenceSimulator.h
// Authors:
//   Julien Dutheil
// Created: 2004-02-04 16:30:51
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

#ifndef BPP_PHYL_SIMULATION_SEQUENCESIMULATOR_H
#define BPP_PHYL_SIMULATION_SEQUENCESIMULATOR_H



// From bpp-core:
#include <Bpp/Clonable.h>

// From bpp-seq:
#include <Bpp/Seq/Container/SiteContainer.h>

#include "SiteSimulator.h"

namespace bpp
{
/**
 * @brief The SequenceSimulator interface.
 * SequenceSimulator classes can simulate whole datasets.
 */
class SequenceSimulator :
  public virtual Clonable
{
public:
  SequenceSimulator() {}
  virtual ~SequenceSimulator() {}

  SequenceSimulator* clone() const = 0;

public:
  virtual std::shared_ptr<SiteContainer> simulate(size_t numberOfSites) const = 0;

  virtual const SiteSimulator& getSiteSimulator(size_t pos) const = 0;

  virtual std::vector<std::string> getSequencesNames() const = 0;

  virtual const Alphabet* getAlphabet() const = 0;

  virtual void outputInternalSequences(bool inter) = 0;
};
} // end of namespace bpp.
#endif // BPP_PHYL_SIMULATION_SEQUENCESIMULATOR_H
