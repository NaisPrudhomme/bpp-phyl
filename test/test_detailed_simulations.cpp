//
// File: test_detailed_simulations.cpp
// Created by: Julien Dutheil
// Created on: Fri Nov 12 15:41 2010
//

/*
Copyright or © or Copr. Bio++ Development Team, (November 17, 2004)

This software is a computer program whose purpose is to provide classes
for numerical calculus. This file is part of the Bio++ project.

This software is governed by the CeCILL  license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL license and that you accept its terms.
*/

#include <Bpp/Numeric/Prob/ConstantDistribution.h>
#include <Bpp/Numeric/Matrix/Matrix.h>
#include <Bpp/Numeric/Matrix/MatrixTools.h>
#include <Bpp/Seq/Alphabet/DNA.h>

#include <Bpp/Phyl/Io/Newick.h>
#include <Bpp/Phyl/Model/Nucleotide/GTR.h>

#include <Bpp/Phyl/Simulation/SimpleSubstitutionProcessSequenceSimulator.h>

#include <Bpp/Phyl/Likelihood/RateAcrossSitesSubstitutionProcess.h>

#include <iostream>

using namespace bpp;
using namespace std;

int main() {
  Newick reader;
  auto phyloTree = std::shared_ptr<PhyloTree>(reader.parenthesisToPhyloTree("((A:0.001, B:0.002):0.003,C:0.01,D:0.1);"));

  auto ids = phyloTree->getAllEdgesIndexes();
  
  //-------------

  NucleicAlphabet* alphabet = new DNA();
  std::shared_ptr<GTR> model(new GTR(alphabet, 1, 0.2, 0.3, 0.4, 0.4, 0.1, 0.35, 0.35, 0.2));
  //DiscreteDistribution* rdist = new GammaDiscreteDistribution(4, 0.4, 0.4);
  auto rdist = std::make_shared<ConstantDistribution>(1.0);

  RateAcrossSitesSubstitutionProcess process(model, rdist, phyloTree);

  process.setPhyloTree(*phyloTree);
  
  SimpleSubstitutionProcessSiteSimulator simulatorS(process);

  unsigned int n = 200000;
  map<uint, RowMatrix<unsigned int> > counts;
  for (size_t j = 0; j < ids.size(); ++j)
    counts[ids[j]].resize(4, 4);
  for (unsigned int i = 0; i < n; ++i) {
    auto result = simulatorS.dSimulateSite();
    for (size_t j = 0; j < ids.size(); ++j) {
      result->getMutationPath(ids[j]).getEventCounts(counts[ids[j]]);
    }
  }

  const auto& Q=model->getGenerator();
  
  map<uint, RowMatrix<double> >freqs;
  map<uint, double> sums;
  for (size_t k = 0; k < ids.size(); ++k) {
    RowMatrix<double>* freqsP = &freqs[ids[k]];
    RowMatrix<unsigned int>* countsP = &counts[ids[k]];
    freqsP->resize(4, 4);
    for (unsigned int i = 0; i < 4; ++i)
      for (unsigned int j = 0; j < 4; ++j)
        (*freqsP)(i, j) = static_cast<double>((*countsP)(i, j)) / (static_cast<double>(n));
  
    //For now we simply compare the total number of substitutions:
    sums[ids[k]] = MatrixTools::sumElements(*freqsP);
  
    cout << "Br" << ids[k] << " BrLen = " << phyloTree->getEdge(ids[k])->getLength() << " counts = " << sums[ids[k]] << endl;
    MatrixTools::print(*freqsP);

    cout << " Comparison with generator (more or less same non-diagonal values on each line):" << endl;
    RowMatrix<double> comp(4,4);
  
    for (unsigned int i = 0; i < 4; ++i)
      for (unsigned int j = 0; j < 4; ++j)
        comp(i, j) = (*freqsP)(i,j)/Q(i,j);
  
    MatrixTools::print(comp);
  }

  
  for (size_t k = 0; k < ids.size(); ++k) {
    if (abs(sums[ids[k]] - phyloTree->getEdge(ids[k])->getLength()) > 0.01) {
      delete alphabet;
      return 1;
    }
  }
  
  //-------------
  delete alphabet;

  //return (abs(obs - 0.001) < 0.001 ? 0 : 1);
  return 0;
}
