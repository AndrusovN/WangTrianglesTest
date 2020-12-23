#ifndef TILESET
#define TILESET
#include <vector>
#include <iostream>
#include <set>
#include <map>
#include <list>

#include "Tile.h"

const int MAX_TILESET_SIZE = 65536;
const int MIN_TILESET_SIZE = 11;

enum ResultCode {
	ResultCode_nothing = 0,
	ResultCode_tooBig = -1,
	ResultCode_tooSmall = 1,
	ResultCode_periodic = 2
};

//�������, ������� ��������� ����� (����� �� ������������ ��� ����� ���� � ����� � �������� ��������)
//�������, ��� ������ ���� � ��� ������ ���������� ������ "���������" - �� ��������� ������,
//� ������� ������ � ������ ����, ������� ����� ���������� ����������
Color MixAB(Color a, Color b) {
	return (a + 8 * b);
}

struct Tileset {
	size_t size;
	Color maxColor;

	std::vector<Tile> tiles;

	Tileset(size_t maxSize = MAX_TILESET_SIZE) {
		size = 0;
		maxColor = 0;
		tiles = std::vector<Tile>(maxSize);
	}

	void Clear() {
		size = 0;
		maxColor = 0;
	}

	const Tile& operator [] (int i) const {
		return tiles[i];
	}

	size_t Size() {
		return size;
	}

	const Tileset& operator = (const Tileset& source) {
		if (this != &source) {
			size = source.size;
			maxColor = source.maxColor;
			//���� ������ ������� �������, �������� ������
			if (size > tiles.size()) {
				tiles = std::vector<Tile>(source.size);
			}
			for (unsigned int i = 0; i < size; i++)
			{
				tiles[i] = source.tiles[i];
			}
		}
		return *this;
	}

