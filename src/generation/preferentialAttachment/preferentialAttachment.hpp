#include <random>
#include <boost/graph/random.hpp>

namespace GraphGeneration {


	template<typename TBoostGraph>
		typename TBoostGraph::vertex_descriptor draw_vertex(TBoostGraph& g, std::mt19937& rand_gen, std::vector<typename TBoostGraph::vertex_descriptor>& ordered_vertices, std::size_t normalizing_sum_degree)
		{
			std::uniform_real_distribution<double> unif_0_1_dist;

			double p = unif_0_1_dist(rand_gen);

			std::size_t v = 0;
			std::size_t sum_degree = boost::degree(ordered_vertices[v],g);
			while(p > (static_cast<double>(sum_degree)/normalizing_sum_degree) && v < boost::num_vertices(g))
			{
				v++;
				if(v < boost::num_vertices(g))
					sum_degree += boost::degree(ordered_vertices[v],g);
			}

			return v;	
		}

	template<typename TBoostGraph>
		void generate_preferential_attachment_undirected(
			TBoostGraph& g, 
			const std::size_t n, 
			const std::size_t m,
			std::mt19937& rand_gen
		)
		{
			// --- initialization ---
			for(std::size_t i = 0; i < m; i++)
				boost::add_vertex(g);

			auto v_start = boost::add_vertex(g);
			for (auto v : boost::make_iterator_range(boost::vertices(g))) {
				if(v != v_start)
					boost::add_edge(v_start, v, g);	
			}

			std::size_t normalizing_sum_degree = 0;
			std::vector<typename TBoostGraph::vertex_descriptor> ordered_vertices;
			for (auto v : boost::make_iterator_range(boost::vertices(g))) 
			{
				ordered_vertices.push_back(v);
				normalizing_sum_degree += boost::degree(v,g);
			}

			std::sort(ordered_vertices.begin(), ordered_vertices.end(), [&g](typename TBoostGraph::vertex_descriptor v1, typename TBoostGraph::vertex_descriptor v2) {
				return boost::degree(v1,g) > boost::degree(v2,g);
			});

				
			// --- loop ---
			for(std::size_t i = m+2; i < n; i++)
			{
				// find all vertices to connect to
				std::set<typename TBoostGraph::vertex_descriptor> vertices_to_connect_to;	
				while(vertices_to_connect_to.size() < m)
				{
					auto w = draw_vertex(g, rand_gen, ordered_vertices, normalizing_sum_degree);
					if(vertices_to_connect_to.find(w) == vertices_to_connect_to.end())
						vertices_to_connect_to.insert(w);
				}

				// add new vertex and connect to all of them
				auto v = boost::add_vertex(g);

				for (auto w : vertices_to_connect_to) {
					boost::add_edge(v,w,g);
				}

				normalizing_sum_degree += 2*m;

				ordered_vertices.push_back(v);
				std::sort(ordered_vertices.begin(), ordered_vertices.end(), [&g](typename TBoostGraph::vertex_descriptor v1, typename TBoostGraph::vertex_descriptor v2) {
						return boost::degree(v1,g) > boost::degree(v2,g);
						});
			}
		}

	template<typename TBoostGraph>
		void generate_preferential_attachment_directed(
			TBoostGraph& g, 
			const std::size_t n, 
			const std::size_t m,
			std::mt19937& rand_gen
		)
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

			std::size_t normalizing_sum_degree = 0;
			std::vector<typename TBoostGraph::vertex_descriptor> ordered_vertices;
			for (auto v : boost::make_iterator_range(boost::vertices(g))) 
			{
				ordered_vertices.push_back(v);
				normalizing_sum_degree += boost::degree(v,g);
			}

			std::sort(ordered_vertices.begin(), ordered_vertices.end(), [&g](typename TBoostGraph::vertex_descriptor v1, typename TBoostGraph::vertex_descriptor v2) {
				return boost::degree(v1,g) > boost::degree(v2,g);
			});

				
			// --- loop ---
			for(std::size_t i = m+1; i < n; i++)
			{
				// find all vertices to connect to
				std::set<typename TBoostGraph::vertex_descriptor> vertices_to_connect_to;	
				// add new vertex and connect to all of them
				auto v = boost::add_vertex(g);
				ordered_vertices.push_back(v);

				while(vertices_to_connect_to.size() < m)
				{
					auto w = draw_vertex(g, rand_gen, ordered_vertices, normalizing_sum_degree);
					if(vertices_to_connect_to.find(w) == vertices_to_connect_to.end() && v != w)
						vertices_to_connect_to.insert(w);
				}


				for (auto w : vertices_to_connect_to) {
						if(unif_0_1_dist(rand_gen) > 0.5)
							boost::add_edge(v,w,g);
						else
							boost::add_edge(w,v,g);
				}

				normalizing_sum_degree += 2*m;

				std::sort(ordered_vertices.begin(), ordered_vertices.end(), [&g](typename TBoostGraph::vertex_descriptor v1, typename TBoostGraph::vertex_descriptor v2) {
						return boost::degree(v1,g) > boost::degree(v2,g);
						});
			}
		}

}
