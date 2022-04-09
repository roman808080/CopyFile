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
	
	void run();

private:
	void tryRun();

private:
	const std::string inputFileName;
	const std::string outputFileName;
	const size_t blockSize;

	std::shared_ptr<Messenger> messenger;
};

