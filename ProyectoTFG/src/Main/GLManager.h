#pragma once
#include "Manager.h"

class GLFWwindow;

class GLManager: public Manager 
{
public:
	static GLManager* GetSingleton();
	static void ShutDownSingleton();

	virtual void init();
	virtual void update();
	virtual void release();

	void processInput(GLFWwindow* window);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	inline GLFWwindow* getWindow() { return window; }
	inline int getWindowWidth() { return SRC_WIDTH; }
	inline int getWindowHeight() { return SRC_HEIGHT; }
private:
	static GLManager* instance;
	GLFWwindow* window;
	unsigned int VBO, VAO, EBO;
	// settings
	const unsigned int SRC_WIDTH = 1280;
	const unsigned int SRC_HEIGHT = 720;

	GLManager();
	~GLManager();
};

