#include "Camera.h"

/**
*   This class defines the camera and the player. The game is in first person
*	which in effect makes the camera the player as well. Some cameras choose to implement
*	roll but that axis is not needed for this Pacman games. Camera is moved with the WASD together with mouse.
*	The player is also forbidden to overdo the pitch of the camera, which is locked at 89.
*
*  @name Camera.cpp
*  @author(s) Elvis Arifagic, Jørgen Eriksen, Salvador Bascunan.
*/

/**
*   Constructor for camera object. This is the one acts as the default for any object.
*	Roll is not needed for this game.
*
*   @param     levelArrayData - Vector with 1's and 0's that make up the map.
*   @param     startPosition  - Where the camera is initially located in the world space.
*   @param     startUp        - Direction of the world's up direction (+y is up).
*   @param     startYaw       - Giving the camera a non-turned view to start.
*   @param     startPitch     - How much the camera looks up and or down when starting.
*   @param     startMoveSpeed - Movement speed of the camera
*   @param     startTurnSpeed - Turn rate speed for the camera.
*
*	@see	   update()
*/
Camera::Camera(std::vector<std::vector<int>> levelArrayData, glm::vec3 startPosition, glm::vec3 startUp, GLfloat startYaw, GLfloat startPitch, GLfloat startMoveSpeed, GLfloat startTurnSpeed)
{
	position = startPosition; // initialize standard constructor way with passed user params.
	worldUp = startUp;
	yaw = startYaw;
	pitch = startPitch;
	front = glm::vec3(0.0f, 0.0f, -1.0f); // mostly abstracting lab03 code.

	moveSpeed = startMoveSpeed;
	turnSpeed = startTurnSpeed;

	levelArray = levelArrayData;

	update();

    std::vector<GLuint> indices = 
   {
        0, 1, 2, 
        1, 3, 2,
   };


   std::vector<GLfloat> vertices = 
    {
         //x        y       z          u     v       
         0.0f,    0.0f,    2.0f,      0.0f,  0.0f,	  // 0
         2.0f,    0.0f,    2.0f,      1.0f,  0.0f,	  // 1
		 0.0f,    0.0f,    0.0f,      0.0f,  1.0f,	  // 2
		 2.0f,    0.0f,    0.0f,      1.0f,  1.0f     // 3
   };

	miniMapPacmanVAO = std::make_shared<VertexArray>();
	miniMapPacmanVAO->bind();
       
	miniMapPacmanVBO = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(GLfloat));
	miniMapPacmanVBO->bind();
       
	miniMapPacmanVBLayout = std::make_unique<VertexBufferLayout>();
	miniMapPacmanVBLayout->Push<float>(3);
	miniMapPacmanVBLayout->Push<float>(2);

       
	miniMapPacmanVAO->addBuffer(*miniMapPacmanVBO, *miniMapPacmanVBLayout);
	miniMapPacmanIBO = std::make_shared<IndexBuffer>(&indices[0], indices.size());

	miniMapPacmanMat = std::make_shared<Material>();
	miniMapPacmanMat->getTexture("assets/textures/pacman_minimap.png");
	miniMapPacmanMat->loadTextureA();

	static const char* vShader = "assets/shaders/texture.vert";
	static const char* fShader = "assets/shaders/texture.frag";

	shader = std::make_shared<Shader>();
	shader->createShaderFromFile(vShader, fShader);

	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();
}

/**
*   Destructor for camera object. Cameras are used as smart pointers.
*
*/
Camera::~Camera() {}

/**
*   Users key control for movement and wall collision
*
*   @param     keys      - Pointer to relevant the ascii key inputs.
*   @param     deltaTime - Calculated delta time to have uniformity in frame-rate.
*/
void Camera::keyControls(bool* keys, GLfloat deltaTime) {

	GLfloat velocity = 4 * deltaTime;
	if (keys[GLFW_KEY_W]) 
	{
		glm::vec3 direction = front;
		if (!checkWallCollision(direction, velocity)) {
			position += front * velocity;
		}
	}

	if (keys[GLFW_KEY_S]) 
	{
		glm::vec3 direction = -front;
		if (!checkWallCollision(direction, velocity)) {
			position -= front * velocity;
		}
	}

	if (keys[GLFW_KEY_A]) 
	{
		glm::vec3 direction = -right;
		if (!checkWallCollision(direction, velocity)) {
			position -= right * velocity;
		}
	}

	if (keys[GLFW_KEY_D]) 
	{
		glm::vec3 direction = right;
		if (!checkWallCollision(direction, velocity)) {
			position += right * velocity;
		}
	}

	 //This creates the teleport from one end to the other.
	if (position.x < 0.6) {
		position.x = levelArray[0].size()*2-0.8f;
		
	}
	else if (position.x > levelArray[0].size()*2-0.6f) {
		position.x = 1;
	}
}

