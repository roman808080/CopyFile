// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <boost/program_options.hpp>

#include "App.h"
#include "Constants.h"

using namespace boost::program_options;
namespace po = boost::program_options;

int main(int argc, char* argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("source,s", po::value<std::string>(), "set source file")
        ("destination,d", po::value<std::string>(), "set destination file")
        ;

	po::variables_map variablesMap;

    // parse regular options
    po::store(po::parse_command_line(argc, argv, desc), variablesMap);
    po::notify(variablesMap);

    if (variablesMap.count("help") || argc == 1) {
        std::cout << desc << std::endl;
        return 0;
    }

	std::string inputFileName {};
    if (variablesMap.count("source")) {
		inputFileName = variablesMap["source"].as<std::string>();
    }

	std::string outputFileName {};
    if (variablesMap.count("destination")) {
		outputFileName = variablesMap["destination"].as<std::string>();
    }

	App app(inputFileName, outputFileName, Constants::Megabyte);
	app.run();

	return 0;
}
