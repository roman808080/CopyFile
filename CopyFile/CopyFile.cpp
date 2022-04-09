// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "App.h"
#include "Constants.h"

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 3)
		{
			std::cout << "Not enough parameters. Should be: CopyFile <source> <destination>" << std::endl;
			return 1;
		}

		const std::string inputFileName = argv[1];
		const std::string outputFileName = argv[2];

		App app(inputFileName, outputFileName, Constants::Megabyte);
		app.run();
	}
	catch (const std::exception& exc)
	{
		std::cout << "An exception has happened: " << exc.what() << std::endl;
		return 1;
	}

    return 0;
}
