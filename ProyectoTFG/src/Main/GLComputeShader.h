#pragma once

#include "GLShader.h"

class GLComputeShader: public GLShader
{
public:
	GLComputeShader();

	~GLComputeShader();

	void init(std::string computeName);
	virtual void use();
	virtual void release() {};

	inline void setSSBO(const StorageBufferObject value) { ssbo = value; }
	inline StorageBufferObject getSSBO() { return ssbo; }

private:
	StorageBufferObject ssbo;
	unsigned int gComputeProgram;
	unsigned int createComputeShaderProgram(unsigned int computeShader);
	void checkMemorySharing();
};