#pragma once
#include "Manager.h"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <vulkan.hpp>

class GLFWwindow;

class VulkanManager : public Manager
{
private:
	struct QueueFamilyIndices {
		uint32_t graphicsFamily = UINT32_MAX;

		bool isValid() {
			return graphicsFamily != UINT32_MAX;
		}
	};

	static VulkanManager* instance;
	GLFWwindow* window;
	/// vulkan instance
	VkInstance vkInstance;
	/// selected physical device (GPU)
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	/// logical device, used to interact with the physical device
	VkDevice logicDevice = VK_NULL_HANDLE;
	/// graphic command queue, needed to interact with the logic device (tell the GPU to do things)
	VkQueue graphicsCommandQueue;

	unsigned int VBO, VAO, EBO;
	// settings
	const unsigned int SRC_WIDTH = 1280;
	const unsigned int SRC_HEIGHT = 720;

	/// creates vulkan instance
	void createInstance();
	/// picks the most suitable device for this
	void pickPhysicalDevice();
	/// basic requirements, if the device fulfills them, returns true
	bool isDeviceSuitable(VkPhysicalDevice device);
	/// finds the queue family that supports GRAPHICS BIT
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	/// set up a logical device to interact with the physical device
	void createLogicalDevice();
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

