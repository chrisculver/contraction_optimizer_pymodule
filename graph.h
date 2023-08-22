#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <set>
#include <vector>

typedef std::pair<unsigned int, unsigned int> iTup;
typedef std::map<iTup, std::set<iTup>> contrType;

class ContractionCost {

  private:
    std::vector<unsigned int> store;
    static unsigned int nDil;

  public:
    ContractionCost() : store(8,0) {}

    ContractionCost& operator+=(const ContractionCost& rhs);
    ContractionCost& operator-=(const ContractionCost& rhs);
    ContractionCost& operator+=(const unsigned int& rhs);
    bool operator<(const ContractionCost& rhs) const;

    std::vector<unsigned int> getCostArray() const { return store; }

    static void setDilutionRange(unsigned int _nDil) { nDil = _nDil; }
    static unsigned int getDilutionRange() { return nDil; }

  private:
    void borrow(unsigned int pos);
};


class Graph
{

  private:
    std::vector<unsigned int> icode;

  public:
    Graph(const std::vector<std::pair<iTup, std::set<iTup>>>& contrListPython);
    Graph(const std::map<iTup, std::set<iTup>>& contrList);
    Graph(const std::vector<unsigned int>& _icode) : icode(_icode) {}

    const std::vector<unsigned int>& __hash__() const { return icode; }
    bool operator==(const Graph& rhs) const { return icode == rhs.icode; }
    bool operator<(const Graph& rhs) const { return icode < rhs.icode; }

    unsigned int isSubexpression(const unsigned int aStep,
				 const std::pair<unsigned int, unsigned int>& tensPair) const;
    std::pair<unsigned int, std::vector<unsigned int>> singleTermOpt() const;

    std::map<iTup, std::set<iTup>> getContractionList() const;
    unsigned int getNumInds(unsigned int tensId) const;
    std::vector<unsigned int> getAllNumInds() const;
#ifdef SAFETY_FLAG
    std::set<unsigned int> getTensorIDSet() const;
#endif

    ContractionCost getRemainingCost() const;
    void getProfit(const unsigned int replStep, ContractionCost& result) const;

    bool replaceSubexpression(unsigned int graphStep);
    void doReplacement(unsigned int replStep) const;
      // given a mapping oldTensId -> newTensId, relabel tensor IDs
    void relabelTensors(const std::vector<unsigned int>& indMap);

    static std::set<iTup> decodeElement(unsigned int aC);

    std::map<iTup, std::set<iTup>> decode_test();

  private:
    unsigned int canonicalize(unsigned int aC) const;
    unsigned int reverse(unsigned int aC) const;
    void encode(const std::map<iTup, std::set<iTup>>& contrList);
    void decode(std::map<iTup, std::set<iTup>>& contrList) const;

      // cache infrastructure
    static std::map<std::pair<Graph, unsigned int>, std::pair<Graph, bool>> replCache;
    static unsigned int replCacheHit, replCacheMiss;
    static std::map<Graph, ContractionCost> costCache;
    static unsigned int costCacheHit, costCacheMiss;
};


class GraphFactory {

  private:
    std::map<iTup, std::set<iTup>> contrList;

  public:

    void addContraction(const iTup& tensPair, const std::set<iTup>& newC) {
      auto mIt = contrList.emplace(tensPair, std::set<iTup>()).first;
      mIt->second.insert(newC.begin(), newC.end());
    }

    void addContraction(const iTup& tensPair, const iTup& newC) {
      auto mIt = contrList.emplace(tensPair, std::set<iTup>()).first;
      mIt->second.insert(newC);
    }

    void addContraction(const unsigned int had1, const unsigned int had2,
		    	const unsigned int q1, const unsigned int q2) {
      if (had1 > had2)
	addContraction(std::make_pair(had2, had1), std::make_pair(q2, q1));
      else if (had1 < had2)
	addContraction(std::make_pair(had1, had2), std::make_pair(q1, q2));
      else
	addContraction(
	    std::make_pair(had1, had2),
	    (q1<q2)?std::make_pair(q1, q2) : std::make_pair(q2, q1));
    }

    Graph getGraph() {
      removeInternalLoops();
      return Graph(contrList);
    }

    void reset() {
      contrList.clear();
    }

     // remove internal loops from graph and relabel remaining indices
     // accordingly.
    void removeInternalLoops() {
      std::set<unsigned int> unique_tensors;

      for (auto aPair : contrList) {
	unique_tensors.insert(aPair.first.first);
	unique_tensors.insert(aPair.first.second);
      }

      for (auto iTens : unique_tensors) {
	auto intLoopIt = contrList.find(std::make_pair(iTens, iTens));
	  // if there is an internal loop on this index
	if (intLoopIt != contrList.end()) {
	    // collect indices to remove
	  std::set<unsigned int> idx_remove;
	  for (auto cPair : intLoopIt->second) {
	    idx_remove.insert(cPair.first);
	    idx_remove.insert(cPair.second);
	  }

	   // remove internal loop from graph
	  contrList.erase(intLoopIt);

	   // rename remaining indices on that tensor
	  for (auto& aPair : contrList) {
	     // if the internal loop tensor is the first one in tensPair
	    if (aPair.first.first == iTens) {
	      std::set<iTup> newContrSet;
	      for (auto& aContr : aPair.second) {
		unsigned int nRem = 0;
		for (auto aRemInd : idx_remove)
		  if (aRemInd < aContr.first) ++nRem;
		newContrSet.insert(std::make_pair(aContr.first-nRem, aContr.second));
	      }
	      aPair.second = newContrSet;
	    }
	     // if the internal loop tensor is the second one in tensPair
	    else if (aPair.first.second == iTens) {
	      std::set<iTup> newContrSet;
	      for (auto& aContr : aPair.second) {
		unsigned int nRem = 0;
		for (auto aRemInd : idx_remove)
		  if (aRemInd < aContr.second) ++nRem;
		newContrSet.insert(std::make_pair(aContr.first, aContr.second-nRem));
	      }
	      aPair.second = newContrSet;
	    }
	  }
	}
      }
    }

};


// ***************************************************************
#endif  
