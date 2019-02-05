#include <fstream>
#include <iostream>
#include <iomanip>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>
#include <boost/graph/graphml.hpp>
#include <boost/algorithm/string/trim.hpp>

#include "configurationModel.hpp"

using ConfigurationEdgeProperty = boost::property< boost::edge_index_t, std::size_t >;
using ConfigurationUGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, ConfigurationEdgeProperty, boost::vecS>;
using ConfigurationDGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, ConfigurationEdgeProperty, boost::vecS>;


std::map<std::size_t,std::size_t> read_degree_distribution(std::istream& istr)
{
	std::map<std::size_t,std::size_t> rtn;
	std::string line;
	while(std::getline(istr, line))
	{
		boost::trim(line);
		if(line.length() > 0)
		{
			std::stringstream ss(line);
			std::size_t deg, count;
			ss >> deg >> count;
			rtn.insert({ deg, count });
		}
	}

	return rtn;
}

void print_distributions(const std::vector<std::pair<std::string, std::map<std::size_t, std::size_t>>>& dists)
{
	// output of original and new degree distribution
	std::size_t max_deg = 0;
	for(auto& dist : dists)
	{
		for (auto i : dist.second) 
			max_deg = std::max(max_deg, i.first);
	}

	std::cout << std::left << std::setw(25) << "Degree" << " ";
	for (std::size_t i = 0; i <= max_deg; i++) 
		std::cout << std::setw(5) << i << " ";
	std::cout << std::endl;
	std::cout << "--------------------------------------------------------------" << std::endl;

	for(auto& dist : dists)
	{
		std::cout << std::left << std::setw(25) << dist.first << " ";
		for (std::size_t i = 0; i <= max_deg; i++) 
			if(dist.second.count(i))
				std::cout << std::setw(5) << dist.second.at(i) << " ";
			else
				std::cout << std::setw(5) << 0 << " ";
		std::cout << std::endl;
	}
}

namespace po = boost::program_options;

int main(int argc, char const *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help", "Produce help message.")
		("graphml-output", po::value<std::string>(), "Specifies the output GraphML file path.")
		("degree-distribution-input", po::value<std::string>(), "File containing the expected degree distribution.")
		("in-degree-distribution-input", po::value<std::string>(), "File containing the expected in degree distribution.")
		("out-degree-distribution-input", po::value<std::string>(), "File containing the expected out degree distribution.")
		("seed", po::value<std::size_t>(), "Specifies the seed to be used for the pseudo random number generator. If not specified then std::random_device is used.")
		;
	po::variables_map vm;
	po::store(po::parse_command_line(argc, argv, desc), vm);
	po::notify(vm);

	// display help if --help was specified
	if (vm.count("help")) {
		std::cout << desc << std::endl;
		return 0;
	}

	// check if necessary parameters were provided
	if(!vm.count("graphml-output"))
	{
		std::cerr << "Please specify the path to the output file in --graphml-output." << std::endl; 
		return 1;
	}

	if(!vm.count("degree-distribution-input") && !(vm.count("in-degree-distribution-input") && vm.count("out-degree-distribution-input")))
	{
		std::cerr << "Please specify degree distributions." << std::endl; 
		return 1;
	}

	const std::string outputGraphPath = vm["graphml-output"].as<std::string>();
	bool is_directed = vm.count("in-degree-distribution-input") > 0;

	std::mt19937 rand_gen;
	if(vm.count("seed"))
		rand_gen.seed(vm["seed"].as<std::size_t>());
	else
	{
		std::random_device rd;
		rand_gen.seed(rd());
	}

	if(!is_directed)
	{
		std::cout << "Generating UNDIRECTED graph" << std::endl;
		std::ifstream degree_dist_file(vm["degree-distribution-input"].as<std::string>());
		auto degree_dist = read_degree_distribution(degree_dist_file);

		ConfigurationUGraph g;
		GraphGeneration::generate_confmodel_graph_undirected(g, degree_dist, rand_gen);

		std::cout << "Distributions: " << std::endl;
		std::map<std::size_t, std::size_t> actual_dist;
		for(auto v : boost::make_iterator_range(boost::vertices(g)))
		{
			auto deg = boost::degree(v,g);
			if(actual_dist.count(deg))
				actual_dist[deg]++;
			else
				actual_dist[deg] = 1;
		}

		std::vector<std::pair<std::string, std::map<std::size_t, std::size_t>>> dists = {
			{ "Original Degree", degree_dist },
			{ "Actual Degree", actual_dist }
		};

		print_distributions(dists);


		std::ofstream outputGraphFile(outputGraphPath);
		boost::dynamic_properties dp;
		boost::write_graphml(outputGraphFile, g, dp);
	}
	else
	{
		std::cout << "Generating DIRECTED graph" << std::endl;
		std::ifstream out_degree_dist_file(vm["out-degree-distribution-input"].as<std::string>());
		std::ifstream in_degree_dist_file(vm["in-degree-distribution-input"].as<std::string>());

		auto out_degree_dist = read_degree_distribution(out_degree_dist_file);
		auto in_degree_dist = read_degree_distribution(in_degree_dist_file);

		ConfigurationDGraph g;
		GraphGeneration::generate_confmodel_graph_directed_independent(g, out_degree_dist, in_degree_dist, rand_gen);

		std::cout << "Distributions: " << std::endl;
		std::map<std::size_t, std::size_t> actual_dist_out, actual_dist_in;
		for(auto v : boost::make_iterator_range(boost::vertices(g)))
		{
			auto out_deg = boost::out_degree(v,g);
			if(actual_dist_out.count(out_deg))
				actual_dist_out[out_deg]++;
			else
				actual_dist_out[out_deg] = 1;

			auto in_deg = boost::in_degree(v,g);
			if(actual_dist_in.count(in_deg))
				actual_dist_in[in_deg]++;
			else
				actual_dist_in[in_deg] = 1;
		}

		std::vector<std::pair<std::string, std::map<std::size_t, std::size_t>>> dists = {
			{ "Original Out-Degree", out_degree_dist },
			{ "Actual Out-Degree", actual_dist_out },
			{ "Original In-Degree", in_degree_dist },
			{ "Actual In-Degree", actual_dist_in }
		};

		print_distributions(dists);
	
		std::ofstream outputGraphFile(outputGraphPath);
		boost::dynamic_properties dp;
		boost::write_graphml(outputGraphFile, g, dp);
	}


	return 0;
}
