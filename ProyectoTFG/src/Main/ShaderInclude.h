#pragma once
#include <string>

class ShaderInclude
{
public:
	static std::string InterpretShader(const char* shaderPath, std::string includeReservedWord = "#include");
};

