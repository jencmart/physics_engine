cmake_minimum_required(VERSION 3.15)

project(opengl_game)

set(CMAKE_CXX_STANDARD 14)

# executable file
add_executable(opengl_game main.cpp common/shader.hpp models/objects.hpp common/controls.hpp octree.hpp models/Sphere.hpp models/Sphere.cpp boundingArea.hpp object.hpp object.cpp octreeNode.hpp loader.hpp Projectile.cpp Projectile.hpp)

# find packages and give them some aliases
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)

# include their directories
include_directories( ${OPENGL_INCLUDE_DIRS} )
include_directories(${GLEW_INCLUDE_DIRS})

# and link them after compiling
target_link_libraries(opengl_game ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES} glfw )