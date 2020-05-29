#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)

#include "VulkanRenderShader.h"
#include "VulkanManager.h"
#include "FileHandler.h"
#include "ShaderInclude.h"

VulkanRenderShader::VulkanRenderShader()
{
}

VulkanRenderShader::~VulkanRenderShader()
{
}

void VulkanRenderShader::init(std::string vertexName, std::string fragmentName)
{
	std::string vertexCode = ShaderInclude::InterpretShader((pathName + vertexName).c_str(), "#include");
	std::string fragmentCode = ShaderInclude::InterpretShader((pathName + fragmentName).c_str(), "#include");

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	std::string rawVertexPath = pathName + rawVertexName + ".vert";
	std::string rawFragmentPath = pathName + rawFragmentName + ".frag";
	std::fstream rawVertex = FileHandler::openOutputTruncatedFile(rawVertexPath.c_str());
	FileHandler::writeRawStringToOutputFile(rawVertex, vShaderCode);
	FileHandler::closeFile(rawVertex);
	std::fstream rawFragment = FileHandler::openOutputTruncatedFile(rawFragmentPath.c_str());
	FileHandler::writeRawStringToOutputFile(rawFragment, fShaderCode);
	FileHandler::closeFile(rawFragment);

	system(("cd .. & cd .. & cd Shaders/compile & AutoCompileShader.bat " + rawVertexName + " .vert").c_str());
	system(("cd .. & cd .. & cd Shaders/compile & AutoCompileShader.bat " + rawFragmentName + " .frag").c_str());

	auto vertShaderCode = FileHandler::readBinaryFile((pathName + rawVertexName + compiledExtension).c_str());
	auto fragShaderCode = FileHandler::readBinaryFile((pathName + rawFragmentName + compiledExtension).c_str());

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

	// delete unused files
	FileHandler::deleteFile(rawVertexPath.c_str());
	FileHandler::deleteFile(rawFragmentPath.c_str());
}

void VulkanRenderShader::release()
{
	destroyModules();
	// delete compiled files
	FileHandler::deleteFile((pathName + rawVertexName + compiledExtension).c_str());
	FileHandler::deleteFile((pathName + rawFragmentName + compiledExtension).c_str());
}

void VulkanRenderShader::setUBO(const UniformBufferObject value)
{
	ubo = value;
	ubo.yDirection = -1;
}

void VulkanRenderShader::destroyModules()
{
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), vertShaderModule, nullptr);
	vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), fragShaderModule, nullptr);
}

#endif
