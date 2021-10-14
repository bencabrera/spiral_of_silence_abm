#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>

#include <boost/graph/topological_sort.hpp>

#include <cxxopts/cxxopts.hpp>

#include "model/model.h"
#include "model/modelProperties.hpp"
#include "simulation/simulateUntilStable.h"
#include "simulation/writeSimulationResultsToCsv.h"
#include "model/modelToGraphviz.h"

int main(int argc, char** argv)
{
    cxxopts::Options options("simulation", "Spiral of Silence simulation tool");
	options.add_options()
		("h,help", "Produce help message.")
		("i,gml-input", "Path to input model in GML format.", cxxopts::value<std::string>())
		("o,results-csv-output", "Path to the csv file to which results should be appended.", cxxopts::value<std::string>())
		("step-results-csv-output", "Path to the csv file to which intermediate results for every step are written.", cxxopts::value<std::string>())
		("wait-for-input-after-step", "If this flag is set the program waits for a key stroke after each step.")
		("step-to-graphviz", "If set then after every step model is drawn .dot graphviz file.", cxxopts::value<std::string>())
		("graphviz-show-confidence", "If set the graphviz model includes confidences.")
		("graphviz-show-expression-threshold", "If set the graphviz model includes expression threshold.")
		("graphviz-show-inner-confidence", "If set the graphviz model includes inner confidence.")
		("graphviz-show-bots", "If set the graphviz model includes marker for bots.")
		("graphviz-fixed-vertex-size", "Fixed vertex size, independent of label.",cxxopts::value<double>())
		("graphviz-vertex-fontsize", "Size of label in vertex.",cxxopts::value<double>())

		("epsilon", "Sensitivity parameter.", cxxopts::value<double>()->default_value("1e-4"))
	;
	auto args = options.parse(argc, argv);
	
	// display help if --help was specified
	if (args.count("help")) {
		std::cout << options.help() << std::endl;
		return 0;
	}

	// check if necessary parameters were provided
	if(!args.count("gml-input"))
	{
		std::cerr << "Please specify --gml-input." << std::endl; 
		return 1;
	}

	const std::string input_graph_path = args["gml-input"].as<std::string>();
	const double epsilon = args["epsilon"].as<double>();
	const bool wait_for_input = args.count("wait-for-input-after-step") > 0;

	GraphvizOptions graphviz_options;
	if(args.count("graphviz-show-confidence"))
		graphviz_options.show_confidence = true;
	if(args.count("graphviz-show-expression-threshold"))
		graphviz_options.show_expression_threshold = true;
	if(args.count("graphviz-show-inner-confidence"))
		graphviz_options.show_inner_confidence = true;
	if(args.count("graphviz-show-bots"))
		graphviz_options.show_bots = true;
	if(args.count("graphviz-fixed-vertex-size"))
		graphviz_options.fixed_vertex_size = args["graphviz-fixed-vertex-size"].as<double>();
	if(args.count("graphviz-vertex-fontsize"))
		graphviz_options.fontsize = args["graphviz-vertex-fontsize"].as<double>();

	// read in model
    std::ifstream input_graph_file(input_graph_path);
	if(!input_graph_file.is_open())
	{
		std::cerr << "Could not open the file specified in --gml-input. Path: " << input_graph_path << std::endl;
		return 1;
	}

	auto m = Model::read_from_gml(input_graph_file);	

	if(args.count("step-to-graphviz"))
	{
		std::ofstream graphviz_file(args["step-to-graphviz"].as<std::string>());
		draw_to_graphviz(graphviz_file, m, graphviz_options);			
	}

	std::ofstream step_results_csv_file;
	if(args.count("step-results-csv-output")) {
		step_results_csv_file.open(args["step-results-csv-output"].as<std::string>());
		step_results_csv_file << GLOBAL_DISTRIBUTION_HEADER;
		step_results_csv_file << CLUSTER_DISTRIBUTION_HEADER;
		step_results_csv_file << std::endl;
	}

	SimulationResults simulation_results = simulate_until_stable(m, epsilon, [&args, &wait_for_input, &graphviz_options, &step_results_csv_file](Model& m) {
		if(wait_for_input)
			std::cin.get();

		if(args.count("step-to-graphviz"))
		{
			std::ofstream graphviz_file(args["step-to-graphviz"].as<std::string>());
			draw_to_graphviz(graphviz_file, m, graphviz_options);			
		}

		if(args.count("step-results-csv-output")) {
			auto [n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red] = write_global_distribution(step_results_csv_file, m);
			write_cluster_distribution(step_results_csv_file, m, n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red);
			step_results_csv_file << std::endl;
		}
	});


	if(args.count("results-csv-output"))
	{
		const std::string csv_path = args["results-csv-output"].as<std::string>();

		std::ofstream csv_file(csv_path);
		write_csv_header(csv_file,m);
		write_simulation_results_to_csv(csv_file, simulation_results, m);
	}

	return 0;
}
