#include "Game.h"

/**
*  The main class of the engine. This class takes in one of every object needed to construct the game.
*  All the functions get ran in the game loop. Aside from objects it constructs shaders and textures.
*
*  As well as creating the game this class also contains OpenGL code needed to update the OpenGL context
*  and eventual update functions for classes that utilize this.
* 
*  @name Game.cpp
*  @author(s) Elvis Arifagic, J�rgen Eriksen, Salvador Bascunan
*/

/**
*   Constructor game object. This works essentially as "game engine startup" mechanism.
*
*/
Game::Game()
	:projection(0), startingPos(0), levelArrayData(0), deltaTime(0), lastTime(0),
	time(0), now(0), uniformModel(0), uniformView(0), uniformProjection(0),model(1.0f), 
	pellets_pos(0), pelletProj(0), pelletView(0)
{
	numberOfGhosts = 4;
}

/**
*   Destructor for game object, everything is initialized as smart pointers.
*/
Game::~Game()
{

}

/**
*   retrieves/makes/compiles all the shaders that will be used in the entire game.
*
*   @see - createShaderFromFile()
*/
void Game::generateShaders()
{
	static const char* vShader = "assets/shaders/lights.vert";
	static const char* fShader = "assets/shaders/lights.frag";

	shader = std::make_shared<Shader>();
	shader->createShaderFromFile(vShader, fShader);

	static const char* v_quadShader = "assets/shaders/screenShader.vert";
	static const char* f_quadShader = "assets/shaders/screenShader.frag";

	screenShader = std::make_shared<Shader>();
	screenShader->createShaderFromFile(v_quadShader, f_quadShader);

	static const char* v_miniMapShader = "assets/shaders/minimap.vert";
	static const char* f_miniMapShader = "assets/shaders/minimap.frag";

	minimapShader = std::make_unique<Shader>();
	minimapShader->createShaderFromFile(v_miniMapShader, f_miniMapShader);

	static const char* pelletsMinimap_vShader = "assets/shaders/pellets_minimap.vert";
	static const char* pelletsMinimap_fShader = "assets/shaders/pellets_minimap.frag";

	pelletMinimapShader = std::make_shared<Shader>();
	pelletMinimapShader->createShaderFromFile(pelletsMinimap_vShader, pelletsMinimap_fShader);

	static const char* pellets_vShader = "assets/shaders/pellets.vert";
	static const char* pellets_fShader = "assets/shaders/pellets.frag";

	pelletShader = std::make_shared<Shader>();
	pelletShader->createShaderFromFile(pellets_vShader, pellets_fShader);
}

