#include "ShaderInclude.h"
#include <iostream>
#include "FileHandler.h"

std::string ShaderInclude::tempPath = "..\\..\\Shaders\\temp\\";
std::fstream ShaderInclude::tempFile;
unsigned int ShaderInclude::index = 0;

std::string ShaderInclude::InterpretShader(const char * shaderPath, bool recursive, std::string includeReservedWord)
{
	includeReservedWord += ' ';

	std::fstream shaderFile = FileHandler::openInputFile(shaderPath);
	std::string currentLine = FileHandler::readLineFromInputFile(shaderFile);
	std::string shaderCode;

#if defined(VULKAN_RELEASE) || defined(VULKAN_DEBUG)
	if(!recursive) shaderCode = "#define VULKAN\n";
#endif

	while (currentLine != "EOF")
	{
		if (currentLine.find(includeReservedWord) != currentLine.npos) // find the include identifier
		{
			currentLine.erase(0, includeReservedWord.size()); // erase the identifier

			std::string includeString;
			//std::fstream includeFile = FileHandler::readInputFile(currentLine.c_str(), includeString); // reads 
			std::string includeContent = InterpretShader(currentLine.c_str(), true);
			shaderCode += includeContent; // adds the included file to shader file
			//FileHandler::closeFile(includeFile); // close

			// reads a new line and continues
			currentLine = FileHandler::readLineFromInputFile(shaderFile);
			continue;
		}

		shaderCode += currentLine + '\n';
		currentLine = FileHandler::readLineFromInputFile(shaderFile);
	}

#if defined(GL_DEBUG) || defined(VULKAN_DEBUG)
	std::string aux = shaderPath;
	auto size = aux.find_last_of('\\');
	aux.erase(0, size);
	tempFile = FileHandler::openOutputTruncatedFile((tempPath + (aux) + "_temp.c").c_str());
	FileHandler::writeRawStringToOutputFile(tempFile, shaderCode);
	FileHandler::closeFile(tempFile);
#endif

	//shaderCode += '\0'; // necesario?
	FileHandler::closeFile(shaderFile);
	index++;

	return shaderCode;
}
