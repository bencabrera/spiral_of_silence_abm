#include <fstream>
#include <iostream>
#include <list>
#include <chrono>
#include <iomanip>

#include <boost/program_options.hpp>
#include <boost/graph/graphml.hpp>

#include "erdoesRenyi.hpp"

using UndirectedBoostGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::no_property, boost::vecS>;
using DirectedBoostGraph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, boost::no_property, boost::no_property, boost::vecS>;


void write_to_gml_helper(std::ostream& ostr, UndirectedBoostGraph& g)
{
	ostr << "\t directed 0" << std::endl;
}

void write_to_gml_helper(std::ostream& ostr, DirectedBoostGraph& g)
{
	ostr << "\t directed 1" << std::endl;
}

template<typename TBoostGraph>
void write_to_gml(std::ostream& ostr, TBoostGraph& g)
{
	ostr << "graph [" << std::endl;
	write_to_gml_helper(ostr, g);

	unsigned vIdx = 0;
	for(auto v : boost::make_iterator_range(boost::vertices(g)))
	{
		ostr << "\t node [" << std::endl;
		ostr << "\t\t id " << vIdx << std::endl;
		ostr << "\t ]" << std::endl;
		vIdx++;
	}

	for(auto e : boost::make_iterator_range(boost::edges(g)))
	{
		ostr << "\t edge [" << std::endl;
		ostr << "\t\t source " << boost::source(e, g) << std::endl;
		ostr << "\t\t target " << boost::target(e, g) << std::endl;
		ostr << "\t ]" << std::endl;
	}

	ostr << "]" << std::endl;
}

namespace po = boost::program_options;

int main(int argc, char const *argv[])
{
	po::options_description desc("Allowed options");
	desc.add_options()
		("help,h", "Produce help message.")
		("gml-output,o", po::value<std::string>(), "Specifies the output GraphML file path.")
		("n,n", po::value<std::size_t>(), "Specifies the number of vertices in the generated graph.")
		("p,p", po::value<double>(), "Specifies the p parameter.")
		("directed,d", po::bool_switch(), "Is graph directed")
		("undirected,u", po::bool_switch(), "Is graph undirected")
		("seed,s", po::value<std::size_t>(), "Specifies the seed to be used for the pseudo random number generator. If not specified then std::random_device is used.")
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
	if(!vm.count("gml-output"))
	{
		std::cerr << "Please specify the path to the output file in --gml-output." << std::endl; 
		return 1;
	}

	if(!vm.count("p"))
	{
		std::cerr << "Please specify p as--p." << std::endl; 
		return 1;
	}

	if(!vm.count("n"))
	{
		std::cerr << "Please specify the numer of vertices by using --n." << std::endl; 
		return 1;
	}

	if(
		(!vm["directed"].as<bool>() && !vm["undirected"].as<bool>()) 
		|| (vm["directed"].as<bool>() && vm["undirected"].as<bool>())
	)
	{
		std::cerr << "Please set exactly one parameter of --directed or --undirected." << std::endl; 
		return 1;
	}

	const double p = vm["p"].as<double>();
	const std::size_t n = vm["n"].as<std::size_t>();
	const std::string outputGraphPath = vm["gml-output"].as<std::string>();

	std::mt19937 rand_gen;
	if(vm.count("seed"))
		rand_gen.seed(vm["seed"].as<std::size_t>());
	else
	{
		std::random_device rd;
		rand_gen.seed(rd());
	}


	if(vm["undirected"].as<bool>())
	{
		UndirectedBoostGraph g;
		GraphGeneration::generate_erdoes_renyi_undirected(g, n, p, rand_gen);

		std::cout << "Number of edges: " << boost::num_edges(g) << std::endl;

		std::ofstream outputGraphFile(outputGraphPath);
		write_to_gml(outputGraphFile, g);
	}

	if(vm["directed"].as<bool>())
	{
		DirectedBoostGraph g;
		GraphGeneration::generate_erdoes_renyi_directed(g, n, p, rand_gen);

		std::cout << "Number of edges: " << boost::num_edges(g) << std::endl;

		std::ofstream outputGraphFile(outputGraphPath);
		write_to_gml(outputGraphFile, g);
	}

	return 0;
}
