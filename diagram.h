#ifndef DIAGRAM_H
#define DIAGRAM_H

#include <map>
#include <set>
#include <vector>
#include <numeric>


#include "graph.h"



class Diagram {

  private:
    Graph graph;
    std::vector<unsigned int> tensIdList;
    std::vector<unsigned int> resultIdList;


  public:
    Diagram(const Graph& _graph, const std::vector<unsigned int>& _tensIdList);
    Diagram(const Graph& _graph, const std::vector<unsigned int>& _tensIdList, const std::vector<unsigned int>& _resultIdList);
    Diagram(const Diagram& rhs);

      // ContractionOptimizer::tune relies on this returning a reference
      // to find the maximum tensor ID
    const std::vector<unsigned int>& getRemainingTensors() const { return tensIdList; }
    std::vector<unsigned int> getResultIdList() const { return resultIdList; }
    Graph getGraph() const { return graph; }
    bool isDone() const;

    std::pair<unsigned int, std::vector<std::pair<unsigned int, iTup>>> singleTermOpt() const;
    bool replaceSubexpression(unsigned int graphStep,
      			      const std::pair<unsigned int, unsigned int>& globTensPair,
			      unsigned int newGlobTensID);
    bool getProfit(const unsigned int graphStep,
		   const iTup& globTensPair,
		   ContractionCost& result);

    bool operator==(const Diagram& rhs) const {
      return ((graph == rhs.graph) && (tensIdList == rhs.tensIdList));
    }

    bool operator<(const Diagram& rhs) const {
      return ((graph < rhs.graph) ||
	      (graph == rhs.graph && tensIdList < rhs.tensIdList));
    }

    void _sortTensorList();

  private:
    void _reorgTensIdList(const iTup& tensPair, unsigned int newGlobId, bool subcDone);
    iTup _getGlobalTensPair(unsigned int stepCode) const;
    inline bool _getLocalTensorIDs(const std::pair<unsigned int, unsigned int>& globTensPair, iTup& res);

};




// ***************************************************************
#endif  
