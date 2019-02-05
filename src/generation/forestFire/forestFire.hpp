#include <random>
#include <boost/graph/random.hpp>
#include <algorithm>
#include <functional>

namespace GraphGeneration {
	enum ForrestFireEdgeDirection {
		OUT_EDGES,
		IN_EDGES
	};

	template<typename TBoostGraph>
		std::vector<typename TBoostGraph::edge_descriptor> draw_allowed_edges(
				const TBoostGraph& g, 
				const typename TBoostGraph::vertex_descriptor w, 
				std::vector<bool>& visited, 
				std::mt19937& rand_gen, 
				const std::size_t numToDraw, 
				const ForrestFireEdgeDirection edge_direction
				)
		{
			std::vector<typename TBoostGraph::edge_descriptor> allowed_edges;

			if(edge_direction == OUT_EDGES)
			{
				for(auto e : boost::make_iterator_range(boost::out_edges(w,g)))
					if(!visited[boost::target(e,g)])
						allowed_edges.push_back(e);
			} 
			else 
			{
				for(auto e : boost::make_iterator_range(boost::in_edges(w,g)))
					if(!visited[boost::source(e,g)])
						allowed_edges.push_back(e);
			}

			std::vector<unsigned> which_edges(allowed_edges.size(), 0);
			std::iota(which_edges.begin(), which_edges.end(), 0);
			std::shuffle(which_edges.begin(), which_edges.end(), rand_gen);

			std::vector<typename TBoostGraph::edge_descriptor> rtn;
			for(std::size_t i = 0; i < std::min(numToDraw,allowed_edges.size()) ; i++)
				rtn.push_back(allowed_edges[which_edges[i]]);

			return rtn;
		}

	template<typename TBoostGraph>
		void recursively_burn_links(
				TBoostGraph& g, 
				typename TBoostGraph::vertex_descriptor v, 
				typename TBoostGraph::vertex_descriptor w, 
				std::vector<bool>& visited, 
				std::mt19937& rand_gen, 
				const double p, 
				const double r,
				std::function<void(typename TBoostGraph::edge_descriptor&)> edge_added_callback = std::function<void(typename TBoostGraph::edge_descriptor&)>()
				)
		{
			if(visited[w])
				return;

			auto res = boost::add_edge(v,w,g);
			if(edge_added_callback)
				edge_added_callback(res.first);
			visited[w] = true;

			// here we compute the parameters to feed to the geometric distribution
			//
			// below find the computation used in igraph (forrestfire.c, ll.117, 118)
			// igraph_real_t param_geom_out=1-fw_prob;
			// igraph_real_t param_geom_in=1-fw_prob*bw_factor;
			double fw_param = 1 - p;
			double bw_param = 1 - r*p;

			std::geometric_distribution<std::size_t> forwardBurnDist(fw_param);
			std::geometric_distribution<std::size_t> backwardBurnDist(bw_param);

			auto numForwardEdges = forwardBurnDist(rand_gen);
			auto numBackwardEdges = backwardBurnDist(rand_gen);

			auto chosenOutEdges = draw_allowed_edges(g, w, visited, rand_gen, numForwardEdges, OUT_EDGES);
			auto chosenInEdges = draw_allowed_edges(g, w, visited, rand_gen, numBackwardEdges, IN_EDGES);

			for(auto e : chosenOutEdges)
				recursively_burn_links(g, v, boost::target(e,g), visited, rand_gen, p, r);
			for(auto e : chosenInEdges)
				recursively_burn_links(g, v, boost::source(e,g), visited, rand_gen, p, r);
		}

	template<typename TBoostGraph>
		void generate_forest_fire_graph(
			TBoostGraph& g, 
			const std::size_t n, 
			const double p, 
			const double r, 
			std::mt19937& rand_gen,
			std::function<void(typename TBoostGraph::vertex_descriptor&)> vertex_added_callback = std::function<void(typename TBoostGraph::vertex_descriptor&)>(),
			std::function<void(typename TBoostGraph::edge_descriptor&)> edge_added_callback = std::function<void(typename TBoostGraph::edge_descriptor&)>()
		)
		{
			for(std::size_t iIteration = 0; iIteration < n; iIteration++)
			{
				auto w = boost::random_vertex(g, rand_gen);
				auto v = boost::add_vertex(g);

				// if this is the first vertex jump to next iteration
				if(iIteration == 0)
					continue;

				std::vector<bool> visited(boost::num_vertices(g), false); // save which vertices have already been visited
				visited[v] = true;

				recursively_burn_links(g, v, w, visited, rand_gen, p, r, edge_added_callback);

				if(vertex_added_callback)
					vertex_added_callback(v);
			}
		}

}