/**
*   Load/create all the lights that will be in the game
*
*/
void Game::generateLights()
{
	/* --- The Directional Lights in the scene --- */

	mapLight = std::make_shared<DirectionalLight>(
		1.0f, 1.0f, 1.0f,	 // Color
		1.0f, 0.5f,			 // Ambient, diffuse 
		5.0f, -25.0f, 6.0f); // Direction

	pelletLight = std::make_shared<DirectionalLight>(
		1.0f, 1.0f, 1.0f,	 // Color
		0.5f, 0.4f,			 // Ambient, diffuse 
	   -5.0f, -25.0f, -6.0f); // Direction

	/* --- The Point Lights in the scene --- */

	pointLights[0] = PointLight(
		0.7f, 0.2f, 0.1f,    // Color
		2.5f, 0.5f, 		 // Ambient, diffuse 
		53.0f, -0.5f, 10.0f,  // Position 
		0.1f, 0.1f, 0.1f); 	 // Constant, linear, exponent

	pointLights[1] = PointLight(
		0.9f, 0.0f, 1.0f,    // Color
		2.5f, 0.5f, 		 // Ambient, diffuse 
		13.0f, -0.5f, 10.0f,  // Position 
		0.1f, 0.1f, 0.1f); 	 // Constant, linear, exponent

	pointLights[2] = PointLight(
		1.0f, 1.0f, 0.4f,    // Color
		2.5f, 0.5f, 		 // Ambient, diffuse 
		3.0f, -0.5f, 65.0f,  // Position 
		0.1f, 0.1f, 0.1f); 	 // Constant, linear, exponent

	pointLights[3] = PointLight(
		1.0f, 0.0f, 0.9f,    // Color
		2.5f, 0.5f, 		 // Ambient, diffuse 
		53.0f, -0.5f, 63.0f,  // Position 
		0.1f, 0.1f, 0.1f); 	 // Constant, linear, exponent

	pointLights[4] = PointLight(
		0.1f, 1.0f, 0.3f,	 // Color
		2.5f, 0.5f, 		 // Ambient, diffuse 
		7.0f, -0.5f, 35.0f, // Position
		0.1f, 0.1f, 0.1f);   // Constant, linear, exponent

	/* --- The Flash light in the scene --- */

	spotLights[0] = SpotLight(
		1.0f, 1.0f, 1.0f,	 // Color
		5.0f, 6.0f, 		 // Ambient, diffuse 
		0.0f, 0.0f, 0.0f,	 // Position 
		0.0f, 0.0f, 0.0f,	 // Direction
		1.0f, 0.0f, 0.0f,	 // Constant, linear, exponent
		13.0f);				 // Edge
}

/**
*   Generate the minimap view. All objects, all needed functions.
*   in turn this cleans up main.cpp greatly.
*/
void Game::generateMinimap(std::shared_ptr<GLWindow>& mainWindow)
{
	float minimapVertices[] = // vertex attributes for a quad that fills the entire screen.
	{ 
		 //x	 y		 u		v			   Switch coordinates to cover entire screen or only top right
		0.40f, 1.0f,	0.0f, 1.0f, // 0		-1.0f,  1.0f,  0.0f, 1.0f,		0.40f, 1.0f,	0.0f, 1.0f, 
		0.40f, 0.25f,	0.0f, 0.0f, // 1		-1.0f, -1.0f,  0.0f, 0.0f,		0.40f, 0.25f,	0.0f, 0.0f,	
		1.0f,  0.25f,	1.0f, 0.0f, // 2		 1.0f, -1.0f,  1.0f, 0.0f,		1.0f,  0.25f,	1.0f, 0.0f,	

		0.40f, 1.0f,	0.0f, 1.0f, // 3		-1.0f,  1.0f,  0.0f, 1.0f,		0.40f, 1.0f,	0.0f, 1.0f,	
		1.0f,  0.25f,	1.0f, 0.0f, // 4		 1.0f, -1.0f,  1.0f, 0.0f,		1.0f,  0.25f,	1.0f, 0.0f,	
		1.0f,  1.0f,	1.0f, 1.0f  // 5		 1.0f,  1.0f,  1.0f, 1.0f		1.0f,  1.0f,	1.0f, 1.0f 	
	};

	minimapVAO = std::make_shared<VertexArray>();
	minimapVAO->bind();

	minimapVBO = std::make_shared<VertexBuffer>(&minimapVertices, sizeof(minimapVertices));
	minimapVBO->bind();

	minimapVBLayout = std::make_shared<VertexBufferLayout>();
	minimapVBLayout->Push<float>(2);
	minimapVBLayout->Push<float>(2);

	minimapVAO->addBuffer(*minimapVBO, *minimapVBLayout);

	frameBuffer = std::make_unique<FrameBuffer>();
	frameBuffer->generateFB();	//Frame buffer
	frameBuffer->generateTBO(mainWindow->getBufferWidth(), mainWindow->getBufferHeight()); // dynamic size
	frameBuffer->generateRBO(mainWindow->getBufferWidth(), mainWindow->getBufferHeight());
	frameBuffer->checkStatus();
	frameBuffer->unbind();
}

