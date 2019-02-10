#include "botAdding.h"

#include <boost/graph/random.hpp>

namespace {
	std::vector<Vertex> add_bots_proportional_to_weights(Graph& g, std::size_t m, std::size_t n_bots, std::vector<double> weights, std::mt19937& mt)
	{
		std::vector<Vertex> bots;

		std::discrete_distribution<std::size_t> dist(weights.begin(),weights.end());

		for(std::size_t i = 0; i < n_bots; i++)
		{
			auto v_bot = boost::add_vertex(g);
			bots.push_back(v_bot);

			std::set<Vertex> vertices_to_connect_to;	
			while(vertices_to_connect_to.size() < m)
			{
				auto w = dist(mt);
				if(vertices_to_connect_to.find(w) == vertices_to_connect_to.end() && v_bot != w)
					vertices_to_connect_to.insert(w);
			}

			for (auto w : vertices_to_connect_to)
				boost::add_edge(v_bot,w,g); // always add bots so that they are influencing
		}

		return bots;
	}
}

std::vector<Vertex> add_bots_via_preferential_attachment(Graph& g, std::size_t m, double gamma, std::size_t n_bots, std::mt19937& mt)
{
	std::vector<double> weights;
	for (auto v : vertices(g))
		weights.push_back(std::pow(boost::degree(v,g),gamma));

	return add_bots_proportional_to_weights(g, m, n_bots, weights, mt);
}
