//
// File: Newick.h
// Authors:
//   Julien Dutheil
// Created: 2003-10-23 15:35:03
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

#ifndef BPP_PHYL_IO_NEWICK_H
#define BPP_PHYL_IO_NEWICK_H


#include "../Tree/PhyloTree.h"
#include "../Tree/TreeTemplate.h"
#include "IoTree.h"

namespace bpp
{
/**
 * @brief The so-called 'newick' parenthetic format.
 *
 * Branch lengths and bootstraps are supported:
 *
 * ex:
 * <code>
 * ((A:0.1, B:0.15)90:0.2, C:0.27);
 * </code>
 *
 * Code example:
 * @code
 * #include <Phyl/Newick.h>
 * #include <Phyl/Tree/Tree.h>
 *
 * Newick * newickReader = new Newick(false); //No comment allowed!
 * try {
 *  Tree * tree = newickReader->read("MyTestTree.dnd"); // Tree in file MyTestTree.dnd
 *  cout << "Tree has " << tree->getNumberOfLeaves() << " leaves." << endl;
 * } catch (Exception e) {
 *	cout << "Error when reading tree." << endl;
 * }
 * delete tree;
 * delete newickReader;
 * @endcode
 *
 * Bootstrap values are stored as node properties, as Number<double> objects and with the tag TreeTools::BOOTSTRAP.
 *
 * This is also possible to read a "tagged" tree, where additional info is provided in place of bootstrap values:
 * ((A,B)N2,(C,D)N3)N1;
 * This is achieved by calling the enableExtendedBootstrapProperty method, and providing a property name to use.
 * The additional information will be stored at each node as a property, in a String object.
 * The disableExtendedBootstrapProperty method restores the default behavior.
 */
class Newick :
  public AbstractITree,
  public AbstractOTree,
  public AbstractIMultiTree,
  public AbstractOMultiTree,
  public AbstractIPhyloTree,
  public AbstractOPhyloTree,
  public AbstractIMultiPhyloTree,
  public AbstractOMultiPhyloTree
{
protected:
  bool allowComments_;
  bool writeId_;
  bool useBootstrap_;
  std::string bootstrapPropertyName_;
  bool verbose_;

public:
  /**
   * @brief Build a new Newick reader/writer.
   *
   * Some newick format allow comments between hooks ('[' ']').
   *
   * @param allowComments Tell if comments between [] are allowed in file.
   * @param writeId       If true, nodes ids will be written in place of bootstrap values.
   * @param verbose       If some info should be displayed, such as progress bar etc.
   */
  Newick(bool allowComments = false, bool writeId = false, bool verbose = false) :
    allowComments_(allowComments),
    writeId_(writeId),
    useBootstrap_(true),
    bootstrapPropertyName_("bootstrap"),
    verbose_(verbose) {}

  virtual ~Newick() {}

public:
  void enableExtendedBootstrapProperty(const std::string& propertyName)
  {
    useBootstrap_ = false;
    bootstrapPropertyName_ = propertyName;
  }
  void disableExtendedBootstrapProperty()
  {
    useBootstrap_ = true;
    bootstrapPropertyName_ = "bootstrap";
  }

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

private:
  std::shared_ptr<PhyloNode> parenthesisToNode(PhyloTree& tree, std::shared_ptr<PhyloNode> father, const std::string& description, unsigned int& nodeCounter, bool bootstrap, const std::string& propertyName, bool withId, bool verbose) const;

public:
  PhyloTree* parenthesisToPhyloTree(const std::string& description, bool bootstrap = false, const std::string& propertyName = "", bool withId = false, bool verbose = false) const;


/** @} */

  /**
   * @name The OTree interface
   *
   * @{
   */

public:
  void writeTree(const Tree& tree, const std::string& path, bool overwrite = true) const
  {
    AbstractOTree::writeTree(tree, path, overwrite);
  }
  void writePhyloTree(const PhyloTree& tree, const std::string& path, bool overwrite = true) const
  {
    AbstractOPhyloTree::writePhyloTree(tree, path, overwrite);
  }

  void writeTree(const Tree& tree, std::ostream& out) const
  {
    write_(tree, out);
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

  IOTree::Element getElement(const std::string& elt) const;

/**
 * @brief Get the Newick description of a subtree.
 *
 * @param tree The tree to convert.
 * @param node The top of the subtree to convert.
 * @param writeId Tells if node ids must be printed.
 *                This will overwrite bootstrap values if there are ones.
 *                Leaves id will be added to the leave names, separated by a '_' character.
 * @return A string in the parenthesis format.
 */

  std::string nodeToParenthesis(const PhyloTree& tree, std::shared_ptr<PhyloNode> node, bool writeId = false) const;

/* @brief Get the parenthesis description of a subtree.
 *
 * @param tree The tree
 * @param node The node defining the subtree.
 * @param bootstrap Tell is bootstrap values must be writen.
 * If so, the content of the property with name "bootstrap" will be written as bootstrap value.
 * The property should be a Number<double> object.
 * Otherwise, the content of the property with name 'propertyName' will be written.
 * In this later case, the property should be a String object.
 * @param propertyName The name of the property to use. Only used if bootstrap = false.
 * @return A string in the parenthesis format.
 */

  std::string nodeToParenthesis(const PhyloTree& tree, std::shared_ptr<PhyloNode> node, bool bootstrap, const std::string& propertyName) const;

/**
 * @brief Get the parenthesis description of a tree.
 *
 * @param tree The tree to convert.
 * @param writeId Tells if node ids must be printed.
 *                This will overwrite bootstrap values if there are ones.
 *                Leaves id will be added to the leave names, separated by a '_' character.
 * @return A string in the parenthesis format.
 */

  std::string treeToParenthesis(const PhyloTree& tree, bool writeId = false) const;

/**
 * @brief Get the parenthesis description of a tree.
 *
 * @param tree The tree to convert.
 * @param bootstrap Tell is bootstrap values must be writen.
 * If so, the content of the property with name "bootstrap" will be written as bootstrap value.
 * The property should be a Number<double> object.
 * Otherwise, the content of the property with name 'propertyName' will be written.
 * In this later case, the property should be a String object.
 * @param propertyName The name of the property to use. Only used if bootstrap = false.
 * @return A string in the parenthesis format.
 */
  std::string treeToParenthesis(const PhyloTree& tree, bool bootstrap, const std::string& propertyName) const;
};
} // end of namespace bpp.
#endif // BPP_PHYL_IO_NEWICK_H
