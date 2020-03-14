#pragma once

#include "VulkanShader.h"

class VulkanRenderShader: public VulkanShader
{
private:
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	UniformBufferObject ubo;

	const std::string rawVertexName = "rawVertex";
	const std::string rawFragmentName = "rawFragment";

	virtual void destroyModules();

public:
	VulkanRenderShader();

	~VulkanRenderShader();

	void init(std::string vertexPath, std::string fragmentPath);
	virtual void use() {};
	virtual void release();

	void setUBO(const UniformBufferObject value);

	inline VkPipelineShaderStageCreateInfo getVertexStageInfo() { return vertShaderStageInfo; }
	inline VkPipelineShaderStageCreateInfo getFragmentStageInfo() { return fragShaderStageInfo; }
	inline UniformBufferObject getUBO() { return ubo; }
};

