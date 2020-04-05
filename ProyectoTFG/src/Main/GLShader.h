#pragma once

#include <string>
#include "ShaderUtils.h"

class GLShader
{
public:
	// constructor generates the shader on the fly
	// ------------------------------------------------------------------------
	GLShader();

	~GLShader();

	virtual void use() = 0;
	virtual void release() = 0;
	inline const unsigned int getId() const { return ID; }
protected:
	unsigned int ID;
	std::string pathName = "..\\..\\Shaders\\";

	// utility function for checking shader compilation/linking errors.
	// ------------------------------------------------------------------------
	unsigned int compileShader(unsigned int type, const char* source);
	void checkCompileErrors(unsigned int shader, std::string type);
};