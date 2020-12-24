#include "ConstructPermutations.h"
#include "Square.h"
#include <fstream>
#include <assert.h>
#include <sstream>

std::set<std::vector<unsigned int>> genAllPermutations(std::set<unsigned int> base, std::vector<unsigned int> fixedBase = std::vector<unsigned int>()) {
	std::set<std::vector<unsigned int>> results;

	if (base.size() == 0) {
		results.insert(fixedBase);
	}

	for (auto num : base)
	{
		auto duplicate = base;
		duplicate.erase(num);
		auto baseDup = fixedBase;
		baseDup.push_back(num);

		std::set<std::vector<unsigned int>> subfunResults = genAllPermutations(duplicate, baseDup);
		for (auto vector : subfunResults)
		{
			results.insert(vector);
		}
	}

	return results;
}

std::set<std::vector<unsigned int>> genAllCompositions(std::set<std::vector<unsigned int>> a, std::set<std::vector<unsigned int>> b) {
	std::set<std::vector<unsigned int>> result;

	for (auto beginning : a)
	{
		for (auto ending : b)
		{
			auto duplicate = beginning;
			duplicate.insert(duplicate.end(), ending.begin(), ending.end());
			result.insert(duplicate);
		}
	}

	return result;
}

TriangleSet genSetFromBijection(GraphObject first, GraphObject second, std::vector<unsigned int> bijection) {
	TriangleSet result;
	result.rightColorsCount = first.rightColorsCount;
	result.triangles.resize(first.rightColorsCount * 2);

	int index = 0;
	int index2 = 0;

	for (int i = 0; i < bijection.size(); i++)
	{
		Color r = index;
		Color l;
		int count = 0;
		for (auto edge : first.edges[index])
		{
			if (count == index2) {
				l = edge;
				break;
			}
			if (count == index2) {
				l = edge;
				break;
			}
			if (count == index2) {
				l = edge;
				break;
			}
			count++;
		}

		Color h;

		int count2 = 0;
		for (auto edge : second.edges[index])
		{
			if (count2 == bijection[i]) {
				h = edge;
				break;
			}
			count2++;
		}
		triangle t;
		t.right = r;
		t.left = l;
		t.top = h;
		t.type = (TriangleType)(index % 2);

		if (result.triangles[index].find(t) != result.triangles[index].end()) {
			StatsManager::addTooSmallTriangles();
			return TriangleSet();
		}

		result.triangles[index].insert(t);

		index2++;
		if (index2 == first.edges[index].size()) {
			index2 = 0;
			index++;
		}
	}

	return result;
}

void createAllBijections(GraphObject first, GraphObject second, std::vector<TriangleSet>& allSets) {
	std::vector<std::set<unsigned int>> counts;
	for (int i = 0; i < first.edges.size(); i++)
	{
		std::set<unsigned int> elements;
		for (int j = 0; j < first.edges[i].size(); j++)
		{
			elements.insert(j);
		}
		counts.push_back(elements);
	}

	std::vector<std::set<std::vector<unsigned int>>> variants;
	for (int i = 0; i < counts.size(); i++)
	{
		variants.push_back(genAllPermutations(counts[i]));
	}

	std::set<std::vector<unsigned int>> allBijections;
	allBijections.insert(std::vector<unsigned int>());

	for (int i = 0; i < variants.size(); i++)
	{
		allBijections = genAllCompositions(allBijections, variants[i]);
	}

	int currentIndex = 0;

	for (auto bijection : allBijections)
	{
		StatsManager::addChecked();
		TriangleSet T = genSetFromBijection(first, second, bijection);
		if (T.rightColorsCount == 0) {
			continue;
		}
		if (T.isSimplePeriodic()) {
			StatsManager::addTriviallyPeriodic();
			continue;
		}
		allSets.push_back(T);
		/*if (allSets.size() % 10000 == 0) {
			std::cout << "saved " << allSets.size() << " tilesets\n";
		}*/
	}
}


