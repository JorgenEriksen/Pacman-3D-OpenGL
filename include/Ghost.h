#pragma once

#include <glm/gtc/type_ptr.hpp>
#include <time.h>
#include <random>
#include <chrono>

#include "GLWindow.h"
#include "Camera.h"
#include "Model.h"
#include "Material.h"
#include "Shader.h"

// global movement vectors
const glm::vec3 UP(0.0f,	0.0f,	-1.0f);
const glm::vec3 DOWN(0.0f,	0.0f,	1.0f);
const glm::vec3 LEFT(-1.0f, 0.0f,	0.0f); 
const glm::vec3 RIGHT(1.0f, 0.0f,	0.0f);
const glm::vec3 NONE(0.0f,	0.0f,	0.0f);

class Ghost
{
private:

	std::unique_ptr<Model> ghostModel;
	std::unique_ptr<Material> ghostMat;
	std::unique_ptr<Shader> shader;

	glm::vec3 velocity;
	glm::vec3 position;

	int calculatedDirectionPosX;
	int calculatedDirectionPosZ;

	int aiValue;

	std::vector<std::vector<int>> levelArray;

	std::vector<glm::vec3> validGhostPositions;

	GLuint uniformSpecularIntensity;
	GLuint uniformShininess;

	GLuint uniformModel;
	GLuint uniformView;
	GLuint uniformProjection;

public:
	
	Ghost(std::vector<std::vector<int>> levelArrayData, int index);
	Ghost();
	~Ghost();

	void generateSpawnPositions();
	glm::vec3 randomSpawnPosition();
	glm::vec3 startVelocity();

	void calculateAiDirection(glm::vec3 pacmanPosition);
	int calculateRandomNumber(int highestRandomNumber);
	void move(float dt, glm::vec3 pacmanPosition);

	void generateGhost();
	bool checkCameraCollision(std::shared_ptr<Camera>& camera);
	void draw(std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader, glm::mat4 model, glm::mat4 projection);
	void drawMinimap(std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader, glm::mat4 model, glm::mat4 projection);

	bool isWall(glm::vec3 direction);

	
	float distance(int pos1, int pos2);
	float distance2D(glm::vec3 vector, glm::vec3 vector2);


	int getUnitLevelArrayIndexX();
	int getUnitLevelArrayIndexZ();

};
