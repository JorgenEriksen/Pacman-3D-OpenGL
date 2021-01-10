#include "Pellets.h"

/**
*	The Pellets class generates, draws, and adds collision to the pellets in our application.
*
*  @name PLACEHOLDER.cpp
*  @author(s) Elvis Arifagic, J�rgen Eriksen, Salvador Bascunan.
*/


/**
*   Places the pellets around on the valid grid places.
*
*   @param levelArrayData - Legal tile data to place pellets on
*   @param mainWindow     - Which window to place them on.
* 
*	@see generatePellets()
* 
*/
Pellets::Pellets(std::vector<std::vector<int>> levelArrayData, std::shared_ptr<GLWindow>& mainWindow)
{
	levelArray = levelArrayData;

	x = 2.0f;
	z = 17.0f;

	int tilesZ = levelArray.size();
	int tilesX = levelArray[0].size();

	numPellets = 0; //Used for tracking
	numPelletsEaten = 0; //Used for tracking

	for (int z = 0; z < tilesZ; z++) 
	{
		for (int x = 0; x < tilesX; x++) 
		{
			if (levelArray[z][x] == 0)
			{
				glm::vec3 pos(x * 2 + 1, 0.5f, z * 2 + 1);
				pelletsPositions.push_back(pos);

				numPellets++;
			}
		}
	}

	generatePellets();

	pelletSpec = std::make_unique<Material>();
	pelletSpec->getSpecular(2.0f, 128);
}

/**
*   Generate the .obj for usage with pellets.
* 
*	@see loadModel(), getVertexArray()
* 
*/
void Pellets::generatePellets()
{
	pelletModel = std::make_unique<Model>();
	pelletModel->loadModel("assets/models/pellet.obj");

	modelMatrices = new glm::mat4[numPellets];

	for (unsigned int i = 0; i < numPellets; i++)
	{
		model = glm::mat4(1.0f);
		model = glm::translate(model, pelletsPositions[i]);
		modelMatrices[i] = model;
	}

	instancedVAO = std::make_shared<VertexArray>();
	instancedVAO = pelletModel->getVertexArray();
	instancedVAO->bind();

	instancedVBO = std::make_shared<VertexBuffer>(&modelMatrices[0], numPellets * sizeof(glm::mat4));
	instancedVBO->bind();

	instancedVAO->addBufferDivisor();

	instancedVAO->unbind();
}

/**
*   Check the collision between the camera and the camera.
*	If there is collision, remove all the pellets, and the position,
*	then re-generate all the pellets minus the removed position.
*
*   @param playerPosition - relevant camera position on map.
* 
*	@see generatePellets()
* 
*/
void Pellets::checkPelletsCollision(glm::vec3 playerPosition)
{
	for (unsigned int i = 0; i < pelletsPositions.size(); i++)
	{
		if (distance(pelletsPositions[i], playerPosition) < 0.7f)
		{
			pelletsPositions.erase(pelletsPositions.begin() + i);
			numPellets--;
			numPelletsEaten++;
			generatePellets();
		}
	}
}

/**
*   Utility function for checking whether all pellets are eaten by the player.
*
*   @return bool - False if there is still pellets on the map.
*/
bool Pellets::allPelletsEaten() 
{
	if (numPellets == 0) 
	{
		return true;
	}
	return false;
}

/**
*   A function that calculates the distance between two vectors.
*
*   @param vec_1 -
*   @param vec_2 -
*/
float distance(glm::vec3 vec_1, glm::vec3 vec_2)
{
	return sqrt(pow(vec_1.x - vec_2.x, 2) + pow(vec_1.z - vec_2.z, 2));
}

/**
*   Draw all the models in positions all across the 0's of the map.
*
*   @param camera	    - Sends in the camera to calculate the view matrix.
*   @param pelletShader - Sends in the pellet shader used in the Game class.
*   @param projection   - Sends in the projection generated in the Game class
* 
*	@see calculateViewMatrix(), setMat4(), renderInstanced()
* 
*/
void Pellets::draw(std::shared_ptr<Shader>& pelletShader)
{
	uniformSpecularIntensity = pelletShader->getSpecularIntensityLocation();
	uniformShininess = pelletShader->getShininessLocation();

	pelletSpec->useMaterial(uniformSpecularIntensity, uniformShininess);
	pelletModel->renderInstanced(numPellets);
}

/**
*   Draw all the models in positions all across the 0's of the map.
*
*   @param camera	    - Sends in the camera to calculate the view matrix for the minimap.
*   @param pelletShader - Sends in the pellet shader used in the Game class.
*   @param projection   - Sends in the projection generated in the Game class
*
*	@see calculateMiniMap(), setMat4(), renderInstanced()
* 
*/

void Pellets::drawMinimap(std::shared_ptr<Camera>& camera, std::shared_ptr<Shader>& pelletShader, glm::mat4 projection)
{
	model = glm::mat4(1.0f);

	uView = pelletShader->getViewLocation();
	glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera->calculateMinimapView()));

	uProj = pelletShader->getProjectionLocation();
	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));

	model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

	pelletModel->renderInstanced(numPellets);
}