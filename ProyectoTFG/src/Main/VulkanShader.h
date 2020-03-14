#pragma once

#include <string>
#include <vulkan.hpp>
#include "ShaderUtils.h"

class VulkanShader
{
protected:
	std::string pathName = "..\\..\\Shaders\\";
	const std::string compiledExtension = ".spv";

	/// helper function. Creates a VkShaderModule
	VkShaderModule createShaderModule(const std::vector<char>& code);

	virtual void destroyModules() = 0;

public:
	VulkanShader();

	~VulkanShader();

	virtual void use() = 0;
	virtual void release() = 0;
};

