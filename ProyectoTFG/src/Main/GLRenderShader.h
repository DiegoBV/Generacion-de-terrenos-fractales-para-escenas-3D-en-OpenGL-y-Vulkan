#pragma once

#include "GLShader.h"

class GLRenderShader: public GLShader
{
public:
	GLRenderShader();
	~GLRenderShader();

	void init(std::string vertexName, std::string fragmentName);
	virtual void use();
	virtual void release() {};

	void setUBO(const UniformBufferObject value);

private:
	UniformBufferObject ubo;

	unsigned int createGLProgram(unsigned int vertexShader, unsigned int fragmentShader);
};