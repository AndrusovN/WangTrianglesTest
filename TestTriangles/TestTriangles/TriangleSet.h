#ifndef TRIANGLE_SET
#define TRIANGLE_SET

#include <vector>
#include <set>
#include <algorithm>

#include "Triangle.h"

struct TriangleSet {
	unsigned int rightColorsCount;
	std::vector<std::set<triangle>> triangles;

	TriangleSet() {
		rightColorsCount = 0;
	}

	bool isSimplePeriodic(Color rightMatchingColor = 0);

	~TriangleSet() {
		triangles.clear();
		triangles.shrink_to_fit();
	}

	int size() {
		int base = sizeof(std::vector<std::set<triangle>>);
		int base2 = sizeof(std::set<triangle>);
		int result = base;
		for (auto set : triangles)
		{
			result += base2 + set.size() * sizeof(triangle);
		}

		return result;
	}
};


#endif