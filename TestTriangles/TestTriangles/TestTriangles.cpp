#include <iostream>
#include <filesystem>
#include <thread>
#include "ConstructPermutations.h"

const unsigned int MAX_TILESETS_IN_MEMORY = 1000000;

//Проверяет все комбинации графов из папки path с rightColorsCount вершин в правой доле
void testAll(std::string path, int rightColorsCount);

// Проверяет все комбинации первого графа из firstDir и второго графа из secondDir, если у первого 
//графа left1ColorsCount цветов в левой доле, а rightColorsCount в правой, а у второго - аналогично
//allSets - список наборов, которые из этих графов получились
void compareAll(std::string firstDir, std::string secondDir, int rightColorsCount, 
	int left1ColorsCount, int left2ColorsCount, std::vector<TriangleSet>& allSets);

int main()
{
	setlocale(LC_ALL, "Russian");
	//Получаем основные данные, запускаем проверку
	std::string path;
	std::cout << "Введите путь к папке с графами:\n";
	std::cin >> path;

	int colorsCount;
	std::cout << "Введите количество правых цветов у графов:\n";
	std::cin >> colorsCount;

	testAll(path, colorsCount);
}

int ALLCOUNT = 0;

void testAll(std::string path, int rightColorsCount)
{
	//Массив, в котором будут храниться все тайлы, которые нам интересны
	std::vector<TriangleSet> allSets;

	/* Файлы хранятся так: в основной директории есть папки. Название каждой папки - это
	перечисленные по очереди входящие и исходящие степени вершин правой доли графов. 
	Т.к. в паре графов будут объединяться вершины правой доли, эти вектора должны совпадать у графов в паре
	то есть графы должны быть из одной папки. 
	Далее подпапки. Название каждой подпапки - это два числа через нижнее подчеркивание
	Первое число - количество вершин левой доли. Второе число - максимальное количество дубликатов.
	Очевидно, что в паре графов, которые мы компонуем, первое число каждого графа должно быть не меньше
	второго числа напарника. А также какое-то из первых чисел должно быть не меньше четырех
	*/


	//Идем по всем папкам директории
	for (const auto & folder : std::filesystem::directory_iterator(path))
	{
		//Все пары графов, которые можно комбинировать лежат в одной папке. 
		//Создаем массив всех подпапок папки
		std::vector<std::string> subdirs = std::vector<std::string>();
		for (const auto & f_subfolder : std::filesystem::directory_iterator(folder.path()))
		{
			subdirs.push_back(f_subfolder.path().filename().string());
		}

		//Идем по всем парам подпапок
		for (auto ptr = subdirs.begin(); ptr != subdirs.end(); ptr++)
		{
			for (auto s_ptr = ptr; s_ptr != subdirs.end(); s_ptr++)
			{
				//Вычисляем первые и вторые числа
				int firstLeftCount = atoi(&(*ptr)[0]);
				int firstDuplicatesCount = atoi(&(*ptr)[2]);

				int secondLeftCount = atoi(&(*s_ptr)[0]);
				int secondDuplicatesCount = atoi(&(*s_ptr)[2]);

				//Проверяем условия
				if (firstLeftCount < secondDuplicatesCount) {
					continue;
				}
				if (secondLeftCount < firstDuplicatesCount) {
					continue;
				}

				if (std::max(secondLeftCount, firstLeftCount) < 4) {
					continue;
				}

				std::string fdir = folder.path().string() + "\\" + *ptr;
				std::string sdir = folder.path().string() + "\\" + *s_ptr;


				//Запускаем проверку всех файлов
				compareAll(fdir, sdir, rightColorsCount, firstLeftCount, secondLeftCount, allSets);
			}
		}
	}

	std::cout << ALLCOUNT << " - RESULT COUNT!!!" << std::endl;

	//Сохраняем все наборы, которые получились
	saveTriangles(allSets, "res");
}

void compareWithGraph(GraphObject current, std::vector<GraphObject> others, std::vector<TriangleSet>* result) {
	for (auto graph2 : others)
	{
		TriangleSet empty = TriangleSet();
		empty.rightColorsCount = current.rightColorsCount;
		empty.triangles.resize(current.rightColorsCount * 2);

		createAllBijections(current, graph2, *result);
	}
}

