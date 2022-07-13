#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <chrono>
#include <thread>

#include "../simulation/simulation.hpp"
#include "models/cube.h"

namespace renderer
{
	// Default shader (used to render objects in the world with or without lighting)
	struct Shader
	{
		GLuint ProgramID;
		GLuint MatrixUniformID;
		GLuint ModelMatrixUniformID;
		GLuint ViewMatrixUniformID;
		GLuint UnlitUniformID;
		GLuint EmissiveUniformID;
		GLuint LightPosUniformID;
		GLuint LightColorUniformID;
		GLuint ModelColorUniformID;

		// Shadow marching Uniforms
		GLuint OccluderCountUniformID;
		GLuint OccluderPositionsUniformIDs[MAX_OCCLUDERS];
		GLuint OccluderRadiusesUniformIDs[MAX_OCCLUDERS];
		GLuint LightRadiusUniformID;
	};

	struct Camera
	{
		float fov;
		int windowWidth, windowHeight;
		glm::vec3 position;
		glm::mat4 viewMatrix;
		float horizontalAngle;
		float verticalAngle;
		float speed;
		float mouseSpeed;
		glm::mat4 ProjectionMatrix;

		Camera();

		void Update(double mouseX, double mouseY, float deltaTime);
	};

	int init();
	void renderVoxel(Voxel *voxel, glm::mat4 projectionMatrix);
	void renderStars(glm::mat4 projectionMatrix);
	void renderGrid(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
	void renderAll();
	void setSimulation(Simulation *simulation);
	void terminate();

	GLFWwindow *getWindow();

	extern Camera camera;
	extern Simulation *simulation;
	extern GLFWwindow *window;

	extern Shader shader;
	extern GLuint FramebufferID;
	extern GLuint ColorBufferTextureID;
	extern GLuint EmissionBufferTextureID;
	extern GLuint rbo;

	extern int windowWidth;
	extern int windowHeight;
}