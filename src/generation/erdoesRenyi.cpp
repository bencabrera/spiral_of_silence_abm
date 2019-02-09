#include "erdoesRenyi.h"

#include <boost/graph/random.hpp>

void generate_erdoes_renyi_undirected(Graph& g, const std::size_t n, const double p, std::mt19937& rand_gen)
{
	for(std::size_t i = 0; i < n; i++)
		boost::add_vertex(g);

	std::uniform_real_distribution<double> unif_0_1_dist;
	for(std::size_t i_from = 0; i_from < n; i_from++)
		for(std::size_t i_to = i_from+1; i_to < n; i_to++)
		{
			if(unif_0_1_dist(rand_gen) < p)
				boost::add_edge(i_from, i_to, g);
		}
}

void generate_erdoes_renyi_directed(Graph& g, const std::size_t n, const double p, std::mt19937& rand_gen)
{
	for(std::size_t i = 0; i < n; i++)
		boost::add_vertex(g);

	std::uniform_real_distribution<double> unif_0_1_dist;
	for(std::size_t i_from = 0; i_from < n; i_from++)
		for(std::size_t i_to = 0; i_to < n; i_to++)
		{
			if(i_from != i_to && unif_0_1_dist(rand_gen) < p)
				boost::add_edge(i_from, i_to, g);
		}
}
