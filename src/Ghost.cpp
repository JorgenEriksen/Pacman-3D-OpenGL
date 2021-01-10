#include "Ghost.h"

/**
*  Ghost class that defines the game object "ghost". 
*  This object has its own AI that stears it. It chooses an optimal path based on camera location
*  If this optimal choice can not be done (always 2 possibilities) it chooses a random direction/less optimal path.
* 
*  This is a model based object. This object has its own AI that stears it. using Assimp.
*
*  @name Ghost.cpp
*  @author(s) Elvis Arifagic, Jørgen Eriksen, Salvador Bascunan.
*/


/**
*   placeholder constructor for ghost
*/
Ghost::Ghost()
{

}

/**
*   Constructor for ghost game object.
*
*   @param     levelArrayData - Data that ghost uses to move around.
*   @see	   generateSpawnPositions(), randomSpawnPosition(), startVelocity(), generateGhost().
*/
Ghost::Ghost(std::vector<std::vector<int>> levelArrayData, int index)
{
	levelArray = levelArrayData;
	generateSpawnPositions();

	position = randomSpawnPosition();
	velocity = startVelocity();

	aiValue = index + 1;

	generateGhost();
}

/**
*   placeholder destructor for ghost, all initialized as smart pointer.
*
*/
Ghost::~Ghost()
{

}

/**
*   Generates the starting direction of the ghost(s)
*
*   @see	isWall()
* 
*	@return glm::vec3 - the box that ghost is allowed to move in.
* 
*/
glm::vec3 Ghost::startVelocity()
{
	std::vector<glm::vec3> placesToMove;
	if (!isWall(UP))
		placesToMove.push_back(glm::vec3(0.0f, 0.0f, -1.0f));
	if (!isWall(DOWN))
		placesToMove.push_back(glm::vec3(0.0f, 0.0f, 1.0f));
	if (!isWall(LEFT))
		placesToMove.push_back(glm::vec3(-1.0f, 0.0f, 0.0f));
	if (!isWall(RIGHT))
		placesToMove.push_back(glm::vec3(1.0f, 0.0f, 0.0f));

	auto index = calculateRandomNumber(placesToMove.size() - 1);

	return placesToMove[index];
}

/**
*   Gives ghost random LEGAL spawn positions. The ghosts are allowed to spawn
*	anywhere expect in the tunnel where the camera spawns.
*
*/
void Ghost::generateSpawnPositions()
{
	for (int z = 0; z < levelArray.size(); z++)
	{
		for (int x = 0; x < levelArray[z].size(); x++)
		{
			if (levelArray[z][x] == 0 && z != 17) {
				glm::vec3 position = { x * 2 + 1, 0.5f, z * 2 + 1 };
				validGhostPositions.push_back(position);
			}
		}
	}
}

/**
*   
*	Calculates a random position for each ghost that is spawned. Used in generateSpawnPositions().
*   
*/
glm::vec3 Ghost::randomSpawnPosition()
{
	auto random = calculateRandomNumber(validGhostPositions.size() - 1);
	return validGhostPositions[random];
}

/**
*   Gets the level array X index
*
*	@return floor(), a value equal to the nearest integer that is less then or equal to x.
* 
*/
int Ghost::getUnitLevelArrayIndexX()
{
	float offset = 0;

	if (velocity == LEFT) {
		offset = 0.9;
	}

	else if (velocity == RIGHT) {
		offset = -0.9;
	}

	return floor((position.x + offset) / 2);
}

/**
*   Gets the level array Z index
*
*	@return floor(), a value equal to the nearest integer that is less then or equal to x.
*
*/
int Ghost::getUnitLevelArrayIndexZ()
{
	float offset = 0;

	if (velocity == UP) {
		offset = 0.9;
	}

	else if (velocity == DOWN) {
		offset = -0.9; // offset for clearing corners
	}

	return floor((position.z + offset) / 2);
}

