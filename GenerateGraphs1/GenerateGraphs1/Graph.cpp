#include "Graph.h"
#include "Multigraph.h"
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <fstream>


std::set<std::vector<unsigned int>> genAllPermutations(std::set<unsigned int> base, 
	std::vector<unsigned int> fixedBase) {
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

std::set<std::vector<unsigned int>> genAllCompositions(std::set<std::vector<unsigned int>> a, 
	std::set<std::vector<unsigned int>> b) {

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


Graph::Graph(unsigned int leftVerticesCount, unsigned int rightVerticesCount)
	: leftVerticesCount(leftVerticesCount), rightVerticesCount(rightVerticesCount)
{
	edges = std::set<Edge>();

	incomingDegrees = std::vector<unsigned int>();
	incomingDegrees.resize(leftVerticesCount + rightVerticesCount, 0);

	outcomingDegrees = std::vector<unsigned int>();
	outcomingDegrees.resize(leftVerticesCount + rightVerticesCount, 0);

	verticesDegrees = std::vector<unsigned int>();
	verticesDegrees.resize(leftVerticesCount + rightVerticesCount, 0);
}

bool Graph::vertexGreater(Vertex first, Vertex second) const
{
	if (verticesDegrees[first] > verticesDegrees[second]) {
		return true;
	}

	if (verticesDegrees[first] < verticesDegrees[second]) {
		return false;
	}

	if (incomingDegrees[first] > incomingDegrees[second]) {
		return true;
	}

	return false;
}

bool Graph::isomorphic(const Graph& other) const
{
	if (leftVerticesCount != other.leftVerticesCount) return false;
	if (rightVerticesCount != other.rightVerticesCount) return false;
	if (edges.size() != other.edges.size()) return false;

	for (int i = 0; i < leftVerticesCount + rightVerticesCount; i++)
	{
		if (incomingDegrees[i] != other.incomingDegrees[i]) return false;
		if (outcomingDegrees[i] != other.outcomingDegrees[i]) return false;
	}

	std::vector<Graph> isomorphicVariants = genAllIsomorphicSortedEqualByLeft();
	for (Graph G : isomorphicVariants)
	{
		if (G == other) {
			return true;
		}
	}

	return false;
}

std::vector<Graph> Graph::genAllIsomorphicSortedEqualByLeft() const
{
	auto result = std::vector<Graph>();

	//result.insert(*this);

	std::vector<std::set<unsigned int>> equivalenceGroups;

	std::set<unsigned int> firstGroup;
	firstGroup.insert(leftVerticesCount);

	equivalenceGroups.push_back(firstGroup);

	for (int i = leftVerticesCount + 1; i < leftVerticesCount + rightVerticesCount; i++)
	{
		if (!vertexGreater(i - 1, i)) {
			equivalenceGroups.back().insert(i);
		}
		else
		{
			std::set<unsigned int> newGroup;
			newGroup.insert(i);
			equivalenceGroups.push_back(newGroup);
		}
	}

	std::vector<std::set<std::vector<unsigned int>>> variants;
	for (auto group : equivalenceGroups)
	{
		variants.push_back(genAllPermutations(group));
	}

	std::set<std::vector<unsigned int>> allVariants;
	allVariants.insert(std::vector<unsigned int>());

	for (auto variant : variants)
	{
		allVariants = genAllCompositions(allVariants, variant);
	}

	for (auto variant : allVariants)
	{
		std::map<unsigned int, unsigned int> remapping;
		for (int i = 0; i < leftVerticesCount; i++)
		{
			remapping.insert(std::make_pair(i, i));
		}

		if (variant.size() != rightVerticesCount) {
			std::cout << "TROUBLES WITH VARIANT SIZE " << variant.size() << " " << rightVerticesCount << std::endl;
		}

		for (int i = 0; i < variant.size(); i++)
		{
			remapping.insert(std::make_pair(variant[i], i + leftVerticesCount));
		}

		Graph copyG = this->copy();

		copyG.remapColors(remapping);
		result.push_back(copyG);
	}

	return result;
}

void Graph::remapColors(std::map<Vertex, Vertex> mapping)
{
	std::set<Edge> nEdges = std::set<Edge>();

	for (auto e : edges)
	{
		nEdges.insert(std::make_pair(mapping.at(e.first), mapping.at(e.second)));
	}

	std::vector<unsigned int> nDegrees, nIncDegrees, nOutcDegrees;
	nDegrees.resize(leftVerticesCount + rightVerticesCount);
	nIncDegrees.resize(leftVerticesCount + rightVerticesCount);
	nOutcDegrees.resize(leftVerticesCount + rightVerticesCount);

	for (Vertex i = 0; i < leftVerticesCount + rightVerticesCount; i++)
	{
		nIncDegrees[mapping.at(i)] = incomingDegrees[i];
		nOutcDegrees[mapping.at(i)] = outcomingDegrees[i];
		nDegrees[mapping.at(i)] = verticesDegrees[i];
	}

	edges = nEdges;

	incomingDegrees = nIncDegrees;
	outcomingDegrees = nOutcDegrees;
	verticesDegrees = nDegrees;
}

bool Graph::sorted() const {
	for (Vertex i = 1; i < leftVerticesCount; i++)
	{
		if (vertexGreater(i, i - 1)) {
			return false;
		}
	}

	for (int i = leftVerticesCount + 1; i < leftVerticesCount + rightVerticesCount; i++)
	{
		if (vertexGreater(i, i - 1)) {
			return false;
		}
	}

	return true;
}

bool Graph::sortedRight(int toIndex) const
{
	if (toIndex == -1) {
		toIndex = rightVerticesCount;
	}

	for (int i = leftVerticesCount + 1; i < leftVerticesCount + toIndex; i++)
	{
		if (vertexGreater(i, i - 1)) {
			return false;
		}
	}
	return true;
}

void Graph::addEdge(Edge e)
{
	if (edges.find(e) == edges.end()) {
		edges.insert(e);
		verticesDegrees[e.first]++;
		verticesDegrees[e.second]++;

		incomingDegrees[e.second]++;
		outcomingDegrees[e.first]++;
	}
}

void Graph::removeEdge(Edge e)
{
	auto ptr = edges.find(e);
	if (ptr != edges.end()) {
		edges.erase(ptr);
		verticesDegrees[e.first]--;
		verticesDegrees[e.second]--;

		incomingDegrees[e.second]--;
		outcomingDegrees[e.first]--;
	}
}

const bool Graph::operator==(const Graph& other) const
{
	if (other.size() != this->size()) {
		return false;
	}
	if (other.rightVerticesCount != this->rightVerticesCount) {
		return false;
	}
	if (other.leftVerticesCount != other.leftVerticesCount) {
		return false;
	}

	if (edges.size() != other.edges.size()) {
		return false;
	}

	for (auto edge : other.edges) {
		if (edges.find(edge) == edges.end()) {
			return false;
		}
	}
	return true;
}

void Graph::tarjanStep(Tarjan& t, Vertex v)
{
	t.index[v] = t.size;
	t.lowlink[v] = t.index[v];

	t.size++;

	t.stack.push_back(v);
	t.onStack[v] = true;

	auto lowestEdgePtr = edges.lower_bound(std::make_pair(v, 0));

	for (auto edgePtr = lowestEdgePtr; edgePtr != edges.end() && (*edgePtr).first == v; edgePtr++)
	{
		Vertex second = (*edgePtr).second;
		if (t.index[second] == UNDEFINED_INDEX) {
			tarjanStep(t, second);
			t.lowlink[v] = std::min(t.lowlink[v], t.lowlink[second]);
		}
		else {
			if (t.onStack[second]) {
				t.lowlink[v] = std::min(t.lowlink[v], t.index[second]);
			}
			else
			{
				t.toRemove.insert(*edgePtr);
			}
		}
	}

	if (t.index[v] == t.lowlink[v]) {
		t.sccSets.push_back(std::set<Vertex>());
		
		Vertex w = -1;
		do {
			w = t.stack.back();
			t.stack.pop_back();

			t.onStack[w] = false;

			t.sccSets[t.sccSize].insert(w);
			t.sccIndexes[w] = t.sccSize;
		} while (w != v);

		t.sccSize++;
	}
}

Tarjan Graph::applyTarjan()
{
	Tarjan t;
	t.size = 1;
	t.sccSize = 0;

	int size = leftVerticesCount + rightVerticesCount;

	t.onStack = std::vector<bool>();
	t.onStack.resize(size, false);

	t.index = std::vector<int>();
	t.index.resize(size, UNDEFINED_INDEX);

	t.lowlink = std::vector<int>();
	t.lowlink.resize(size, UNDEFINED_INDEX);

	t.sccSets = std::vector<std::set<Vertex>>();

	t.toRemove = std::set<Edge>();

	t.stack = std::vector<Vertex>();

	t.sccIndexes = std::vector<int>();
	t.sccIndexes.resize(size);

	for (Vertex v = 0; v < size; v++)
	{
		if (t.index[v] == UNDEFINED_INDEX) {
			tarjanStep(t, v);
		}
	}

	for (auto edge : edges)
	{
		if (t.sccIndexes[edge.first] != t.sccIndexes[edge.second]) {
			t.toRemove.insert(edge);
		}
	}

	return t;
}

bool Graph::canCreateElevenSquares(int additionalEdgesCount)
{
	int maxDiffR = 0;
	int summR = 0;

	for (int i = leftVerticesCount; i < leftVerticesCount + rightVerticesCount; i++)
	{
		int oldProduct = incomingDegrees[i] * outcomingDegrees[i];
		summR += oldProduct;

		int small = incomingDegrees[i] < outcomingDegrees[i] ? incomingDegrees[i] : outcomingDegrees[i];
		int big = incomingDegrees[i] > outcomingDegrees[i] ? incomingDegrees[i] : outcomingDegrees[i];

		if (big - small < additionalEdgesCount) {
			additionalEdgesCount -= (big - small);
			small = big + additionalEdgesCount / 2;
			big += additionalEdgesCount - (additionalEdgesCount / 2);
		}
		else
		{
			small += additionalEdgesCount;
		}

		int diff = big * small - oldProduct;

		maxDiffR = std::max(diff, maxDiffR);
	}

	int maxDiffL = 0;
	int summL = 0;

	for (int i = 0; i < leftVerticesCount; i++)
	{
		int oldProduct = incomingDegrees[i] * outcomingDegrees[i];
		summL += oldProduct;

		int small = incomingDegrees[i] < outcomingDegrees[i] ? incomingDegrees[i] : outcomingDegrees[i];
		int big = incomingDegrees[i] > outcomingDegrees[i] ? incomingDegrees[i] : outcomingDegrees[i];

		if (big - small < additionalEdgesCount) {
			additionalEdgesCount -= (big - small);
			small = big + additionalEdgesCount / 2;
			big += additionalEdgesCount - (additionalEdgesCount / 2);
		}
		else
		{
			small += additionalEdgesCount;
		}

		int diff = big * small - oldProduct;

		maxDiffL = std::max(diff, maxDiffL);
	}

	
	return ((summR + maxDiffR) >= MIN_TILES_COUNT) && ((summL + maxDiffL) >= MIN_TILES_COUNT);
}

bool Graph::hasIsolatedVertices() const
{
	for (int i = 0; i < leftVerticesCount + rightVerticesCount; i++)
	{
		if (verticesDegrees[i] < 2) {
			//std::cout << "ISOLATED VERTEX " << i << std::endl;
			return true;
		}
	}
	return false;
}

unsigned int Graph::size() const
{
	return edges.size();
}

bool Graph::isRightDirected() const
{
	/*auto lowestEdge = edges.lower_bound(std::make_pair(leftVerticesCount, 0));

	int rightDirected = std::distance(edges.begin(), lowestEdge);
	int leftDirected = std::distance(lowestEdge, edges.end());*/

	int difference = 0;
	for (int i = leftVerticesCount; i < leftVerticesCount + rightVerticesCount; i++)
	{
		difference += incomingDegrees[i] - outcomingDegrees[i];
	}

	return difference >= 0;
}

unsigned int Graph::vertexIncomingDegree(Vertex v) const
{
	int result = 0;

	int start = 0;
	int end = leftVerticesCount;
	if (v < leftVerticesCount) {
		start = leftVerticesCount;
		end = leftVerticesCount + rightVerticesCount;
	}

	for (auto edgePtr = edges.lower_bound(std::make_pair(start, 0)); edgePtr != edges.end() && (*edgePtr).first < end; edgePtr++)
	{
		if ((*edgePtr).second == v) {
			result++;
		}
	}

	return result;
}

unsigned int Graph::vertexOutComingDegree(Vertex v) const
{
	if (v != leftVerticesCount + rightVerticesCount - 1) {
		auto lower = edges.lower_bound(std::make_pair(v, 0));
		auto upper = edges.lower_bound(std::make_pair(v + 1, 0));

		return std::distance(lower, upper);
	}
	else
	{
		auto lower = edges.lower_bound(std::make_pair(v, 0));
		auto upper = edges.end();

		return std::distance(lower, upper);
	}
}

void Graph::save(std::string root) const
{
	std::string saveDir = root;

	if (!std::filesystem::exists(saveDir) || !std::filesystem::is_directory(saveDir)) {
		std::filesystem::create_directory(saveDir);
	}

	std::stringstream ss;
	ss << rightVerticesCount;
	ss.flush();
	std::string rvCount;
	ss >> rvCount;
	saveDir += "\\" + rvCount;

	if (!std::filesystem::exists(saveDir) || !std::filesystem::is_directory(saveDir)) {
		std::filesystem::create_directory(saveDir);
	}

	std::string scolors = "";
	std::stringstream ss1;
	ss1 << leftVerticesCount;
	ss1 >> scolors;

	saveDir += "\\" + scolors;

	if (!std::filesystem::exists(saveDir) || !std::filesystem::is_directory(saveDir)) {
		std::filesystem::create_directory(saveDir);
	}

	int filesCount = 0;
	for (const auto& file : std::filesystem::directory_iterator(saveDir)) {
		filesCount++;
	}

	std::string filename = "";
	std::stringstream ss3;
	ss3 << filesCount;
	ss3.flush();
	ss3 >> filename;
	filename += ".graph";

	saveDir += "\\" + filename;

	//std::cout << "Saving graph to " << saveDir << std::endl;

	std::ofstream file;
	file.open(saveDir, std::ios_base::out);
	for (auto edge : edges)
	{
		file << edge.first << " " << edge.second << std::endl;
	}

	file.close();
}

int Graph::saveAllVariants(Tarjan T, int additionalEdgesCount, std::string root)
{
	int count = 0;
	Multigraph G = Multigraph(*this);

	std::vector<Multigraph> variants = G.variants(T, additionalEdgesCount);

	for (auto G1 : variants)
	{
		if (!G1.isRightDirected()) {
			continue;
		}
		G1.sort();
		std::vector<Multigraph> permutations = G1.genAllIsomorphicSortedEqualByLeft();
		if (permutations.size() > 6 || permutations.size() < 1) {
			std::cout << "CRINGE PERMUTATIONS SIZE!!! " << permutations.size() << std::endl;
		}
		for (auto G2 : permutations)
		{
			G2.save(root);
			count++;
		}
	}
	return count;
}


std::set<Edge> Graph::getEdges() const
{
	return edges;
}

Graph Graph::copy() const
{
	Graph result = Graph(leftVerticesCount, rightVerticesCount);
	for (auto edge : edges)
	{
		result.addEdge(std::make_pair(edge.first, edge.second));
	}
	return result;
}

unsigned int Graph::getLeftVerticesCount() const
{
	return leftVerticesCount;
}

unsigned int Graph::getRightVerticesCount() const
{
	return rightVerticesCount;
}

std::vector<unsigned int> Graph::getVerticesDegrees() const
{
	return verticesDegrees;
}

std::vector<unsigned int> Graph::getIncomingVerticesDegrees() const
{
	return incomingDegrees;
}

std::vector<unsigned int> Graph::getOutcomingVerticesDegrees() const
{
	return outcomingDegrees;
}