void checkNextPermutationStep(TriangleSet current, 
		GraphObject first, 
		GraphObject second, 
		std::vector<TriangleSet>& allSets, 
		TriangleType currentType
	)
{
	/*int a = 0;
	for (int i = 0; i < first.edges.size(); i++)
	{
		a += first.edges[i].size();
	}*/
	//std::cout << a << std::endl;
	StatsManager::addChecked();
	Color workingIndex;
	if (first.firstNotEmptyVertex % 2 == currentType) {
		workingIndex = first.firstNotEmptyVertex;
	}
	else
	{
		if (first.firstNotEmptyVertex == 0) {
			workingIndex = 1;
		}
		else
		{
			workingIndex = first.firstNotEmptyVertex + 1;
		}
	}

	auto firstColorPtr = first.edges[workingIndex].begin();

	for (auto ptr = second.edges[workingIndex].begin(); ptr != second.edges[workingIndex].end(); ptr++)
	{
		triangle t = triangle();
		t.left = *firstColorPtr;
		t.right = workingIndex / 2;
		t.top = *ptr;
		t.type = currentType;


		if (current.triangles[workingIndex].find(t) != current.triangles[workingIndex].end()) {
			StatsManager::addTooSmallTriangles();
			continue;
		}

		current.triangles[workingIndex].insert(t);

		if (current.isSimplePeriodic(workingIndex / 2)) {
			current.triangles[workingIndex].erase(t);
			StatsManager::addTriviallyPeriodic();
			continue;
		}

		Color a = *firstColorPtr;
		Color b = *ptr;
		//std::cout << "there\n";
		if (currentType == TriangleType_upward) {
			GraphObject f = first;
			assert(first.leftColorsCount < 100);
			assert(workingIndex < 100);
			f.removeEdge(first.leftColorsCount + workingIndex / 2, *firstColorPtr);

			GraphObject s = second;
			s.removeEdge(second.leftColorsCount + workingIndex / 2, *ptr);
			//first.removeEdge(first.leftColorsCount + workingIndex / 2, *firstColorPtr);
			//second.removeEdge(first.leftColorsCount + workingIndex / 2, *ptr);

			if (f.edges[workingIndex + 1].size() == 0) {
				if (f.edges[workingIndex].size() == 0) {
					if (1 + workingIndex / 2 == f.rightColorsCount) {
						allSets.push_back(current);
						//std::cout << allSets.size() << "\n";
						return;
					}
					else
					{
						checkNextPermutationStep(current, f, s, allSets, TriangleType_upward);
					}
				}
				else
				{
					checkNextPermutationStep(current, f, s, allSets, TriangleType_upward);
				}
			}
			else
			{
				checkNextPermutationStep(current, f, s, allSets, TriangleType_downward);
			}

			//first.addEdge(first.leftColorsCount + workingIndex / 2, a);
			//second.addEdge(first.leftColorsCount + workingIndex / 2, b);
		}
		else
		{
			GraphObject f = first;
			assert(first.leftColorsCount < 100);
			assert(workingIndex < 100);
			f.removeEdge(*firstColorPtr, first.leftColorsCount + workingIndex / 2);

			GraphObject s = second;
			s.removeEdge(*ptr, second.leftColorsCount + workingIndex / 2);

			if (f.edges[workingIndex - 1].size() == 0) {
				if (f.edges[workingIndex].size() == 0) {
					if (f.rightColorsCount == 1 + workingIndex / 2) {
						allSets.push_back(current);
						if (allSets.size() % 10000 == 0) {
							//std::cout << current.size() << " - tileset size\n";
							//std::cout << "saved " << allSets.size() << " tilesets\n";
						}
						return;
					}
					else
					{
						checkNextPermutationStep(current, f, s, allSets, TriangleType_upward);
					}
				}
				else
				{
					checkNextPermutationStep(current, f, s, allSets, TriangleType_downward);
				}
			}
			else
			{
				checkNextPermutationStep(current, f, s, allSets, TriangleType_upward);
			}

			/*first.addEdge(a, first.leftColorsCount + workingIndex / 2);
			second.addEdge(b, first.leftColorsCount + workingIndex / 2);*/
		}

		current.triangles[workingIndex].erase(t);

	}
}

