#include "renderer.h"

Simulation *renderer::simulation;
GLFWwindow *renderer::window;

renderer::Shader renderer::shader;

renderer::Camera renderer::camera;

GLuint renderer::FramebufferID;
GLuint renderer::ColorBufferTextureID;
GLuint renderer::EmissionBufferTextureID;
GLuint renderer::rbo;

int renderer::windowWidth;
int renderer::windowHeight;

Cube *voxelModel;

double lastTime; // Used for deltaTime calculation

GLuint LoadShaderProgram(const char *vertex_file_path, const char *fragment_file_path)
{
	GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// Read the Vertex Shader code from the file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
	if (VertexShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << VertexShaderStream.rdbuf();
		VertexShaderCode = sstr.str();
		VertexShaderStream.close();
	}
	else
	{
		printf("Unable to open %s.\n", vertex_file_path);
		getchar();
		return 0;
	}

	// Read the Fragment Shader code from the file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
	if (FragmentShaderStream.is_open())
	{
		std::stringstream sstr;
		sstr << FragmentShaderStream.rdbuf();
		FragmentShaderCode = sstr.str();
		FragmentShaderStream.close();
	}

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Compile Vertex Shader
	printf("Compiling shader : %s\n", vertex_file_path);
	char const *VertexSourcePointer = VertexShaderCode.c_str();
	glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
	glCompileShader(VertexShaderID);

	// Check Vertex Shader
	glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
		printf("%s\n", &VertexShaderErrorMessage[0]);
	}

	// Compile Fragment Shader
	printf("Compiling shader : %s\n", fragment_file_path);
	char const *FragmentSourcePointer = FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
	glCompileShader(FragmentShaderID);

	// Check Fragment Shader
	glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
	glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
		glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
		printf("%s\n", &FragmentShaderErrorMessage[0]);
	}

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, VertexShaderID);
	glAttachShader(ProgramID, FragmentShaderID);
	glLinkProgram(ProgramID);

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0)
	{
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);

	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);

	renderer::windowWidth = width;
	renderer::windowHeight = height;

	renderer::camera.windowWidth = width;
	renderer::camera.windowHeight = height;

	glBindTexture(GL_TEXTURE_2D, renderer::ColorBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, renderer::EmissionBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, renderer::rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
}

int renderer::init()
{
	std::cout << "Init start" << std::endl;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1920, 1080, "Mountain Generator", NULL, NULL);
	if (!window) // Has the window been created?
	{
		fprintf(stderr, "Failed to open GLFW window.\n");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide the mouse

	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	glEnable(GL_DEPTH_TEST); // Enable depth test
	glDepthFunc(GL_LESS);	 // Accept fragment if it closer to the camera than the former one

	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	glfwSetWindowAspectRatio(window, 16, 9);

	glewInit();

	std::cout << "Shader load start" << std::endl;

	shader.ProgramID = LoadShaderProgram("../shaders/default/vertexShader.glsl", "../shaders/default/fragmentShader.glsl");

	shader.MatrixUniformID = glGetUniformLocation(shader.ProgramID, "MVP");
	shader.ViewMatrixUniformID = glGetUniformLocation(shader.ProgramID, "V");
	shader.ModelMatrixUniformID = glGetUniformLocation(shader.ProgramID, "M");
	shader.ModelColorUniformID = glGetUniformLocation(shader.ProgramID, "ModelColor");

	for (int i = 0; i < MAX_OCCLUDERS; i++)
	{
		std::string indexStr = "[" + std::to_string(i) + "]";

		std::string location1Str = "OccluderPositions" + indexStr;
		std::string location2Str = "OccluderRadiuses" + indexStr;

		shader.OccluderPositionsUniformIDs[i] = glGetUniformLocation(shader.ProgramID, location1Str.c_str());
		shader.OccluderRadiusesUniformIDs[i] = glGetUniformLocation(shader.ProgramID, location2Str.c_str());
	}

	std::cout << "Shader load end" << std::endl;

	std::cout << "Setting render parameters" << std::endl;

	// Optimizing: Don't draw the back of every face
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	// AA
	glfwWindowHint(GLFW_SAMPLES, 4);
	glEnable(GL_MULTISAMPLE);

	// Framebuffer
	glGenFramebuffers(1, &FramebufferID);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferID);

	// Generate a texture for the color (since we will later need to read from it)
	glGenTextures(1, &ColorBufferTextureID);
	glBindTexture(GL_TEXTURE_2D, ColorBufferTextureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Attach it to currently bound framebuffer object
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ColorBufferTextureID, 0);

	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);

	// OpenGL still needs a depth and stencil buffer but we don't need to access them so we don't have to use a texture and can use a RBO instead
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);

	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	std::cout << "Render parameters set" << std::endl;

	camera = Camera();
	camera.windowWidth = windowWidth;
	camera.windowHeight = windowHeight;

	voxelModel = (new Cube());

	lastTime = glfwGetTime();

	std::cout << "Init end" << std::endl;

	return 1;
}

