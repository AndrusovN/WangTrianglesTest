#ifndef TEST
#define TEST

#include <chrono>
#include <thread>
#include "StatsManager.h"
#include "Tarjan.h"

const int MAX_WIDTH = 10000;
const int TICK = 20;

//��������� �� ������� �������� ������� �� ���� ������: ���� ���� ������ � ������, � �������� ������ ������ maxTileIndex � 
//������ � �������� maxTileIndex, �� ������ true, ���� ������ ������� ���, ������ false. ����� ������� ���� ��������� ��� �������
//��� ���� �������� ����������, �� �������� ��� ����, ������ ��� ����������
bool TwoTilesPeriodicity(Tileset T, int maxTileIndex) {
	//���� ����� ��������� ������� ����, ��� ����� ������ � ����� ����� ��� ������
	if (T[maxTileIndex].north() == T[maxTileIndex].south()) {
		for (int i = 0; i < maxTileIndex; i++)
		{
			if (T[i].north() == T[i].south() &&
					T[i].east() == T[maxTileIndex].west() &&
					T[i].west() == T[maxTileIndex].east()) {
				return true;
			}
		}
	}

	//���� ����� ��������� ������� ����, ��� ����� ������ � ����� ����� ��� ������
	if (T[maxTileIndex].east() == T[maxTileIndex].west()) {
		for (int i = 0; i < maxTileIndex; i++)
		{
			if (T[i].east() == T[i].west() &&
					T[i].north() == T[maxTileIndex].south() && 
					T[i].south() == T[maxTileIndex].north()) {
				return true;
			}
		}
	}

	//���� ����� ��������� ����� ���������� ���� (��� ��������� ������)
	for (int i = 0; i < maxTileIndex; i++)
	{
		if (T[i].north() == T[maxTileIndex].south() &&
			T[i].south() == T[maxTileIndex].north() &&
			T[i].west() == T[maxTileIndex].east() &&
			T[i].east() == T[maxTileIndex].west()) {
			return true;
		}
	}

	return false;
}

//�������� ������� �������� �� ��������������
//���������� ��������� ��������
//W - �������, ������� ���������
//width - ����������, � ������� ����� �������� ������������ ������ �������, ������� ���������������
ResultCode TestAperiodic(Tileset W, int& width) {
	//����� �������� �� ������ � ���������, �� � � ��� ����������������� �������
	Tileset W_transposed = W.Transposed();

	//��� ��� �������� (�������� ������� ��������)
	Tileset W_power = W;
	Tileset W_transposed_power = W_transposed;

	//���� ����� ���-�� �� ��� ��� �������������, �� ����� �����������
	if (W.IsPeriodic() || W_transposed.IsPeriodic()) {
		return ResultCode_periodic;
	}

	width = 1;
	for (width; width < MAX_WIDTH; width++)
	{
		//tmp-������ - ����� �� ���� ��� � ����������� ������ ����������
		Tileset W_power_tmp = W_power;

#if  DEBUG

		long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
#endif //  DEBUG

		//������� ���������� � ��������� ��
		ResultCode powerResult = W_power.Composition(W_power_tmp, W);

		//���� ��� ��������� ���������� ��� ���-�� ����������, ����� ������ ���
		if (powerResult != ResultCode_nothing) {
			return powerResult;
		}

		if (W_power.IsPeriodic()) {
			return ResultCode_periodic;
		}

		//���������� ��������� � ����������������� �������
		Tileset W_transposed_power_tmp = W_transposed_power;
		ResultCode transposedPowerResult = W_transposed_power.Composition(W_transposed_power_tmp, W_transposed);
		if (transposedPowerResult != ResultCode_nothing) {
			return transposedPowerResult;
		}

		if (W_transposed_power.IsPeriodic()) {
			return ResultCode_periodic;
		}

#if DEBUG
		//���� ����� �������� ������ ����������
		long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();

		//���� �����, ����������� �� ����������, ������ �������, �� ����� ������� ����������
		int computationalTime = endTime - startTime;
		if (computationalTime > 1000) {

			//�������� ����� ������ ��������� � ���������� ������
			int size = W_power.size;
			int transposed_size = W_transposed_power.size;

			int colors = W_power.maxColor;
			int transposed_color = W_transposed_power.maxColor;

			std::cout << "����������� ���������� � �������� ������ " << width
				<< "\n������ ������ �������� ��������: " << size << ", ���������� ������: " << colors
				<< "\n������ ������ ������������������ ��������: " << transposed_size << ", ���������� ������: " << transposed_color 
				<< "\n���������� ������ " << (float)computationalTime / (float)1000 << " ������"
				<< std::endl << std::endl;
		}
#endif
	}

	//���� ������� ��� ����� ������� ������, � ���������� ���, �� ���������� �������(((
	return ResultCode_nothing;
}

