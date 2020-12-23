#ifndef GRAPH_OBJECT
#define GRAPH_OBJECT

#include <vector>
#include <set>
#include <algorithm>

typedef unsigned int Color;

struct GraphObject {
	std::vector<std::multiset<Color>> edges;
	unsigned int rightColorsCount;
	unsigned int leftColorsCount;
	unsigned int firstNotEmptyVertex;

	GraphObject(unsigned int rightColorsCount, unsigned int leftColorsCount) {
		this->rightColorsCount = rightColorsCount;
		this->leftColorsCount = leftColorsCount;
		firstNotEmptyVertex = rightColorsCount * 2;
		edges = std::vector<std::multiset<Color>>();
		edges.resize(rightColorsCount * 2);
	}

	void addEdge(Color first, Color second) {
		if (first > second) {
			edges[(first - leftColorsCount) * 2].insert(second);
			firstNotEmptyVertex = std::min(firstNotEmptyVertex, (first - leftColorsCount) * 2);
		}
		else
		{
			edges[(second - leftColorsCount) * 2 + 1].insert(first);
			firstNotEmptyVertex = std::min(firstNotEmptyVertex, (second - leftColorsCount) * 2 + 1);
		}
	}

	void removeEdge(Color first, Color second) {
		Color index, value;

		if (first > second) {
			index = (first - leftColorsCount) * 2;
			value = second;
		}
		else
		{
			index = (second - leftColorsCount) * 2 + 1;
			value = first;
		}

		auto ptr = edges[index].find(value);
		if (ptr != edges[index].end()) {
			edges[index].erase(ptr);

			if (firstNotEmptyVertex == index && edges[index].size() == 0) {
				do {
					firstNotEmptyVertex++;
				} while (firstNotEmptyVertex < edges.size() && edges[firstNotEmptyVertex].size() == 0);
			}
		}
	}
};

#endif