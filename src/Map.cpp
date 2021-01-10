#include <math.h> 

#include "Map.h"

/**
*  Map class that loads it's level with LevelLoader and constructs the map.
*  This algorithm creates "wall faces". This means no two walls are necessarily the same.
*  Each wall is created for a specific direction.
*
*  @name Map.cpp
*  @author(s) Elvis Arifagic, Jørgen Eriksen, Salvador Bascunan.
*/

/**
*   Default constructor for map objects. Generates the walls and floor. Takes in the level file and 
*	generates the texture objects for wall and floor.
*
*   @param mainWindow - Window to generate to.
* 
*	@see   generateMap(), generateFloor(), getTexture(), loadTextureA().
*/
Map::Map(std::shared_ptr<GLWindow>& mainWindow)
	: wallPos(0), floorPos(0)
{
	generateMap("assets/levels/level0", mainWindow);
	generateFloor(tilesX*2, tilesZ*2);

	wallMat = std::make_unique<Material>();
	wallMat->getTexture("assets/textures/wall_tex.png");
	wallMat->loadTextureA();

	floorMat = std::make_unique<Material>();
	floorMat->getTexture("assets/textures/floor_tex.png");
	floorMat->loadTextureA();

	minimapFloorMat = std::make_unique<Material>();
	minimapFloorMat->getTexture("assets/textures/plain_floor.png");
	minimapFloorMat->loadTextureA();

	minimapWallMat = std::make_unique<Material>();
	minimapWallMat->getTexture("assets/textures/plain_walls.png");
	minimapWallMat->loadTextureA();
}

/**
*   Destructor for map object
*/
Map::~Map()
{

}

/**
*   Generate the different wall faces.
*
*   @param buildDirection - What face to make.
*   @param x			  - location of build start in X.
*   @param z              - location of build start in Z.
*   @param numberOfWalls  - Amount of walls constructed.
*/
void Map::generateWall(Wall buildDirection, int x, int z, int numberOfWalls) 
{
	if (buildDirection == Wall::UP)
	{
		startX = x * 2;
		endX = x * 2 + 2;
		startZ = z * 2;
		endZ = z * 2;
	} 

	else if (buildDirection == Wall::DOWN)
	{
		startX = x * 2;
		endX = x * 2 + 2;
		startZ = z * 2 + 2;
		endZ = z * 2 + 2;
	} 

	else if (buildDirection == Wall::LEFT)
	{
		startX = x * 2;
		endX = x * 2;
		startZ = z * 2;
		endZ = z * 2 + 2;
	}

	else if (buildDirection == Wall::RIGHT)
	{
		startX = x * 2 + 2;
		endX = x * 2 + 2;
		startZ = z * 2;
		endZ = z * 2 + 2;
	}

	std::vector<GLfloat> verticePlaceHolder = {
		  startX,	0.0f,	startZ,     0.0f,  0.0f,	    0.0f,   -1.0f,  0.0f,  // 0
		  startX,	2.0f,	startZ,     0.0f,  1.0f,	    0.0f,   -1.0f,  0.0f,  // 1
		  endX,		0.0f,	endZ,		1.0f,  0.0f,	    0.0f,   -1.0f,  0.0f,  // 2
		  endX,		2.0f,	endZ,		1.0f,  1.0f,	    0.0f,   -1.0f,  0.0f,  // 3
	};

	vertices.insert(end(vertices), verticePlaceHolder.begin(), verticePlaceHolder.end());

	GLuint num = numberOfWalls * 4;
	std::vector<GLuint> indicesPlaceHolder = {
	   num, num + 1, num + 2,
	   num + 1, num + 3, num + 2
	};
	indices.insert(end(indices), indicesPlaceHolder.begin(), indicesPlaceHolder.end());
}

/**
*   Generate all the different wall faces making up the map, calculates the normals for walls
*	and loads the vertices and indices to create the mesh.
*
*   @param levelPath  - what it should build based on
*   @param mainWindow - the window to generate on.
* 
*	@see calcAverageNormals(), loadMesh()
*/
void Map::generateMap(std::string levelPath, std::shared_ptr<GLWindow>& mainWindow)
{
	LevelLoader levelLoader;
	levelLoader.loadLevel(levelPath);

	levelArray = levelLoader.getLevel();
	tilesX = levelLoader.getTilesX();
	tilesZ = levelLoader.getTilesY();

	int numberOfWalls = 0;
	
	//To avoid that the calculation will overflow the original (smaller) type before conversion 
	//to the result (larger) type we can either cast to double or use unsigned long int
	for (unsigned long int z = 0; z < tilesZ; z++)
	{
		for (unsigned long int x = 0; x < tilesX; x++)
		{
			if (levelArray[z][x] == 0 || levelArray[z][x] == 2)
			{
				if (levelArray[z - 1][x] == 1)
				{
					generateWall(Wall::UP, x, z, numberOfWalls);
					numberOfWalls++;
				}
			
				if (levelArray[z + 1][x] == 1)
				{
					generateWall(Wall::DOWN, x, z, numberOfWalls);
					numberOfWalls++;
				}

				if (x > 0) 
				{
					if (levelArray[z][x - 1] == 1)
					{
						generateWall(Wall::LEFT, x, z, numberOfWalls);
						numberOfWalls++;
					}
				}
				
				if (x < 27) 
				{
					if (levelArray[z][x + 1] == 1)
					{
						generateWall(Wall::RIGHT, x, z, numberOfWalls);
						numberOfWalls++;
					}
				}
			}

			if (levelArray[z][x] == 2) // player starting position
			{
				startingPlayerPos = glm::vec3(x*2+1, 1.0f, z*2+1);
			}
		}
	}

	shader->calculateAverageNormals(indices, indices.size(), vertices, vertices.size(), 8, 5);

	mapVAO = std::make_shared<VertexArray>();
	mapVAO->bind();
	
	mapVBO = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(GLfloat));
	mapVBO->bind();
	
	mapVBLayout = std::make_unique<VertexBufferLayout>();
	mapVBLayout->Push<float>(3);
	mapVBLayout->Push<float>(2);
	mapVBLayout->Push<float>(3);
	
	mapVAO->addBuffer(*mapVBO, *mapVBLayout);
	mapIBO = std::make_shared<IndexBuffer>(&indices[0], indices.size());
}

