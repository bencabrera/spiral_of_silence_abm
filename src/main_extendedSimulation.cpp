#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>
#include <thread>
#include <future>
#include <mutex>
#include <sstream>

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

void cli_progress_bar(std::size_t i_progress, std::size_t n, std::string additional_msg = "", std::size_t bar_width = 80)
{
	double ratio = static_cast<double>(i_progress) / n;
	double percentage = 100 * ratio;

	std::cout << "[";
	std::size_t pos = bar_width * ratio;
	for (std::size_t i = 0; i < bar_width; ++i) {
		if (i < pos) std::cout << "=";
		else if (i == pos) std::cout << ">";
		else std::cout << " ";
	}
	std::cout << "] ";

	std::stringstream ss;

	ss << std::setw(5) << std::fixed << std::setprecision(1) << percentage << " %";
	std::cout << ss.str();
	std::cout << " " << additional_msg;
	std::cout << "\r"; // set carriage return
	std::cout.flush();
}

void run_simulation(std::ostream& csv_file, const std::vector<GenerationParams>& parameter_space, const std::size_t n_runs, std::mt19937 mt, std::mutex& mutex, std::size_t& i_param, const std::size_t n_params, const double epsilon = 1e-4)
{
	for (auto params : parameter_space) 
	{
		std::stringstream ss; // first write all lines for one param config to stringstream
		for(std::size_t i_run = 0; i_run < n_runs; i_run++)
		{
			// generate model
			auto m = generate(params,mt);

			// simulate
			auto simulation_results = simulate_until_stable(m, epsilon);

			// write simulatio results to csv
			write_simulation_results_to_csv(ss, simulation_results, m);
		}

		std::lock_guard<std::mutex> lock(mutex);
		i_param++;
		cli_progress_bar(i_param, n_params);
		csv_file << ss.rdbuf(); // now write to file after obtaining global lock
	}
}


int main(int argc, const char** argv)
{
    cxxopts::Options options("simulation", "Spiral of Silence simulation tool");
	options.add_options()
		("h,help", "Produce help message.")
		("c,parameter-space-json-input", "Path to input model in GML format.", cxxopts::value<std::string>())
		("o,csv-output", "Path to csv file to which results should be appended.", cxxopts::value<std::string>())
		("r,number-of-runs", "Number of runs.", cxxopts::value<std::size_t>()->default_value("1000"))
		("epsilon", "Sensitivity parameter.", cxxopts::value<double>()->default_value("1e-4"))
		("number-of-threads", "How many threads should be used.", cxxopts::value<std::size_t>()->default_value("1"))
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
	const std::size_t n_threads = args["number-of-threads"].as<std::size_t>();

	std::random_device rd;
	std::mt19937 mt(rd());

	// read in model
    std::ifstream input_param_space_file(input_param_space_path);
	auto parameter_space = build_param_space_from_json(input_param_space_file);

	// write header if file did not exist
	std::ifstream csv_file_test(csv_path);
	std::ofstream csv_file(csv_path);
	if(!csv_file_test.good())
	{
		auto m = generate(parameter_space[0],mt);
		write_csv_header(csv_file, m);
	}
	csv_file_test.close();

	// split parameter space into n_threads equal parts
	std::vector<std::vector<GenerationParams>> thread_params(n_threads);
	for(std::size_t i = 0; i < parameter_space.size(); i++)
		thread_params[i%n_threads].push_back(parameter_space[i]);

	std::vector<std::mt19937> mts;
	std::uniform_int_distribution<std::size_t> unif;
	for(std::size_t i_thread = 0; i_thread < n_threads; i_thread++)
		mts.emplace_back(unif(mt));

	std::vector<std::future<void>> futures;
	std::mutex mutex;
	std::size_t i_param = 0, n_params = parameter_space.size();
	{
		std::lock_guard<std::mutex> lock(mutex);
		cli_progress_bar(i_param, n_params);
	}
	for(std::size_t i_thread = 0; i_thread < n_threads; i_thread++)
	{
		futures.push_back(std::async(std::launch::async, run_simulation, std::ref(csv_file), std::ref(thread_params[i_thread]), n_runs, mts[i_thread], std::ref(mutex), std::ref(i_param), n_params, epsilon));
	}

	for (auto& f : futures)
		f.get();

	return 0;
}
