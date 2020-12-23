#ifndef TARJAN
#define TARJAN

#include <vector>
#include <set>
#include <iostream>

typedef unsigned int Vertex;
typedef std::pair<Vertex, Vertex> Edge;

const int UNDEFINED_INDEX = -1;

struct Tarjan
{
	int size;
	int sccSize;
	std::vector<Vertex> stack;
	std::vector<int> index, lowlink;
	std::vector<std::set<Vertex>> sccSets;
	std::vector<int> sccIndexes;

	std::vector<bool> onStack;

	std::set<Edge> toRemove;
};

#endif