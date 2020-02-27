#pragma once
#include <string>
#include <fstream>
#include <vector>

class FileHandler
{
public:
	/// open a file with read permissions
	static std::fstream openInputFile(const char* inputFilePath);
	/// open a file with write permissions, deleting all previous content
	static std::fstream openOutputTruncatedFile(const char* filePath);
	/// open a file with read permissions, setting the pointer at the eof, appending previous and new content
	static std::fstream openOutputAppendedFile(const char* filePath);

	/// opens the file from inputFilePath, reads the entire file, stores it in buffer and returns the fstream handler
	static std::fstream readInputFile(const char* inputFilePath, std::string& buffer);
	/// reads the entire given inputFile and returns the string
	static std::string readInputFile(std::fstream& inputFile);
	/// reads a line from the input file, moving the file pointer as expected. If the file reaches EOF, returns string = "EOF"
	static std::string readLineFromInputFile(std::fstream& inputFile);
	/// read s abinary file and returns the all the data
	static std::vector<char> readBinaryFile(const std::string& filename);

	/// writes a string without formatting it to an output file, moving the file pointer as expected
	static void writeRawStringToOutputFile(std::fstream& outputFile, const std::string& stringToWrite);
	/// writes a string with a "\n" at the end of the file
	static void writeLineToOutputFile(std::fstream& outputFile, const std::string& lineToWrite);

	static bool isFileOpen(const std::fstream& file);
	static void closeFile(std::fstream& file);
	/// delets the given file from disk
	static void deleteFile(const char* filePath);

private:
	/// private constructor because it's a static class
	FileHandler() {};
	static std::fstream _openFilePrivate(const char* filePath, const int& permissions);
	static std::string _readInputFilePrivate(std::fstream& inputFile);
	static void _writeStringToOutputFile(std::fstream& outputFile, const std::string& stringToWrite);
	static void _logMessageError(const std::string& message);
};

