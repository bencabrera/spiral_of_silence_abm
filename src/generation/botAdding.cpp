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

std::vector<Vertex> add_bots_via_barabasi_albert(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt)
{
	std::vector<double> degrees;
	for (auto v : vertices(g))
		degrees.push_back(boost::degree(v,g));

	return add_bots_proportional_to_weights(g, m, n_bots, degrees, mt);
}

std::vector<Vertex> add_bots_via_inverse_barabasi_albert(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt)
{
	std::vector<double> inverse_degrees;
	for (auto v : vertices(g))
		inverse_degrees.push_back(1.0 / boost::degree(v,g));

	return add_bots_proportional_to_weights(g, m, n_bots, inverse_degrees, mt);
}

std::vector<Vertex> add_bots_uniformly(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt)
{
	std::vector<double> unif_weights(boost::num_vertices(g), 1.0);

	return add_bots_proportional_to_weights(g, m, n_bots, unif_weights, mt);
}
