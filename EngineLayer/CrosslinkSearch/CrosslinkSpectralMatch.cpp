﻿#include "CrosslinkSpectralMatch.h"
#include "../Ms2ScanWithSpecificMass.h"
#include "Crosslinker.h"
#include "Sort.h"

#include <numeric>
#include <algorithm>
#include <sstream>
using namespace Proteomics::Fragmentation;
using namespace Proteomics::ProteolyticDigestion;
namespace EngineLayer
{
    namespace CrosslinkSearch
    {
        
        CrosslinkSpectralMatch::CrosslinkSpectralMatch(PeptideWithSetModifications *theBestPeptide, int notch, double score, int scanIndex,
                                                       Ms2ScanWithSpecificMass *scan, DigestionParams *digestionParams,
                                                       std::vector<MatchedFragmentIon*> &matchedFragmentIons) :
            PeptideSpectralMatch(theBestPeptide, notch, score, scanIndex, scan, digestionParams, matchedFragmentIons)
        {
            this->setXLTotalScore(score);
        }
        
        CrosslinkSpectralMatch *CrosslinkSpectralMatch::getBetaPeptide() const
        {
            return privateBetaPeptide;
        }
        
        void CrosslinkSpectralMatch::setBetaPeptide(CrosslinkSpectralMatch *value)
        {
            privateBetaPeptide = value;
        }
        
        std::vector<int> CrosslinkSpectralMatch::getLinkPositions() const
        {
            return privateLinkPositions;
        }
        
        void CrosslinkSpectralMatch::setLinkPositions(const std::vector<int> &value)
        {
            privateLinkPositions = value;
        }
        
        double CrosslinkSpectralMatch::getDeltaScore() const
        {
            return privateDeltaScore;
        }
        
        void CrosslinkSpectralMatch::setDeltaScore(double value)
        {
            privateDeltaScore = value;
        }
        
        double CrosslinkSpectralMatch::getXLTotalScore() const
        {
            return privateXLTotalScore;
        }
        
        void CrosslinkSpectralMatch::setXLTotalScore(double value)
        {
            privateXLTotalScore = value;
        }
        
        int CrosslinkSpectralMatch::getXlProteinPos() const
        {
            return privateXlProteinPos;
        }
        
        void CrosslinkSpectralMatch::setXlProteinPos(int value)
        {
            privateXlProteinPos = value;
        }
        
        std::vector<int> CrosslinkSpectralMatch::getXlRank() const
        {
            return privateXlRank;
        }
        
        void CrosslinkSpectralMatch::setXlRank(const std::vector<int> &value)
        {
            privateXlRank = value;
        }
        
        std::string CrosslinkSpectralMatch::getParentIonExist() const
        {
            return privateParentIonExist;
        }
        
        void CrosslinkSpectralMatch::setParentIonExist(const std::string &value)
        {
            privateParentIonExist = value;
        }
        
        int CrosslinkSpectralMatch::getParentIonExistNum() const
        {
            return privateParentIonExistNum;
        }
        
        void CrosslinkSpectralMatch::setParentIonExistNum(int value)
        {
            privateParentIonExistNum = value;
        }
        
        std::vector<int> CrosslinkSpectralMatch::getParentIonMaxIntensityRanks() const
        {
            return privateParentIonMaxIntensityRanks;
        }
        
        void CrosslinkSpectralMatch::setParentIonMaxIntensityRanks(const std::vector<int> &value)
        {
            privateParentIonMaxIntensityRanks = value;
        }
        
        PsmCrossType CrosslinkSpectralMatch::getCrossType() const
        {
            return privateCrossType;
        }
        
        void CrosslinkSpectralMatch::setCrossType(PsmCrossType value)
        {
            privateCrossType = value;
        }
        
