//
// File: SitePatterns.cpp
// Authors:
//   Julien Dutheil
// Created: 2005-11-29 15:37:00
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


#include "SitePatterns.h"

// From the SeqLib library:
#include <Bpp/Seq/SiteTools.h>
#include <Bpp/Seq/Container/AlignedValuesContainer.h>
#include <Bpp/Seq/Container/VectorSiteContainer.h>
#include <Bpp/Seq/Container/VectorProbabilisticSiteContainer.h>

using namespace bpp;
using namespace std;

/******************************************************************************/

SitePatterns::SitePatterns(const AlignedValuesContainer* sequences , bool own) :
  names_(sequences->getSequencesNames()),
  sites_(),
  weights_(),
  indices_(),
  alpha_(sequences->getAlphabet()),
  own_(own)
{
  init_(sequences, names_);
}
  
SitePatterns::SitePatterns(const AlignedValuesContainer* sequences , std::vector<std::string> names) :
  names_(),
  sites_(),
  weights_(),
  indices_(),
  alpha_(sequences->getAlphabet()),
  own_(false)
{
  names_=sequences->getSequencesNames();
  if (names.size()!=0)
    names_=VectorTools::vectorIntersection(names_, names);
  init_(sequences, names_);
}

void SitePatterns::init_(const AlignedValuesContainer* sequences , std::vector<std::string> names)
{
  // positions of the names in sequences list
  std::vector<size_t> posseq;
  for (const auto& n:names)
    posseq.push_back(sequences->getSequencePosition(n));

  int nbSeq = (int)sequences->getNumberOfSequences();
  std::vector<size_t> posnseq;

  std::stable_sort(posseq.begin(), posseq.end());
  for (int i = nbSeq-1; i >= 0; i--)
  {
    if (!std::binary_search(posseq.begin(), posseq.end(), i))
      posnseq.push_back((size_t)i);
  }

  own_ = own_ || (posnseq.size()!=0); // New ownership only if different Sites, ie not all sequences

  // Then build Sortable sites with correct sequences
  size_t nbSites = sequences->getNumberOfSites();

  vector<SortableSite> ss(nbSites);
  for (size_t i = 0; i < nbSites; i++)
  {
    const CruxSymbolListSite* currentSite;
    if (own_)
    {      
      CruxSymbolListSite* currentSitetmp = sequences->getSymbolListSite(i).clone();
      for (auto pos:posnseq)
        currentSitetmp->deleteElement(pos);
      currentSite = currentSitetmp;
    }
    else
      currentSite = &sequences->getSymbolListSite(i);

    SortableSite* ssi = &ss[i];
    ssi->siteS = currentSite->toString();
    ssi->siteP = currentSite;
    ssi->originalPosition = i;
  }

  if (nbSites > 0)
  {
    // Quick sort according to site contents:
    sort(ss.begin(), ss.end());

    // Now build patterns:

    SortableSite* ss0 = &ss[0];
    const CruxSymbolListSite* previousSite = ss0->siteP;
    indices_.resize(Eigen::Index(nbSites));
    indices_[Eigen::Index(ss0->originalPosition)] = 0;
    sites_.push_back(previousSite);
    weights_.push_back(1);

    size_t currentPos = 0;
    for (size_t i = 1; i < nbSites; i++)
    {
      SortableSite* ssi = &ss[i];
      const CruxSymbolListSite* currentSite = ssi->siteP;

      bool siteExists = SymbolListTools::areSymbolListsIdentical(*currentSite, *previousSite);
      if (siteExists)
      {
        weights_[currentPos]++;
        if (own_)
          delete currentSite;
      }
      else
      {
        sites_.push_back(currentSite);
        weights_.push_back(1);
        currentPos++;
        previousSite = currentSite;
      }
      indices_[Eigen::Index(ssi->originalPosition)] = currentPos;
    }
  }
}

/******************************************************************************/

std::shared_ptr<AlignedValuesContainer> SitePatterns::getSites() const
{
  if (sites_.size() == 0)
    throw Exception("SitePatterns::getSites : empty set.");

  AlignedValuesContainer* sites;

  if (dynamic_cast<const Site*>(sites_[0]))
    sites = new VectorSiteContainer(sites_, alpha_);
  else
    sites = new VectorProbabilisticSiteContainer(sites_, alpha_);

  sites->setSequencesNames(names_, false);

  auto ret= std::shared_ptr<AlignedValuesContainer>(sites);
  return ret;
}

/******************************************************************************/
