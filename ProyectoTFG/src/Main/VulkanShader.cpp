#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)

#include "VulkanShader.h"
#include "VulkanManager.h"
#include "FileHandler.h"
#include "ShaderInclude.h"

std::string pathName = "..\\..\\Shaders\\";

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

void VulkanShader::init(std::string vertexName, std::string fragmentName)
{
	std::string vertexCode = ShaderInclude::InterpretShader((pathName + vertexName).c_str(), "#include");
	std::string fragmentCode = ShaderInclude::InterpretShader((pathName + fragmentName).c_str(), "#include");
	std::string computeCode = ShaderInclude::InterpretShader((pathName + "compute.c").c_str(), "#include");

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	const char* cShaderCode = computeCode.c_str();

	std::string rawVertexPath = pathName + rawVertexName + ".vert";
	std::string rawFragmentPath = pathName + rawFragmentName + ".frag";
	std::string rawComputePath = pathName + rawComputeName + ".comp";
	std::fstream rawVertex = FileHandler::openOutputTruncatedFile(rawVertexPath.c_str());
	FileHandler::writeRawStringToOutputFile(rawVertex, vShaderCode);
	FileHandler::closeFile(rawVertex);
	std::fstream rawFragment = FileHandler::openOutputTruncatedFile(rawFragmentPath.c_str());
	FileHandler::writeRawStringToOutputFile(rawFragment, fShaderCode);
	FileHandler::closeFile(rawFragment);
	std::fstream rawCompute = FileHandler::openOutputTruncatedFile(rawComputePath.c_str());
	FileHandler::writeRawStringToOutputFile(rawCompute, cShaderCode);
	FileHandler::closeFile(rawCompute);

	system(("cd .. & cd .. & cd Dependencies/Vulkan & AutoCompileShaders.bat " + rawVertexName + " " + rawFragmentName + " " + rawComputeName).c_str());

	auto vertShaderCode = FileHandler::readBinaryFile((pathName + rawVertexName + compiledExtension).c_str());
	auto fragShaderCode = FileHandler::readBinaryFile((pathName + rawFragmentName + compiledExtension).c_str());
	auto compShaderCode = FileHandler::readBinaryFile((pathName + rawComputeName + compiledExtension).c_str());

	vertShaderModule = createShaderModule(vertShaderCode);
	fragShaderModule = createShaderModule(fragShaderCode);
	compShaderModule = createShaderModule(compShaderCode);

	vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module = vertShaderModule;
	vertShaderStageInfo.pName = "main";

	fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module = fragShaderModule;
	fragShaderStageInfo.pName = "main";

	compShaderStageInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	compShaderStageInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	compShaderStageInfo.stage.module = compShaderModule;
	compShaderStageInfo.stage.pName = "main";
	compShaderStageInfo.layout = VulkanManager::GetSingleton()->getComputePipelineLayout();

	// delete unused files
	FileHandler::deleteFile(rawVertexPath.c_str());
	FileHandler::deleteFile(rawFragmentPath.c_str());
	FileHandler::deleteFile(rawComputePath.c_str());
}

void VulkanShader::use()
{
}

void VulkanShader::release()
{
	destroyModules();
	// delete compiled files
	FileHandler::deleteFile((pathName + rawVertexName + compiledExtension).c_str());
	FileHandler::deleteFile((pathName + rawFragmentName + compiledExtension).c_str());
	FileHandler::deleteFile((pathName + rawComputeName + compiledExtension).c_str());
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
	ubo = value;
	ubo.yDirection = -1;
}

void VulkanShader::destroyModules()
{
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), fragShaderModule, nullptr);
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), compShaderModule, nullptr);
}

#endif
