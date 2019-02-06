#pragma once

#include "../../graph/graph.h"

#include <random>

void generate_preferential_attachment_directed(Graph& g, const std::size_t n, const std::size_t m, std::mt19937& rand_gen)
{
	std::uniform_real_distribution<double> unif_0_1_dist;
	// --- initialization ---
	for(std::size_t i = 0; i < m; i++)
		boost::add_vertex(g);

	auto v_start = boost::add_vertex(g);
	for (auto v : boost::make_iterator_range(boost::vertices(g))) {
		if(v != v_start)
			boost::add_edge(v_start, v, g);	
	}

	std::vector<std::size_t> degrees;
	for (auto v : vertices(g))
		degrees.push_back(boost::degree(v,g));	

	// --- loop ---
	for(std::size_t i = m+1; i < n; i++)
	{
		// find all vertices to connect to
		std::set<Vertex> vertices_to_connect_to;	

		// add new vertex and connect to all of them
		auto v = boost::add_vertex(g);
		degrees.push_back(0);

		std::discrete_distribution<std::size_t> dist(degrees.begin(),degrees.end());

		while(vertices_to_connect_to.size() < m)
		{
			auto w = dist(rand_gen);
			if(vertices_to_connect_to.find(w) == vertices_to_connect_to.end() && v != w)
				vertices_to_connect_to.insert(w);
		}


		for (auto w : vertices_to_connect_to) {
			if(unif_0_1_dist(rand_gen) > 0.5)
				boost::add_edge(v,w,g);
			else
				boost::add_edge(w,v,g);
			degrees[v]++;
			degrees[w]++;
		}
	}
}
