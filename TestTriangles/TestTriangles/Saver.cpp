#include "Saver.h"

std::queue<TriangleSet> Saver::toProcess;
std::set<std::set<Square>> Saver::squareSets;
std::mutex Saver::tilesetsQueueMutex;
std::mutex Saver::fileMutex;
std::mutex Saver::squareTilesetsMutex;
std::mutex Saver::finishedMutex;
int Saver::filesCount = 0;
bool Saver::finished = false;

const int Saver::WAIT_TIME = 200;
const std::string Saver::savePath = "res";
const int Saver::MAX_TILESETS_COUNT = (int)1e7;

void Saver::save()
{
	fileMutex.lock();
	squareTilesetsMutex.lock();

	std::string filename = "allSets_part_";

	std::string id;
	std::stringstream ss;
	ss << filesCount;
	ss >> id;

	filename += id;

	std::ofstream resultFile;

	std::filesystem::path saveDir(savePath);
	saveDir /= (filename + ".txt");

	resultFile.open(saveDir, std::ios_base::out);

	for (auto tileset : squareSets)
	{
		StatsManager::addSavedSquare();
		for (Square s : tileset)
		{
			resultFile << s.sides[0] << " " << s.sides[1] << " " << s.sides[2] << " " << s.sides[3] << " ";
		}

		resultFile << std::endl;
	}

	resultFile.close();
	filesCount++;

	std::cout << "Saved and cleared collection of " << squareSets.size() << " tilesets\n";

	squareSets.clear();

	fileMutex.unlock();
	squareTilesetsMutex.unlock();
}

void Saver::finish()
{
	finishedMutex.lock();
	finished = true;
	finishedMutex.unlock();

	squareTilesetsMutex.lock();
	tilesetsQueueMutex.lock();

	while(toProcess.size() > 0)
	{
		auto tileset = toProcess.front();
		toProcess.pop();

		processTileset(tileset);
	}

	squareTilesetsMutex.unlock();
	tilesetsQueueMutex.unlock();

	save();
}

void Saver::process() {
	while (true) {
		finishedMutex.lock();
		if (finished) {
			finishedMutex.unlock();
			return;
		}
		finishedMutex.unlock();

		bool needToWait = false;

		squareTilesetsMutex.lock();
		tilesetsQueueMutex.lock();

		if (toProcess.size() == 0) {
			needToWait = true;
		}
		else {
			TriangleSet current = toProcess.front();
			toProcess.pop();

			processTileset(current);
		}

		squareTilesetsMutex.unlock();
		tilesetsQueueMutex.unlock();

		if (squareSets.size() > MAX_TILESETS_COUNT) {
			std::thread saveThread(save);
			saveThread.detach();
		}

		if (needToWait) {
			std::this_thread::sleep_for(std::chrono::milliseconds(WAIT_TIME));
		}
	}
}

void Saver::processTileset(TriangleSet& S)
{
	std::set<Square> tileset;

	for (Color i = 0; i < S.rightColorsCount; i++)
	{
		for (auto tr1 : S.triangles[i * 2])
		{
			for (auto tr2 : S.triangles[i * 2 + 1])
			{
				Square s;
				s.sides.resize(4);
				s.sides[0] = tr1.left;
				s.sides[1] = tr2.left;
				s.sides[2] = tr1.top;
				s.sides[3] = tr2.top;

				tileset.insert(s);
			}
		}

	}
	if (tileset.size() >= 11) {
		if (squareSets.find(tileset) != squareSets.end()) {
			StatsManager::addAlreadyFound();
		}
		squareSets.insert(tileset);
	}
	else
	{
		StatsManager::addTooSmallSquares();
	}
}

void Saver::addToProcessQueue(std::vector<TriangleSet> sets) {
	tilesetsQueueMutex.lock();

	for (auto s : sets)
	{
		toProcess.push(s);
	}

	tilesetsQueueMutex.unlock();
}
