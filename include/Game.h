#pragma once

#include <iostream>
#include <set>
#include <cmath>
#include <vector>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <stdlib.h>
#include <chrono>
#include <thread>

#include "LevelLoader.h"
#include "GLWindow.h"
#include "Map.h"
#include "Pellets.h"
#include "Camera.h"
#include "Ghost.h"
#include "FrameBuffer.h"

#include "Material.h"
#include "Renderer.h"

#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"

class Game 
{

private:

	GLfloat deltaTime;
	GLfloat lastTime;
	GLfloat time;
	GLfloat now;

	int numberOfGhosts;

	std::unique_ptr<Map> map;
	std::unique_ptr<Pellets> pellets;
	std::unique_ptr<FrameBuffer> frameBuffer;

	std::vector<std::unique_ptr<Ghost>> ghosts;

	std::shared_ptr<Renderer> renderer;

	std::shared_ptr<VertexArray>		minimapVAO;
	std::shared_ptr<VertexBuffer>		minimapVBO;
	std::shared_ptr<VertexBufferLayout> minimapVBLayout;

	std::shared_ptr<Shader> shader;
	std::shared_ptr<Shader> minimapShader;
	std::shared_ptr<Shader> screenShader;
	std::shared_ptr<Shader> pelletShader;
	std::shared_ptr<Shader> pelletMinimapShader;

	std::shared_ptr<Camera> camera;

	std::shared_ptr<GLWindow> mainWindow;

	std::shared_ptr<Material> wallMat;
	std::shared_ptr<Material> floorMat;
	std::shared_ptr<Material> minimapFloorMat;
	std::shared_ptr<Material> wallMinimapMat;
	std::shared_ptr<Material> pelletSpec;

	std::shared_ptr<LevelLoader> levelLoader;
	
	std::shared_ptr<DirectionalLight> mapLight;
	std::shared_ptr<DirectionalLight> pelletLight;

	PointLight pointLights[15];
	SpotLight spotLights[3];

	GLuint uniformModel;
	GLuint uniformView;
	GLuint uniformProjection;
	GLuint uniformEyePosition;
	GLuint uniformAmbientIntensity;
	GLuint uniformDiffuseIntensity;
	GLuint uniformSpecularIntensity;
	GLuint uniformShininess;

	GLuint uniformMinimapTexture;

	GLuint uView;
	GLuint uProj;
	GLuint uMod;

	glm::mat4 pelletProj;
	glm::mat4 pelletView;
	glm::mat4 pelletMinimapView;

	glm::mat4 model;
	glm::mat4 projection;
	glm::mat4 projectionMinimap;

	glm::vec3 map_pos;
	glm::vec3 floor_pos;
	glm::vec3 pellets_pos;
	glm::vec3 startingPos;

	std::vector<std::vector<int>> levelArrayData;

	glm::vec3 lowerLight;

public:

	Game();
	~Game();

	void generateGame(std::shared_ptr<GLWindow>& mainWindow);
	void updateGame(std::shared_ptr<GLWindow>& mainWindow);
	void updateMinimap();

	void generateShaders();
	void generateLights();
	void generateMinimap(std::shared_ptr<GLWindow>& mainWindow);

	void generateMVP();
	void generateMinimapMVP();

	void updateLights();
	void updateMVP();
	void updateMinimapMVP();
	void updateTime();

};
