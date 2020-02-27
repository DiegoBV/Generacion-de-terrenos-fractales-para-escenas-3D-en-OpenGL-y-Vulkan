#pragma once

#include <string>
#include <vulkan.hpp>
#include "ShaderUtils.h"

class VulkanShader
{
private:
	/// helper function. Creates a VkShaderModule
	VkShaderModule createShaderModule(const std::vector<char>& code);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	UniformBufferObject ubo;

	const std::string rawVertexName = "rawVertex";
	const std::string rawFragmentName = "rawFragment";
	const std::string compiledExtension = ".spv";

	void destroyModules();

public:
	VulkanShader();

	~VulkanShader();

	void init(std::string vertexPath, std::string fragmentPath);
	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	void release();

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string& name, bool value) const;

	// ------------------------------------------------------------------------
	void setInt(const std::string& name, int value) const;

	// ------------------------------------------------------------------------
	void setFloat(const std::string& name, float value) const;

	// ------------------------------------------------------------------------
	void setVec2(const std::string& name, const glm::vec2& value) const;

	void setVec2(const std::string& name, float x, float y) const;

	// ------------------------------------------------------------------------
	void setVec3(const std::string& name, const glm::vec3& value) const;

	void setVec3(const std::string& name, float x, float y, float z) const;

	// ------------------------------------------------------------------------
	void setVec4(const std::string& name, const glm::vec4& value) const;

	void setVec4(const std::string& name, float x, float y, float z, float w);

	// ------------------------------------------------------------------------
	void setMat2(const std::string& name, const glm::mat2& mat) const;

	// ------------------------------------------------------------------------
	void setMat3(const std::string& name, const glm::mat3& mat) const;

	// ------------------------------------------------------------------------
	void setMat4(const std::string& name, const glm::mat4& mat) const;

	void setStruct(const UniformBufferObject value);

	inline VkPipelineShaderStageCreateInfo getVertexStageInfo() { return vertShaderStageInfo; }
	inline VkPipelineShaderStageCreateInfo getFragmentStageInfo() { return fragShaderStageInfo; }
	inline UniformBufferObject getStruct() { return ubo; }
};

