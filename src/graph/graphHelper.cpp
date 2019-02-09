#include "graphHelper.h"

#include <iostream>

void set_increasing_indices(Graph& g)
{
	std::size_t i_edge = 0;
	for (auto e : edges(g)) 
		boost::put(boost::edge_index, g, e, i_edge++);
}

void add_inverse_edges(Graph& g)
{
	std::vector<std::pair<Vertex,Vertex>> vertex_pairs;
	for (auto e : edges(g)) 
		vertex_pairs.push_back({ boost::source(e,g), boost::target(e,g) });	
	
	for (auto [v1,v2] : vertex_pairs) {
		if(!boost::edge(v2,v1,g).second)
			boost::add_edge(v2,v1,g);
	}
}

std::vector<std::size_t> compute_degree_distribution(Graph& g)
{
	std::vector<std::size_t> dist(boost::num_vertices(g),0);

	for (auto v : vertices(g)) 
		dist[boost::degree(v,g)]++;
	
	return dist;
}
