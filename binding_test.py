import ContractionOptimizerCPP as CO

CO.ContractionCost.setDilutionRange(64)
print(CO.ContractionCost.getDilutionRange())

graph1 = CO.Graph([
    ((0,2), {(0,0), (1,1)}),
    ((1,3), {(1,1), (2,2)}),
    ((1,2), {(0,2)}),
    ((0,3), {(2,0)}),
])

graph2 = CO.Graph([
		  ((0,2), {(0,0), (1,1)}),
		  ((1,3), {(1,2), (2,1)}),
		  ((1,2), {(0,2)}),
		  ((0,3), {(2,0)}),
	  ])

d1 = CO.Diagram(graph1, [0,1,2,3])
d2 = CO.Diagram(graph2, [0,1,2,4])

optimizer = CO.ContractionOptimizer([d1,d2])

for diag in optimizer.getDiagramList():
    print(diag.getGraph().getContractionList())

optimizer.tune()

print("No CSE cost = ", optimizer.getNoCSECost().getCostArray())
print("W/ CSE cost = ",optimizer.getCSECost().getCostArray())
