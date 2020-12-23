#ifndef TRIANGLE
#define TRIANGLE

#include "GraphObject.h"
#include <iostream>

enum TriangleType {
	TriangleType_upward = 0,
	TriangleType_downward = 1
};

struct triangle {
	Color left, right, top;
	TriangleType type;

	bool operator < (const triangle& other) const{
		return left < other.left;
	}

	void show() {
		std::cout << left << " " << right << " " << top << " " << type << "\n";
	}
};

#endif