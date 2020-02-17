#include "ShaderInclude.h"
#include <iostream>
#include "FileHandler.h"

std::string ShaderInclude::tempPath = "..\\..\\Shaders\\temp.c";
std::fstream ShaderInclude::tempFile;

std::string ShaderInclude::InterpretShader(const char * shaderPath, std::string includeReservedWord)
{
	includeReservedWord += ' ';

	std::fstream shaderFile = FileHandler::openInputFile(shaderPath);
	std::string currentLine = FileHandler::readLineFromInputFile(shaderFile);
	std::string shaderCode;

	while (currentLine != "EOF")
	{
		if (currentLine.find(includeReservedWord) != currentLine.npos) // find the include identifier
		{
			currentLine.erase(0, includeReservedWord.size()); // erase the identifier

			std::string includeString;
			//std::fstream includeFile = FileHandler::readInputFile(currentLine.c_str(), includeString); // reads 
			std::string includeContent = InterpretShader(currentLine.c_str());
			shaderCode += includeContent; // adds the included file to shader file
			//FileHandler::closeFile(includeFile); // close

			// reads a new line and continues
			currentLine = FileHandler::readLineFromInputFile(shaderFile);
			continue;
		}

		shaderCode += currentLine + '\n';
		currentLine = FileHandler::readLineFromInputFile(shaderFile);
	}

#if _DEBUG
	tempFile = FileHandler::openOutputTruncatedFile(tempPath.c_str());
	FileHandler::writeRawStringToOutputFile(tempFile, shaderCode);
	FileHandler::closeFile(tempFile);
#endif

	//shaderCode += '\0'; // necesario?
	FileHandler::closeFile(shaderFile);

	return shaderCode;
}
