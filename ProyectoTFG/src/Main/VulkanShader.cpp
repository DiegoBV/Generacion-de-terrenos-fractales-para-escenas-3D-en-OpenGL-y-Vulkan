#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)

#include "VulkanShader.h"
#include "VulkanManager.h"
#include "FileHandler.h"
#include "ShaderInclude.h"

void VulkanShader::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanManager::GetSingleton()->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBufferMemory);

	VulkanManager::GetSingleton()->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		*VulkanManager::GetSingleton()->getVertexBuffer(), *VulkanManager::GetSingleton()->getVertexBufferMemory());

	VulkanManager::GetSingleton()->copyBuffer(stagingBuffer, *VulkanManager::GetSingleton()->getVertexBuffer(), bufferSize);

	vkDestroyBuffer(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void VulkanShader::createIndexBuffer()
{
	VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	VulkanManager::GetSingleton()->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, indices.data(), (size_t)bufferSize);
	vkUnmapMemory(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBufferMemory);

	VulkanManager::GetSingleton()->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
		*VulkanManager::GetSingleton()->getIndexBuffer(), *VulkanManager::GetSingleton()->getIndexBufferMemory());

	VulkanManager::GetSingleton()->copyBuffer(stagingBuffer, *VulkanManager::GetSingleton()->getIndexBuffer(), bufferSize);

	vkDestroyBuffer(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(VulkanManager::GetSingleton()->getLogicalDevice(), stagingBufferMemory, nullptr);
}

void VulkanShader::createUniformBuffers()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	VulkanManager::GetSingleton()->getUniformBuffers().resize(VulkanManager::GetSingleton()->getSwapChainImages().size());
	VulkanManager::GetSingleton()->getUniformBuffersMemory().resize(VulkanManager::GetSingleton()->getSwapChainImages().size());

	for (size_t i = 0; i < VulkanManager::GetSingleton()->getSwapChainImages().size(); i++) {
		VulkanManager::GetSingleton()->createBuffer(bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
			VulkanManager::GetSingleton()->getUniformBuffers()[i], VulkanManager::GetSingleton()->getUniformBuffersMemory()[i]);
	}
}

void VulkanShader::createDescriptorSets()
{
	std::vector<VkDescriptorSetLayout> layouts(VulkanManager::GetSingleton()->getSwapChainImages().size(), VulkanManager::GetSingleton()->getDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = VulkanManager::GetSingleton()->getDescriptorPool();
	allocInfo.descriptorSetCount = static_cast<uint32_t>(VulkanManager::GetSingleton()->getSwapChainImages().size());
	allocInfo.pSetLayouts = layouts.data();

	VulkanManager::GetSingleton()->getDescriptorSets().resize(VulkanManager::GetSingleton()->getSwapChainImages().size());
	if (vkAllocateDescriptorSets(VulkanManager::GetSingleton()->getLogicalDevice(), &allocInfo, VulkanManager::GetSingleton()->getDescriptorSets().data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < VulkanManager::GetSingleton()->getSwapChainImages().size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = VulkanManager::GetSingleton()->getUniformBuffers()[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = VulkanManager::GetSingleton()->getDescriptorSets()[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;

		vkUpdateDescriptorSets(VulkanManager::GetSingleton()->getLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
	}
}

VulkanShader::VulkanShader()
{
}

VulkanShader::~VulkanShader()
{
	delete shaderStages;
	shaderStages = nullptr;
}

void VulkanShader::init(const char* vertexPath, const char* fragmentPath)
{
	std::string vertexCode = ShaderInclude::InterpretShader(vertexPath, "#include");
	std::string fragmentCode = ShaderInclude::InterpretShader(fragmentPath, "#include");

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	system("cd .. & cd .. & cd Dependencies/Vulkan & AutoCompileShaders.bat vvert vfrag");

	auto vertShaderCode = FileHandler::readBinaryFile("..\\..\\Shaders\\vvert.spv");
	auto fragShaderCode = FileHandler::readBinaryFile("..\\..\\Shaders\\vfrag.spv");

	VkShaderModule vertShaderModule = VulkanManager::GetSingleton()->createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = VulkanManager::GetSingleton()->createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	shaderStages = new VkPipelineShaderStageCreateInfo[2];
	shaderStages[0] = vertShaderStageInfo;
	shaderStages[0] = fragShaderStageInfo;

	vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), vertShaderModule, nullptr);
}

void VulkanShader::use()
{
}

VkPipelineShaderStageCreateInfo* VulkanShader::getShaderStages()
{
	return shaderStages;
}

VkPipelineVertexInputStateCreateInfo* VulkanShader::getVertexInputInfo()
{
	return &vertexInputInfo;
}

void VulkanShader::setBool(const std::string& name, bool value) const
{
}

void VulkanShader::setInt(const std::string& name, int value) const
{
}

void VulkanShader::setFloat(const std::string& name, float value) const
{
}

void VulkanShader::setVec2(const std::string& name, const glm::vec2& value) const
{
}

void VulkanShader::setVec2(const std::string& name, float x, float y) const
{
}

void VulkanShader::setVec3(const std::string& name, const glm::vec3& value) const
{
}

void VulkanShader::setVec3(const std::string& name, float x, float y, float z) const
{
}

void VulkanShader::setVec4(const std::string& name, const glm::vec4& value) const
{
}

void VulkanShader::setVec4(const std::string& name, float x, float y, float z, float w)
{
}

void VulkanShader::setMat2(const std::string& name, const glm::mat2& mat) const
{
}

void VulkanShader::setMat3(const std::string& name, const glm::mat3& mat) const
{
}

void VulkanShader::setMat4(const std::string& name, const glm::mat4& mat) const
{
}

#endif
