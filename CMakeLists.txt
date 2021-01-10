
#Indicate the minimum version of CMake needed to use
#this CMakeLists.txt file.
cmake_minimum_required(VERSION 3.12)

# Declare the name of the project.
project(Pacman3D)

# Ask CMake to find the OpenGL package
find_package(OpenGL REQUIRED)

#https://stackoverflow.com/questions/45955272/modern-way-to-set-compiler-flags-in-cross-platform-cmake-project
set(COMPILER_WARNINGS_AND_ERRORS
    $<$<CXX_COMPILER_ID:MSVC>:/W4>
    $<$<CXX_COMPILER_ID:MSVC>:/w44061>
    $<$<CXX_COMPILER_ID:MSVC>:/w44062>
    $<$<CXX_COMPILER_ID:MSVC>:/w44265>
    $<$<CXX_COMPILER_ID:MSVC>:/permissive->)


# Set a global output directory for libraries and runtime
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


# Instructs CMake to get into the external
add_subdirectory(external/glfw-3.3.2)
add_subdirectory(external/glew-cmake-2.1.0)
add_subdirectory(external/glm)
add_subdirectory(external/assimp)


# Add a new executable to our project
add_executable(${PROJECT_NAME}
	main.cpp
	"include/Camera.h" 
	"include/DirectionalLight.h" 
	"include/Game.h" 
	"include/Ghost.h" 
	"include/GLWindow.h" 
	"include/IndexBuffer.h" 
	"include/LevelLoader.h" 
	"include/Light.h" 
	"include/Map.h" 
	"include/Material.h" 
	"include/Model.h" 
	"include/Pellets.h" 
	"include/PointLight.h" 
	"include/Renderer.h" 
	"include/Shader.h"  
	"include/SpotLight.h" 
	"include/stb_image.h" 
	"include/VertexArray.h" 
	"include/VertexBuffer.h" 
	"include/VertexBufferLayout.h" 
	"include/FrameBuffer.h" 
	"src/Camera.cpp" 
	"src/DirectionalLight.cpp" 
	"src/Game.cpp" 
	"src/Ghost.cpp" 
	"src/GLWindow.cpp" 
	"src/IndexBuffer.cpp" 
	"src/LevelLoader.cpp" 
	"src/Light.cpp" 
	"src/Map.cpp" 
	"src/Material.cpp" 
	"src/Model.cpp" 
	"src/Pellets.cpp" 
	"src/PointLight.cpp" 
	"src/Renderer.cpp" 
	"src/Shader.cpp" 
	"src/SpotLight.cpp" 
	"src/stb_image.cpp" 
	"src/VertexArray.cpp" 
	"src/VertexBuffer.cpp" 
	"src/FrameBuffer.cpp" 
	 )


target_compile_definitions(Pacman3D PRIVATE GLEW_STATIC)

target_link_libraries(Pacman3D
  PRIVATE
  libglew_static
  glfw
  glm
  assimp
  OpenGL::GL)

target_include_directories(Pacman3D PRIVATE include)

add_custom_command(
  TARGET ${PROJECT_NAME} POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_directory
  ${CMAKE_CURRENT_SOURCE_DIR}/assets
  ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/assets)


