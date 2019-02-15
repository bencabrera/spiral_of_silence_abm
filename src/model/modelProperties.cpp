#include "modelProperties.hpp"

Vertex most_central_vertex(const Model& m)
{
	Vertex max_v = 0;
	std::size_t max_degree = boost::degree(max_v,m.graph());
	for (auto v : vertices(m.graph())) {
		if(boost::degree(v,m.graph()) > max_degree)	{
			max_degree = boost::degree(v,m.graph());
			max_v = v;
		}
	}	

	return max_v;
}

// how often does each color appear in subset of vertices with highest degree
std::pair<std::size_t, std::size_t> count_colors_in_top_degrees(const Model& m, double ratio_size_subset)
{
	std::vector<Vertex> vertices_vec(boost::num_vertices(m.graph()));
	std::iota(vertices_vec.begin(), vertices_vec.end(), 0);

	std::size_t n_top_vertices = ratio_size_subset * vertices_vec.size();

	std::pair<std::size_t, std::size_t> rtn = {0,0};
	for(std::size_t i = 0; i < n_top_vertices; i++)
	{
		if(m.valence(vertices_vec[i]) == GREEN)
			rtn.first++;
		if(m.valence(vertices_vec[i]) == RED)
			rtn.second++;
	}

	return rtn;
}
