#ifndef SAVER 
#define SAVER

#include "TriangleSet.h"
#include "Square.h"
#include "StatsManager.h"
#include <iostream>
#include <filesystem>
#include <thread>
#include <queue>
#include <mutex>
#include <sstream>

class Saver {
	static std::queue<TriangleSet> toProcess;
	static std::set<std::set<Square>> squareSets;
	static std::mutex tilesetsQueueMutex;
	static std::mutex fileMutex;
	static std::mutex squareTilesetsMutex;
	static std::mutex finishedMutex;
	static int filesCount;
	static bool finished;

	static const int WAIT_TIME;
	static const std::string savePath;
	static const int MAX_TILESETS_COUNT;

	static void processTileset(TriangleSet& S);

public:
	static void addToProcessQueue(std::vector<TriangleSet> sets);

	static void process();

	static void save();

	static void finish();
};

#endif
