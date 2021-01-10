#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <string>

class LevelLoader 
{
private:

	int tilesX;
	int tilesY;

	std::vector<std::vector<int>> levelArray;

public:

	LevelLoader();

	void loadLevel(std::string levelPath);
	std::vector<std::vector<int>> getLevel();

	int getTilesX();
	int getTilesY();

};