void saveTriangles(std::vector<TriangleSet> allSets, std::string path)
{
	int count = 0;
	int filesCount = 0;

	/*std::stringstream ss;
	ss << filesCount;
	std::string filename;
	ss >> filename;*/
	std::string filename = "allSets";


	std::set<std::set<Square>> tilesets;

	for (TriangleSet T : allSets)
	{
		StatsManager::addSaved();
		std::set<Square> tileset;
		count++;
		for (Color i = 0; i < T.rightColorsCount; i++)
		{
			for (auto tr1 : T.triangles[i * 2])
			{
				for (auto tr2 : T.triangles[i * 2 + 1])
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
			if (tilesets.find(tileset) != tilesets.end()) {
				StatsManager::addAlreadyFound();
			}
			tilesets.insert(tileset);
		}
		else
		{
			StatsManager::addTooSmallSquares();
		}
		/*if (count > 1024) {
			count = 0;
			resultFile.flush();
			resultFile.close();
			filesCount++;

			std::stringstream ss1;
			ss1 << filesCount;
			std::string filename1;
			ss1 >> filename1;
			resultFile.open(path + "\\" + filename1 + ".txt", std::ios_base::out);
		}*/
		
	}


	std::ofstream resultFile;
	resultFile.open(path + "\\" + filename + ".txt", std::ios_base::app);

	for (auto tileset : tilesets)
	{
		StatsManager::addSavedSquare();
		for (Square s : tileset)
		{
			resultFile << s.sides[0] << " " << s.sides[1] << " " << s.sides[2] << " " << s.sides[3] << " ";
		}

		resultFile << "\n";
	}

	resultFile.close();
	
}

GraphObject uploadGraph(std::string path, int rightColorsCount, int leftColorsCount) {
	std::ifstream file;
	file.open(path, std::ios_base::in);

	GraphObject graph = GraphObject(rightColorsCount, leftColorsCount);

	std::string s;

	Color buffer = -1;
	do {
		s = "";
		file >> s;
		if (s == "") {
			break;
		}

		if (buffer == -1) {
			buffer = atoi(s.c_str());
		}
		else
		{
			graph.addEdge(buffer, atoi(s.c_str()));
			buffer = -1;
		}

	} while (s != "");

	file.close();

	return graph;
}

std::pair<GraphObject, GraphObject> uploadGraphs(std::string path1, 
		std::string path2, 
		int colorsCount, 
		int left1ColorsCount, 
		int left2ColorsCount
	)
{
	std::ifstream first;
	first.open(path1, std::ios_base::in);

	GraphObject firstObj = GraphObject(colorsCount, left1ColorsCount);

	std::string s;

	Color buffer = -1;
	do {
		s = "";
		first >> s;
		if (s == "") {
			break;
		}

		if (buffer == -1) {
			buffer = atoi(s.c_str());
		}
		else
		{
			firstObj.addEdge(buffer, atoi(s.c_str()));
			buffer = -1;
		}

	} while (s != "");

	first.close();

	std::ifstream second;
	second.open(path2, std::ios_base::in);

	GraphObject secondObj = GraphObject(colorsCount, left2ColorsCount);

	std::string s1;

	Color buffer1 = -1;
	do {
		s1 = "";
		second >> s1;
		if (s1 == "") {
			break;
		}

		if (buffer1 == -1) {
			buffer1 = atoi(s1.c_str());
		}
		else
		{
			secondObj.addEdge(buffer1, atoi(s1.c_str()));
			buffer1 = -1;
		}

	} while (s1 != "");

	return std::make_pair(firstObj, secondObj);
}


