#pragma once

#include <glm/gtc/matrix_transform.hpp>

#include "LevelLoader.h"
#include "GLWindow.h"
#include "Material.h"
#include "Shader.h"
#include "Camera.h"
#include "Renderer.h"

/*Objects that make up the map - Floor, walls*/

class Map
{
private:

	GLfloat startX;
	GLfloat endX;
	GLfloat startZ;
	GLfloat endZ;

	int tilesX;
	int tilesZ;

	glm::vec3 wallPos;
	glm::vec3 floorPos;

	GLuint uniformModel;
	GLuint uniformView;
	GLuint uniformProjection;

	enum class Wall { UP, DOWN, LEFT, RIGHT };

	std::unique_ptr<Shader> shader;

	std::vector<glm::vec3> wallPositions;
	std::vector <unsigned int> wallIndices;
	std::vector<GLuint> indices;
	std::vector<GLfloat> vertices;
	std::vector<std::vector<int>> levelArray;

	glm::vec3 startingPlayerPos;

	std::shared_ptr<VertexArray>		mapVAO;
	std::unique_ptr<VertexBuffer>		mapVBO;
	std::unique_ptr<VertexBufferLayout> mapVBLayout;
	std::shared_ptr<IndexBuffer>		mapIBO;

	std::shared_ptr<VertexArray>		floorVAO;
	std::unique_ptr<VertexBuffer>		floorVBO;
	std::unique_ptr<VertexBufferLayout> floorVBLayout;
	std::shared_ptr<IndexBuffer>		floorIBO;

	std::shared_ptr<Renderer> mapRenderer;

	std::unique_ptr <Material> floorMat;
	std::unique_ptr <Material> wallMat;

	std::unique_ptr <Material> minimapFloorMat;
	std::unique_ptr <Material> minimapWallMat;

public:

	Map(std::shared_ptr<GLWindow>& mainWindow);
	~Map();

	void generateWall(Wall buildDirection, int x, int z, int numberOfWalls);
	void generateMap(std::string levelPath, std::shared_ptr<GLWindow>& mainWindow);
	void generateFloor(int x, int y);

	void draw(glm::mat4 model, glm::mat4 projection, std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader);
	void drawMinimap(glm::mat4 model, glm::mat4 projection, std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader);

	std::vector<std::vector<int>> getLevelArray();

	glm::vec3 getStartingPosition();
};