#include "preferentialAttachment.h"

void generate_iterative_attachment_directed(Graph& g, const std::size_t n, const std::size_t m, double gamma, std::mt19937& rand_gen)
{
	std::uniform_real_distribution<double> unif_0_1_dist;

	// start with m isolated vertices
	g = Graph(m); 

	// add one vertex to everyone?
	auto v_start = boost::add_vertex(g);
	for (auto v : vertices(g)) {
		if(v != v_start)
			boost::add_edge(v_start, v, g);	
	}

	std::vector<std::size_t> degrees;
	std::vector<double> weights;
	for (auto v : vertices(g))
	{
		degrees.push_back(boost::degree(v,g));	
		weights.push_back(std::pow(degrees[v],gamma));
	}

	// --- loop ---
	for(std::size_t i = m+1; i < n; i++)
	{
		// find all vertices to connect to
		std::set<Vertex> vertices_to_connect_to;	

		// add new vertex and connect to all of them
		auto v = boost::add_vertex(g);

		std::discrete_distribution<std::size_t> dist(weights.begin(),weights.end());

		while(vertices_to_connect_to.size() < m)
		{
			auto w = dist(rand_gen);
			if(vertices_to_connect_to.find(w) == vertices_to_connect_to.end() && v != w)
				vertices_to_connect_to.insert(w);
		}

		degrees.push_back(0);
		weights.push_back(0);

		for (auto w : vertices_to_connect_to) {
			if(unif_0_1_dist(rand_gen) > 0.5)
				boost::add_edge(v,w,g);
			else
				boost::add_edge(w,v,g);
			degrees[v]++;
			degrees[w]++;
			weights[v] = std::pow(degrees[v],gamma);
			weights[w] = std::pow(degrees[w],gamma);
		}
	}
}
