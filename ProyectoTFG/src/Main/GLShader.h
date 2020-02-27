#pragma once

#include <string>
#include <glm.hpp>

class GLShader
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
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	GLShader();

	~GLShader();

	void init(const char* vertexPath, const char* fragmentPath);
	// activate the shader
	// ------------------------------------------------------------------------
	void use();

	void release() {};

	// utility uniform functions
	// ------------------------------------------------------------------------
	void setBool(const std::string &name, bool value) const;

	// ------------------------------------------------------------------------
	void setInt(const std::string &name, int value) const;

	// ------------------------------------------------------------------------
	void setFloat(const std::string &name, float value) const;

	// ------------------------------------------------------------------------
	void setVec2(const std::string &name, const glm::vec2 &value) const;

	void setVec2(const std::string &name, float x, float y) const;

	// ------------------------------------------------------------------------
	void setVec3(const std::string &name, const glm::vec3 &value) const;

	void setVec3(const std::string &name, float x, float y, float z) const;

	// ------------------------------------------------------------------------
	void setVec4(const std::string &name, const glm::vec4 &value) const;

	void setVec4(const std::string &name, float x, float y, float z, float w);

	// ------------------------------------------------------------------------
	void setMat2(const std::string &name, const glm::mat2 &mat) const;

	// ------------------------------------------------------------------------
	void setMat3(const std::string &name, const glm::mat3 &mat) const;

	// ------------------------------------------------------------------------
	void setMat4(const std::string &name, const glm::mat4 &mat) const;

	void setStruct(const UniformBufferObject value);
private:
	unsigned int ID;
	UniformBufferObject ubo;
	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	unsigned int compileShader(unsigned int type, const char* source);

	unsigned int createGLProgram(unsigned int vertexShader, unsigned int fragmentShader);

	void checkCompileErrors(unsigned int shader, std::string type);
};