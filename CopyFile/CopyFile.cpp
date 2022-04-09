// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include "App.h"
#include "Constants.h"

int main()
{
    // input file CopyFile.cpp
    // output file file.txt
    // Constants::Kilobyte
    App app("CopyFile.cpp", "file.txt", Constants::Kilobyte);
    app.run();

    return 0;
}
