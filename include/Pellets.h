#pragma once

#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "DirectionalLight.h"

#include "GLWindow.h"
#include "Shader.h"
#include "Camera.h"

#include "Material.h"
#include "Renderer.h"

#include "Model.h"

class Pellets {

private:

	std::unique_ptr<Model> pelletModel;
	std::unique_ptr<Material> pelletSpec;
	std::unique_ptr<Material> minimapPelletSpec;

	std::shared_ptr<VertexArray> instancedVAO;
	std::shared_ptr<VertexBuffer> instancedVBO;
	std::shared_ptr<VertexBufferLayout> instancedVBLayout;

	std::shared_ptr<Shader> shader;

	int numPellets;
	int numPelletsEaten;

	float x;
	float z;

	std::vector<std::vector<int>> levelArray;
	std::vector<glm::vec3> pelletsPositions;

	glm::mat4 projection;
	glm::mat4 model;
	glm::vec3 position;

	glm::mat4* modelMatrices; // Raw because of.

	GLuint uniformStoneTex = 0;
	GLuint uniformSpecularIntensity;
	GLuint uniformShininess;

	GLuint uView;
	GLuint uProj;
	GLuint uMod;

public:

	Pellets(std::vector<std::vector<int>> levelArrayData, std::shared_ptr<GLWindow>& mainWindow);

	void generatePellets();
	void checkPelletsCollision(glm::vec3 playerPosition);
	bool allPelletsEaten();

	void draw(std::shared_ptr<Shader>& pelletShader);
	void drawMinimap(std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& pelletShader, glm::mat4 projection);

	inline int getNumPellets() { return numPellets; }
	inline int getNumPelletsEaten() { return numPelletsEaten; }
};