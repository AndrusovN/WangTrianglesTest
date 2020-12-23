#include <iostream>
#include "Graph.h"

void GenerateGraphs(int oneSideColors, int tilesCount);

int main()
{
    setlocale(LC_ALL, "Russian");

    std::cout << "Введите количество цветов\n";
    int oneSideColors;
    std::cin >> oneSideColors;

    std::cout << "Введите кол-во тайлов\n";
    int tilesCount;
    std::cin >> tilesCount;
    std::cout << "Starting generating graphs!\n";

    GenerateGraphs(oneSideColors, tilesCount);
}

int tilesChecked = 0;
int tilesSaved = 0;
int uniqueTiles = 0;

void MainFunction(Graph g, int expectedSize, Edge lastAddedEdge = std::make_pair(0, 0)) {
    tilesChecked++;
    if (tilesChecked % 65536 == 0) {
        Edge e = *g.getEdges().begin();
        std::cout << "Smallest edge: " << e.first << " " << e.second << "\n";
        std::cout << tilesChecked - 1 << " tilesets checked\n";
        std::cout << tilesSaved << " tilesets saved\n";
        std::cout << uniqueTiles << " unique tilesets saved\n\n";
    }

    if (lastAddedEdge.first >= g.getLeftVerticesCount()) {
        if (!g.sortedRight(lastAddedEdge.first - g.getLeftVerticesCount())) {
            return;
        }
    }

    if (lastAddedEdge.first == g.getLeftVerticesCount() + g.getRightVerticesCount() - 1) {
        if (g.sorted()) {
            if (/*g.isRightDirected()*/true) {
                if (g.canCreateElevenSquares(expectedSize - g.size())) {
                    Tarjan T = g.applyTarjan();
                    if (T.toRemove.size() == 0) {
                        //g.save("Graphs");
                        //tilesSaved++;
                        uniqueTiles++;
                        int tilesAdded = g.saveAllVariants(T, expectedSize - g.size(), "FullGraphs");
                        tilesSaved += tilesAdded;
                    }
                }
            }
        }
    }
    
    if (g.size() == expectedSize) {
        return;
    }

    int firstEdgeEndingIndex = lastAddedEdge.first >= g.getLeftVerticesCount() ? 
        g.getLeftVerticesCount() : g.getRightVerticesCount() + g.getLeftVerticesCount();

    if (lastAddedEdge.first == 0 && lastAddedEdge.second == 0) {
        lastAddedEdge.second = g.getLeftVerticesCount() - 1;
    }

    for (Vertex u = lastAddedEdge.second + 1; u < firstEdgeEndingIndex; u++)
    {
        Edge newEdge = std::make_pair(lastAddedEdge.first, u);
        //Graph copy = g.copy();
        g.addEdge(newEdge);
        MainFunction(g, expectedSize, newEdge);
        g.removeEdge(newEdge);
    }

    if (lastAddedEdge.first == 0 && lastAddedEdge.second == g.getLeftVerticesCount() - 1) {
        return;
    }

    if (lastAddedEdge.first == g.getLeftVerticesCount() + g.getRightVerticesCount() - 1) {
        return;
    }

    int secondEdgeEndingIndex = (lastAddedEdge.first + 1) >= g.getLeftVerticesCount() ?
        g.getLeftVerticesCount() : g.getRightVerticesCount() + g.getLeftVerticesCount();

    int secondEdgeBeginningIndex = (lastAddedEdge.first + 1) >= g.getLeftVerticesCount() ?
        0 : g.getLeftVerticesCount();

    for (Vertex u = secondEdgeBeginningIndex; u < secondEdgeEndingIndex; u++)
    {
        Edge newEdge = std::make_pair(lastAddedEdge.first + 1, u);
        g.addEdge(newEdge);
        //Graph copy = g.copy();
        //copy.addEdge(newEdge);
        //MainFunction(copy, expectedSize, newEdge);
        MainFunction(g, expectedSize, newEdge);
        g.removeEdge(newEdge);
    }

}

void GenerateGraphs(int oneSideColors, int tilesCount) {
    for (int leftColors = 2; leftColors <= oneSideColors; leftColors++)
    {
        Graph empty = Graph(leftColors, oneSideColors);
        MainFunction(empty, tilesCount);
        std::cout << "ALL FINISHED\n";
        std::cout << tilesChecked << " tilesets checked\n";
        std::cout << tilesSaved << " tilesets saved\n";
        std::cout << uniqueTiles << " unique tilesets saved\n\n";
    }
}
