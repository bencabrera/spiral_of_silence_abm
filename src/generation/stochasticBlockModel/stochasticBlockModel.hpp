#pragma once

#include "../../graph/graph.h"

#include <random>

void generate_stochastic_block_model_undirected(Graph& g, const std::size_t n, const std::vector<double> priors, std::vector<std::vector<double>> community_connectivity, std::mt19937& mt)
{
	// check if dimensions match

	std::vector<std::size_t> vertex_communities;
	std::discrete_distribution<std::size_t> community_dist(priors.begin(), priors.end());

	// add vertices and determine community for each one
	for(std::size_t i = 0; i < n; i++)
	{
		boost::add_vertex(g);
		vertex_communities.push_back(community_dist(mt));
	}

	std::uniform_real_distribution<double> unif_0_1_dist;
	for(std::size_t i_from = 0; i_from < n; i_from++)
		for(std::size_t i_to = i_from+1; i_to < n; i_to++)
		{
			if(unif_0_1_dist(mt) < community_connectivity[vertex_communities[i_from]][vertex_communities[i_to]])
				boost::add_edge(i_from, i_to, g);
		}
}

void generate_stochastic_block_model_directed(Graph& g, const std::size_t n, const std::vector<double> priors, std::vector<std::vector<double>> community_connectivity, std::mt19937& mt)
{
	// check if dimensions match

	std::vector<std::size_t> vertex_communities;
	std::discrete_distribution<std::size_t> community_dist(priors.begin(), priors.end());

	// add vertices and determine community for each one
	for(std::size_t i = 0; i < n; i++)
	{
		boost::add_vertex(g);
		vertex_communities.push_back(community_dist(mt));
	}

	std::uniform_real_distribution<double> unif_0_1_dist;
	for(std::size_t i_from = 0; i_from < n; i_from++)
		for(std::size_t i_to = 0; i_to < n; i_to++)
		{
			if(i_from != i_to && unif_0_1_dist(mt) < community_connectivity[vertex_communities[i_from]][vertex_communities[i_to]])
				boost::add_edge(i_from, i_to, g);
		}
}
