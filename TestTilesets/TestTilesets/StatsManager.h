#ifndef STATSMANAGER
#define STATSMANAGER
#include <iostream>
#include <fstream>
#include <algorithm>

class StatsManager {
	static int maxWidth;
	static int periodicCount;
	static int withoutTilingCount;

public:

	static void init() {
		maxWidth = 0;
		periodicCount = 0;
		withoutTilingCount = 0;
	}

	static void addPeriodicCount() {
		periodicCount++;
	}

	static void addWithoutTilingCount() {
		withoutTilingCount++;
	}

	static void changeWidth(int width) {
		maxWidth = std::max(width, maxWidth);
	}

	static void show() {
		std::cout << "Max width: " << maxWidth << "\nWithout tiling: " << withoutTilingCount 
			<< "\nPeriodic tiling: " << periodicCount << std::endl;
	}
};

int StatsManager::maxWidth = 0;
int StatsManager::periodicCount = 0;
int StatsManager::withoutTilingCount = 0;
#endif