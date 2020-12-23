#ifndef GRAPH
#define GRAPH

#include <vector>
#include <set>
#include <map>
#include <iostream>

#include "Tarjan.h"

const int MIN_TILES_COUNT = 11;
/*
typedef std::_Tset_traits<Edge, std::less<Edge>, std::allocator<Edge>, false> EdgeTreeElement;

template <
	typename Set_t,
	typename = typename std::enable_if<std::is_base_of<std::_Tree<EdgeTreeElement>, Set_t>::value>::type
	>*/
class Graph {
protected:
	const unsigned int leftVerticesCount;
	const unsigned int rightVerticesCount;

	std::set<Edge> edges;

	std::vector<unsigned int> verticesDegrees;

	std::vector<unsigned int> incomingDegrees;
	std::vector<unsigned int> outcomingDegrees;

public:
	Graph(unsigned int leftVerticesCount, unsigned int rightVerticesCount);

	bool vertexGreater(Vertex first, Vertex second) const;

	bool sorted() const;

	bool sortedRight(int toIndex = -1) const;

	void addEdge(Edge e);

	void removeEdge(Edge e);

	const bool operator == (const Graph& other) const;

	void tarjanStep(Tarjan& t, Vertex v);

	Tarjan applyTarjan();

	bool canCreateElevenSquares(int additionalEdgesCount);

	bool hasIsolatedVertices() const;

	unsigned int size() const;

	bool isRightDirected() const;

	unsigned int vertexIncomingDegree(Vertex v) const;

	unsigned int vertexOutComingDegree(Vertex v) const;

	virtual void save(std::string root = ".") const;

	int saveAllVariants(Tarjan T, int additionalEdgesCount, std::string root = ".");

	std::set<Edge> getEdges() const;

	Graph copy() const;

	unsigned int getLeftVerticesCount() const;

	unsigned int getRightVerticesCount() const;

	std::vector<unsigned int> getVerticesDegrees() const;
	std::vector<unsigned int> getIncomingVerticesDegrees() const;
	std::vector<unsigned int> getOutcomingVerticesDegrees() const;

};

#endif