void TestComposition(Tileset* source, Tileset toAdd, ResultCode* result) {
	Tileset copy;
	copy.AddTileset(*source);
	ResultCode compositionResult = source->Composition(copy, toAdd);
	if (compositionResult != ResultCode_nothing) {
		*result = compositionResult;
		return;
	}

	if (source->IsPeriodic()) {
		*result = ResultCode_periodic;
		return;
	}
	*result = ResultCode_nothing;
}


ResultCode TestAperiodic(std::vector<Tileset> tilesets, int& maxWidth) {
	int size = tilesets.size() * 2;

	Tileset* allTilesets = new Tileset[size];

	Tileset* powers = new Tileset[size];

	bool* toUse = new bool[size];
	int toUseCount = size;

	for (int i = 0; i < tilesets.size(); i++)
	{
		allTilesets[i * 2] = tilesets[i];
		allTilesets[i * 2 + 1] = tilesets[i].Transposed();

		powers[i * 2] = allTilesets[i * 2];
		powers[i * 2 + 1] = allTilesets[i * 2 + 1];

		toUse[i * 2] = true;
		toUse[i * 2 + 1] = true;
	}

	for (maxWidth = 1; maxWidth < MAX_WIDTH; maxWidth++)
	{
#if  DEBUG

		long long startTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();
#endif //  DEBUG

		std::thread** threads = new std::thread * [size];
		ResultCode* results = new ResultCode[size];
		bool* finished = new bool[size];
		int finishedCount = 0;

		ResultCode mainResult = ResultCode_nothing;

		for (int i = 0; i < size; i++)
		{
			if (toUse[i]) {
				//results[i] = ResultCode_nothing;
				TestComposition(&powers[i], allTilesets[i], &results[i]);

				if (results[i] == ResultCode_periodic || results[i] == ResultCode_tooSmall) {
					//std::cout << "found\n";
					mainResult = results[i];
				}
				if (results[i] == ResultCode_tooBig) {
					toUse[i] = false;
					toUseCount--;
				}
				finished[i] = true;

				finishedCount++;
				//std::thread* thread = new std::thread( TestComposition, &powers[ i ], allTilesets[ i ], &results[ i ] );

				//threads[i] = thread;
				//finished[i] = false;
			}
			else
			{
				finished[i] = true;
				finishedCount++;
			}
		}
		/*while (finishedCount < size)
		{
			//std::cout << threads.size() << " - threads size\n";
			for (int i = 0; i < size; i++)
			{
				if (!finished[i]) {
					if (threads[i]->joinable()) {
						//std::cout << "joinjng\n";
						threads[i]->join();
						//std::cout << results[i] << std::endl;
						if (results[i] == ResultCode_periodic || results[i] == ResultCode_tooSmall) {
							//std::cout << "found\n";
							mainResult = results[i];
						}
						if (results[i] == ResultCode_tooBig) {
							toUse[i] = false;
							toUseCount--;
						}
						finished[i] = true;

						finishedCount++;
						//threads.erase(threads.begin() + i);
						//indexes.erase(indexes.begin() + i);
						//results.erase(results.begin() + i);
						break;
					}
					else
					{
						std::cout << "thread is not joibable\n";
					}
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(TICK));
		}*/

#if DEBUG
		//���� ����� �������� ������ ����������
		long long endTime = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::system_clock::now().time_since_epoch()).count();

		//���� �����, ����������� �� ����������, ������ �������, �� ����� ������� ����������
		int computationalTime = endTime - startTime;
		if (computationalTime > 1000) {

			//�������� ����� ������ ��������� � ���������� ������
			int maxSize = 0;
			int maxColors = 0;
			for (int i = 0; i < size; i++)
			{
				if (toUse[i]) {
					if (powers[i].size > maxSize) {
						maxSize = powers[i].size;
					}
					if (powers[i].maxColor > maxColors) {
						maxColors = powers[i].maxColor;
					}
				}
			}

			std::cout << "����������� ���������� � �������� ������ " << maxWidth
				<< "\n������ ������������ ������ ��������: " << maxSize << ", ���������� ������: " << maxColors
				<< "\n���������� ������ " << (float)computationalTime / (float)1000 << " ������"
				<< std::endl << std::endl;
		}
#endif

		delete[] finished;
		delete[] results;
		delete[] threads;

		if (mainResult != ResultCode_nothing) {
			if (mainResult == ResultCode_periodic) {
				StatsManager::addPeriodicCount();
			}
			else
			{
				StatsManager::addWithoutTilingCount();
			}

			delete[] allTilesets;
			delete[] powers;
			delete[] toUse;

			return mainResult;
		}

		if (toUseCount == 0) {
			delete[] allTilesets;
			delete[] powers;
			delete[] toUse;

			return ResultCode_tooBig;
		}
	}

	delete[] allTilesets;
	delete[] powers;
	delete[] toUse;

	return ResultCode_nothing;
}
#endif