cmake_minimum_required (VERSION 3.0)
project (MountainGen)

find_package(OpenGL REQUIRED)

#
# CMake setup
#
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set (CMAKE_VERBOSE_MAKEFILE 0) # 1 should be used for debugging
set (CMAKE_SUPPRESS_REGENERATION TRUE) # Suppresses ZERO_CHECK
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -std=c++11")
  if(NOT WIN32)
    set(GLAD_LIBRARIES dl)
  endif()
endif()

#
# GLFW options
#
option (GLFW_INSTALL OFF)
option (GLFW_BUILD_DOCS OFF)
option (GLFW_BUILD_EXAMPLES OFF)
option (GLFW_BUILD_TESTS OFF)
add_subdirectory (lib/glfw)

#
# Set include paths
#
include_directories (.
                     lib/glad/include/
                     lib/glfw/include/
                     lib/glm/
                     lib/glew/build/cmake
                     lib/stb/
                     lib/glut/include)

#
# Add files
#
file (GLOB         VENDORS_SOURCES lib/glad/src/glad.c)
file (GLOB_RECURSE PROJECT_HEADERS src/*.hpp
                                   src/*.h)
file (GLOB_RECURSE PROJECT_SOURCES src/*.cpp
                                   src/*.cxx
                                   src/*.cc
                                   src/*.c)
file (GLOB_RECURSE PROJECT_RENDERING  rendering/*.hpp
                                      rendering/*.cpp)
file (GLOB_RECURSE PROJECT_SIM        simulation/*.hpp
                                      simulation/*.cpp)
file (GLOB_RECURSE PROJECT_SHADERS shaders/*.glsl)
file (GLOB         PROJECT_CONFIGS CMakeLists.txt
                                   README.rst
                                  .gitignore
                                  .gitmodules)
file (GLOB_RECURSE PROJECT_SCENES *.scene *.txt)

#
# Organizing files
#
source_group ("headers" FILES ${PROJECT_HEADERS})
source_group ("sources" FILES ${PROJECT_SOURCES})
source_group ("vendors" FILES ${VENDORS_SOURCES})
source_group ("shaders" FILES ${PROJECT_SHADERS})
source_group ("sim" FILES ${PROJECT_SIM})
source_group ("rendering" FILES ${PROJECT_RENDERING})
source_group ("scenes" FILES ${PROJECT_SCENES})

#
# Set executable and target link libraries
#
add_definitions (-DGLFW_INCLUDE_NONE
                 -DPROJECT_SOURCE_DIR=\"${PROJECT_SOURCE_DIR}\")
add_executable (${PROJECT_NAME} 
                ${PROJECT_SOURCES} 
                ${PROJECT_HEADERS}
                ${PROJECT_CONFIGS}
                ${PROJECT_SHADERS}
                ${VENDORS_SOURCES})

target_link_libraries (${PROJECT_NAME}
                       ${OPENGL_LIBRARY}
                       glfw
                       GLEW
                       glut
                       ${GLAD_LIBRARIES})
set_target_properties (${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${PROJECT_NAME})