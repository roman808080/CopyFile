#pragma once

#include <memory>
#include <string>

#include "Constants.h"

class Messenger;

class App
{
public:
	App(const std::string& inputFileName, const std::string& outputFileName,
		const size_t blockSize = Constants::Kilobyte);
	
	void setRemote(const std::string& host, const std::string& port);
	void run();

private:
	const std::string inputFileName;
	const std::string outputFileName;
	const size_t blockSize;
	
	std::string host;
	std::string port;
};

