#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)

#include "VulkanShader.h"
#include "VulkanManager.h"
#include "FileHandler.h"
#include "ShaderInclude.h"

VkShaderModule VulkanShader::createShaderModule(const std::vector<char>& code)
{
	// we need to specify the code and its length
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

VulkanShader::VulkanShader()
{
}

VulkanShader::~VulkanShader()
{
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

	vertShaderModule = createShaderModule(vertShaderCode);
	fragShaderModule = createShaderModule(fragShaderCode);

	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";
}

void VulkanShader::use()
{
}

void VulkanShader::release()
{
	destroyModules();
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

void VulkanShader::setStruct(const UniformBufferObject value)
{
	this->ubo = value;
}

void VulkanShader::destroyModules()
{
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), fragShaderModule, nullptr);
}

#endif