/**
*   Checks all the collision against the wall and the camera object.
*
*   @param     unitDirection - The vector which points from the direction the camera is moving.
*   @param     speed		 - Ghost speed
*   @return    bool			 - returns false if there was no collision.
*/
bool Camera::checkWallCollision(glm::vec3 unitDirection, GLfloat speed)
{

	float offset = 0.3f;
	int currentTileX = floor(position.x / 2);
	int projectionTileX = floor((position.x / 2) + unitDirection.x * (speed + offset));

	int currentTileZ = floor(position.z / 2);
	int projectionTileZ = floor((position.z / 2) + unitDirection.z * (speed + offset));


	if (projectionTileX < 0 || projectionTileZ < 0 || projectionTileX > levelArray[0].size()-1 || projectionTileZ > levelArray.size()-1) { // if about to teleport
		return false;
	}


	if (projectionTileZ != currentTileZ) // if unit is about to walk to a new z tile
	{
		if (levelArray[projectionTileZ][currentTileX] == 1)
		{
			return true;
		}
	}

	else if (projectionTileX != currentTileX)
	{
		if (levelArray[currentTileZ][projectionTileX] == 1) {
			return true;
		}
	}

	return false; // if no collision
}


/**
*   Makes the camera able to be moved dynamically in the world space.
*	The pitch is locked at 89 degrees such that you can't see behind yourself.
*
*   @param     changeX - Amount of mouse movement in X direction.
*   @param     changeY - Amount of mouse movement in y direction.
*   @see       update()
*/
void Camera::mouseControl(GLfloat changeX, GLfloat changeY) 
{
	changeX *= turnSpeed;
	changeY *= turnSpeed;

	yaw += changeX;
	pitch += changeY;

	if (pitch > 89.0f) 
	{
		pitch = 89.0f;
	} // lock the up-and down scroll of the mouse.

	if (pitch < -89.0f) 
	{
		pitch = -89.0f;
	}

	update();
}

/**
*   Retrieves the class data member for the position of the camera.
*
*	@return glm::vec3 - vector with the position
*/
glm::vec3 Camera::getCameraPosition()
{
	return position;
}

/**
*   Retrieves the direction the camera is looking.
*	Utilizing glm to get correct value.
*
*   @return glm::vec3 - vector containing "front" which is where the camera points.
*/
glm::vec3 Camera::getCameraDirection()
{
	return glm::normalize(front);
}

/**
*   Utilizes glm library to give the correct view matrix for the view space transformation.
*
*   @return glm::mat4 - returns view matrix for view space.
*/
glm::mat4 Camera::calculateViewMatrix() 
{
	//can place an object to lookAt where "position + front" is. Third-person view
	return glm::lookAt(position, position + front, up);
}

/**
*   Calculates the view matrix for the 3d projection of the map.
*
*   @return glm::mat4 - returns the view matrix.
*/
glm::mat4 Camera::calculateMinimapView()
{
	//For 3D minimap view
	glm::vec3 camPos(28, 73, 36);
	return glm::lookAt(camPos, camPos + glm::vec3(0, -90, 0), glm::vec3(0, -1, -1));
}

/**
*   Update function which in real time updates values making the camera move.
*/
void Camera::update() 
{
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	//front.y = sin(glm::radians(pitch)); // if you want to fly, use this instead of 0.0f
	front.y = 0.0f;
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	front = glm::normalize(front);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
}

/**
*   Draws the minimap Pacman on the minimap. Takes in the projection and shader from the Game class.
* 
*	@param projection	-	The projection matrix from the Game class.
*	@param shader		-	The shader object from the Game class.
* 
*	@see useShader(), useTexture(), drawElements().
*/
void Camera::drawMiniMapPacman(glm::mat4 projection, std::shared_ptr<Shader>& shader) 
{
	shader->useShader();

	glm::mat4 model(1.0f);
	glm::vec3 pacmanMiniMapPosition = position + glm::vec3(-1.0f, 2.0f, -1.0f);
	model = glm::translate(model, glm::vec3(pacmanMiniMapPosition));

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	miniMapPacmanMat->useTexture();
	miniMapPacmanRenderer->drawElements(miniMapPacmanVAO, miniMapPacmanIBO);
}
