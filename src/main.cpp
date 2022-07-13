// REMEMBER TO RUN export LIBGL_ALWAYS_INDIRECT=0
// cat /etc/resolv.conf, export DISPLAY=172.25.96.1:0.0
// export MESA_GL_VERSION_OVERRIDE=3.3

#include "rendering/baseModels/cube.h"
#include "rendering/renderer.h"
#include "rendering/color.h"

#include "universe/scene_loader.h"

int main(void)
{
	if (!renderer::init())
	{
		std::cout << "Failed to initialize renderer!" << std::endl;
		return -1;
	}

	GLFWwindow *window = renderer::getWindow();

	Universe *universe = loadScene("../Scenes/default.scene");

	renderer::setUniverse(universe);

	while (!glfwWindowShouldClose(window))
	{
		renderer::renderAll();
	}

	delete renderer::loadedUniverse;

	renderer::terminate();

	return 0;
}