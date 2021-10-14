#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>
#include <thread>
#include <future>
#include <mutex>
#include <sstream>
#include <chrono>
#include <cmath>

#include <boost/graph/topological_sort.hpp>

#include <cxxopts/cxxopts.hpp>

#include "model/model.h"
#include "model/modelProperties.hpp"
#include "simulation/performOneStep.h"
#include "model/modelToGraphviz.h"
#include "generation/generationParams.h"
#include "generation/generation.h"
#include "simulation/simulateUntilStable.h"
#include "simulation/writeSimulationResultsToCsv.h"
#include "cliHelpers.h"

void cli_progress_bar(std::size_t i_progress, std::size_t n, std::string additional_msg = "", std::size_t bar_width = 60)
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

void progress_update(std::size_t i_progress, std::size_t n)
{
	static auto start_time = std::chrono::steady_clock::now();

	std::string additional_msg = "";
	if(i_progress > 0)
	{
		auto now = std::chrono::steady_clock::now();
		auto diff = std::chrono::duration<double, std::milli>(now-start_time).count();

		auto to_go = (n - i_progress) * (static_cast<double>(diff) / i_progress);

		auto remainder = to_go;
		auto hours_to_go = std::floor(remainder / (1000*60*60));
		remainder = remainder - hours_to_go * (1000*60*60);
		auto mins_to_go =  std::floor(remainder / (1000*60));
		remainder = remainder - mins_to_go * (1000*60);
		auto seconds_to_go =  std::floor(remainder / 1000);

		std::stringstream ss;
		ss << "[";
		// if(hours_to_go > 0)
			ss << hours_to_go << "h ";
		// if(mins_to_go > 0)
			ss << std::setw(2) << mins_to_go << "m "; 
		// if(seconds_to_go > 0)
			ss << std::setw(2) << seconds_to_go << "s ";
		ss << "left]";

		additional_msg = ss.str();
	}

	cli_progress_bar(i_progress, n, additional_msg);
}

void run_simulation(
	std::size_t i_thread,
	std::ostream& csv_file, 
	std::vector<GenerationParams>& parameter_space, 
	const std::size_t n_runs, 
	std::mt19937 mt, 
	std::mutex& csv_mutex, 
	std::mutex& param_mutex, 
	std::size_t& i_param, 
	const std::size_t n_params, 
	const double epsilon = 1e-4
)
{
	while(parameter_space.size() > 0)
	{
		GenerationParams params;
		{
			std::lock_guard<std::mutex> lock(param_mutex);
			if(parameter_space.size() == 0)
				break;
			
			params = parameter_space.back();
			parameter_space.pop_back();
		}

		std::stringstream ss; // first write all lines for one param config to stringstream
		for(std::size_t i_run = 0; i_run < n_runs; i_run++)
		{
			// generate model
			auto m = generate(params,mt);

			// // print params
			// {
				// std::ofstream f(std::string("model_") + std::to_string(i_thread) + ".gml");
				// params.write_to_json(f);
				// m.write_to_gml(f);
			// }

			// simulate
			auto simulation_results = simulate_until_stable(m, epsilon);

			// write simulatio results to csv
			write_simulation_results_to_csv(ss, simulation_results, m);
			// {
				// std::ofstream f(std::string("model_") + std::to_string(i_thread) + ".gml");
				// f << "empty";
			// }
		}

		std::lock_guard<std::mutex> lock(csv_mutex);
		i_param++;
		progress_update(i_param, n_params);
		csv_file << ss.rdbuf(); // now write to file after obtaining global lock
	}
}


int main(int argc, char** argv)
{
    cxxopts::Options options("param_simulation", "Automated model generation and simulation of the SOS model for a whole parameter space");
	options.add_options()
		("h,help", "Produce help message.")
		("c,param-json-input", "Path to input model in GML format.", cxxopts::value<std::string>())
		("o,results-csv-output", "Path to csv file to which results should be appended.", cxxopts::value<std::string>())
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
	if(!args.count("param-json-input") || !args.count("results-csv-output"))
	{
		std::cerr << "Please specify --param-json-input && --results-csv-output." << std::endl; 
		return 1;
	}

	const std::string input_param_space_path = args["param-json-input"].as<std::string>();
	const double epsilon = args["epsilon"].as<double>();
	const std::size_t n_runs = args["number-of-runs"].as<std::size_t>();
	const std::string csv_path = args["results-csv-output"].as<std::string>();
	const std::size_t n_threads = args["number-of-threads"].as<std::size_t>();

	std::random_device rd;
	std::mt19937 mt(rd());

	// read in model
    std::ifstream input_param_space_file(input_param_space_path);
	auto parameter_space = build_param_space_from_json(input_param_space_file);

	std::cout << "Size of parameter space: " << parameter_space.size() << std::endl;

	std::ofstream csv_file(csv_path);
	auto m = generate(parameter_space[0],mt);
	write_csv_header(csv_file, m);

	std::vector<std::mt19937> mts;
	std::uniform_int_distribution<std::size_t> unif;
	for(std::size_t i_thread = 0; i_thread < n_threads; i_thread++)
		mts.emplace_back(unif(mt));

	std::vector<std::future<void>> futures;
	std::mutex csv_mutex, param_mutex;
	std::size_t i_param = 0, n_params = parameter_space.size();
	{
		std::lock_guard<std::mutex> lock(csv_mutex);
		progress_update(i_param, n_params);
	}

	auto start_time = get_timestamp();
	for(std::size_t i_thread = 0; i_thread < n_threads; i_thread++)
	{
		futures.push_back(std::async(std::launch::async, run_simulation, i_thread, std::ref(csv_file), std::ref(parameter_space), n_runs, mts[i_thread], std::ref(csv_mutex), std::ref(param_mutex), std::ref(i_param), n_params, epsilon));
	}

	for (auto& f : futures)
		f.get();

	auto end_time = get_timestamp();

	std::cout << std::endl << "Computation duration: " << duration_to_readable(Duration(end_time-start_time)) << std::endl;

	return 0;
}
