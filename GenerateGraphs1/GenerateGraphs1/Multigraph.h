#ifndef MULTIGRAPH
#define MULTIGRAPH

#include "Graph.h"

class Multigraph : public Graph{
protected:
	std::vector<unsigned int> edgesCopiesCount;

	std::set<Vertex> hasChoiceVertices;

public:
	Multigraph(Graph source);

	void remapColors(std::map<Vertex, Vertex> mapping);

	void sort();

	std::vector<Multigraph> genAllIsomorphicSortedEqualByLeft();

	unsigned int edgeIndex(Edge e);

	void duplicateEdge(Edge e);

	bool hasPrimitiveCycle(Tarjan T);

	std::vector<Multigraph> variants(Tarjan T, int count, Edge lastAddedEdge = std::make_pair(0,0));

	Multigraph copyMultigraph();

	void save(std::string root = ".") const override;
};

#endif