#pragma once

#include <memory>
#include <string>

#include "Constants.h"

class Messenger;

class App
{
public:
	App(const size_t blockSize = Constants::Kilobyte);
	
	void run();

	void setInputFile(const std::string& inputFileName);
	void setOutputFile(const std::string& outputFileName);

	void setMethod(const std::string& method);

private:
	void copyFileDefaultMethod();
	void copyFileSharedMemoryMethod();


private:
	std::string inputFileName;
	std::string outputFileName;
	std::string method;

	const size_t blockSize;
};