GLFWwindow *renderer::getWindow()
{
	return window;
}

renderer::Camera::Camera()
{
	Camera::fov = 60.0f;
	Camera::windowWidth = 1;
	Camera::windowHeight = 1;

	Camera::position = glm::vec3(0, 30, 50);
	Camera::horizontalAngle = 3.14f;
	Camera::verticalAngle = 0.0f;
	Camera::fov = 45.0f;
	Camera::speed = 15.0f; // 3 units / second
	Camera::mouseSpeed = 0.005f;
	Camera::viewMatrix;
	Camera::ProjectionMatrix;
}

// Handles all camera movement. Called every frame.
void renderer::Camera::Update(double mouseX, double mouseY, float deltaTime)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// Compute new orientation
	horizontalAngle += mouseSpeed * float(1024 / 2 - xpos);
	verticalAngle += mouseSpeed * float(768 / 2 - ypos);

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	glm::vec3 direction(
		cos(verticalAngle) * sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle) * cos(horizontalAngle));

	// Right vector
	glm::vec3 right = glm::vec3(
		sin(horizontalAngle - 3.14f / 2.0f),
		0,
		cos(horizontalAngle - 3.14f / 2.0f));

	// Up vector
	glm::vec3 up = glm::cross(right, direction);

	// Move forward
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		position += direction * deltaTime * speed;
	}
	// Move backward
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		position -= direction * deltaTime * speed;
	}
	// Strafe right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		position += right * deltaTime * speed;
	}
	// Strafe left
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		position -= right * deltaTime * speed;
	}
	// Strafe up
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	{
		position += up * deltaTime * speed;
	}
	// Strafe down
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		position -= up * deltaTime * speed;
	}

	// Projection matrix : 45� Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	ProjectionMatrix = glm::perspective(glm::radians(fov), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	viewMatrix = glm::lookAt(
		position,			  // Camera is here
		position + direction, // and looks here : at the same position, plus "direction"
		up					  // Head is up (set to 0,-1,0 to look upside-down)
	);
}

// Both are in OpenGL screenspace [-1; 1]
double previousMouseX = 0;
double previousMouseY = 0;

void renderer::setSimulation(Simulation *simulation)
{
	renderer::simulation = simulation;
}

