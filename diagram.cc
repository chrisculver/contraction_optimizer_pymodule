#include "diagram.h"

#include <algorithm>
#include <functional>


using namespace std;


  /*
   *
   * 	Diagram implementation
   *
   */


  Diagram::Diagram(const Graph& _graph,
      		   const std::vector<unsigned int>& _tensIdList) :
    	graph(_graph), tensIdList(_tensIdList) {

    _sortTensorList();
  }

  Diagram::Diagram(const Graph& _graph,
      		   const std::vector<unsigned int>& _tensIdList,
		   const std::vector<unsigned int>& _resultIdList) :
    	graph(_graph), tensIdList(_tensIdList), resultIdList(_resultIdList) {
	
    _sortTensorList();
  }

  Diagram::Diagram(const Diagram& rhs) :
    	graph(rhs.graph), tensIdList(rhs.tensIdList), resultIdList(rhs.resultIdList) {}

  void Diagram::_sortTensorList() {
    if (is_sorted(tensIdList.begin(), tensIdList.end())) return;

    vector<unsigned int> indexMap;
    for (unsigned int iI=0; iI < tensIdList.size(); ++iI)
      indexMap.push_back(iI);

     // indexMap[newIndex] = oldIndex
    sort(indexMap.begin(), indexMap.end(),
	[this](unsigned int i, unsigned int j) { return tensIdList[i] < tensIdList[j]; });

     // revMap[newIndex] = oldIndex
    auto revMap(indexMap);
    for (unsigned int iI=0; iI < tensIdList.size(); ++iI)
      revMap[indexMap[iI]] = iI;

     // relabel tensors in graph
    graph.relabelTensors(revMap);

     // store the sorted tensIdList -- the naive way
    auto oldList(tensIdList);
    tensIdList.clear();
    for (unsigned int iI=0; iI < oldList.size(); ++iI)
      tensIdList.push_back(oldList[indexMap[iI]]);

  }

  bool Diagram::isDone() const {
#ifdef SAFETY_FLAG
    if (tensIdList.empty() != graph.getContractionList().empty())
      throw("Inconsistent tensIdList and graph.");
#endif
    return tensIdList.empty();
  }



    // this implementation assumes that tensor IDs are sorted in tensIdList
  bool Diagram::_getLocalTensorIDs(const std::pair<unsigned int, unsigned int>& globTensPair, iTup& res) {
    bool found = false;
    unsigned int pos1, pos2;
    for (auto iP=0u; iP < tensIdList.size(); ++iP) {
      if (tensIdList[iP] > globTensPair.first)
	return false;
      if (tensIdList[iP] == globTensPair.first) {
	found = true;
	pos1 = iP;
	break;
      }
    }

    if (!found) return false;
    found = false;

    for (auto iP=0u; iP < tensIdList.size(); ++iP) {
      if (tensIdList[iP] > globTensPair.second)
	return false;
      if (tensIdList[iP] == globTensPair.second) {
	found = true;
	pos2 = iP;
	break;
      }
    }

    if (!found) return false;
    res = pair(pos1, pos2);
    return true;

      // check if the tensors in the proposed step occur in this diagram
    /*auto tIt1 = std::find(tensIdList.begin(), tensIdList.end(), globTensPair.first);
    if (tIt1 == tensIdList.end()) return false;
    auto tIt2 = std::find(tensIdList.begin(), tensIdList.end(), globTensPair.second);
    if (tIt2 == tensIdList.end()) return false;
    
    res = pair(std::distance(tensIdList.begin(), tIt1),
	       std::distance(tensIdList.begin(), tIt2));
    return true;*/
  }



  bool Diagram::replaceSubexpression(const unsigned int graphStep,
      				     const std::pair<unsigned int, unsigned int>& globTensPair,
				     const unsigned int newGlobTensID) {
      // check if the tensors in the proposed step occur in this diagram
    iTup tensPair;
    if (!_getLocalTensorIDs(globTensPair, tensPair)) return false;

      // check if the proposed contraction occurs in the diagram
    unsigned int replStep = graph.isSubexpression(graphStep, tensPair);

    if (replStep == 0)
      return false;

    bool subcDone = graph.replaceSubexpression(replStep);
    _reorgTensIdList(tensPair, newGlobTensID, subcDone);
    return true;
  }

  bool Diagram::getProfit(const unsigned int graphStep,
			  const iTup& globTensPair,
		   	  ContractionCost& result) {
      // check if the tensors in the proposed step occur in this diagram
    iTup tensPair;
    if (!_getLocalTensorIDs(globTensPair, tensPair)) return false;

      // check if the proposed contraction occurs in the diagram
    unsigned int replStep = graph.isSubexpression(graphStep, tensPair);

    if (replStep == 0)
      return false;

     // it's a subexpression, go get profit from Graph
    graph.getProfit(replStep, result);
    return true;
  }



  iTup Diagram::_getGlobalTensPair(unsigned int stepCode) const {
    unsigned int tId1 = (stepCode >> 4) & 0xf;
    unsigned int tId2 = stepCode & 0xf;

    return pair(*(tensIdList.begin()+tId1), *(tensIdList.begin()+tId2));
  }


  std::pair<unsigned int, std::vector<std::pair<unsigned int, iTup>>> Diagram::singleTermOpt() const {
    std::vector<unsigned int> stepList;
    unsigned int cost;
    std::tie(cost, stepList) = graph.singleTermOpt();

    std::vector<std::pair<unsigned int, iTup>> globStepList;
    for (auto mIt : stepList)
      globStepList.push_back(std::make_pair(mIt & 0xffffff00u, _getGlobalTensPair(mIt)));

    return std::make_pair(cost, globStepList);
  }


  void Diagram::_reorgTensIdList(const iTup& tensPair, unsigned int newGlobId, bool subcDone) {
      // remove the two tensors indexed by tensPair
    if (tensPair.first > tensPair.second) {
      tensIdList.erase(tensIdList.begin()+tensPair.first);
      tensIdList.erase(tensIdList.begin()+tensPair.second);
    }
    else {
      tensIdList.erase(tensIdList.begin()+tensPair.second);
      tensIdList.erase(tensIdList.begin()+tensPair.first);
    }

      // add new tensor ID either to result list or tensIdList
    if (subcDone)
      resultIdList.push_back(newGlobId);
    else
      tensIdList.push_back(newGlobId);
  }
