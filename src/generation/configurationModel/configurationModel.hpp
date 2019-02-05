#pragma once

#include <random>
#include <iostream>
#include <map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/container/flat_set.hpp>

namespace GraphGeneration {

	template<typename TBoostGraph>
		void generate_confmodel_graph_undirected(TBoostGraph& g, const std::map<std::size_t, std::size_t>& degree_histogramm, std::mt19937& rand_gen)
		{
			// calculate the number of vertices
			std::size_t n_vertices = 0;
			for (auto i : degree_histogramm) {
				n_vertices += i.second;	
			}

			// add all vertices to the graph
			for(std::size_t i = 0; i < n_vertices; i++)
				boost::add_vertex(g);

			// setup vector that tells every vertex how many stubs it has
			std::vector<std::size_t> stubs(n_vertices);
			auto stub_it = stubs.begin();
			for (auto& p : degree_histogramm) {
				auto& deg = p.first;
				auto& count = p.second;

				if(degree_histogramm.count(deg))
				{
					std::fill(stub_it, stub_it + count, deg);
					stub_it += count;
				}
			}
			std::shuffle(stubs.begin(), stubs.end(), rand_gen);


			// setup helper vertices_with_stubs lookup
			boost::container::flat_set<std::size_t> vertices_with_stubs;
			for (std::size_t v = 0; v < stubs.size(); v++) {
				if(stubs[v] > 0)
					vertices_with_stubs.insert(v);	
			}

			std::size_t no_change_since = 0;
			std::uniform_int_distribution<std::size_t> unif_dist;
			while(vertices_with_stubs.size() > 1 && no_change_since < 500) // if there is only one vertex with stubs we would create loops
			{
				// draw randomly two vertices with stubs to connect with each other
				auto i_1 = unif_dist(rand_gen) % vertices_with_stubs.size();
				auto i_2 = unif_dist(rand_gen) % vertices_with_stubs.size();
				while(i_1 == i_2)
					i_2 = unif_dist(rand_gen) % vertices_with_stubs.size();

				auto v_1 = *(vertices_with_stubs.begin()+i_1);
				auto v_2 = *(vertices_with_stubs.begin()+i_2);
				
				// connect them
				if(boost::edge(v_1,v_2,g).second)
				{
					no_change_since++;
					continue;
				}
				else
				{
					boost::add_edge(v_1, v_2, g);
					no_change_since = 0;
				}

				// decrease stubs count and possibly remove vertex from vertices_with_stubs
				stubs[v_1]--;
				stubs[v_2]--;

				if(stubs[v_1] == 0)
					vertices_with_stubs.erase(v_1);
				if(stubs[v_2] == 0)
					vertices_with_stubs.erase(v_2);
			}
		}


	// --- directed versions ---
	
	template<typename TBoostGraph>
		void helper_graph_from_stubs(TBoostGraph& g, std::vector<std::pair<std::size_t, std::size_t>>& stubs, std::mt19937& rand_gen)
		{
			auto n_vertices = stubs.size();
			// add all vertices to the graph
			for(std::size_t i = 0; i < n_vertices; i++)
				boost::add_vertex(g);

			// setup helper vertices_with_stubs lookup
			boost::container::flat_set<std::size_t> vertices_with_out_stubs, vertices_with_in_stubs;
			for (std::size_t v = 0; v < stubs.size(); v++) {
				if(stubs[v].first > 0)
					vertices_with_out_stubs.insert(v);	
				if(stubs[v].second > 0)
					vertices_with_in_stubs.insert(v);	
			}

			std::size_t no_change_since = 0;
			std::uniform_int_distribution<std::size_t> unif_dist;
			while( // not create loops
					((vertices_with_out_stubs.size() > 1 && vertices_with_in_stubs.size() > 1) 
					 || (vertices_with_out_stubs.size() == 1 && vertices_with_in_stubs.size() == 1 && (*vertices_with_out_stubs.begin()) != (*vertices_with_in_stubs.begin())) )
					&& no_change_since < 500
				 )
			{
				// draw randomly two vertices with stubs to connect with each other
				auto i_from = unif_dist(rand_gen) % vertices_with_out_stubs.size();
				auto i_to = unif_dist(rand_gen) % vertices_with_in_stubs.size();
				while(i_from == i_to)
				{
					no_change_since++;
					i_to = unif_dist(rand_gen) % vertices_with_in_stubs.size();

					if(no_change_since > 500)
						return;
				}

				auto v_from = *(vertices_with_out_stubs.begin()+i_from);
				auto v_to = *(vertices_with_in_stubs.begin()+i_to);

				// connect them
				if(boost::edge(v_from,v_to,g).second)
				{
					no_change_since++;
					continue;
				}
				else
				{
					boost::add_edge(v_from, v_to, g);
					no_change_since = 0;
				}

				// decrease stubs count and possibly remove vertex from vertices_with_stubs
				stubs[v_from].first--;
				stubs[v_to].second--;

				if(stubs[v_from].first == 0)
					vertices_with_out_stubs.erase(v_from);
				if(stubs[v_to].second == 0)
					vertices_with_in_stubs.erase(v_to);
			}
		}

