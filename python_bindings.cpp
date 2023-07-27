#include "graph.h"
#include "diagram.h"
#include "contraction_optimizer.h"

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>
#include <pybind11/iostream.h>
namespace py = pybind11;

PYBIND11_MODULE(ContractionOptimizerCPP, m) { //Name conflict with ContractionOptimizer class
    py::class_<Graph>(m, "Graph")
        .def(py::init<const std::vector<std::pair<iTup, std::set<iTup>>>&>())
        .def("getContractionList", &Graph::getContractionList);
        .def("decode", &Graph::decode);

    py::class_<ContractionCost>(m, "ContractionCost")
        .def_static("setDilutionRange", &ContractionCost::setDilutionRange)
        .def_static("getDilutionRange", &ContractionCost::getDilutionRange)
        .def("getCostArray", &ContractionCost::getCostArray);

    py::class_<Diagram>(m, "Diagram")
        .def(py::init<const Graph&, const std::vector<unsigned int>&>())
        .def("getGraph", &Diagram::getGraph)
        .def("getRemainingTensors", &Diagram::getRemainingTensors)
        .def("getResultIdList", &Diagram::getResultIdList);

    py::class_<ContractionOptimizer>(m, "ContractionOptimizer")
        .def(py::init<const std::vector<Diagram>&>())
        .def("tune", &ContractionOptimizer::tune) 
        .def("getDiagramList", &ContractionOptimizer::getDiagramList)
        .def("getCSECost", &ContractionOptimizer::getCSECost)
        .def("getNoCSECost", &ContractionOptimizer::getNoCSECost)
        .def("getCompStepList", &ContractionOptimizer::getCompStepList);
}
