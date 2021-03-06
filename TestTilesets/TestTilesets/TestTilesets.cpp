﻿#include <iostream>
#include <filesystem>
#include <string>
#include <sstream>

// Включает или отключает вывод дополнительной информации о процессе тестирования в консоль
#define DEBUG 1

#include "Test.h"
#include "TilesUploader.h"

// Проверка всех тайлсетов из папки (1 файл в папке = 1 тайлсет)
void TestAllFromDir(std::string dir);

// Проверка одного тайлсета из файла (1 файл = 1 тайлсет)
void TestOne(std::string path);

// Основная функция! Проверка всех тайлсетов из файла (1 строка = 1 тайлсет)
std::vector<Tileset> TestAllFromFile(std::string path);

// Сохранение списка кандидатов в отдельный файл по пути path
void save(std::vector<Tileset> candidates, std::string path);

int main()
{
	// Добавляем русские буквы для вывода
	setlocale(LC_ALL, "Russian");

	//Получаем основную информацию
	std::cout << "Тестировать файл (0) или все файлы из папки (1) или все тайлсеты из файла (2): \n";

	int answer;
	std::cin >> answer;
	std::string path;
	std::vector<Tileset> candidates;

	StatsManager::init();

	switch (answer)
	{
	case 0:
		std::cout << "Введите путь к файлу:\n";
		std::cin >> path;
		TestOne(path);
		break;
	case 1:
		std::cout << "Введите путь к папке:\n";
		std::cin >> path;
		TestAllFromDir(path);
		break;
	case 2:
		std::cout << "Введите путь к файлу:\n";
		std::cin >> path;
		candidates = TestAllFromFile(path);
		save(candidates, "candidates.txt");
		break;
	default:
		break;
	}

	StatsManager::show();

}

void TestAllFromDir(std::string dir) {
	//Идем по всем файлам в папке и вызываем функцию тестирования одного файла
	for (const auto & entry : std::filesystem::directory_iterator(dir))
	{
		if (!entry.is_directory()) {
			TestOne(dir + "\\" + entry.path().filename().string());
		}
	}
}

void TestOne(std::string path) {
	std::cout << "Проверка набора из файла " + path << std::endl;

	Tileset W = Upload(path);

	// В вектор надо загрузить лишь 1 тайлсет
	std::vector<Tileset> onlyTileset = std::vector<Tileset>();
	onlyTileset.push_back(W);

	// resultSize - переменная для статистики. До какого размера пришлось собирать набор
	int resultSize = 0;

	//Основное тестирование
	ResultCode result = TestAperiodic(onlyTileset, resultSize);

	if (DEBUG) {
		std::cout << "Ширина полоски до которой дошла проверка: " << resultSize << std::endl;
	}

	//Выводим результат
	switch (result)
	{
	case ResultCode_periodic: {
		std::cout << "Тайлсет периодический!\n";
		break;
	}
	case ResultCode_tooSmall: {
		std::cout << "Тайлсет слишком маленький!\n";
		break;
	}
	default:
		std::cout << "Не удалось проверить тайлсет!\n";
		break;
	}
}

std::vector<Tileset> TestAllFromFile(std::string path)
{
	std::ifstream file;
	file.open(path, std::ios_base::in);
	std::string data;

	//Массив, в котором будут хранится наборы, которые проверить не удалось
	std::vector<Tileset> candidates;

	// Количество проверенных наборов. Используется для вывода информации.
	int count = 0;
	//Читаем каждую строку файла
	while (std::getline(file, data)) {
		std::istringstream ss(data);

		//color - это текущий цвет, который мы прочитали
		std::string color;
		ss >> color;
		Tileset T;

		do {
			//Создаем новый тайл и назначаем ему цвета из файла. 
			Tile toAdd;
			for (int i = 0; i < 4; i++)
			{
				toAdd[i] = atoi(color.c_str());
				color = "";
				//Читаем следующий цвет
				ss >> color;
			}

			T.AddTile(toAdd);
		} while (color != "");

		//Запускаем алгоритм тестирования
		std::vector<Tileset> onlyTileset = std::vector<Tileset>();
		onlyTileset.push_back(T);
		int resultSize = 0;
		ResultCode result = TestAperiodic(onlyTileset, resultSize);

		StatsManager::changeWidth(resultSize);

		count++;
		if (count % 1000 == 0) {
			std::cout << "Проверено " << count << " наборов\n";
		}

		//Обрабатываем результаты
		switch (result)
		{
		case ResultCode_periodic: {
			//Все ок, просто продолжаем
			break;
		}
		case ResultCode_tooSmall: {
			//Все ок, просто продолжаем
			break;
		}
		default:
			//Проверить не удалось. Сохраняем как кандидата
			std::cout << "Не удалось проверить тайлсет!\n";
			candidates.push_back(T);
			break;
		}
	}

	return candidates;
}

void save(std::vector<Tileset> candidates, std::string path)
{
	std::ofstream file;
	file.open(path, std::ios_base::out);
	//Идем по всем наборам и выписываем их в строчку
	for (Tileset T : candidates)
	{
		for (auto tile : T.tiles)
		{
			file << tile[0] << " " << tile[1] << " " << tile[2] << " " << tile[3] << "  ";
		}
		file << "\n";
	}
	file.flush();
	file.close();
}
