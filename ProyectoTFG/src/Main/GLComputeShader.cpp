#if defined (GL_DEBUG) || defined (GL_RELEASE)
#include "GLComputeShader.h"
#include <glad/glad.h>
#include "FileHandler.h"
#include "ShaderInclude.h"
#include <GLFW/glfw3.h>
#include <iostream>

GLComputeShader::GLComputeShader()
{
}

GLComputeShader::~GLComputeShader()
{
}

void GLComputeShader::init(std::string computeName)
{
	std::string computeCode = ShaderInclude::InterpretShader((pathName + computeName).c_str(), "#include");

	const char* cShaderCode = computeCode.c_str();

	unsigned int compute;
	// compute shader
	compute = compileShader(GL_COMPUTE_SHADER, cShaderCode);
	checkCompileErrors(compute, "COMPUTE");
	// compute program
	gComputeProgram = createComputeShaderProgram(compute);
	checkCompileErrors(gComputeProgram, "PROGRAM");

	glDeleteShader(compute);
}

void GLComputeShader::use()
{
	checkMemorySharing();
	glUseProgram(ID);
}

void GLComputeShader::setSSBO(const StorageBufferObject value)
{
	ssbo = value;
}

unsigned int GLComputeShader::createComputeShaderProgram(unsigned int computeShader)
{
	// Create the compute program, to which the compute shader will be assigned
	unsigned int computeProgramID = glCreateProgram();

	// Attach and link the shader against to the compute program
	glAttachShader(computeProgramID, computeShader);
	glLinkProgram(computeProgramID);

	checkCompileErrors(computeShader, "COMPUTE");

	return computeProgramID;
}

void GLComputeShader::checkMemorySharing()
{
	// compute shader
	GLuint SSBO = 0;

	std::vector<GLfloat> initPos;
	int num_numeros = 12;

	glUseProgram(gComputeProgram);

	glGenBuffers(1, &SSBO);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, num_numeros * sizeof(GLfloat), &initPos, GL_DYNAMIC_DRAW);

	glDispatchCompute(num_numeros / 2, 1, 1);
	glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, 0);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBO);

	GLfloat* ptr;
	ptr = (GLfloat*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
	initPos.clear();

	// glGetNamedBufferSubData(SSBO, 0, 12 * sizeof(float), initPos.data());
	for (int i = 0; i < num_numeros; i++) {
		initPos.push_back(ptr[i]);
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
}
#endif