        std::vector<int> CrosslinkSpectralMatch::GetPossibleCrosslinkerModSites(std::vector<char> &crosslinkerModSites, PeptideWithSetModifications *peptide)
        {
            std::vector<int> possibleXlPositions;
#ifdef ORIG
            bool wildcard = crosslinkerModSites.Any([&] (std::any p)   {
                    return p == 'X';
                });
#endif
            bool wildcard =false;
            for ( char p: crosslinkerModSites ){
                if (  p == 'X' ) {
                    wildcard = true;
                    break;
                }                      
            }
            
            for (int r = 0; r < (int) peptide->getBaseSequence().size(); r++)
            {
                //if (crosslinkerModSites.Contains(peptide->getBaseSequence()[r]) || wildcard)
                if ( std::find(crosslinkerModSites.begin(), crosslinkerModSites.end(), peptide->getBaseSequence()[r]) !=
                     crosslinkerModSites.end() || wildcard ) 
                {
                    possibleXlPositions.push_back(r + 1);
                }
            }
            
            return possibleXlPositions;
        }
        
        std::vector<int> CrosslinkSpectralMatch::GenerateIntensityRanks(std::vector<double> &experimental_intensities)
        {
            auto y = experimental_intensities;
#ifdef ORIG
            auto x = Enumerable::Range(1, y.size()).OrderBy([&] (std::any p) {
                    return p;
                })->ToArray();
            Array::Sort(y, x);
#endif
            std::vector<int> x(y.size());
            std::iota(x.begin(), x.end(), 1);
            Sort::SortPairs(y, x, y.size() );
            
#ifdef ORIG
            auto experimental_intensities_rank = Enumerable::Range(1, y.size()).OrderByDescending([&] (std::any p)  {
                    return p;
                })->ToArray();
            Array::Sort(x, experimental_intensities_rank);
#endif
            std::vector<int> experimental_intensities_rank(y.size() );
            int n = y.size();
            std::generate(experimental_intensities_rank.begin(), experimental_intensities_rank.end(), [&] () {return n--;});
            Sort::SortPairs(x, experimental_intensities_rank, x.size() );
            
            return experimental_intensities_rank;
        }
        