void compareAll(std::string firstDir, std::string secondDir, int rightColorsCount, 
	int left1ColorsCount, int left2ColorsCount, std::vector<TriangleSet>& allSets)
{
	std::vector<GraphObject> graphs1, graphs2;
	for (const auto& file1 : std::filesystem::directory_iterator(firstDir))
	{
		graphs1.push_back(uploadGraph(file1.path().string(), rightColorsCount, left1ColorsCount));
	}
	for (const auto& file2 : std::filesystem::directory_iterator(secondDir))
	{
		graphs2.push_back(uploadGraph(file2.path().string(), rightColorsCount, left2ColorsCount));
	}

	std::vector<std::thread*> currentThreads;
	std::vector<std::vector<TriangleSet>*> temporaries;
	std::vector<int> associated;
	std::vector<std::vector<GraphObject>> graphsForElements;
	std::set<int> freeGraphs;

	const int MAX_THREADS_COUNT = 10;

	for (int i = 0; i < MAX_THREADS_COUNT; i++)
	{
		freeGraphs.insert(i);
		//associated.push_back(-1);
		graphsForElements.push_back(graphs2);
	}

	while (graphs1.size() > 0)
	{
		for (int i = 0; i < currentThreads.size(); i++)
		{
			if (currentThreads[i]->joinable()) {
				currentThreads[i]->join();
				delete currentThreads[i];
				currentThreads.erase(currentThreads.begin() + i);
				if (temporaries[i]->size() > 0) {
					allSets.insert(allSets.end(), temporaries[i]->begin(), temporaries[i]->end());
				}
				delete temporaries[i];
				temporaries.erase(temporaries.begin() + i);
				freeGraphs.insert(associated[i]);
				associated.erase(associated.begin() + i);
				if (allSets.size() > MAX_TILESETS_IN_MEMORY) {
					saveTriangles(allSets, "res");
					std::cout << "Tilesets collection saved and cleared\n";
					allSets = std::vector<TriangleSet>();
				}
			}
		}
		while (currentThreads.size() < MAX_THREADS_COUNT && graphs1.size() > 0) {
			std::vector<TriangleSet>* temp = new std::vector<TriangleSet>();

			associated.push_back(*freeGraphs.begin());
			std::thread* nThread = new std::thread(compareWithGraph, graphs1.back(), graphsForElements[*freeGraphs.begin()], temp);
			freeGraphs.erase(freeGraphs.begin());
			currentThreads.push_back(nThread);
			temporaries.push_back(temp);
			graphs1.pop_back();
		}
	}

	/*for (auto graph1 : graphs1)
	{
		int summTime = 0;

		for (auto graph2 : graphs2)
		{
			/*std::pair<GraphObject, GraphObject> fs = uploadGraphs(file1.path().string(), 
				file2.path().string(), rightColorsCount, left1ColorsCount, left2ColorsCount);/*
			
			TriangleSet empty = TriangleSet();
			empty.rightColorsCount = rightColorsCount;
			empty.triangles.resize(rightColorsCount * 2);
			long long startTime = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count();

			//checkNextPermutationStep(empty, graph1, graph2, allSets);
			createAllBijections(graph1, graph2, allSets);
			long long endTime = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::system_clock::now().time_since_epoch()).count();

			//Если время, затраченное на вычисления, больше секунды, то стоит вывести информацию
			int computationalTime = endTime - startTime;

			ALLCOUNT++;

			summTime += computationalTime;
		}

		if (allSets.size() > MAX_TILESETS_IN_MEMORY) {
			saveTriangles(allSets, "res");
			std::cout << "Tilesets collection saved and cleared\n";
			allSets = std::vector<TriangleSet>();
		}

		//if (summTime > 1000000) {
			//std::cout << summTime << std::endl;
		//}
	}*/

	std::cout << "finished working with files " << firstDir << " and " << secondDir << std::endl;
	std::cout << "current size of saved tilesets collection: " << allSets.size() << std::endl;

}
