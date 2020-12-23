#ifndef TILE
#define TILE
#include <array>

typedef unsigned int Color;

class Tile {
private:
	std::array<Color, 4> colors;

public:
	const Color& north() const {
		return colors[0];
	}

	const Color& south() const {
		return colors[1];
	}

	const Color& west() const {
		return colors[2];
	}

	const Color& east() const {
		return colors[3];
	}

	Tile() {
		colors = std::array<Color, 4>();
		for (int i = 0; i < 4; i++)
		{
			colors[i] = 0;
		}
	}

	Color operator[] (int i) const {
		return colors[i];
	}

	Color& operator[] (int i) {
		return colors[i];
	}

	bool operator < (const Tile& other) const {
		//Ищем первое различие. Если в месте различия второй цвет больше, чем первый, то весь тайлсет считается больше первого
		//По сути сам принцип не оч важен. Важнее наличие порядка
		for (int i = 0; i < 4; i++)
		{
			if (other.colors[i] > this->colors[i]) {
				return true;
			}
			if (other.colors[i] < this->colors[i]) {
				return false;
			}
		}

		return false;
	}

	bool operator == (const Tile& other) const {
		for (int i = 0; i < 4; i++)
		{
			if (other.colors[i] != this->colors[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator != (const Tile& other) const {
		return !(*this == other);
	}

	Tile(Color north, Color south, Color west, Color east) {
		colors = std::array<Color, 4>();
		colors[0] = north;
		colors[1] = south;
		colors[2] = west;
		colors[3] = east;
	}
};

#endif