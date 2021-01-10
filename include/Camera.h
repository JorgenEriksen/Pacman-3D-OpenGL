#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GLFW/glfw3.h>
#include <glm/gtx/rotate_vector.hpp>

#include <iostream>
#include <vector>

#include "Renderer.h"
#include "Material.h"
#include "Shader.h"

class Camera
{

private:

	glm::vec3 position;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 worldUp;

	GLfloat yaw;
	GLfloat pitch;

	GLfloat moveSpeed;
	GLfloat turnSpeed;

	std::vector<std::vector<int>> levelArray;

	std::shared_ptr<VertexArray>		miniMapPacmanVAO;
	std::unique_ptr<VertexBuffer>		miniMapPacmanVBO;
	std::unique_ptr<VertexBufferLayout> miniMapPacmanVBLayout;
	std::shared_ptr<IndexBuffer>		miniMapPacmanIBO;

	std::shared_ptr<Material> miniMapPacmanMat;
	std::shared_ptr<Renderer> miniMapPacmanRenderer;

	std::shared_ptr<Shader> shader;

	GLuint uniformModel;
	GLuint uniformView;
	GLuint uniformProjection;

	enum class direction {
		UP, DOWN, LEFT, RIGHT
	};

	void update();

public:

	Camera(std::vector<std::vector<int>> levelArrayData, glm::vec3 startPosition, glm::vec3 startUp, 
		      GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed);
	~Camera();

	void keyControls(bool* keys, GLfloat deltaTime);
	void mouseControl(GLfloat changeX, GLfloat changeY);

	glm::vec3 getCameraPosition();
	glm::vec3 getCameraDirection();

	glm::mat4 calculateViewMatrix();
	glm::mat4 calculateMinimapView();

	void drawMiniMapPacman(glm::mat4 projection, std::shared_ptr<Shader>& shader);

	bool checkWallCollision(glm::vec3 unitDirection, GLfloat speed);


};

