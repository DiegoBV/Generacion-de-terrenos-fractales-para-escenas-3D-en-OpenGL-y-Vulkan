#pragma once

#include <string>
#include <glm.hpp>
#include <vulkan.hpp>

class VulkanShader
{
private:
	VkPipelineShaderStageCreateInfo* shaderStages;
	VkPipelineVertexInputStateCreateInfo vertexInputInfo;

public:
	VulkanShader();

	~VulkanShader();

	void init(const char* vertexPath, const char* fragmentPath);
	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	VkPipelineShaderStageCreateInfo* getShaderStages();
	VkPipelineVertexInputStateCreateInfo* getVertexInputInfo();

	/// variables that can be read by the graphic card (in attributes of the vertex shader) (SHADER)
	void createVertexBuffer();
	/// an index buffer allows us to reuse existing data, reordenate the vertex buffer data (SHADER)
	void createIndexBuffer();
	/// fills the uniformBuffer vector
	void createUniformBuffers();
	/// creation of the descriptors
	void createDescriptorSets();

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
};

