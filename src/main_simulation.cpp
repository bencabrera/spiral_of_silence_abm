#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>

#include <boost/graph/topological_sort.hpp>

#include "../libs/cxxopts/cxxopts.hpp"

#include "model/model.h"
#include "model/modelProperties.hpp"

int main(int argc, const char** argv)
{
    cxxopts::Options options("simulation", "Spiral of Silence simulation tool");
	options.add_options()
		("h,help", "Produce help message.")
		("i,gml-input", "Path to input model in GML format.", cxxopts::value<std::string>())
		("o,append-csv-output", "Path to csv file to which results should be appended.", cxxopts::value<std::string>())
		("epsilon", "Sensitivity parameter.", cxxopts::value<double>()->default_value("1e-4"))
	;
	auto args = options.parse(argc, argv);
	
	// display help if --help was specified
	if (args.count("help")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	// check if necessary parameters were provided
	if(!args.count("gml-input") || !args.count("append-csv-output"))
	{
		std::cerr << "Please specify --gml-input and --append-csv-output." << std::endl; 
		return 1;
	}

	const std::string input_graph_path = args["gml-input"].as<std::string>();
	const std::string output_csv_path = args["append-csv-output"].as<std::string>();

	// read in model
    std::ifstream input_graph_file(input_graph_path);
	if(!input_graph_file.is_open())
	{
		std::cerr << "Could not open the file specified in --gml-input. Path: " << input_graph_path << std::endl;
		return 1;
	}

	auto m = Model::read_from_gml(input_graph_file);	

	std::cout << count_vertices_with_predicate(m, [](auto v, auto m) { return m.is_silenced(v); }) << " silenced. " << std::endl; 

	return 0;
}