	inline std::vector<std::pair<std::size_t, std::size_t>> helper_build_stubs_independently_from_in_out_dists(const std::map<std::size_t, std::size_t>& out_degree_histogramm, const std::map<std::size_t, std::size_t>& in_degree_histogramm, std::mt19937& rand_gen)
	{
		// calculate the number of vertices and number of edges
		std::size_t n_vertices = 0;
		for (auto i : out_degree_histogramm) 
			n_vertices += i.second;	
		std::size_t n_vertices_tmp = 0;
		for (auto i : out_degree_histogramm) 
			n_vertices_tmp += i.second;	
		if(n_vertices != n_vertices_tmp)
			throw std::logic_error("Number of vertices in out_degree_distribution and in in_degree_distribution do not match.");

		std::size_t n_edges = 0;
		for (std::size_t deg = 0; deg < out_degree_histogramm.size(); deg++) 
			n_edges += deg * out_degree_histogramm.at(deg);
		std::size_t n_in_edges = 0;
		for (std::size_t deg = 0; deg < in_degree_histogramm.size(); deg++) 
			n_in_edges += deg * in_degree_histogramm.at(deg);
		if(n_edges != n_in_edges)
			throw std::logic_error("Number of edges in out_degree_distribution and in in_degree_distribution do not match.");

		// setup vector that tells every vertex how many stubs it has

		std::vector<std::size_t> out_stubs(n_vertices);
		auto out_stub_it = out_stubs.begin();
		for (auto& p : out_degree_histogramm) {
			auto& deg = p.first;
			auto& count = p.second;
			std::fill(out_stub_it, out_stub_it + count, deg);
			out_stub_it += out_degree_histogramm.at(deg);
		}
		std::shuffle(out_stubs.begin(), out_stubs.end(), rand_gen);

		std::vector<std::size_t> in_stubs(n_vertices);
		auto in_stub_it = in_stubs.begin();
		for (auto& p : in_degree_histogramm) {
			auto& deg = p.first;
			auto& count = p.second;
			std::fill(in_stub_it, in_stub_it + count, deg);
			in_stub_it += in_degree_histogramm.at(deg);
		}
		std::shuffle(in_stubs.begin(), in_stubs.end(), rand_gen);

		std::vector<std::pair<std::size_t, std::size_t>> stubs; // [out_degree] [in_degree]
		for(std::size_t v = 0; v < n_vertices; v++)
			stubs.push_back(std::make_pair(out_stubs.at(v), in_stubs.at(v)));

		return stubs;
	}

	template<typename TBoostGraph>
		void generate_confmodel_graph_directed_independent(TBoostGraph& g, const std::map<std::size_t, std::size_t>& out_degree_histogramm, const std::map<std::size_t, std::size_t>& in_degree_histogramm, std::mt19937& rand_gen)
		{
			auto stubs = helper_build_stubs_independently_from_in_out_dists(out_degree_histogramm, in_degree_histogramm, rand_gen);
			helper_graph_from_stubs(g, stubs, rand_gen);
		}

	// idea for even better directed configuration model:
	// model degree distribution wrt to in and out degree together and distribute vertices on these pairs 
}
