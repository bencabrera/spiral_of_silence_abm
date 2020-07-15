#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>

#include <boost/graph/topological_sort.hpp>

#include <cxxopts/cxxopts.hpp>

#include "model/model.h"
#include "generation/generationParams.h"
#include "generation/generation.h"

int main(int argc, char** argv)
{
    cxxopts::Options options("generation", "Generates model instances (as .gml files) based on the provided model parameters (as .json files)");
	options.add_options()
		("h,help", "Produce help message.")
		("o,gml-output", "File path where the generated model should be stored (as GML)", cxxopts::value<std::string>())
		("c,param-json-input", "File path of model parameters (as JSON).", cxxopts::value<std::string>())

		("generate-example-config", "Generates an exemplary config file containing some default values.", cxxopts::value<std::string>())
		("print-n-edges", "Print number of edges in generated graph.")
	;
	auto args = options.parse(argc, argv);
	
	// display help if --help was specified
	if (args.count("help")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	if (args.count("generate-example-config")) {
		GenerationParams default_params;
		std::ofstream file(args["generate-example-config"].as<std::string>());
		file << std::setw(4);
		default_params.write_to_json(file);
		return 0;
	}

	// check if necessary parameters were provided
	if(!args.count("param-json-input") || !args.count("gml-output"))
	{
		std::cerr << "Please specify --param-json-input and --gml-output." << std::endl; 
		return 1;
	}

	std::ifstream config_file(args["param-json-input"].as<std::string>());
	GenerationParams params = GenerationParams::parse_from_json(config_file);

	std::random_device rd;
	std::mt19937 mt(rd());

	auto m = generate(params,mt);

	std::ofstream gml_output_file(args["gml-output"].as<std::string>());
	m.write_to_gml(gml_output_file);

	if(args.count("print-n-edges")) {
		std::cout << boost::num_edges(m.graph()) << std::endl;
	}

	return 0;
}
