#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>

#include <boost/graph/topological_sort.hpp>

#include "../libs/cxxopts/cxxopts.hpp"

#include "model/model.h"
#include "model/modelProperties.hpp"
#include "model/performOneStep.h"
#include "model/modelToGraphviz.h"

int main(int argc, const char** argv)
{
    cxxopts::Options options("simulation", "Spiral of Silence simulation tool");
	options.add_options()
		("h,help", "Produce help message.")
		("i,gml-input", "Path to input model in GML format.", cxxopts::value<std::string>())
		("o,stationary-config-csv-output", "Path to csv file to which results should be appended.", cxxopts::value<std::string>())
		("wait-for-input-after-step", "If this flag is set the program waits for a key stroke after each step.")
		("step-to-graphviz", "If set then after every step model is drawn .dot graphviz file.", cxxopts::value<std::string>())
		("graphviz-show-confidence", "If set the graphviz model includes confidences.")
		("graphviz-show-expression-threshold", "If set the graphviz model includes expression threshold.")
		("graphviz-show-inner-confidence", "If set the graphviz model includes inner confidence.")
		("graphviz-show-bots", "If set the graphviz model includes marker for bots.")

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

	StepResults res;
	do {
		if(wait_for_input)
			std::cin.get();

		res = perform_one_step(m,epsilon);

		if(args.count("step-to-graphviz"))
		{
			std::ofstream graphviz_file(args["step-to-graphviz"].as<std::string>());
			draw_to_graphviz(graphviz_file, m, graphviz_options);			
		}
	}
	while(res.n_humans_not_changed_confidence != 0);

	if(args.count("stationary-config-csv-output"))
	{
		const std::string csv_path = args["stationary-config-csv-output"].as<std::string>();

		// if file empty -> add header
		bool header = true;
		std::ifstream csv_file_test(csv_path);
		if(csv_file_test.good())
			header = false;
		csv_file_test.close();

		std::ofstream csv_file(csv_path, std::ios_base::app);
		if(header)
		{
			for (auto [k,v] : m.global_properties) {
				csv_file << k << ",";
			}

			csv_file << "n_individuals, n_users, n_bots, alpha, n_silenced" << std::endl;
		}

		for (auto [k,v] : m.global_properties) {
			csv_file << "\"" << v << "\"" << ",";
		}

		// n_individuals
		csv_file << boost::num_vertices(m.graph()) << ",";
		// n_users
		csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return !m.is_bot(v); }) << ",";
		// n_bots
		csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.is_bot(v); }) << ",";
		// alpha
		csv_file << m.alpha() << ",";
		// n_silenced
		csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.is_silenced(v); });

		csv_file << std::endl;
// [1] "m"                                         
// "n_users",
// "n_bots",
// "alpha",
// "gamma",
// "bot_attachment_method",
// "percent_positive_valence",
// "confidence_initialization_method",
// "expression_threshold_initialization_method",
// "bot_influence",
// "link_type_is_directed",
// "percent_green",
// "percent_red",
// "percent_gray",
// "red_wins",
// "green_wins",
// "ticks_until_stable",
// "percent_0_color_flips",
// "percent_1_color_flips",
// "percent_2_color_flips",
// "percent_more_color_flips",
// "percent_accuracy_below_50",
// "percent_accuracy_50_to_60",
// "percent_accuracy_60_to_70",
// "percent_accuracy_70_to_80",
// "percent_accuracy_80_to_90",
// "percent_accuracy_above_90",
// "color_most_central_before",
// "color_most_central_after",
// "opinion_most_central"
	}

	return 0;
}
