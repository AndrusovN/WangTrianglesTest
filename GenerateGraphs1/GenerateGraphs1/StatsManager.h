#ifndef STATSMANAGER
#define STATSMANAGER
#include <iostream>
#include <fstream>

class StatsManager {
	static int notSortedRight;
	static int notSorted;
	static int tooSmall;
	static int notSCC;

	static int graphsChecked;
	static int graphsSaved;
	static int skeletonGraphsSaved;

public:

	static void init() {
		notSCC = 0;
		notSorted = 0;
		notSortedRight = 0;
		tooSmall = 0;
		graphsChecked = 0;
		graphsSaved = 0;
		skeletonGraphsSaved = 0;
	}

	static void addGraphsChecked() {
		graphsChecked++;
	}

	static void addGraphsSaved(int count = 1) {
		graphsSaved += count;
	}

	static void addSkeletonGraphsCount() {
		skeletonGraphsSaved++;
	}

	static void addNotSortedRight() {
		notSortedRight++;
	}

	static void addNotSorted() {
		notSorted++;
	}

	static void addTooSmall() {
		tooSmall++;
	}

	static void addNotSCC() {
		notSCC++;
	}

	static void show() {
		std::cout << "Total checked: " << graphsChecked << "\nTotal saved: " << graphsSaved << "\nTotal skeleton: " << skeletonGraphsSaved
			<< "\nNot sorted right: " << notSortedRight << "\nNot sorted: " << notSorted
			<< "\nNot SCC: " << notSCC << "\nToo small: " << tooSmall << std::endl;
	}
};

int StatsManager::notSortedRight = 0;
int StatsManager::notSorted;
int StatsManager::tooSmall;
int StatsManager::notSCC;
int StatsManager::graphsChecked;
int StatsManager::graphsSaved;
int StatsManager::skeletonGraphsSaved;
#endif