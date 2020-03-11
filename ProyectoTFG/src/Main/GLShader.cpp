#if defined (GL_DEBUG) || defined (GL_RELEASE)
#include "GLShader.h"
#include <glad/glad.h>
#include "FileHandler.h"
#include "ShaderInclude.h"
#include <GLFW/glfw3.h>
#include <iostream>

std::string pathName = "..\\..\\Shaders\\";

GLShader::GLShader()
{
}

GLShader::~GLShader()
{
}

void GLShader::init(std::string vertexName, std::string fragmentName)
{
	std::string vertexCode = ShaderInclude::InterpretShader((pathName + vertexName).c_str(), "#include");
	std::string fragmentCode = ShaderInclude::InterpretShader((pathName + fragmentName).c_str(), "#include");
	std::string computeCode = ShaderInclude::InterpretShader((pathName + "compute.c").c_str(), "#include");
	
	const char* vShaderCode = vertexCode.c_str();
	const char * fShaderCode = fragmentCode.c_str();
	const char* cShaderCode = computeCode.c_str();

	// COMPILE SHADERS
	unsigned int vertex, fragment, compute;
	// vertex shader
	vertex = compileShader(GL_VERTEX_SHADER, vShaderCode);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = compileShader(GL_FRAGMENT_SHADER, fShaderCode);
	checkCompileErrors(fragment, "FRAGMENT");
	// compute shader
	compute = compileShader(GL_COMPUTE_SHADER, cShaderCode);
	checkCompileErrors(compute, "COMPUTE");
	// shader Program
	ID = createGLProgram(vertex, fragment);
	checkCompileErrors(ID, "PROGRAM");
	// compute program
	gComputeProgram = createComputeShaderProgram(compute);
	checkCompileErrors(gComputeProgram, "PROGRAM");

	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(compute);
}

unsigned int GLShader::compileShader(unsigned int type, const char* source)
{
	unsigned int shader;
	shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, NULL);
	glCompileShader(shader);

	return shader;
}

unsigned int GLShader::createGLProgram(unsigned int vertexShader, unsigned int fragmentShader)
{
	unsigned int programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	return programID;
}

void GLShader::use()
{
	checkMemorySharing();
	glUseProgram(ID);
}

void GLShader::setBool(const std::string & name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void GLShader::setInt(const std::string & name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setFloat(const std::string & name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setVec2(const std::string & name, const glm::vec2 & value) const
{
	glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void GLShader::setVec2(const std::string & name, float x, float y) const
{
	glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
}

void GLShader::setVec3(const std::string & name, const glm::vec3 & value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void GLShader::setVec3(const std::string & name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void GLShader::setVec4(const std::string & name, const glm::vec4 & value) const
{
	glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}

void GLShader::setVec4(const std::string & name, float x, float y, float z, float w)
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
}

void GLShader::setMat2(const std::string & name, const glm::mat2 & mat) const
{
	glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setMat3(const std::string & name, const glm::mat3 & mat) const
{
	glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setMat4(const std::string & name, const glm::mat4 & mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void GLShader::setStruct(const UniformBufferObject value)
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

void GLShader::checkCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
unsigned int GLShader::createComputeShaderProgram(unsigned int computeShader)
{
	// Create the compute program, to which the compute shader will be assigned
	unsigned int computeProgramID = glCreateProgram();

	// Attach and link the shader against to the compute program
	glAttachShader(computeProgramID, computeShader);
	glLinkProgram(computeProgramID);

	checkCompileErrors(computeShader, "COMPUTE");

	return computeProgramID;
}

void GLShader::checkMemorySharing()
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

	for (int i = 0; i < num_numeros; i++) {
		std::cout << "p" << i << ": " << initPos[i] << std::endl;
	}
	std::cout << std::endl;
}
#endif
