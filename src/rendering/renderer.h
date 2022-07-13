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

#include "../universe/universe.h"
#include "../universe/mass_body.h"
#include "baseModels/cube.h"

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

	// Shader used to render star spheres
	struct StarShader
	{
		GLuint ProgramID;
		GLuint MatrixUniformID;
		GLuint ModelMatrixUniformID;
		GLuint ViewMatrixUniformID;
	};

	struct Camera
	{
		float fov;
		int windowWidth, windowHeight;
		// Stuff that is computed for every frame
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
	void renderModel(RenderModel model, glm::mat4 projectionMatrix, glm::mat4 viewMatrix, glm::mat4 modelMatrix, Color color);
	void renderBody(MassBody *body, glm::mat4 projectionMatrix);
	void renderStars(glm::mat4 projectionMatrix);
	void renderGrid(glm::mat4 projectionMatrix, glm::mat4 viewMatrix);
	void renderAll();
	void preRender();
	void setUniverse(Universe *universe);
	void terminate();

	GLFWwindow *getWindow();

	extern Camera camera;
	extern Universe *loadedUniverse;
	extern GLFWwindow *window;

	extern Shader shader;

	extern std::vector<Cube *> bodyLODModels; // Array of spheres with different resolutions used to render bodies

	extern GLuint FramebufferID;
	extern GLuint ColorBufferTextureID;
	extern GLuint EmissionBufferTextureID;
	extern GLuint rbo;

	extern int windowWidth;
	extern int windowHeight;
}