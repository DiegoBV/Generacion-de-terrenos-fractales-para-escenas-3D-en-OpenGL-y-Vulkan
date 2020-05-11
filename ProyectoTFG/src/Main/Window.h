#pragma once
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

class Window
{
private:
	GLFWwindow* window = nullptr;
	static Window* instance;
	// settings
	const unsigned int SRC_WIDTH = 1280;
	const unsigned int SRC_HEIGHT = 720;

	Window();
	~Window();
	void processInput(GLFWwindow* window);

public:
	void init();
	void update();
	bool shouldClose();

	inline GLFWwindow* getWindow() { return window; };
	inline const unsigned int getWindowWidth() { return SRC_WIDTH; };
	inline const unsigned int getWindowHeight() { return SRC_HEIGHT; };
	void setKeyCallback(GLFWkeyfun function);
	void setCursorCallback(GLFWcursorposfun function);

	static Window* GetSingleton();
	static void ShutDownSingleton();

};