	bool operator == (const Tileset& other) const {
		if (this->size != other.size) {
			return false;
		}

		for (int i = 0; i < size; i++)
		{
			if (this->tiles[i] != other.tiles[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator < (const Tileset& other) const {
		if (this->size != other.size) {
			return this->size < other.size;
		}

		for (int i = 0; i < size; i++)
		{
			if (this->tiles[i] != other.tiles[i]) {
				return this->tiles[i] < other.tiles[i];
			}
		}
		return false;
	}

	void Swap(Tileset& other) {
		int tmpSize = this->size;
		this->size = other.size;
		other.size = tmpSize;

		int tmpMaxColor = this->maxColor;
		this->maxColor = other.maxColor;
		other.maxColor = tmpMaxColor;

		this->tiles.swap(other.tiles);
	}

	void Print() {
		for (int i = 0; i < size; i++)
		{
			std::cout << tiles[i][0] << " " << tiles[i][1] << " " << tiles[i][2] << " " << tiles[i][3] << " | ";
		}
		std::cout << " size: " << size << " maxColor: " << maxColor << std::endl;
	}

	//��������������, ��� ��� ����� � ��������� �� 0 �� ������-�� �����, ������ ��� ��������
	int CountHorizontalColors() const {
		int count = 0;
		for (int i = 0; i < size; i++)
		{
			if (count <= tiles[i].east()) {
				count = tiles[i].east() + 1;
			}
			if (count <= tiles[i].west()) {
				count = tiles[i].west() + 1;
			}
		}

		return count;
	}

	//��������������, ��� ��� ����� � ��������� �� 0 �� ������-�� �����, ������ ��� ��������
	int CountVerticalColors() const {
		int count = 0;
		for (int i = 0; i < size; i++)
		{
			if (count <= tiles[i].north()) {
				count = tiles[i].north() + 1;
			}
			if (count <= tiles[i].south()) {
				count = tiles[i].south() + 1;
			}
		}

		return count;
	}

	void AddTile(const Tile& t) {
		if (maxColor <= t.north()) {
			maxColor = t.north() + 1;
		}
		if (maxColor <= t.south()) {
			maxColor = t.south() + 1;
		}

		if (tiles.size() == size) {
			tiles.push_back(t);
		}
		else
		{
			tiles[size] = t;
		}
		//� ��������� ������-�� ������ ������������� ������ � ��� ������, ����� ���� ����������� �� ��� ������������ �����... ����������� ��, ������...
		size++;
	}

	void AddTileset(const Tileset& other) {
		for (int i = 0; i < other.size; i++)
		{
			AddTile(other[i]);
		}
	}

	//������������� ������������ ����� �� ���, ��� ���� � ��, ��� ������� mapping
	void RemapColors(const std::map<Color, Color>& mapping) {
		for (int i = 0; i < size; i++)
		{
			Color newNorthColor = mapping.at(tiles[i].north());
			Color newSouthColor = mapping.at(tiles[i].south());
			tiles[i] = Tile(newNorthColor, newSouthColor, tiles[i].west(), tiles[i].east());
		}
	}

	void Sort() {
		std::sort(tiles.begin(), tiles.end());
	}

	bool IsSorted() const {
		for (int i = 1; i < size; i++)
		{
			if (tiles[i] < tiles[i - 1]) {
				return false;
			}
		}

		return true;
	}

	void RemoveDuplicate() {
		//���� ������ 0, �� ������ �������
		if (size == 0) {
			return;
		}

		//������� ���������, ����� ��� ���������� ����� ���� �����
		Sort();

		//����� ����� �������� ���-�� ���������� ������
		int newSize = 1;

		/* ������ ���� �� ���� ������. ���� ������� ����������, �� ���������� ��� ��� �� "������" ���� ������������
		 * �� ���� �������� ��� ���, ����� � ����� � ������ ������� ���� ������ ����������. 
		 * �� ��, ��� � ����� ������� ���-�� �������� ��������, �.�. ��� ��� �� �����. */
		for (int i = 1; i < size; i++)
		{
			if (tiles[newSize - 1] != tiles[i]) {
				if (newSize != i) {
					tiles[newSize] = tiles[i];
					newSize++;
				}
			}
		}

		size = newSize;
	}

	//��������� ���������� ���� ������� T � Q.
	ResultCode Composition(const Tileset& Q, const Tileset& T) {
		this->size = 0;
		std::set<Color> south, north;

		//���� ��� ��������� ����� �������. ������ ������� ���-�� ������
		for (int i = 0; i < Q.size; i++)
		{
			for (int j = 0; j < T.size; j++)
			{
				if (Q[i].west() == T[j].east()) {
					Color mixedNorth = MixAB(T[j].north(), Q[i].north());
					north.insert(mixedNorth);

					Color mixedSouth = MixAB(T[j].south(), Q[i].south());
					south.insert(mixedSouth);

					this->size++;
				}
			}
		}

		//������� ��� �����, � ������� ��� ������� ��� ������ ������
		int oldSize = 0;
		do {
			oldSize = size;
			std::set<Color> newSouth, newNorth;

			size = 0;
			//����� ���������� ��� ����� � ���������, ����� ������ ����� ���� ���� ��� ���������� ������, 
			//� ������ ������ - ���� ��� �����. ���� ��� �� ���, �� �� ��������� ���� � ����� �����
			for (int i = 0; i < Q.size; i++)
			{
				for (int j = 0; j < T.size; j++)
				{
					if (Q[i].west() == T[j].east()) {
						Color mixedNorth = MixAB(T[j].north(), Q[i].north());
						if (south.find(mixedNorth) == south.end()) {
							continue;
						}

						Color mixedSouth = MixAB(T[j].south(), Q[i].south());
						if (north.find(mixedSouth) == north.end()) {
							continue;
						}

						newSouth.insert(mixedSouth);
						newNorth.insert(mixedNorth);

						size++;
					}
				}
			}


			north.swap(newNorth);
			south.swap(newSouth);
		} while (oldSize != size);

		if (size <= south.size()) {
			if (DEBUG) {
				//std::cout << "������: " << size << "\n���������� ������: " << south.size() << std::endl;
			}
			return ResultCode_tooSmall;
		}

		if (size > tiles.size()) {
			if (DEBUG) {
				//std::cout << "������: " << size << "\n���������� ������: " << south.size() << std::endl;
			}
			return ResultCode_tooBig;
		}

		//� ������ ��� ���������� �����
		size = 0;
		maxColor = south.size();

		for (int i = 0; i < Q.size; i++)
		{
			for (int j = 0; j < T.size; j++)
			{
				if (Q[i].west() == T[j].east()) {
					Color mixedNorth = MixAB(T[j].north(), Q[i].north());
					if (south.find(mixedNorth) == south.end()) {
						continue;
					}

					Color mixedSouth = MixAB(T[j].south(), Q[i].south());
					if (south.find(mixedSouth) == south.end()) {
						continue;
					}

					//������������� ������ ������. ����� ��� ���� �� ���� �� ������������� ����� ��� "�����"
					//������� ��������� ������ ����� ��� ����� � ���������
					Color southIndex = std::distance(south.begin(), south.lower_bound(mixedSouth));
					Color northIndex = std::distance(south.begin(), south.lower_bound(mixedNorth));

					tiles[size] = Tile(northIndex, southIndex, T[j].west(), Q[i].east());
					size++;
				}
			}
		}

		if (size < MIN_TILESET_SIZE) {
			if (DEBUG) {
				//std::cout << "������: " << size << "\n���������� ������: " << south.size() << std::endl;
			}
			return ResultCode_tooSmall;
		}

		return ResultCode_nothing;
	}

	//����������� Composition, �� �� ������� �����, � ������� ��� �������. ������ ��� ��� ������� ��������� ��������.
	ResultCode CompositionNoSimplification(const Tileset& Q, const Tileset& T) {
		size = 0;
		std::set<Color> colors;

		//��� ������ �������� ��������� ������
		for (int i = 0; i < Q.size; i++)
		{
			for (int j = 0; j < T.size; j++)
			{
				if (Q[i].west() == T[j].east()) {
					Color mixedNorth = MixAB(T[j].north(), Q[i].north());
					colors.insert(mixedNorth);

					Color mixedSouth = MixAB(T[j].south(), Q[i].south());
					colors.insert(mixedSouth);

					this->size++;
				}
			}
		}

		//� ����� ������ �������� ������� � ������������� �����
		for (int i = 0; i < Q.size; i++)
		{
			for (int j = 0; j < T.size; j++)
			{
				if (Q[i].west() == T[j].east()) {
					Color mixedNorth = MixAB(T[j].north(), Q[i].north());
					Color mixedSouth = MixAB(T[j].south(), Q[i].south());
					
					Color southIndex = std::distance(colors.begin(), colors.lower_bound(mixedSouth));
					Color northIndex = std::distance(colors.begin(), colors.lower_bound(mixedNorth));

					tiles[size] = Tile(northIndex, southIndex, T[j].west(), Q[i].east());
					size++;
				}
			}
		}
		if (size < MIN_TILESET_SIZE) {
			return ResultCode_tooSmall;
		}

		return ResultCode_nothing;
	}

	//���� ���������� � ������� edges ��������, � �������� ������ ������� - start. ���������� ��� ������
	static int BinSearch(const std::vector<std::pair<Color, Color>> edges, Color start) {
		int lowerBound = 0;
		int upperBound = edges.size();

		while (upperBound > lowerBound)
		{
			int median = (lowerBound + upperBound) / 2;
			if (edges[median].first >= start) {
				upperBound = median;
			}
			else
			{
				lowerBound = median + 1;
			}
		}

		return lowerBound;
	}

	//������������ ��� �������� ������� �������. ���������� ������� ����� � ������ �����
	static bool HasCycle(std::vector<std::pair<Color, Color>> edges, Color maxColor) {
		std::sort(edges.begin(), edges.end());

		for (Color i = 0; i < maxColor; i++)
		{
			std::set<int> wasHere;
			std::list<int> toSee;

			toSee.push_back(i);

			while (!toSee.empty())
			{
				int current = toSee.front();
				toSee.pop_front();

				if (wasHere.find(current) != wasHere.end()) {
					continue;
				}

				wasHere.insert(current);

				int startIndex = BinSearch(edges, current);
				int currentIndex = startIndex;
				while (currentIndex < edges.size() && edges[currentIndex].first == current)
				{
					Color next = edges[currentIndex].second;

					if (next == i) {
						return true;
					}
					if (wasHere.find(next) == wasHere.end()) {
						toSee.push_back(next);
					}

					currentIndex++;
				}
			}
		}

		return false;
	}

	//true, ���� �������������� �������������, false, ���� �� ��������������
	//����������� ��������, ������� ��������� �� ������� "�������" �������� (��� ������� � ������ ����� �����) (�� ��������� ������)
	bool IsPeriodic() const {
		//������������� ����� - ��� ��, ��� ������� � ������ ����� ���������
		std::vector<std::pair<Color, Color>> periodicTiles;

		periodicTiles.clear();
		for (int i = 0; i < size; i++)
		{
			if (tiles[i].west() == tiles[i].east()) {
				std::pair<Color, Color> periodic = std::make_pair(tiles[i].north(), tiles[i].south());
				periodicTiles.push_back(periodic);
			}
		}

		return HasCycle(periodicTiles, maxColor);
	}

	//������ �������������� � ������������ ����������� �� ���� ������ � ���������� ��������� (���� ����� �� ��������)
	Tileset Transposed() const {
		Tileset result = Tileset();
		for (int i = 0; i < this->size; i++)
		{
			Tile transposedTile = Tile(tiles[i].west(), tiles[i].east(), tiles[i].north(), tiles[i].south());
			result.AddTile(transposedTile);
		}

		return result;
	}
};

#endif
