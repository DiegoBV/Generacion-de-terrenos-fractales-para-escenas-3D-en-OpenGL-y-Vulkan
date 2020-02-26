#pragma once
#include "Manager.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include "Shader.h"

class GLFWwindow;

class GLManager: public Manager 
{
private:
	static GLManager* instance;
	GLFWwindow* window;

	unsigned int VBO, VAO, EBO;
	// settings
	const unsigned int SRC_WIDTH = 1280;
	const unsigned int SRC_HEIGHT = 720;

	GLManager();
	~GLManager();

public:
	static GLManager* GetSingleton();
	static void ShutDownSingleton();

	virtual void init();
	virtual void update();
	virtual void release();
	virtual void waitUntilFinishEverything();

	void setShader(Shader* shader) {}

	void setKeyCallback(GLFWkeyfun function);
	void setCursorCallback(GLFWcursorposfun function);
	void processInput(GLFWwindow* window);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	bool shouldClose();
	inline GLFWwindow* getWindow() { return window; }
	inline int getWindowWidth() { return SRC_WIDTH; }
	inline int getWindowHeight() { return SRC_HEIGHT; }
};