        std::string CrosslinkSpectralMatch::GetTabSepHeaderCross()
        {
            auto sb = new StringBuilder();
            sb->append("File Name" + StringHelper::toString('\t'));
            sb->append("Scan Number" + StringHelper::toString('\t'));
            sb->append("Precursor Scan Number" + StringHelper::toString('\t'));
            sb->append("Precursor MZ" + StringHelper::toString('\t'));
            sb->append("Precursor Charge" + StringHelper::toString('\t'));
            sb->append("Precursor Mass" + StringHelper::toString('\t'));
            sb->append("Cross Type" + StringHelper::toString('\t'));
            sb->append(std::string("Link Residues") + "\t");
            
            sb->append("Peptide" + StringHelper::toString('\t'));
            sb->append("Protein Accession" + StringHelper::toString('\t'));
            sb->append("Protein Link Site" + StringHelper::toString('\t'));
            sb->append("Base Sequence" + StringHelper::toString('\t'));
            sb->append("Full Sequence" + StringHelper::toString('\t'));
            sb->append("Peptide Monoisotopic Mass" + StringHelper::toString('\t'));
            sb->append("Score" + StringHelper::toString('\t'));
            sb->append("Rank" + StringHelper::toString('\t'));
            
            sb->append("Matched Ion Series" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass-To-Charge Ratios" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass Diff (Da)" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass Diff (Ppm)" + StringHelper::toString('\t'));
            sb->append("Matched Ion Intensities" + StringHelper::toString('\t'));
            sb->append("Matched Ion Counts" + StringHelper::toString('\t'));
            
            sb->append("Beta Peptide" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Protein Accession" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Protein LinkSite" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Base Sequence" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Full Sequence" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Theoretical Mass" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Score" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Rank" + StringHelper::toString('\t'));
            
            sb->append("Beta Peptide Matched Ion Series" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Matched Ion Mass-To-Charge Ratios" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Matched Ion Mass Diff (Da)" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Matched Ion Mass Diff (Ppm)" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Matched Ion Intensities" + StringHelper::toString('\t'));
            sb->append("Beta Peptide Matched Ion Counts" + StringHelper::toString('\t'));
            
            sb->append("Summary" + StringHelper::toString('\t'));
            sb->append("XL Total Score" + StringHelper::toString('\t'));
            sb->append("Mass Diff (Da)" + StringHelper::toString('\t'));
            sb->append("Parent Ions" + StringHelper::toString('\t'));
            sb->append("ParentIonsNum" + StringHelper::toString('\t'));
            sb->append("ParentIonMaxIntensityRank" + StringHelper::toString('\t'));
            sb->append("Decoy/Contaminant/Target" + StringHelper::toString('\t'));
            sb->append("QValue" + StringHelper::toString('\t'));
            
            
            std::string s = sb->toString();
            delete sb;
            return s;
        }
        
        std::string CrosslinkSpectralMatch::GetTabSepHeaderSingle()
        {
            auto sb = new StringBuilder();
            sb->append("File Name" + StringHelper::toString('\t'));
            sb->append("Scan Number" + StringHelper::toString('\t'));
            sb->append("Precursor Scan Number" + StringHelper::toString('\t'));
            sb->append("Precursor MZ" + StringHelper::toString('\t'));
            sb->append("Precursor Charge" + StringHelper::toString('\t'));
            sb->append("Precursor Mass" + StringHelper::toString('\t'));
            sb->append("Cross Type" + StringHelper::toString('\t'));
            sb->append(std::string("Link Residues") + "\t");
            
            sb->append("Peptide" + StringHelper::toString('\t'));
            sb->append("Protein Accession" + StringHelper::toString('\t'));
            sb->append("Protein Link Site" + StringHelper::toString('\t'));
            sb->append("Base Sequence" + StringHelper::toString('\t'));
            sb->append("Full Sequence" + StringHelper::toString('\t'));
            sb->append("Peptide Monoisotopic Mass" + StringHelper::toString('\t'));
            sb->append("Score" + StringHelper::toString('\t'));
            sb->append("Rank" + StringHelper::toString('\t'));
            
            sb->append("Matched Ion Series" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass-To-Charge Ratios" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass Diff (Da)" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass Diff (Ppm)" + StringHelper::toString('\t'));
            sb->append("Matched Ion Intensities" + StringHelper::toString('\t'));
            sb->append("Matched Ion Counts" + StringHelper::toString('\t'));
            sb->append("Decoy/Contaminant/Target" + StringHelper::toString('\t'));
            sb->append("QValue" + StringHelper::toString('\t'));
            
            std::string s = sb->toString();
            delete sb;
            return s;
        }
        
        std::string CrosslinkSpectralMatch::GetTabSepHeaderGlyco()
        {
            auto sb = new StringBuilder();
            sb->append("File Name" + StringHelper::toString('\t'));
            sb->append("Scan Number" + StringHelper::toString('\t'));
            sb->append("Precursor Scan Number" + StringHelper::toString('\t'));
            sb->append("Precursor MZ" + StringHelper::toString('\t'));
            sb->append("Precursor Charge" + StringHelper::toString('\t'));
            sb->append("Precursor Mass" + StringHelper::toString('\t'));
            sb->append("Cross Type" + StringHelper::toString('\t'));
            sb->append(std::string("Link Residues") + "\t");
            
            sb->append("Peptide" + StringHelper::toString('\t'));
            sb->append("Protein Accession" + StringHelper::toString('\t'));
            sb->append("Protein Link Site" + StringHelper::toString('\t'));
            sb->append("Base Sequence" + StringHelper::toString('\t'));
            sb->append("Full Sequence" + StringHelper::toString('\t'));
            sb->append("Peptide Monoisotopic Mass" + StringHelper::toString('\t'));
            sb->append("Score" + StringHelper::toString('\t'));
            sb->append("Rank" + StringHelper::toString('\t'));
            
            sb->append("Matched Ion Series" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass-To-Charge Ratios" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass Diff (Da)" + StringHelper::toString('\t'));
            sb->append("Matched Ion Mass Diff (Ppm)" + StringHelper::toString('\t'));
            sb->append("Matched Ion Intensities" + StringHelper::toString('\t'));
            sb->append("Matched Ion Counts" + StringHelper::toString('\t'));
            
            sb->append("Decoy/Contaminant/Target" + StringHelper::toString('\t'));
            sb->append("QValue" + StringHelper::toString('\t'));
            
            sb->append("GlyID" + StringHelper::toString('\t'));
            sb->append("GlyMass" + StringHelper::toString('\t'));
            sb->append("GlyStruct(H,N,A,G,F)" + StringHelper::toString('\t'));
            
            std::string s = sb->toString();
            delete sb;
            return s;
        }
        
        std::string CrosslinkSpectralMatch::ToString()
        {
            std::string position = "";
            switch (getCrossType())
            {
                case PsmCrossType::Single:
                    break;
                    
                case PsmCrossType::Loop:
                    position = "(" + std::to_string(getLinkPositions()[0]) + "-" + std::to_string(getLinkPositions()[1]) + ")";
                    break;
                    
                default:
                    position = "(" + std::to_string(getLinkPositions()[0]) + ")";
                    break;
            }
            
            auto sb = new StringBuilder();
            sb->append(getFullFilePath() + "\t");
            sb->append(std::to_string(getScanNumber()) + "\t");
            if ( getPrecursorScanNumber().has_value() ) {
                sb->append(std::to_string(getPrecursorScanNumber().value()) + "\t");
            }
            else {
                std::string s = "-\t";
                sb->append(s);
            }
            sb->append(std::to_string(getScanPrecursorMonoisotopicPeakMz()) + "\t");
            sb->append(std::to_string(getScanPrecursorCharge()) + "\t");
            sb->append(std::to_string(getScanPrecursorMass()) + "\t");
            auto crosslinktype = getCrossType();
            sb->append(PsmCrossTypeToString(crosslinktype) + "\t");
            
            if (getLinkPositions().size() > 0)
            {
                if (getCrossType() == PsmCrossType::Loop)
                {
                    std::stringstream ss;
                    ss << getBaseSequence()[getLinkPositions()[0] - 1] << ';' <<
                        getBaseSequence()[getLinkPositions()[1] - 1] << '\t';
                    sb->append(ss.str() );
                }
                else if (getCrossType() == PsmCrossType::Inter || getCrossType() == PsmCrossType::Intra)
                {
                    std::stringstream ss;
                    ss << getBaseSequence()[getLinkPositions()[0] - 1] << ';' <<
                        getBetaPeptide()->getBaseSequence()[getBetaPeptide()->getLinkPositions()[0] - 1] << '\t';
                    sb->append(ss.str());
                }
                else
                {
                    // deadend
                    std::stringstream ss;
                    ss << getBaseSequence()[getLinkPositions()[0] - 1] << "\t";
                    sb->append(ss.str() );
                }
            }
            else
            {
                sb->append("\t");
            }
            
            sb->append("\t");
            sb->append(getProteinAccession() + "\t");
            sb->append(std::to_string(getXlProteinPos()) + "\t");
            sb->append(getBaseSequence() + "\t");
            sb->append(getFullSequence() + position + "\t");
            sb->append((getPeptideMonisotopicMass().has_value() ? std::to_string(getPeptideMonisotopicMass().value()) : "---"));
            sb->append("\t");
            sb->append(std::to_string(getScore()) + "\t");
            sb->append(std::to_string(getXlRank()[0]) + "\t");
            
            for (auto mid : MatchedIonDataDictionary(this))
            {
                sb->append(std::get<1>(mid));
                sb->append("\t");
            }
            
            if (getBetaPeptide() != nullptr)
            {
                sb->append("\t");
                sb->append(getBetaPeptide()->getProteinAccession() + "\t");
                sb->append(std::to_string(getBetaPeptide()->getXlProteinPos()) + "\t");
                sb->append(getBetaPeptide()->getBaseSequence() + "\t");
                sb->append(getBetaPeptide()->getFullSequence() + "(" + std::to_string(getBetaPeptide()->getLinkPositions()[0]) +
                           ")" + "\t");
                sb->append(std::to_string(getBetaPeptide()->getPeptideMonisotopicMass().value()) + "\t");
                sb->append(std::to_string(getBetaPeptide()->getScore()) + "\t");
                sb->append(std::to_string(getXlRank()[1]) + "\t");
                
                for (auto betamid : MatchedIonDataDictionary(this->getBetaPeptide()))
                {
                    sb->append(std::get<1>(betamid));
                    sb->append("\t");
                }
                
                sb->append("\t");
                sb->append(std::to_string(getXLTotalScore()) + "\t");
                
                // mass of crosslinker
                sb->append(((getPeptideMonisotopicMass().has_value()) ? std::to_string(getScanPrecursorMass() -
                            getBetaPeptide()->getPeptideMonisotopicMass().value() - getPeptideMonisotopicMass().value()) : "---"));
                sb->append("\t");
                
#ifdef ORIG
                int alphaNumParentIons = getMatchedFragmentIons().size()([&] (std::any p)    {
                        delete sb;
                        return p::NeutralTheoreticalProduct->ProductType == ProductType::M;
                    });
#endif
                int alphaNumParentIons = 0;
                for ( auto p : getMatchedFragmentIons() ) {
                    if ( p->NeutralTheoreticalProduct->productType == ProductType::M ) {
                        alphaNumParentIons++;
                    }
                }
#ifdef ORIG
                int betaNumParentIons = getBetaPeptide()->getMatchedFragmentIons().size()([&] (std::any p) {
                        delete sb;
                        return p::NeutralTheoreticalProduct->ProductType == ProductType::M;
                    });
#endif
                int betaNumParentIons = 0;
                for ( auto p :  getBetaPeptide()->getMatchedFragmentIons() ) {
                    if ( p->NeutralTheoreticalProduct->productType == ProductType::M ) {
                        alphaNumParentIons++;
                    }
                }

                
                sb->append(std::to_string(alphaNumParentIons) + ";" + std::to_string(betaNumParentIons) + "\t");
                sb->append(std::to_string(alphaNumParentIons) + std::to_string(betaNumParentIons) + "\t");
                sb->append(((getParentIonMaxIntensityRanks().size() > 0) && (!getParentIonMaxIntensityRanks().empty()) ?
                   std::to_string(*std::min_element(getParentIonMaxIntensityRanks().begin(), getParentIonMaxIntensityRanks().end()))
                            : "-"));
                sb->append("\t");                            
            }
            
            if (getBetaPeptide() == nullptr)
            {
                sb->append((getIsDecoy()) ? "D" : (getIsContaminant()) ? "C" : "T");
                sb->append("\t");
            }
            else
            {
                sb->append((getIsDecoy() || getBetaPeptide()->getIsDecoy()) ? "D" :
                           (getIsContaminant() || getBetaPeptide()->getIsContaminant()) ? "C" : "T");
                sb->append("\t");
            }
            
            sb->append(std::to_string(getFdrInfo()->getQValue()));
            sb->append("\t");
            
            
            std::string s= sb->toString();
            delete sb;
            return s;
        }
        
        std::vector<std::tuple<std::string, std::string>> CrosslinkSpectralMatch::MatchedIonDataDictionary(PeptideSpectralMatch *psm)
        {
            std::vector<std::tuple<std::string, std::string>> s;
            AddMatchedIonsData(s, psm);
            return s;
        }
    }
}

