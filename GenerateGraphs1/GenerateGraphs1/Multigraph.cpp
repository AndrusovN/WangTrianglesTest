#include "Multigraph.h"
#include <algorithm>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>

Multigraph::Multigraph(Graph source)
    : Graph(source.getLeftVerticesCount(), source.getRightVerticesCount())
{
    this->incomingDegrees = source.getIncomingVerticesDegrees();
    this->outcomingDegrees = source.getOutcomingVerticesDegrees();
    this->verticesDegrees = source.getVerticesDegrees();

    this->edges = source.getEdges();

    edgesCopiesCount = std::vector<unsigned int>();
    edgesCopiesCount.resize(source.size(), 1);

    this->hasChoiceVertices = std::set<Vertex>();
    for (Vertex i = 0; i < leftVerticesCount + rightVerticesCount; i++)
    {
        if (outcomingDegrees[i] > 1) {
            hasChoiceVertices.insert(i);
        }
    }
}

void Multigraph::sort()
{
    std::map<Vertex, Vertex> mapping = std::map<Vertex, Vertex>();

    for (Vertex i = 0; i < leftVerticesCount; i++)
    {
        unsigned int myIndex = 0;
        for (Vertex j = 0; j < i; j++)
        {
            if (!vertexGreater(i, j)) {
                myIndex++;
            }
        }
        for (Vertex j = i+1; j < leftVerticesCount; j++)
        {
            if (vertexGreater(j, i)) {
                myIndex++;
            }
        }
        mapping.insert(std::make_pair(i, myIndex));
    }

    for (Vertex i = leftVerticesCount; i < leftVerticesCount + rightVerticesCount; i++)
    {
        unsigned int myIndex = leftVerticesCount;
        for (Vertex j = leftVerticesCount; j < i; j++)
        {
            if (!vertexGreater(i, j)) {
                myIndex++;
            }
        }
        for (Vertex j = i + 1; j < leftVerticesCount + rightVerticesCount; j++)
        {
            if (vertexGreater(j, i)) {
                myIndex++;
            }
        }
        mapping.insert(std::make_pair(i, myIndex));
    }


    remapColors(mapping);

    if (!sorted()) {
        std::cout << "TROUBLE MUBBLE BUBBLE\n";
    }
}


void Multigraph::remapColors(std::map<Vertex, Vertex> mapping)
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

    std::vector<unsigned int> nEdgeCopiesCount = std::vector<unsigned int>();
    nEdgeCopiesCount.resize(size());
    int index = 0;
    for (auto edgePtr = edges.begin(); edgePtr != edges.end(); edgePtr++)
    {
        Edge nEdge = std::make_pair(mapping.at((*edgePtr).first), mapping.at((*edgePtr).second));

        int index1 = 0;
        for (auto e : nEdges)
        {
            if (e == nEdge) {
                nEdgeCopiesCount[index1] = edgesCopiesCount[index];
            }
            index1++;
        }

        index++;
    }

    std::set<Vertex> nHasChoiceVertex = std::set<Vertex>();
    for (Vertex v : hasChoiceVertices)
    {
        nHasChoiceVertex.insert(mapping.at(v));
    }

    edges = nEdges;
    edgesCopiesCount = nEdgeCopiesCount;

    incomingDegrees = nIncDegrees;
    outcomingDegrees = nOutcDegrees;
    verticesDegrees = nDegrees;

    hasChoiceVertices = nHasChoiceVertex;
}

std::vector<Multigraph> Multigraph::genAllIsomorphicSortedEqualByLeft()
{
    auto result = std::vector<Multigraph>();

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

        Multigraph copyG = copyMultigraph();

        copyG.remapColors(remapping);
        result.push_back(copyG);
    }

    return result;
}

unsigned int Multigraph::edgeIndex(Edge e)
{
    int index = 0;
    for (auto e1 : edges)
    {
        if (e1 == e) {
            return index;
        }
        index++;
    }
    return -1;
}

void Multigraph::duplicateEdge(Edge e)
{
    int index = edgeIndex(e);
    if (index != -1) {
        edgesCopiesCount[index]++;
        hasChoiceVertices.insert(e.first);

        incomingDegrees[e.second]++;
        outcomingDegrees[e.first]++;

        verticesDegrees[e.first]++;
        verticesDegrees[e.second]++;
    }
    else
    {
        std::cout << "ERROR!\n";
    }
}

