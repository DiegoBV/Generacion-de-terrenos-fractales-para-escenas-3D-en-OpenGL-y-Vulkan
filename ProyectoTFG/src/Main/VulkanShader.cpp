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

#endif