/**
*   Generate the MVP for the game. All uniforms needed are set here.
*   Used in generateGame function.
*
*   @see - Shader class
*/
void Game::generateMVP()
{
	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();
}

/**
*   Generate the MVP for the minimap. All uniforms needed are set here.
*   Used in generateGame function.
*
*   @see - Shader class
*/
void Game::generateMinimapMVP()
{
	uniformModel = minimapShader->getModelLocation();
	uniformProjection = minimapShader->getProjectionLocation();
	uniformView = minimapShader->getViewLocation();
	uniformMinimapTexture = screenShader->getMinimapTextureLocation();
}

/**
*   Generate the entire game. All objects, all needed functions.
*
*   @see - generateShaders(), generateLights(), generateTextures(), getLevelArray(), getBufferWidth(), getBufferHeight().
*/
void Game::generateGame(std::shared_ptr<GLWindow>& mainWindow)
{
	float offset = mainWindow->getBufferWidth() - mainWindow->getBufferHeight() + 40.0f;
	
	generateShaders();
	generateLights();
	 
	map = std::make_unique<Map>(mainWindow);

	levelArrayData = map->getLevelArray();

	for (int i = 0; i < numberOfGhosts; i++) 
	{
		auto ghost = std::make_unique<Ghost>(levelArrayData, i);
		ghosts.push_back(std::move(ghost));
	}

	pellets = std::make_unique<Pellets>(levelArrayData, mainWindow);

	startingPos = map->getStartingPosition();

	camera = std::make_shared<Camera>(levelArrayData, startingPos, glm::vec3(0.0f, 1.0f, 0.0f), 0.0f, 0.0f, 4.0f, 0.03f);

	projection = glm::perspective(glm::radians(45.0f), ((GLfloat)mainWindow->getBufferWidth() / mainWindow->getBufferHeight()), 0.1f, 1200.0f);
	projectionMinimap = glm::perspective(glm::radians(45.0f), (((GLfloat)mainWindow->getBufferWidth() - offset) / mainWindow->getBufferHeight()), 0.1f, 2000.0f);

	generateMinimap(mainWindow);
	generateMVP();
	generateMinimapMVP();
}

/**
*   Updates lights
*
*   @see - setDirectionalLight(), setSpotLights(), getCameraPosition(), setFlash(), getCameraDirection().
*
*/
void Game::updateLights()
{
	shader->setDirectionalLight(mapLight);
	shader->setPointLights(pointLights, 5);

	shader->setSpotLights(spotLights, 1);
	lowerLight = camera->getCameraPosition();
	lowerLight.y -= 0.1f; // Offsets the flashlight
	spotLights[0].setFlash(lowerLight, camera->getCameraDirection());
}

/**
*   Updates the model, view and projection matrices for the main game framebuffer.
*
*   @see - getModelLocation(), getProjectionLocation(), getViewLocation(), getEyePositionLocation(), getCameraPosition().
*/
void Game::updateMVP()
{
	shader->useShader();
	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
}

/**
*   Update the MVP matrices for the minimap.
*
*   @see - getModelLocation(), getProjectionLocation(), getViewLocation().
*/
void Game::updateMinimapMVP()
{
	shader->useShader();
	uniformModel = shader->getModelLocation();
	uniformProjection = shader->getProjectionLocation();
	uniformView = shader->getViewLocation();
	glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projectionMinimap));
	glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera->calculateMinimapView()));
	glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
}

/**
*	
*	Checks for the collision of the pellets within certain time intervals.
*	
*/
void Game::updateTime()
{
	now = glfwGetTime();
	deltaTime = now - lastTime;
	lastTime = now;

	time += deltaTime;
	if (time > 0.2)
	{
		pellets->checkPelletsCollision(camera->getCameraPosition());
		time = 0;
	}
}

