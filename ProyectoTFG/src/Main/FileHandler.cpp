#include <sstream>
#include <iostream>
#include "FileHandler.h"

std::fstream FileHandler::openInputFile(const char* inputFilePath)
{
	std::fstream file;

	try {
		file = _openFilePrivate(inputFilePath, std::fstream::in);
	}
	catch(std::ifstream::failure exception){
		_logMessageError("ERROR:: " + std::string(inputFilePath) + " ::FILE_NOT_SUCCESFULLY_OPEN");
	}

	return file;
}

std::fstream FileHandler::openOutputTruncatedFile(const char* filePath)
{
	std::fstream file;

	try {
		file = _openFilePrivate(filePath, std::fstream::out | std::fstream::trunc);
	}
	catch (std::fstream::failure exception) {
		_logMessageError("ERROR::" + std::string(filePath) + " ::FILE_NOT_SUCCESFULLY_OPEN");
	}

	return file;
}

std::fstream FileHandler::openOutputAppendedFile(const char * filePath)
{
	std::fstream file;

	try {
		file = _openFilePrivate(filePath, std::fstream::out | std::fstream::app);
	}
	catch (std::fstream::failure exception) {
		_logMessageError("ERROR::" + std::string(filePath) + " ::FILE_NOT_SUCCESFULLY_OPEN");
	}

	return file;
}

std::string FileHandler::readInputFile(const char* inputFilePath)
{
	std::fstream inputFile = openInputFile(inputFilePath);
	std::string fileContent = _readInputFilePrivate(inputFile);

	return fileContent;
}

std::string FileHandler::readInputFile(std::fstream & inputFile)
{
	std::string fileContent = _readInputFilePrivate(inputFile);

	return fileContent;
}

std::string FileHandler::readLineFromInputFile(std::fstream& inputFile)
{
	std::string fileLine;
	try {
		std::getline(inputFile, fileLine);
	}
	catch (std::fstream::failure exception) {
		_logMessageError("ERROR::LINE_NOT_SUCCESFULLY_READ::MAYBE_YOU_HAVE_REACHED_EOF");
	}

	return fileLine;
}

void FileHandler::writeRawStringToOutputFile(std::fstream& outputFile, const std::string & stringToWrite)
{	
	_writeStringToOutputFile(outputFile, stringToWrite);
}

void FileHandler::writeLineToOutputFile(std::fstream & outputFile, const std::string & lineToWrite)
{
	std::string formattedString = lineToWrite + "\n";
	_writeStringToOutputFile(outputFile, formattedString);
}

bool FileHandler::isFileOpen(const std::fstream & file)
{
	return file.is_open();
}

void FileHandler::closeFile(std::fstream& file)
{
	try {
		file.close();
	}
	catch (std::fstream::failure exception) {
		_logMessageError("ERROR::FILE_NOT_SUCCESFULLY_CLOSED::MAYBE_ALREADY_CLOSED");
	}
}

std::fstream FileHandler::_openFilePrivate(const char* filePath, const int& permissions)
{
	std::fstream file;
	// ensure fstream objects can throw exceptions
	file.exceptions(std::ifstream::failbit | std::ifstream::badbit |
		std::ofstream::failbit | std::ofstream::badbit);

	file.open(filePath, permissions);

	return file;
}

std::string FileHandler::_readInputFilePrivate(std::fstream& file)
{
	std::stringstream stream;
	try {
		//get the entire file
		stream << file.rdbuf();
	}
	catch(std::fstream::failure exception) {
		_logMessageError("ERROR::FILE_NOT_SUCCESFULLY_READ");
	}

	//convert stream into string
	return stream.str();
}

void FileHandler::_writeStringToOutputFile(std::fstream & outputFile, const std::string & stringToWrite)
{
	try{
		outputFile << stringToWrite; //dump the string to the output file
	}
	catch(std::fstream::failure exception) {
		_logMessageError("ERROR::FILE_NOT_SUCCESSFULY_WRITTEN::"
			"FILE_IS_NOT_OPEN_OR_DOES_NOT_HAVE_WRITE_PERMISSIONS");
	}
}

void FileHandler::_logMessageError(const std::string& message)
{
	std::cout << message << std::endl;
}
