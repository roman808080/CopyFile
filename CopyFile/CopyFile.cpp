// CopyFile.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include <iostream>
#include <boost/program_options.hpp>

#include "App.h"
#include "Constants.h"

using namespace boost::program_options;
namespace po = boost::program_options;

int main(int argc, char *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
					("help,h", "produce help message")
					("method,m", po::value<std::string>()->default_value("default"), "method (default, shared)")
					("source,s", po::value<std::string>(), "set source file")
					("destination,d", po::value<std::string>(), "set destination file");

	po::variables_map variablesMap;

	// parse regular options
	po::store(po::parse_command_line(argc, argv, desc), variablesMap);
	po::notify(variablesMap);

	if (variablesMap.count("help") || argc == 1)
	{
		std::cout << desc << std::endl;
		return 0;
	}

	App app;

	if (variablesMap.count("source"))
	{
		const std::string inputFileName = variablesMap["source"].as<std::string>();
		app.setInputFile(inputFileName);
	}

	if (variablesMap.count("destination"))
	{
		const std::string outputFileName = variablesMap["destination"].as<std::string>();
		app.setOutputFile(outputFileName);
	}

	const std::string method = variablesMap["method"].as<std::string>();
	app.setMethod(method);

	app.run();

	return 0;
}
