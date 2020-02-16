#pragma once
#include "Manager.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vulkan.hpp>

class GLFWwindow;

class VulkanManager : public Manager
{
private:
	static VulkanManager* instance;
	GLFWwindow* window;
	VkInstance vkInstance;
	unsigned int VBO, VAO, EBO;
	// settings
	const unsigned int SRC_WIDTH = 1280;
	const unsigned int SRC_HEIGHT = 720;

	VulkanManager();
	~VulkanManager();

public:
	static VulkanManager* GetSingleton();
	static void ShutDownSingleton();

	virtual void init();
	virtual void update();
	virtual void release();

	void setKeyCallback(GLFWkeyfun function);
	void setCursorCallback(GLFWcursorposfun function);
	void processInput(GLFWwindow* window);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	bool shouldClose();
	inline GLFWwindow* getWindow() { return window; }
	inline int getWindowWidth() { return SRC_WIDTH; }
	inline int getWindowHeight() { return SRC_HEIGHT; }
};