bool Multigraph::hasPrimitiveCycle(Tarjan T)
{
    for (int i = 0; i < T.sccSize; i++)
    {
        bool found = false;
        for (auto v : T.sccSets[i])
        {
            if (hasChoiceVertices.find(v) != hasChoiceVertices.end()) {
                found = true;
                break;
            }
        }
        if (!found) {
            return true;
        }
    }
    return false;
}

std::vector<Multigraph> Multigraph::variants(Tarjan T, int count, Edge lastAddedEdge)
{
    if (!canCreateElevenSquares(count)) {
        return std::vector<Multigraph>();
    }

    if (count == 0) {
        if (hasPrimitiveCycle(T)) {
            return std::vector<Multigraph>();
        }
        else
        {

            auto onlyMe = std::vector<Multigraph>();
            onlyMe.push_back(*this);

            return onlyMe;
        }
    }

    std::vector<Multigraph> result = std::vector<Multigraph>();

    auto edgePtr = edges.begin();
    for (int i = 0; i < size(); i++)
    {
        if (*edgePtr < lastAddedEdge) {
            edgePtr++;
            continue;
        }
        if (edgesCopiesCount.size() <= i) {
            std::cout << "ERROR1\n";
        }
        if (edgesCopiesCount[i] < rightVerticesCount) {
            Multigraph child = copyMultigraph();
            child.duplicateEdge(*edgePtr);
            std::vector<Multigraph> childVariants = child.variants(T, count - 1, *edgePtr);

            for (auto G : childVariants)
            {
                result.push_back(G);
            }
        }
        edgePtr++;
    }
    return result;
}

Multigraph Multigraph::copyMultigraph()
{
    Multigraph g(copy());
    g.verticesDegrees = this->verticesDegrees;
    g.incomingDegrees = this->incomingDegrees;
    g.outcomingDegrees = this->outcomingDegrees;
    g.hasChoiceVertices = this->hasChoiceVertices;
    g.edgesCopiesCount = this->edgesCopiesCount;

    return g;
}

void Multigraph::save(std::string root) const
{
    //std::cout << "SAVING GRAPH WITH " << leftVerticesCount << " " << rightVerticesCount << std::endl;
    std::string saveDir = root;

    if (!std::filesystem::exists(saveDir) || !std::filesystem::is_directory(saveDir)) {
        std::filesystem::create_directory(saveDir);
    }

    std::stringstream ss;
    ss << rightVerticesCount;
    std::string rvCount;
    ss >> rvCount;

    std::filesystem::path dir(saveDir);

    dir /= std::filesystem::path(rvCount);
    //saveDir += "\\" + rvCount;

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        std::filesystem::create_directory(dir);
    }

    std::string hash = "";
    for (Vertex i = leftVerticesCount; i < leftVerticesCount + rightVerticesCount; i++)
    {
        std::string incoming;
        std::stringstream ss2;
        ss2 << incomingDegrees[i];
        ss2 >> incoming;
        hash += incoming;
        hash += "_";

        std::string outcoming;
        std::stringstream ss3;
        ss3 << outcomingDegrees[i];
        ss3 >> outcoming;
        hash += outcoming;
        hash += i == leftVerticesCount + rightVerticesCount - 1 ? "" : "_";
    }

    dir /= std::filesystem::path(hash);
    //saveDir += "\\" + hash;

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        std::filesystem::create_directory(dir);
    }

    std::string scolors = "";
    std::stringstream ss1;
    ss1 << leftVerticesCount;
    ss1 >> scolors;

    std::string maxDuplicates;
    std::stringstream ss2;
    int maxDuplicatesCount = *std::max_element(edgesCopiesCount.begin(), edgesCopiesCount.end());
    ss2 << maxDuplicatesCount;
    ss2 >> maxDuplicates;

    scolors += "_" + maxDuplicates;

    dir /= std::filesystem::path(scolors);

    if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) {
        std::filesystem::create_directory(dir);
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

    dir /= std::filesystem::path(filename);

    //std::cout << "Saving graph to " << saveDir << std::endl;

    std::ofstream file;
    file.open(dir, std::ios_base::out);
    int index = 0;
    for (auto edge : edges)
    {
        for (int i = 0; i < edgesCopiesCount[index]; i++)
        {
            file << edge.first << " " << edge.second << std::endl;
        }
        index++;
    }

    file.close();
}
