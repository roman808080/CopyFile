#pragma once

#include <memory>
#include <vector>
#include <atomic>
#include <string>

class InputFile;
class Router;

class Reader
{
public:
	Reader(std::shared_ptr<InputFile> inputFile,
           std::shared_ptr<Router> router);

	~Reader();

	Reader(const Reader&) = delete;
    Reader& operator=(const Reader&) = delete;
    Reader(Reader&&) = delete;
    Reader& operator=(Reader&&) = delete;

	void read();

private:
	void tryReadFromFile();

private:
	std::shared_ptr<InputFile> inputFile;
	std::shared_ptr<Router> router;
};

