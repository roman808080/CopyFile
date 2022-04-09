// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "App.h"
#include "Constants.h"

int main()
{
	try
	{
		App app("CopyFile.cpp", "file.txt", Constants::Megabyte);
		app.run();
	}
	catch (const std::exception& exc)
	{
		std::cout << "An exception has happened: " << exc.what() << std::endl;
		return 1;
	}

    return 0;
}
