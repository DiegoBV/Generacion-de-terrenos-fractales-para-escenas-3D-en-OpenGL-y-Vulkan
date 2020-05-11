#pragma once
#include <vulkan.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <list>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

// test shader variables
class StorageBufferObject;

struct Vertex {
	glm::vec3 pos;
	glm::vec2 texCoord;

	bool operator==(const Vertex& other) const {
		return pos == other.pos && texCoord == other.texCoord;
	}

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
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT; // format. see vulkan help
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		// color
		/*attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);*/

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, texCoord);

		return attributeDescriptions;
	}
};

namespace std {
	template<> struct hash<Vertex> {
		size_t operator()(Vertex const& vertex) const {
			return ((hash<glm::vec3>()(vertex.pos) ^ (hash<glm::vec2>()(vertex.texCoord) << 1)));
		}
	};
}

// end of test shader variables

class GLFWwindow;
class VulkanRenderShader;
class VulkanComputeShader;

class VulkanManager
{
private:
	std::list<VulkanRenderShader*> renderShaders;
	std::list<VulkanComputeShader*> computeShaders;

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

	QueueFamilyIndices _indices;

	// struct auxiliar para el swap chain
	struct SwapChainSupportDetails {
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};
	static VulkanManager* instance;

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
	const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_MAINTENANCE1_EXTENSION_NAME };
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
	// vertex descriptor
	VkDescriptorSetLayout descriptorSetLayout;
	// compute descriptor
	VkDescriptorSetLayout computeDescriptorSetLayout = nullptr;
	// used for uniform variables
	VkPipelineLayout pipelineLayout;
	// used for uniform variables in compute shader
	VkPipelineLayout computePipelineLayout = nullptr;
	// handler of the pipeline
	std::vector<VkPipeline> graphicsPipelines;
	// handler of the compute pipeline
	VkPipeline computePipeline = nullptr;
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
	// handle of the vertex buffer memory
	std::vector<VkBuffer> vertexBuffers;
	std::vector<VkDeviceMemory> vertexBuffersMemory;
	std::vector<VkBuffer> indexBuffers;
	std::vector<VkDeviceMemory> indexBuffersMemory;
	// we want more than one uniform buffer (avoid reading and writing collision)
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;
	VkBuffer storageBuffer;
	VkDeviceMemory storageBufferMemory;
	VkDescriptorPool descriptorPool;
	VkDescriptorPool computeDescriptorPool = nullptr;
	std::vector<VkDescriptorSet> descriptorSets;
	VkDescriptorSet computeDescriptorSets;
	// Compute shader
	VkPipelineCache pipelineCache;
	VkCommandPool computeCommandPool;
	VkQueue computeCommandQueue;
	VkCommandBuffer computeCommandBuffer;

	// texturas
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;

	VkImage depthImage;
	VkDeviceMemory depthImageMemory;
	VkImageView depthImageView;

	const std::string MODEL_PATH = "..\\Assets\\Models\\pacman\\pacman.obj";
	const std::string TEXTURE_PATH = "..\\Assets\\Models\\pacman\\pacmantex.jpg";

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
	/// define the descriptor of the matrix (SHADER)
	void createDescriptorSetLayout();
	/// define the computer descriptor of the matrix (SHADER)
	void createComputeDescriptorSetLayout();
	/// pipeline: vertez shader, tessellation, geometry shader, fragment shader...
	void createGraphicsPipelines();

	void memoryBarrier(VkCommandBuffer cmd, VkAccessFlags srcAccessMask,
		VkAccessFlags dstAccessMask, VkPipelineStageFlags srcStageMask,
		VkPipelineStageFlags dstStageMask);

	void createComputePipeline();
	/// fill the frameBuffer vector
	void createFramebuffers();
	/// creates and fills the commandPool object
	void createCommandPool();
	/// variables that can be read by the graphic card (in attributes of the vertex shader) (SHADER)
	void createVertexBuffers();
	/// an index buffer allows us to reuse existing data, reordenate the vertex buffer data (SHADER)
	void createIndexBuffers();
	/// fills the uniformBuffer vector
	void createUniformBuffers();
	/// fills the storageBuffer vector
	void createStorageBuffers();
	/// descriptors cant be created directly, thera must be a descriptor pool
	void createDescriptorPool();
	void createComputeDescriptorPool();
	/// creation of the descriptors
	void createDescriptorSets();
	/// creation of the compute descriptors
	void createComputeDescriptorSets();
	/// creates the command buffers (one for each image of the swap chain) (SHADER + OTRAS COSAS)
	void createCommandBuffers();
	/// creates the needed semaphores
	void createSyncObjects();

	/// graphic cards offer different types of memory, so... (TODO ESTO ES SHADER)
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	void updateUniformBuffer(uint32_t currentImage);

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
	void runComputeShader();

	// compute shaders
	void createComputePipelineLayout();
	void createPipelineCache();
	void createComputeCommandPool();
	void createComputeCommandBuffer();
	void createTextureSampler();

	void updateStorageBuffer();
	StorageBufferObject getStorageBuffer();

	// modelo y texturas
	// load texture
	void createTextureImage();
	void createImage(uint32_t width, uint32_t height, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
	void createTextureImageView();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	void createDepthResources();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
	VkFormat findDepthFormat();
	bool hasStencilComponent(VkFormat format);

	void loadModel();

	VulkanManager();
	~VulkanManager();

public:
	std::vector<std::vector<Vertex>> vertexVector = {
		{{{-1.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
		{{1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
		{{1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
		{{-1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}}}
		,{}
	};

	std::vector<std::vector<uint32_t>> indexVector = {
		{0, 1, 2, 2, 3, 0},
		{}
	};

	static VulkanManager* GetSingleton();
	static void ShutDownSingleton();

	void init();
	void update();
	void render();
	void release();
	void waitUntilFinishEverything();
	void setUpGraphicsPipeline();

	inline VkDevice getLogicalDevice() { return logicalDevice; }
	inline VkPipelineLayout getComputePipelineLayout() { return computePipelineLayout; }
	inline void addRenderShader(VulkanRenderShader* shader) { renderShaders.push_back(shader); }
	inline void setModelShader(VulkanRenderShader* shader) { renderShaders.push_back(shader); }
	inline void addComputeShader(VulkanComputeShader* shader) { computeShaders.push_back(shader); }
};

