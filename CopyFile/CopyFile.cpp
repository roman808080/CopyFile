// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "WriteFile.h"

int main()
{
    std::cout << "Hello World!\n";
    WriteFile file("file.txt");
    std::vector<char> data{'R', 'o', 'm', 'a', 'n'};

    file.write(data);
    file.write(data);
    file.write(data);

    return 0;
}
