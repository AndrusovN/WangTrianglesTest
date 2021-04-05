#ifndef SQUARE
#define SQUARE
#include "GraphObject.h"

struct Square {
	std::vector<Color> sides;

	~Square() {
		sides.clear();
		sides.shrink_to_fit();
	}

	bool operator < (const Square& other) const {
		return (sides < other.sides);
	}

	bool operator == (const Square& other) const {
		if (sides.size() != other.sides.size() || sides.size() != 4) {
			return false;
		}
		return (sides[0] == other.sides[0] &&
			sides[1] == other.sides[1] &&
			sides[2] == other.sides[2] &&
			sides[3] == other.sides[3]);
	}
};

#endif