/**
*   Updating the minimap as long as the window is open.
* 
*   @see  bind(), enableDepth, clear(), useShader(), updateMinimapMVP(), updateTime(), 
*		  updateLights(), drawMinimap(), getCameraPosition(), drawMinimapPacman(), unbind(), 
*		  move(), disableDepth(), bindTBO(), drawArrays()
* 
*/
void Game::updateMinimap()
{
	frameBuffer->bind();

	renderer->enableDepth(); // enable depth testing (is disabled for rendering view quad)
	renderer->clear(0.0f, 0.0f, 0.0f, 1.0f);

	minimapShader->useShader();

	updateMinimapMVP();
	updateTime();

	map->drawMinimap(model, projectionMinimap, camera, minimapShader);

	for (int i = 0; i < numberOfGhosts; i++)
	{
		ghosts[i]->move(deltaTime, camera->getCameraPosition());
		ghosts[i]->drawMinimap(camera, shader, model, projectionMinimap);
	}
	camera->drawMiniMapPacman(projectionMinimap, minimapShader);

	pelletMinimapShader->useShader();

	pellets->drawMinimap(camera, pelletMinimapShader, projectionMinimap);

	frameBuffer->unbind();

	renderer->disableDepth(); // disable depth test so view quad isn't discarded due to depth test.
	
	screenShader->useShader();
	frameBuffer->bindTBO();
	renderer->drawArrays(minimapVAO);
}

/**
*   Updating the game as long as the window is open.
*	Function handles all event updating.
*
*   @param mainWindow - Current open window.
*   @see   useShader(), updateMVP(), updateTime(), keyControls(), mouseControl(), updateLights(),
*		   retrieveKeys(), toggleFlashLight(), clear(), enableDepth(), draw(), checkCameraCollision(),
*		   closeWindow(), getViewLocation(), getProjectionLocation(), calculateViewMatrix(),
*		   setDirectionalLight(), setSpotLights(), allPelletsEaten(), closeWindow(), updateMinimap().
*/
void Game::updateGame(std::shared_ptr<GLWindow>& mainWindow)
{
	shader->useShader();
	updateMVP();

	updateTime();
	camera->keyControls(mainWindow->retrieveKeys(), deltaTime);
	camera->mouseControl(mainWindow->getChangeX(), mainWindow->getChangeY());

	updateLights();

	// Toggle the Flash light on and off with the L key.
	if (mainWindow->retrieveKeys()[GLFW_KEY_F])
	{
		spotLights[0].toggleFlashLight();

		// Set it to false to only catch the button pressed, and not while it's pressed.
		mainWindow->retrieveKeys()[GLFW_KEY_F] = false;
	}

	renderer->clear(0.1f, 0.1f, 0.1f, 1.0f);
	renderer->enableDepth();

	map->draw(model, projection, camera, shader);

	for (int i = 0; i < numberOfGhosts; i++)
	{
		ghosts[i]->move(deltaTime, camera->getCameraPosition());
		ghosts[i]->draw(camera, shader, model, projection);
		if (ghosts[i]->checkCameraCollision(camera)) // if collision with one of the ghosts
		{
			std::cout << "\nCollided with ghost. Game over!";
			mainWindow->closeWindow();
		}
	}

	pelletShader->useShader();

	uView = pelletShader->getViewLocation();
	uProj = pelletShader->getProjectionLocation();

	glUniformMatrix4fv(uProj, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(uView, 1, GL_FALSE, glm::value_ptr(camera->calculateViewMatrix()));

	pellets->draw(pelletShader);

	pelletShader->setDirectionalLight(mapLight);
	pelletShader->setDirectionalLight(pelletLight);
	pelletShader->setSpotLights(spotLights, 1);

	if (pellets->allPelletsEaten()) // if all pellets are eaten
	{ 
		std::cout << "\nYou ate all the pellets. Game win, good job!";
		mainWindow->closeWindow();
	}

	updateMinimap();

	glUseProgram(0);
}

