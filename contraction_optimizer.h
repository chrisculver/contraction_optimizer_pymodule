#ifndef CONTRACTION_OPTIMIZER_H
#define CONTRACTION_OPTIMIZER_H

#include "diagram.h"
#include <list>

typedef std::tuple<unsigned int, iTup, unsigned int>  compStep_t;

class ContractionOptimizer {

  private:
    std::vector<Diagram> diagList;
    std::list<compStep_t> compStepList;
    ContractionCost CSECost, noCSECost;

  public:
    ContractionOptimizer(const std::vector<Diagram>& _diagList);

    void test();
    void tune();

    std::list<compStep_t> getCompStepList() const {return compStepList; }
    std::vector<Diagram> getDiagramList() const { return diagList; }
    ContractionCost getCSECost() const { return CSECost; }
    ContractionCost getNoCSECost() const { return noCSECost; }

  private:
    ContractionCost get_global_profit(const unsigned int graphStep,
					  const iTup& globTensPair);

};

/*
namespace py = pybind11;

PYBIND11_MODULE(ContractionOptimizer, m) {
  py::class_<ContractionOptimizer>(m, "ContractionOptimizer")
    .def(py::init)
}
*/




// ***************************************************************
#endif  
