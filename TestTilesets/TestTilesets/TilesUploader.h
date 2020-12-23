#ifndef TILES_UPLOADER
#define TILES_UPLOADER

#include "Tileset.h"
#include <fstream>

Tileset Upload(std::string path) {
	std::ifstream file;
	file.open(path, std::ios_base::in);

	int size;
	file >> size;
	
	Tileset result;

	for (int i = 0; i < size; i++)
	{
		Tile toAdd = Tile();

		for (int j = 0; j < 4; j++)
		{
			file >> toAdd[j];
		}

		result.AddTile(toAdd);
	}

	file.close();

	return result;
}

#endif