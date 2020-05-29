#if defined(VULKAN_DEBUG) || defined(VULKAN_RELEASE)

#include "VulkanComputeShader.h"
#include "VulkanManager.h"
#include "FileHandler.h"
#include "ShaderInclude.h"

VulkanComputeShader::VulkanComputeShader()
{
}

VulkanComputeShader::~VulkanComputeShader()
{
}

void VulkanComputeShader::init(std::string computeName)
{
	std::string computeCode = ShaderInclude::InterpretShader((pathName + computeName).c_str(), "#include");

	const char* cShaderCode = computeCode.c_str();

	std::string rawComputePath = pathName + rawComputeName + ".comp";
	std::fstream rawCompute = FileHandler::openOutputTruncatedFile(rawComputePath.c_str());
	FileHandler::writeRawStringToOutputFile(rawCompute, cShaderCode);
	FileHandler::closeFile(rawCompute);

	system(("cd .. & cd .. & cd Shaders/compile & AutoCompileShader.bat " + rawComputeName + " .comp").c_str());

	auto compShaderCode = FileHandler::readBinaryFile((pathName + rawComputeName + compiledExtension).c_str());

	compShaderModule = createShaderModule(compShaderCode);

	compShaderStageInfo.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	compShaderStageInfo.stage.stage = VK_SHADER_STAGE_COMPUTE_BIT;
	compShaderStageInfo.stage.module = compShaderModule;
	compShaderStageInfo.stage.pName = "main";
	compShaderStageInfo.layout = VulkanManager::GetSingleton()->getComputePipelineLayout();

	// delete unused files
	FileHandler::deleteFile(rawComputePath.c_str());
}

void VulkanComputeShader::release()
{
	destroyModules();
	// delete compiled files
	FileHandler::deleteFile((pathName + rawComputeName + compiledExtension).c_str());
}

void VulkanComputeShader::destroyModules()
{
	if (compShaderModule != nullptr) {
		vkDestroyShaderModule(VulkanManager::GetSingleton()->getLogicalDevice(), compShaderModule, nullptr);
	}
}

#endif
