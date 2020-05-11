#pragma once
#include <string>

class ShaderInclude
{
private:
	static std::string tempPath;
	static std::fstream tempFile;
	static unsigned int index;
public:
	static std::string InterpretShader(const char* shaderPath, bool recursive = false, std::string includeReservedWord = "#include");
};

