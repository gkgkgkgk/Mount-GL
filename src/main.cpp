// REMEMBER TO RUN export LIBGL_ALWAYS_INDIRECT=0
// cat /etc/resolv.conf, export DISPLAY=172.25.96.1:0.0
// export MESA_GL_VERSION_OVERRIDE=3.3

#include "rendering/models/cube.h"
#include "rendering/renderer.h"

int main(void)
{
	if (!renderer::init())
	{
		std::cout << "Failed to initialize renderer!" << std::endl;
		return -1;
	}

	GLFWwindow *window = renderer::getWindow();

	Simulation *simulation = new Simulation();
	renderer::setSimulation(simulation);

	while (!glfwWindowShouldClose(window))
	{
		renderer::renderAll();
	}

	delete renderer::simulation;

	renderer::terminate();

	return 0;
}