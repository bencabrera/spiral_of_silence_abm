#include "botAdding.h"

#include "preferentialAttachment/preferentialAttachment.hpp"

std::vector<Vertex> add_bots_via_barabasi_albert(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt)
{
	std::vector<Vertex> bots;

	std::size_t normalizing_sum_degree = 0;
	std::vector<Vertex> vertices_sorted_by_degree;
	for (auto v : boost::make_iterator_range(boost::vertices(g))) 
	{
		vertices_sorted_by_degree.push_back(v);
		normalizing_sum_degree += boost::degree(v,g);
	}
	std::sort(vertices_sorted_by_degree.begin(), vertices_sorted_by_degree.end(), [&g](Vertex v1, Vertex v2) { return boost::degree(v1,g) > boost::degree(v2,g); });

	for(std::size_t i = 0; i < n_bots; i++)
	{
		auto v_bot = boost::add_vertex(g);
		bots.push_back(v_bot);
		vertices_sorted_by_degree.push_back(v_bot);

		std::set<Vertex> vertices_to_connect_to;	
		while(vertices_to_connect_to.size() < m)
		{
			auto w = GraphGeneration::draw_vertex(g, mt, vertices_sorted_by_degree, normalizing_sum_degree);
			if(vertices_to_connect_to.find(w) == vertices_to_connect_to.end() && v_bot != w)
				vertices_to_connect_to.insert(w);
		}

		for (auto w : vertices_to_connect_to) {
			boost::add_edge(v_bot,w,g); // always add bots so that they are influencing
		}

		normalizing_sum_degree += 2*m;

		std::sort(vertices_sorted_by_degree.begin(), vertices_sorted_by_degree.end(), [&g](Vertex v1, Vertex v2) { return boost::degree(v1,g) > boost::degree(v2,g); });
	}

	return bots;
}

// TODO: std::vector<Vertex> add_bots_via_inverse_barabasi_albert(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt)

std::vector<Vertex> add_bots_uniformly(Graph& g, std::size_t n_bots, std::mt19937& mt, std::size_t m)
{
	std::vector<Vertex> bots;

	for(std::size_t i = 0; i < n_bots; i++)
	{
		auto v_bot = boost::add_vertex(g);
		bots.push_back(v_bot);

		std::set<Vertex> vertices_to_connect_to;	
		for (std::size_t i_edge = 0; i_edge < m; i_edge++)
		{
			Vertex w;
			do {
				w = boost::random_vertex(g,mt);
			}
			while(v_bot == w || vertices_to_connect_to.count(w) > 0);
			vertices_to_connect_to.insert(w);
		}

		for (auto v : vertices_to_connect_to) 
			boost::add_edge(v_bot, v, g);	
	}

	return bots;
}
