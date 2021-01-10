#include "Game.h"

/**
*   Our implementation of Pacman in 3D. This is based loosely on the game.
*   The game engine is written in C++ with rendering supported by OpenGL.
*   Model importing is done through Assimp and GLFW supports I/O. Images
*   are loaded through the stb_image header library. Project is built with CMake.
* 
*
*   @name Pacman3D.exe
*   @author(s) Elvis Arifagic, Jørgen Eriksen, Salvador Bascunan.
*/

int main() 
{
	auto mainWindow = std::make_shared<GLWindow>(800, 600); // make the window.
	mainWindow->initialise(); 

	auto pacmangame = std::make_unique<Game>();
	pacmangame->generateGame(mainWindow); // create the game.

    while (!mainWindow->shouldClose()) 
    {
		glfwPollEvents();
		pacmangame->updateGame(mainWindow);
		mainWindow->swapBuffer();
    }
    return EXIT_SUCCESS;
}