/**
*   AI movement algorithm for the ghost.
*	Based on which quadrant pacman is in, it chooses the most optimal path towards him.
*	If that is not possible it chooses a random direction.
*
*   @param pacmanPosition - Where the camera is located on the tile map.
*
*	@see getUnitLevelArrayIndexZ(), getUnitLevelArrayIndexX(), isWall()
*
*/
void Ghost::calculateAiDirection(glm::vec3 pacmanPosition)
{
	int unitLevelArrayIndexZ = getUnitLevelArrayIndexZ();
	int unitLevelArrayIndexX = getUnitLevelArrayIndexX();

	bool pacmanAboveGhost = false;
	bool pacmanLeftOfGhost = false;

	if (position.z > pacmanPosition.z)  // if pacman is above ghost
	{
		pacmanAboveGhost = true;
	}

	if (position.x > pacmanPosition.x) // if pacman is left of ghost
	{
		pacmanLeftOfGhost = true;
	}



	// the chance that it will do an AI calculation is 1/aiValue
	bool shouldCalculateAi = false;
	auto randomNum = calculateRandomNumber(aiValue - 1);
	if (randomNum == 0) {
		shouldCalculateAi = true;
	}

	glm::vec3 newDirection = NONE; // this change and is just a placeholder


	if (shouldCalculateAi) // if Z distance between pacman and ghost is farther away than Z distance, choose the optimal X path, and if and ghost should calculate ai direction
	{

		glm::vec3 aiDirectionX = NONE; // this change and is just a placeholder
		glm::vec3 aiDirectionZ = NONE; // this change and is just a placeholder

		// Z axis
		if (pacmanAboveGhost && !isWall(UP))
		{
			aiDirectionZ = UP;
		}
		else if (!pacmanAboveGhost && !isWall(DOWN))
		{
			aiDirectionZ = DOWN;
		}

		// X axis
		if (pacmanLeftOfGhost && !isWall(LEFT))
		{
			aiDirectionX = LEFT;
		}
		else if (!pacmanLeftOfGhost && !isWall(RIGHT))
		{
			aiDirectionX = RIGHT;
		}


		// if pacman is further to the z axis and x axis
		if (distance(pacmanPosition.z, position.z) > distance(pacmanPosition.x, position.x))
		{
			newDirection = aiDirectionZ;
		}

		// if no new direction is set
		if (newDirection == NONE)
		{
			newDirection = aiDirectionX;
		}

	}

	// if no new direction is set, either because no AI direction has been done, or because no AI direction 
	// was possible to do.
	if (newDirection == NONE) // if still no new direction has been given
	{
		// choose a random position
		std::vector<glm::vec3> placesToMove;
		if (!isWall(UP))
			placesToMove.push_back(UP);

		if (!isWall(DOWN))
			placesToMove.push_back(DOWN);

		if (!isWall(LEFT))
			placesToMove.push_back(LEFT);

		if (!isWall(RIGHT))
			placesToMove.push_back(RIGHT);


		auto index = calculateRandomNumber(placesToMove.size() - 1);

		newDirection = placesToMove[index];
	}
	velocity = newDirection;
}

/**
*   Calculates a random number between 0 and the argument value
*
*	@param	highestRandomNumber - The highest possible number
*/
int Ghost::calculateRandomNumber(int highestRandomNumber) {
	unsigned int seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::mt19937 engine(seed); // mersenne twister engine for good PRNG.
	std::uniform_int_distribution<> dist(0, highestRandomNumber);

	auto randomNum = dist(engine);
	return randomNum;
}


/**
*   Calculates the distance between two position in the world space.
*
*   @param     pos 1 - location of the first point.
*   @param     pos 2 - location of the second point.
* 
*	@return    float - distance between points.
*/
float Ghost::distance(int pos1, int pos2)
{
	return sqrt(pow(pos1 - pos2, 2));
}

/**
*   Sets the movement of the ghost(s).
*
*	@param dt             - Used to clamp time.
*   @param pacmanPosition - Where the camera is located in the world space.
* 
*	@see getUnitLevelArrayIndexZ(), getUnitLevelArrayIndexX(), isWall(), calculateAiDirection()
* 
*/
void Ghost::move(float dt, glm::vec3 pacmanPosition)
{
	float delta = dt;

	if (delta > 0.03f) 
	{
		delta = 0.02f;
	}
	position += velocity * delta * 2.0f;


	bool calculcatedDirectionInTile = false;

	if (calculatedDirectionPosX != getUnitLevelArrayIndexX() || calculatedDirectionPosZ != getUnitLevelArrayIndexZ()) // if in a new tile
	{ 
		calculatedDirectionPosX = getUnitLevelArrayIndexX();
		calculatedDirectionPosZ = getUnitLevelArrayIndexZ();
		calculcatedDirectionInTile = true;
	}

	if (calculcatedDirectionInTile) // has not calculate direction in this tile
	{
		bool canChangeDirection = false;

		if (!isWall(UP) && (!isWall(LEFT) || !isWall(RIGHT)))
		{
			canChangeDirection = true; // checking for L-shape
		}
		else if (!isWall(DOWN) && (!isWall(LEFT) || !isWall(RIGHT)))
		{
			canChangeDirection = true;
		}

		if (canChangeDirection) 
		{
			calculateAiDirection(pacmanPosition);
		}
	}

	// this is the teleport from edge to edge.
	if (position.x < 1) {
		position.x = levelArray[0].size() * 2 - 1;
	}
	else if (position.x > levelArray[0].size() * 2 - 0.8) {
		position.x = 1;
	}

}

