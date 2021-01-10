#include "LevelLoader.h"

/**
*  LevelLoader is the class that reads in the level data.
*  This defines the map and is dynamic. If tile amount was to change, the game
*  would adapt accordingly. X and Y sizes are stored in regular variables.
*
*  @name LevelLoader.cpp
*  @author(s) Elvis Arifagic, Jørgen Eriksen, Salvador Bascunan.
*/

/**
*   Default constructor for levelLoader objects.
*/
LevelLoader::LevelLoader()
	: tilesX(0), tilesY(0)
{

}

/**
*   Retrieve the level from the file given through the game.
*
*   @param levelPath - path of the level file in the system.
*/
void LevelLoader::loadLevel(std::string levelPath) 
{
	std::ifstream inFile;
	inFile.open(levelPath); // from parameter
	if (!inFile) 
	{
		std::cerr << "Unable to open file level0";
		exit(1);   // call system to stop
	}

	inFile >> tilesX;
	inFile.ignore();
	inFile >> tilesY;

	levelArray.resize(tilesY, std::vector<int>(tilesX));

	int x = 0;
	int y = 0;
	int n;

	while (inFile >> n) 
	{
		if (x % tilesX == 0 && x != 0) // if new line
		{
			y++;	
			x = 0; // resets x
		}
		levelArray[y][x] = n;
		x++;
	}
}

/**
*   Getter for diverse use throughout the engine.
*/
std::vector<std::vector<int>> LevelLoader::getLevel()
{
	return levelArray;
}

/**
*   Getter for value of tiles in X direction.
*
*   @return int - x value.
*/
int LevelLoader::getTilesX()
{
	return tilesX;
}

/**
*   Getter for value of tiles in Y direction.
*
*   @return int - y value.
*/
int LevelLoader::getTilesY()
{
	return tilesY;
}


