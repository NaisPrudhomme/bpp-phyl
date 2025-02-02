//
// File: BppOTreeReaderFormat.cpp
// Authors:
//   Julien Dutheil
// Created: 2021-06-19 13:16:00
//

/*
  Copyright or ÃÂ© or Copr. Bio++ Development Team, (November 2016)
  
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

#include <Bpp/Text/KeyvalTools.h>
#include <memory>
#include <string>

#include "BppOTreeReaderFormat.h"
#include "Newick.h"
#include "NexusIoTree.h"
#include "Nhx.h"

using namespace bpp;
using namespace std;

ITree* BppOTreeReaderFormat::readITree(const std::string& description)
{
  unparsedArguments_.clear();
  string format = "";
  KeyvalTools::parseProcedure(description, format, unparsedArguments_);
  unique_ptr<ITree> iTree;
  if (format == "Newick")
  {
    bool allowComments = ApplicationTools::getBooleanParameter("allow_comments", unparsedArguments_, false, "", true, warningLevel_);
    iTree.reset(new Newick(allowComments));
  }
  else if (format == "Nhx")
  {
    iTree.reset(new Nhx());
  }
  else if (format == "Nexus")
  {
    iTree.reset(new NexusIOTree());
  }
  else
  {
    throw Exception("Tree format '" + format + "' unknown.");
  }

  return iTree.release();
}
