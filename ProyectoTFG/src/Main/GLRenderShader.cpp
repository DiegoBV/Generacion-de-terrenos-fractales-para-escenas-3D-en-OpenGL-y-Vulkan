#if defined (GL_DEBUG) || defined (GL_RELEASE)
#include "GLRenderShader.h"
#include <glad/glad.h>
#include "FileHandler.h"
#include "ShaderInclude.h"
#include <GLFW/glfw3.h>
#include <iostream>

GLRenderShader::GLRenderShader()
{
}

GLRenderShader::~GLRenderShader()
{
}

void GLRenderShader::init(std::string vertexName, std::string fragmentName)
{
	std::string vertexCode = ShaderInclude::InterpretShader((pathName + vertexName).c_str(), "#include");
	std::string fragmentCode = ShaderInclude::InterpretShader((pathName + fragmentName).c_str(), "#include");

	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();

	// COMPILE SHADERS
	unsigned int vertex, fragment;
	// vertex shader
	vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	ID = createGLProgram(vertex, fragment);
	checkCompileErrors(ID, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

unsigned int GLRenderShader::createGLProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	return programID;
}

void GLRenderShader::use()
{
	glUseProgram(ID);
}

void GLRenderShader::setUBO(const UniformBufferObject value)
{
	ubo = value;
	ubo.yDirection = 1;

	GLuint uniformBlockIndex = glGetUniformBlockIndex(ID, "ubo");
	glUniformBlockBinding(ID, uniformBlockIndex, 0);

	unsigned int uboMatrices;
	glGenBuffers(1, &uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(ubo), NULL, GL_STATIC_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	// define the range of the buffer that links to a uniform binding point
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, uboMatrices, 0, sizeof(ubo));

	glBindBuffer(GL_UNIFORM_BUFFER, uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ubo), &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

#endif