void renderer::renderVoxel(Voxel *body, glm::mat4 projectionMatrix)
{
	glm::mat4 modelMatrix = glm::mat4(1);

	modelMatrix = glm::translate(modelMatrix, body->position);

	glm::mat4 mvp = projectionMatrix * camera.viewMatrix * modelMatrix;

	glUniformMatrix4fv(shader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(shader.ModelMatrixUniformID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shader.ViewMatrixUniformID, 1, GL_FALSE, &camera.viewMatrix[0][0]);

	glBindVertexArray((*voxelModel).VertexArrayID);

	// Bind the model index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (*voxelModel).IndexBufferID);

	// Draw the triangles !
	glDrawElements(
		GL_TRIANGLES,				   // mode
		(*voxelModel).GetIndexCount(), // index count
		GL_UNSIGNED_INT,			   // type
		(void *)0					   // element array buffer offset
	);
	glUniform1i(shader.OccluderCountUniformID, 0);
}

// Requires the default shader
void renderer::renderGrid(glm::mat4 projectionMatrix, glm::mat4 viewMatrix)
{
	glm::mat4 modelMatrix = glm::mat4(1);
	glm::mat4 mvp = projectionMatrix * viewMatrix * modelMatrix;

	glUniformMatrix4fv(shader.MatrixUniformID, 1, GL_FALSE, &mvp[0][0]);
	glUniformMatrix4fv(shader.ModelMatrixUniformID, 1, GL_FALSE, &modelMatrix[0][0]);
	glUniformMatrix4fv(shader.ViewMatrixUniformID, 1, GL_FALSE, &viewMatrix[0][0]);

	glLineWidth(3);
	glUniform3f(shader.ModelColorUniformID, 1.0f, 0.0f, 0.0f);
	glBegin(GL_LINE_STRIP);
	for (float j = -1000; j <= 1000; j += 100)
	{
		glVertex3f(j, 0.0f, 0.0f);
	}
	glEnd();

	glUniform3f(shader.ModelColorUniformID, 0.0f, 0.0f, 1.0f);
	glBegin(GL_LINE_STRIP);
	for (float j = -1000; j <= 1000; j += 100)
	{
		glVertex3f(0, 0.0f, j);
	}
	glEnd();

	glLineWidth(1);
	glUniform3f(shader.ModelColorUniformID, 0.8f, 0.9f, 1.0f);
	glBegin(GL_LINE_STRIP);
	for (float i = -1000; i <= 1000; i += 1)
	{
		for (float j = -1000; j <= 1000; j += 100)
		{
			glVertex3f(i, 0.0f, j);
		}
		glPrimitiveRestartNV();

		for (float j = -1000; j <= 1000; j += 100)
		{
			glVertex3f(j, 0.0f, i);
		}
		glPrimitiveRestartNV();
	}
	glEnd();
}

void renderer::renderAll()
{
	glClearColor(0.52f, 0.8f, 0.95f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // we're not using the stencil buffer now
	glEnable(GL_DEPTH_TEST);
	glUseProgram(shader.ProgramID);

	double preRenderTime = glfwGetTime();
	double deltaTime = preRenderTime - lastTime;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Fill the void

	double mouseX, mouseY;
	glfwGetCursorPos(window, &mouseX, &mouseY);

	glm::mat4 projectionMatrix = glm::perspective(renderer::camera.fov, (float)windowWidth / (float)windowHeight, 0.1f, 500.0f);

	camera.Update(mouseX, mouseY, (float)deltaTime);

	renderGrid(projectionMatrix, camera.viewMatrix);

	std::vector<Voxel *> voxels = *simulation->GetVoxels();
	for (unsigned int i = 0; i < voxels.size(); i++)
	{
		renderVoxel(voxels[i], projectionMatrix);
	}

	/* Swap front and back buffers */
	glfwSwapBuffers(window);

	glfwPollEvents();

	lastTime = preRenderTime;

	glfwGetCursorPos(window, &mouseX, &mouseY);
	mouseX = mouseX / windowWidth * 2 - 1;
	mouseY = -(mouseY / windowHeight * 2 - 1);

	if (mouseX != previousMouseX || mouseY != previousMouseY)
	{
		previousMouseX = mouseX;
		previousMouseY = mouseY;
	}
}

void renderer::terminate()
{
	glDeleteTextures(1, &ColorBufferTextureID);
	glDeleteTextures(1, &EmissionBufferTextureID);
	glDeleteFramebuffers(1, &FramebufferID);

	glfwTerminate();
}