/**
*   Checks if there is a wall in the tile that "direction" points in.
*
*   @param direction - The vector pointing towards where you are moving.
* 
*	@return true to build a wall
*/
bool Ghost::isWall(glm::vec3 direction) {

	unsigned long int unitLevelArrayIndexZ = getUnitLevelArrayIndexZ();
	unsigned long int unitLevelArrayIndexX = getUnitLevelArrayIndexX();

	int tilesX = levelArray[0].size();
	int tilesZ = levelArray.size();

	//To avoid that the calculation will overflow the original (smaller) type before conversion 
	//to the result (larger) type we can either cast to double or use unsigned long int
	if (direction == UP && levelArray[unitLevelArrayIndexZ - 1][unitLevelArrayIndexX] != 1)
	{
		return false;
	}
	if (direction == DOWN && levelArray[unitLevelArrayIndexZ + 1][unitLevelArrayIndexX] != 1)
	{
		return false;
	}
	if (direction == LEFT && unitLevelArrayIndexX != 0) { // if not at left edge of map
		if (levelArray[unitLevelArrayIndexZ][unitLevelArrayIndexX - 1] != 1)
		{
			return false;
		}
	}
	if (direction == RIGHT && unitLevelArrayIndexX != tilesZ) { // if not at right edge of map
		if (levelArray[unitLevelArrayIndexZ][unitLevelArrayIndexX + 1] != 1)
		{
			return false;
		}
	}

	return true;
}

/**
*   Check for the collision between the camera and the ghost
*
*   @param     camera - Object to check distance between.
* 
*	@return	   bool   - Return false if you have not collided.
*/
bool Ghost::checkCameraCollision(std::shared_ptr<Camera>& camera)
{
	
	if (distance2D(camera->getCameraPosition(), position) < 1.65f) {
		return true;
	}
	return false;
}

/**
*   Checks the distance between two vectors in 2D space.
*
*   @param vector	-	The first vector.
*   @param vector2	-	The second vector.
* 
*	@return a distance between to vector in 2D space.
*/
float Ghost::distance2D(glm::vec3 vector, glm::vec3 vector2) {
	return sqrt(pow(vector.x - vector2.x, 2) + pow(vector.z - vector2.z, 2));
}


/**
*   Load the ghost model using the Model class and Assimp.
* 
*	@see loadModel()
* 
*/
void Ghost::generateGhost()
{
	ghostModel = std::make_unique<Model>();
	ghostModel->loadModel("assets/models/ghost.obj");
}

/**
*   Gets the camera position from the Game class. This is used for collision detection. Sets the
*	movement of the ghost(s) and render the model. 
*
*   @param     camera		      - Sends a shared pointer of the Camera from the Game class
*   @param     shader		      - Sends a shared pointer of the Shader from the Game class
*   @param     model              - Sends the model matrix from the Game class
*   @param     projection		  - Sends the projection from the Game class
* 
*	@see useShader(), getModelLocation(), getProjectionLocation(), getViewLocation(), getCameraPostion(),
*		 calculateViewMatrix(), renderElements()
*/
void Ghost::draw(std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader, glm::mat4 model, glm::mat4 projection)
{
	shader->useShader();

	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();

	glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);

	float ghostY = position.y + glm::sin(position.x + position.z) * 0.2f; // bobbing effect on ghost.
	glm::vec3 ghostPosition(position.x, ghostY, position.z);
	model = glm::translate(model, ghostPosition);

	float angle = 0.0f;
	if (velocity == UP) { angle = 180.0f; };
	if (velocity == DOWN) { angle = 0.0f; }; // turning the ghosts.
	if (velocity == LEFT) { angle = 270.0f; };
	if (velocity == RIGHT) { angle = 90.0f; };


	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	ghostModel->renderElements();
}

/**
*   Gets the camera position from the Game class. Calculates the minimap view matrix. 
*	Sets the movement of the ghost(s) and render the model.
*
*   @param     camera		      - Sends a shared pointer of the Camera from the Game class
*   @param     shader		      - Sends a shared pointer of the Shader from the Game class
*   @param     model              - Sends the model matrix from the Game class
*   @param     projection		  - Sends the projection from the Game class
*
*	@see useShader(), getModelLocation(), getProjectionLocation(), getViewLocation(), getCameraPostion(), 
*		 calculateMinimapView(), renderElements()
*
*/
void Ghost::drawMinimap(std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& shader, glm::mat4 model, glm::mat4 projection)
{
	shader->useShader();

	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();

	glm::vec3 rotationAxis(0.0f, 1.0f, 0.0f);

	float ghostY = position.y + glm::sin(position.x + position.z) * 0.2f;
	glm::vec3 ghostPosition(position.x, ghostY, position.z);
	model = glm::translate(model, ghostPosition);

	float angle = 0.0f;
	if (velocity == UP) { angle = 180.0f; };
	if (velocity == DOWN) { angle = 0.0f; };
	if (velocity == LEFT) { angle = 270.0f; }; // ghost rotation for minimap ghost.
	if (velocity == RIGHT) { angle = 90.0f; };


	model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));

	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateMinimapView()));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

	ghostModel->renderElements();
}
