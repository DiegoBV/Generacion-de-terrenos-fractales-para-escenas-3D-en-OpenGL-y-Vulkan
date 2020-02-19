#pragma once
#include "Manager.h"
#include <vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm.hpp>

// test shader variables

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0; // index of the binding in the array of bindings
		bindingDescription.stride = sizeof(Vertex); // number of bytes from one entry to the next
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // move to the next data entry after each vertex
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		// how to extract a vertex attribute from a chunk of vertex data originating from a binding description
		// position
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0; // from wich binding this comes
		attributeDescriptions[0].location = 0; // location at vertex shader
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT; // format. see vulkan help
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// color
		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
	{{-0.5f, -0.5f}, {0.5f, 0.0f, 0.0f}},
	{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

// end of test shader variables

class GLFWwindow;

class VulkanManager : public Manager
{
private:
	// struct auxiliar para la familia de colas
	struct QueueFamilyIndices {
		// support graphics integration
		uint32_t graphicsFamily = UINT32_MAX;
		// support device window presentation
		uint32_t presentFamily = UINT32_MAX;

		bool isValid() {
			return graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX;
		}
	};

	// struct auxiliar para el swap chain
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	static VulkanManager* instance;
	GLFWwindow* window;
	/// vulkan instance
	VkInstance vkInstance;
	/// debug and validation layers
	VkDebugUtilsMessengerEXT debugMessenger;
	/// selected physical device (GPU)
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	/// logical device, used to interact with the physical device
	VkDevice logicalDevice = VK_NULL_HANDLE;
	/// graphic command queue, needed to interact with the logic device (tell the GPU to do things)
	VkQueue graphicsCommandQueue;
	// window surface, needed to interact with the window system
	VkSurfaceKHR surface;
	// presentation command queue
	VkQueue presentCommandQueue;
	// swap chain things
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME};
	// swap chain storage
	VkSwapchainKHR swapChain;
	// storing the handles
	std::vector<VkImage> swapChainImages;
	// swap chain image format
	VkFormat swapChainImageFormat;
	// swap chain extent
	VkExtent2D swapChainExtent;
	// image views storage
	std::vector<VkImageView> swapChainImageViews;
	// info about render buffers
	VkRenderPass renderPass;
	// used for uniform variables
	VkPipelineLayout pipelineLayout;
	// handler of the pipeline
	VkPipeline graphicsPipeline;
	// one frame buffer for each swap chain image
	std::vector<VkFramebuffer> swapChainFramebuffers;
	// command Pool, needs to be created in order to use command buffers
	VkCommandPool commandPool;
	// command buffer, this one is for drawing commands
	std::vector<VkCommandBuffer> commandBuffers;
	// handle image semaphore, one for each frame in flight
	std::vector<VkSemaphore> imageAvailableSemaphores;
	// handle render semaphore,  one for each frame in flight
	std::vector<VkSemaphore> renderFinishedSemaphores;
	// fences are like semaphores but are available to wait in our code
	std::vector<VkFence> inFlightFences;
	// avoid rendering images that are currently being rendered
	std::vector<VkFence> imagesInFlight;
	// current frame presented
	size_t currentFrame = 0;

	// Shader buffer
	VkBuffer vertexBuffer;
	// handle of the vertex buffer memory
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	// validation layers
	const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
	};

#ifdef VULKAN_RELEASE
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

	unsigned int VBO, VAO, EBO;
	// settings
	const unsigned int SRC_WIDTH = 1280;
	const unsigned int SRC_HEIGHT = 720;
	const int MAX_FRAMES_IN_FLIGHT = 2;

	/// creates vulkan instance
	void createInstance();
	/// sets up the debugger and the validation layers
	void setupDebugMessenger();
	/// picks the most suitable device for this
	void pickPhysicalDevice();
	/// basic requirements, if the device fulfills them, returns true
	bool isDeviceSuitable(VkPhysicalDevice device);
	/// finds the queue family that supports GRAPHICS BIT and presentation for our surface
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	/// sets up a logical device to interact with the physical device
	void createLogicalDevice();
	/// sets up the surface. This needs to be called right after the "createInstance" method, as it directly influences the physical device selection
	void createSurface();
	/// creates and sets the swap chain
	void createSwapChain();
	/// creates a basic image view for every image in the swap chain
	void createImageViews();
	/// tells vulkan the information about our render buffers
	void createRenderPass();
	/// pipeline: vertez shader, tessellation, geometry shader, fragment shader...
	void createGraphicsPipeline();
	/// fill the frameBuffer vector
	void createFramebuffers();
	/// creates and fills the commandPool object
	void createCommandPool();
	/// variables that can be read by the graphic card (in attributes of the vertex shader) (SHADER)
	void createVertexBuffer();
	/// an index buffer allows us to reuse existing data, reordenate the vertex buffer data (SHADER)
	void createIndexBuffer();
	/// creates the command buffers (one for each image of the swap chain) (SHADER + OTRAS COSAS)
	void createCommandBuffers();
	/// creates the needed semaphores
	void createSyncObjects();

	/// graphic cards offer different types of memory, so... (TODO ESTO ES SHADER)
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	/// chek if the device has swap chain support
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	/// populate the swap chain struct
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	/// format the surface
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	/// choose the presentation mode
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
	/// resolution of the swap chain images
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	/// helper function. Creates a VkShaderModule (maybe we could move this to our shader class)
	VkShaderModule createShaderModule(const std::vector<char>& code);

	/// checks if all the requested layers are available
	bool checkValidationLayerSupport();
	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
	/// return the required list of extensions based on whether validation layers are enabled or not
	std::vector<const char*> getRequiredExtensions();
	/// debug callback function
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
	static void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

	/// acquire image from swap chain, execute the next command and returns the modified image to the swap chain for presentation. Maybe this is not the place for this.
	void drawFrame();

	VulkanManager();
	~VulkanManager();

public:
	static VulkanManager* GetSingleton();
	static void ShutDownSingleton();

	virtual void init();
	virtual void update();
	virtual void release();
	virtual void waitUntilFinishEverything();

	void setKeyCallback(GLFWkeyfun function);
	void setCursorCallback(GLFWcursorposfun function);
	void processInput(GLFWwindow* window);

	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);

	bool shouldClose();
	inline GLFWwindow* getWindow() { return window; }
	inline int getWindowWidth() { return SRC_WIDTH; }
	inline int getWindowHeight() { return SRC_HEIGHT; }
};

