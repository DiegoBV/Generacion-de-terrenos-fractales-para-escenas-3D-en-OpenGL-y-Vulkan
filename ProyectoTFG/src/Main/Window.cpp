#include "Window.h"
#include <iostream>

Window* Window::instance = nullptr;

Window::Window() 
{

}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) // esc to exit
		glfwSetWindowShouldClose(window, true);
}

void Window::init()
{
	if (window == nullptr) {
		// glfw: initialize and configure
		// ------------------------------
		glfwInit();
		std::string windowName = "";
#if defined (GL_DEBUG) || defined (GL_RELEASE)
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		windowName = "Terrain generation through fractals - OpenGL";
#elif defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // different from OPENGL
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
		windowName = "Terrain generation through fractals - Vulkan";
#elif __APPLE__
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
		window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, windowName.c_str(), NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(window);
		glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void Window::update()
{
	// input
	processInput(window);
	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	glfwSwapBuffers(window);
	glfwPollEvents();
}

bool Window::shouldClose()
{
	return glfwWindowShouldClose(getWindow());
}

void Window::setKeyCallback(GLFWkeyfun function)
{
	glfwSetKeyCallback(getWindow(), function);
}

void Window::setCursorCallback(GLFWcursorposfun function)
{
	glfwSetCursorPosCallback(getWindow(), function);
}

Window* Window::GetSingleton()
{
	if (instance == nullptr) {
		instance = new Window();
	}
	return instance;
}

void Window::ShutDownSingleton()
{
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}
