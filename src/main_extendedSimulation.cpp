#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>

#include <boost/graph/topological_sort.hpp>

#include "../libs/cxxopts/cxxopts.hpp"

#include "model/model.h"
#include "model/modelProperties.hpp"
#include "simulation/performOneStep.h"
#include "model/modelToGraphviz.h"
#include "generation/generationParams.h"
#include "generation/generation.h"
#include "simulation/simulateUntilStable.h"
#include "simulation/writeSimulationResultsToCsv.h"

int main(int argc, const char** argv)
{
    cxxopts::Options options("simulation", "Spiral of Silence simulation tool");
	options.add_options()
		("h,help", "Produce help message.")
		("c,parameter-space-json-input", "Path to input model in GML format.", cxxopts::value<std::string>())
		("o,csv-output", "Path to csv file to which results should be appended.", cxxopts::value<std::string>())
		("r,number-of-runs", "Number of runs.", cxxopts::value<std::size_t>()->default_value("1000"))
		("epsilon", "Sensitivity parameter.", cxxopts::value<double>()->default_value("1e-4"))
	;
	auto args = options.parse(argc, argv);
	
	// display help if --help was specified
	if (args.count("help")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	// check if necessary parameters were provided
	if(!args.count("parameter-space-json-input") || !args.count("csv-output"))
	{
		std::cerr << "Please specify --parameter-space-json-input && --csv-output." << std::endl; 
		return 1;
	}

	const std::string input_param_space_path = args["parameter-space-json-input"].as<std::string>();
	const double epsilon = args["epsilon"].as<double>();
	const std::size_t n_runs = args["number-of-runs"].as<std::size_t>();
	const std::string csv_path = args["csv-output"].as<std::string>();

	std::random_device rd;
	std::mt19937 mt(rd());

	// read in model
    std::ifstream input_param_space_file(input_param_space_path);
	auto parameter_space = build_param_space_from_json(input_param_space_file);

	for (auto params : parameter_space) 
	{
		std::cout << "params" << std::endl;
		for(std::size_t i_run = 0; i_run < n_runs; i_run++)
		{
		std::cout << i_run << std::endl;
			// generate model
			auto m = generate(params,mt);

			// simulate
			auto simulation_results = simulate_until_stable(m, epsilon);
			
			// write simulatio results to csv
			write_simulation_results_to_csv(csv_path, simulation_results, m);
		}
	}

	return 0;
}
