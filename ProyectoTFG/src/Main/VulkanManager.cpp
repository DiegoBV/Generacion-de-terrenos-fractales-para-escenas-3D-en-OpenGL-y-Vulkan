#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)
#include "VulkanManager.h"
#include <iostream>
#define GLFW_INCLUDE_VULKAN

VulkanManager* VulkanManager::instance = nullptr;

VulkanManager* VulkanManager::GetSingleton()
{
	if (instance == nullptr) instance = new VulkanManager();
	return instance;
}

void VulkanManager::ShutDownSingleton()
{
	if (instance != nullptr) {
		instance->release();
		delete instance; instance = nullptr;
	}
}

void VulkanManager::init()
{
	// glfw: initialize and configure
	// ------------------------------
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

	// glfw window creation
	// --------------------
	window = glfwCreateWindow(SRC_WIDTH, SRC_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(getWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	createInstance();
	pickPhysicalDevice();
}

void VulkanManager::update()
{
	// input
	// -----
	//std::cout << camera.getFront().x << " " << camera.getFront().z << std::endl;
	processInput(window);

	// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
	// -------------------------------------------------------------------------------
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void VulkanManager::release()
{
	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	vkDestroyDevice(logicDevice, nullptr);
	vkDestroyInstance(vkInstance, nullptr);
	glfwDestroyWindow(window);
	glfwTerminate();
}

void VulkanManager::setKeyCallback(GLFWkeyfun function)
{
	glfwSetKeyCallback(getWindow(), function);
}

void VulkanManager::setCursorCallback(GLFWcursorposfun function)
{
	glfwSetCursorPosCallback(getWindow(), function);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void VulkanManager::processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void VulkanManager::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	
}

bool VulkanManager::shouldClose()
{
	return glfwWindowShouldClose(getWindow());
}

void VulkanManager::createInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &vkInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}
}

void VulkanManager::pickPhysicalDevice()
{
	// lo primero es saber cuantas gpus tenemos
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);
	if (deviceCount == 0) {
		throw std::runtime_error("ERROR::NO_GPUS_WITH_VULKAN_SUPPORT");
	}

	// si hay, las almacenamos
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

	// saber si las gpus encontradas son adecuadas
	for (const auto& device : devices) {
		if (isDeviceSuitable(device)) {
			physicalDevice = device;
			break;
		}
	}

	if (physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error("ERROR::NO_SUITABLE_GPU");
	}
}

bool VulkanManager::isDeviceSuitable(VkPhysicalDevice device)
{
	// atendemos a propiedades basicas, se puede dar un score a cada grafica si lo vemos necesario (tutorial), si cumple los requisitos, devuelve true
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);
	QueueFamilyIndices indices = findQueueFamilies(device);

	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
		deviceFeatures.geometryShader && indices.isValid();
}

VulkanManager::QueueFamilyIndices VulkanManager::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	// al menos una
	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
			if (indices.isValid()) {
				break;
			}
		}
		i++;
	}

	return indices;
}

void VulkanManager::createLogicalDevice()
{
	// especifica las colas que van a ser creadas. Solo necesitaremos una en nuestro caso, con capacidades graficas
	QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

	VkDeviceQueueCreateInfo queueCreateInfo = {};
	queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCreateInfo.queueFamilyIndex = indices.graphicsFamily;
	queueCreateInfo.queueCount = 1;

	float queuePriority = 1.0f;
	queueCreateInfo.pQueuePriorities = &queuePriority;

	// nada importante por ahora
	VkPhysicalDeviceFeatures deviceFeatures = {};

	// creacion del dispositivo logico
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = &queueCreateInfo;
	createInfo.queueCreateInfoCount = 1;
	createInfo.pEnabledFeatures = &deviceFeatures;

	// por ahora, no tenemos validation layers, asi que se deja a 0
	createInfo.enabledExtensionCount = 0;
	//if (enableValidationLayers) {
	//	createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
	//	createInfo.ppEnabledLayerNames = validationLayers.data();
	//}
	//else {
	createInfo.enabledLayerCount = 0;
	//}

	// creacion del dispositivo logico
	if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &logicDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(logicDevice, indices.graphicsFamily, 0, &graphicsCommandQueue);
}

VulkanManager::VulkanManager()
{
}

VulkanManager::~VulkanManager()
{
}

#endif
