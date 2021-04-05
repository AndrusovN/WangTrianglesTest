#include <iostream>
#include <filesystem>
#include <thread>
#include "ConstructPermutations.h"
#include "Saver.h"

//const unsigned int MAX_TILESETS_IN_MEMORY = 1000000;
int MAX_THREADS_COUNT;

//Проверяет все комбинации графов из папки path с rightColorsCount вершин в правой доле
void testAll(std::string path, int rightColorsCount);

// Проверяет все комбинации первого графа из firstDir и второго графа из secondDir, если у первого 
//графа left1ColorsCount цветов в левой доле, а rightColorsCount в правой, а у второго - аналогично
//allSets - список наборов, которые из этих графов получились
void compareAll(std::string firstDir, std::string secondDir, int rightColorsCount, 
	int left1ColorsCount, int left2ColorsCount);

int main()
{
	setlocale(LC_ALL, "Russian");
	StatsManager::init();
	//Получаем основные данные, запускаем проверку
	std::string path;
	std::cout << "Введите путь к папке с графами:\n";
	std::cin >> path;

	int colorsCount;
	std::cout << "Введите количество цветов на правой доле графов:\n";
	std::cin >> colorsCount;

	std::cout << "Введите максимальное количество потоков:\n";
	std::cin >> MAX_THREADS_COUNT;

	std::cout << "Введите размер буфера тайлсетов:\n";
	int bufferSize;
	std::cin >> bufferSize;
	Saver::setup(bufferSize);

	testAll(path, colorsCount);
	StatsManager::show();
}

void testAll(std::string path, int rightColorsCount)
{
	/* Файлы хранятся так: в основной директории есть папки. Название каждой папки - это
	перечисленные по очереди входящие и исходящие степени вершин правой доли графов. 
	Т.к. в паре графов будут объединяться вершины правой доли, эти вектора должны совпадать у графов в паре
	то есть графы должны быть из одной папки. 
	Далее подпапки. Название каждой подпапки - это два числа через нижнее подчеркивание
	Первое число - количество вершин левой доли. Второе число - максимальное количество дубликатов.
	Очевидно, что в паре графов, которые мы компонуем, первое число каждого графа должно быть не меньше
	второго числа напарника. А также какое-то из первых чисел должно быть не меньше четырех
	*/
	std::thread saverProcessThread(Saver::process);
	saverProcessThread.detach();

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

				std::filesystem::path fdir = folder.path();
				fdir /= std::filesystem::path(*ptr);

				std::filesystem::path sdir = folder.path();
				sdir /= std::filesystem::path(*s_ptr);

				//std::string fdir = folder.path().string() + "\\" + *ptr;
				//std::string sdir = folder.path().string() + "\\" + *s_ptr;


				//Запускаем проверку всех файлов
				compareAll(fdir.string(), sdir.string(), rightColorsCount, firstLeftCount, secondLeftCount);
				//std::this_thread::sleep_for(std::chrono::milliseconds(2000));
			}
		}
	}

	Saver::finish();
}

void compareWithGraph(GraphObject current, std::vector<GraphObject> others, std::vector<TriangleSet>* result) {
	for (auto graph2 : others)
	{
		TriangleSet empty = TriangleSet();
		empty.rightColorsCount = current.rightColorsCount;
		empty.triangles.resize(current.rightColorsCount * 2);
		StatsManager::addChecked();
		//createAllBijections(current, graph2, *result);
		checkNextPermutationStep(empty, current, graph2, *result);
	}
}

void compareAll(std::string firstDir, std::string secondDir, int rightColorsCount, 
	int left1ColorsCount, int left2ColorsCount)
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

	while (graphs1.size() >= 0)
	{
		for (int i = 0; i < currentThreads.size(); i++)
		{
			currentThreads[i]->join();
			delete currentThreads[i];
			//currentThreads.erase(currentThreads.begin() + i);

			if (temporaries[i]->size() > 0) {
				std::thread saveThread(Saver::addToProcessQueue, *temporaries[i]);
				saveThread.detach();
			}

			delete temporaries[i];

			//temporaries.erase(temporaries.begin() + i);
			//freeGraphs.insert(associated[i]);
			//associated.erase(associated.begin() + i);
		}

		currentThreads.clear();
		temporaries.clear();

		if (graphs1.size() == 0) break;

		for (int i = 0; i < MAX_THREADS_COUNT && graphs1.size() > 0; i++) {
			std::vector<TriangleSet>* temp = new std::vector<TriangleSet>();

			//associated.push_back(*freeGraphs.begin());
			std::thread* nThread = new std::thread(compareWithGraph, graphs1.back(), graphsForElements[i], temp);
			//freeGraphs.erase(freeGraphs.begin());
			currentThreads.push_back(nThread);
			temporaries.push_back(temp);
			graphs1.pop_back();
		}
	}

	std::cout << "finished working with files " << firstDir << " and " << secondDir << std::endl;
	//std::cout << "current size of saved tilesets collection: " << allSets.size() << std::endl;

}