void Map::generateFloor(int x, int y)
{
	float tilesX = static_cast<float>(x);
	float tilesY = static_cast<float>(y);

	//Indices for the floor vertices
	std::vector<GLuint> indices =
	{
		0, 1, 2,
		1, 3, 2
	};

	//Vertices for the floor - Position - UV texCoord - Normals
	std::vector<GLfloat> vertices =
	{
		// x        y       z          u     v        nx     ny    nz
		 0.0f,    0.0f,    tilesY,    0.0f,  0.0f,	 0.0f, -1.0f, 0.0f, // 0
		 tilesX,  0.0f,    tilesY,    25.0f, 0.0f,	 0.0f, -1.0f, 0.0f, // 1
		 0.0f,    0.0f,    0.0f,      0.0f,  25.0f,	 0.0f, -1.0f, 0.0f, // 2
		 tilesX,  0.0f,    0.0f,      25.0f, 25.0f,  0.0f, -1.0f, 0.0f  // 3
	};

	//Calculate the normals of the object. 
	shader->calculateAverageNormals(indices, 6, vertices, 32, 8, 5);

	floorVAO = std::make_shared<VertexArray>();
	floorVAO->bind();

	floorVBO = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(GLfloat));
	floorVBO->bind();

	floorVBLayout = std::make_unique<VertexBufferLayout>();
	floorVBLayout->Push<float>(3);
	floorVBLayout->Push<float>(2);
	floorVBLayout->Push<float>(3);

	floorVAO->addBuffer(*floorVBO, *floorVBLayout);
	floorIBO = std::make_shared<IndexBuffer>(&indices[0], indices.size());
}

/**
*   Utility getter for the map data.
*
*   @return vector - contains map tiles.
*/
std::vector<std::vector<int>> Map::getLevelArray()
{
	return levelArray;
}

/**
*   Gives the tunnel position for the camera.
*
*   @return glm::vec3 - location in world space for camera.
*/
glm::vec3 Map::getStartingPosition()
{
	return startingPlayerPos;
}

/**
*   Draw the map object and use the camera, texture, model, projection, shader and position from the Game class. 
*
*   @param  model        - Sends the model matrix from the Game class
*   @param  projection   - Sends the projection matrix from the Game class
*   @param  camera		 - Sends the camera object from the Game class
*   @param  shader		 - Sends the shader object from the Game class
* 
*	@see useShader(), getModelLocation(), getProjectionLocation(), getViewLocation(),useTexture(), drawElements()
* 
*/
void Map::draw(glm::mat4 model, glm::mat4 projection, std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader)
{
	shader->useShader();

	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	model = glm::translate(model, glm::vec3(wallPos));
	wallMat->useTexture();
	mapRenderer->drawElements(mapVAO, mapIBO);

	model = glm::translate(model, glm::vec3(floorPos));
	floorMat->useTexture();
	mapRenderer->drawElements(floorVAO, floorIBO);
}

/**
*   Draw the map object for the minimap and use the camera, texture, model projection, shader and position from the Game class. 
*
*   @param  model        - Sends the model matrix from the Game class
*   @param  projection   - Sends the projection matrix from the Game class
*   @param  camera		 - Sends the camera object from the Game class
*   @param  shader		 - Sends the shader object from the Game class
*
*	@see useShader(), getModelLocation(), getProjectionLocation(), getViewLocation(),useTexture(), drawElements()
*
*/
void Map::drawMinimap(glm::mat4 model, glm::mat4 projection, std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader)
{
	shader->useShader();

	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateMinimapView()));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	model = glm::translate(model, glm::vec3(wallPos));
	minimapWallMat->useTexture();
	mapRenderer->drawElements(mapVAO, mapIBO);

	model = glm::translate(model, glm::vec3(floorPos));
	minimapFloorMat->useTexture();
	mapRenderer->drawElements(floorVAO, floorIBO);
}
