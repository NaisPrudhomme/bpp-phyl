//
// File: NexusIoTree.h
// Authors:
//   Julien Dutheil
// Created: 2009-05-27 19:06:00
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

#ifndef BPP_PHYL_IO_NEXUSIOTREE_H
#define BPP_PHYL_IO_NEXUSIOTREE_H


#include "../Tree/PhyloTree.h"
#include "../Tree/TreeTemplate.h"
#include "IoTree.h"

namespace bpp
{
/**
 * @brief a simple parser for reading trees from a Nexus file.
 *
 * This reader is not supposed to be a full parser of the Nexus files,
 * but only extract the tree data. Only a basic subset of the options
 * are and will be supported.
 *
 * This format is described in the following paper:
 * Maddison D, Swofford D, and Maddison W (1997), _Syst Biol_ 46(4):590-621
 *
 * @author Julien Dutheil
 */
class NexusIOTree :
  public virtual AbstractITree,
  public virtual AbstractOTree,
  public virtual AbstractIMultiTree,
  public virtual AbstractOMultiTree,
  public AbstractIPhyloTree,
  public AbstractOPhyloTree,
  public AbstractIMultiPhyloTree,
  public AbstractOMultiPhyloTree
{
public:
  /**
   * @brief Build a new Nexus tree parser.
   */
  NexusIOTree() {}

  virtual ~NexusIOTree() {}

public:
  /**
   * @name The IOTree interface
   *
   * @{
   */
  const std::string getFormatName() const;
  const std::string getFormatDescription() const;
  /* @} */

  /**
   * @name The ITree interface
   *
   * @{
   */
  TreeTemplate<Node>* readTree(const std::string& path) const
  {
    return dynamic_cast<TreeTemplate<Node>*>(AbstractITree::readTree(path));
  }

  TreeTemplate<Node>* readTree(std::istream& in) const;

  PhyloTree* readPhyloTree(const std::string& path) const
  {
    return AbstractIPhyloTree::readPhyloTree(path);
  }

  PhyloTree* readPhyloTree(std::istream& in) const;

  /** @} */

  /**
   * @name The OTree interface
   *
   * @{
   */
  void writeTree(const Tree& tree, const std::string& path, bool overwrite = true) const
  {
    AbstractOTree::writeTree(tree, path, overwrite);
  }
  void writeTree(const Tree& tree, std::ostream& out) const
  {
    write_(tree, out);
  }
  void writePhyloTree(const PhyloTree& tree, const std::string& path, bool overwrite = true) const
  {
    AbstractOPhyloTree::writePhyloTree(tree, path, overwrite);
  }
  void writePhyloTree(const PhyloTree& tree, std::ostream& out) const
  {
    write_(tree, out);
  }
  /** @} */

  /**
   * @name The IMultiTree interface
   *
   * @{
   */
  void readTrees(const std::string& path, std::vector<Tree*>& trees) const
  {
    AbstractIMultiTree::readTrees(path, trees);
  }
  void readTrees(std::istream& in, std::vector<Tree*>& trees) const;

  void readPhyloTrees(const std::string& path, std::vector<PhyloTree*>& trees) const
  {
    AbstractIMultiPhyloTree::readPhyloTrees(path, trees);
  }

  void readPhyloTrees(std::istream& in, std::vector<PhyloTree*>& trees) const;
  /**@}*/

  /**
   * @name The OMultiTree interface
   *
   * @{
   */
  void writeTrees(const std::vector<const Tree*>& trees, const std::string& path, bool overwrite = true) const
  {
    AbstractOMultiTree::writeTrees(trees, path, overwrite);
  }
  void writeTrees(const std::vector<const Tree*>& trees, std::ostream& out) const
  {
    write_(trees, out);
  }
  void writePhyloTrees(const std::vector<const PhyloTree*>& trees, const std::string& path, bool overwrite = true) const
  {
    AbstractOMultiPhyloTree::writePhyloTrees(trees, path, overwrite);
  }
  void writePhyloTrees(const std::vector<const PhyloTree*>& trees, std::ostream& out) const
  {
    write_(trees, out);
  }
  /** @} */

protected:
  void write_(const Tree& tree, std::ostream& out) const;

  void write_(const PhyloTree& tree, std::ostream& out) const;

  template<class N>
  void write_(const TreeTemplate<N>& tree, std::ostream& out) const;

  void write_(const std::vector<const Tree*>& trees, std::ostream& out) const;

  void write_(const std::vector<const PhyloTree*>& trees, std::ostream& out) const;

  template<class N>
  void write_(const std::vector<TreeTemplate<N>*>& trees, std::ostream& out) const;
};
} // end of namespace bpp.
#endif // BPP_PHYL_IO_NEXUSIOTREE_H
