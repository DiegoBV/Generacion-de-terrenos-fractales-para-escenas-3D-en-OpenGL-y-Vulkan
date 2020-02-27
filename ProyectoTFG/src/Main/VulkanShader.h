#pragma once

#include <string>
#include <glm.hpp>
#include <vulkan.hpp>


class VulkanShader
{
public:
	struct UniformBufferObject {
		alignas(4) float time;
		alignas(8) glm::vec2 resolution;
		alignas(16) glm::vec3 cameraEye;
		alignas(16) glm::vec3 cameraFront;
		alignas(16) glm::vec3 worldUp;
		alignas(16) glm::mat4 viewMat;
	};

private:
	/// helper function. Creates a VkShaderModule
	VkShaderModule createShaderModule(const std::vector<char>& code);
	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	VkShaderModule vertShaderModule;
	VkShaderModule fragShaderModule;
	UniformBufferObject ubo;
	void destroyModules();

public:
	VulkanShader();

	~VulkanShader();

	void init(const char* vertexPath, const char* fragmentPath);
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

