#pragma once
#include <memory>
#include <string>

class OutputFile;
class Router;

class Writer
{
public:
	Writer(std::shared_ptr<OutputFile> outputFile,
		   std::shared_ptr<Router> router);
	~Writer();

	Writer(const Writer&) = delete;
    Writer& operator=(const Writer&) = delete;
    Writer(Writer&&) = delete;
    Writer& operator=(Writer&&) = delete;

	void write();

private:
	void tryWriteToFile();

private:
	std::shared_ptr<OutputFile> outputFile;
	std::shared_ptr<Router> router;
};

