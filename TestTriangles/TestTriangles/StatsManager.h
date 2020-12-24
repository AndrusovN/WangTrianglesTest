#ifndef STATSMANAGER
#define STATSMANAGER
#include <iostream>
#include <fstream>
#include <algorithm>

class StatsManager {
	static long long tooSmallSquares;
	static long long triviallyPeriodic;
	static long long saved;
	static long long checked;
	static long long tooSmallTriangles;
	static long long savedSquares;
	static long long alreadyFound;

public:

	static void init() {
		tooSmallSquares = 0;
		triviallyPeriodic = 0;
		saved = 0;
		checked = 0;
		tooSmallTriangles = 0;
		savedSquares = 0;
		alreadyFound = 0;
	}

	static void addAlreadyFound() {
		alreadyFound++;
	}

	static void addTooSmallSquares() {
		tooSmallSquares++;
	}

	static void addTooSmallTriangles() {
		tooSmallTriangles++;
	}

	static void addTriviallyPeriodic() {
		triviallyPeriodic++;
	}

	static void addSaved() {
		saved++;
	}

	static void addSavedSquare() {
		savedSquares++;
	}

	static void addChecked() {
		checked++;
	}

	static void show() {
		std::cout << "Small squares: " << tooSmallSquares << "\nSmall triangles: " << tooSmallTriangles <<
			"\nTrivially periodic: " << triviallyPeriodic << "\nChecked: " << checked << "\nSaved: " << saved << 
			"\nSaved squares: " << savedSquares	<< "\nAlready found square sets: " << alreadyFound << std::endl;
	}
};
#endif