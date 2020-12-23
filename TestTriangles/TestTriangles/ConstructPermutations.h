#ifndef CONSTRUCT_PERMUTATIONS
#define CONSTRUCT_PERMUTATIONS

#include "TriangleSet.h"
#include <iostream>

void createAllBijections(GraphObject first, GraphObject second, std::vector<TriangleSet>& allSets);

void checkNextPermutationStep(TriangleSet current, 
		GraphObject first, 
		GraphObject second, 
		std::vector<TriangleSet>& allSets, 
		TriangleType currentType = TriangleType_upward
	);

void saveTriangles(std::vector<TriangleSet> allSets, std::string path);

GraphObject uploadGraph(std::string path, int rightColorsCount, int leftColorsCount);

std::pair<GraphObject, GraphObject> uploadGraphs(std::string path1,
		std::string path2,
		int colorsCount,
		int left1ColorsCount,
		int left2ColorsCount
	);

#endif
