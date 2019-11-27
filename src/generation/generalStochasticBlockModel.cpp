#include "generalStochasticBlockModel.h"

#include "graphGeneration.h"

std::vector<std::size_t> generate_general_stochastic_block_model_undirected(Graph& g, const std::size_t n, const std::vector<double> priors, const double inter_cluster_density_ratio, const std::string network_model, std::mt19937& mt)
{
	double avg_density = 0.0;

	std::vector<std::size_t> cluster_sizes;
	for (auto p : priors)
	{
		cluster_sizes.push_back(static_cast<std::size_t>(n*p));

		GenerationParams params;	
		params.is_directed = false;
		params.n_user = cluster_sizes.back();
		params.network_model = network_model;

		Graph cluster;
		graph_generation(cluster, params, mt);

		Vertex cur = boost::add_vertex(g);
		for(std::size_t i = 1; i < cluster_sizes.back(); i++)
			boost::add_vertex(g);

		for (auto e : edges(cluster)) 
			boost::add_edge(boost::source(e,cluster)+cur,boost::target(e,cluster)+cur,g);

		avg_density += 2 * static_cast<double>(boost::num_edges(cluster)) / std::pow(boost::num_vertices(cluster),2);
	}

	avg_density = avg_density / priors.size();

	// add edges between clusters with a fraction of their density
	std::uniform_real_distribution<double> unif_0_1(0,1);
	for(std::size_t i_cluster = 0; i_cluster < priors.size(); i_cluster++)
	{
		Vertex i_start = 0;
		for(std::size_t i = 0; i < i_cluster; i++)
			i_start += cluster_sizes[i];
		for(std::size_t j_cluster = i_cluster+1;  j_cluster < priors.size();  j_cluster++)
		{
			Vertex j_start = 0;
			for(std::size_t i = 0; i < j_cluster; i++)
			{
				j_start += cluster_sizes[i];
			}

			for(auto v_1 = i_start; v_1 < i_start + cluster_sizes[i_cluster]; v_1++)
				for(auto v_2 = j_start; v_2 < j_start + cluster_sizes[j_cluster]; v_2++)
				{
					if(unif_0_1(mt) < avg_density * inter_cluster_density_ratio)
						boost::add_edge(v_1, v_2, g);
				}
		}
	}

	std::vector<std::size_t> vertex_cluster;
	for(std::size_t i_cluster = 0; i_cluster < cluster_sizes.size(); i_cluster++)
		for(std::size_t j = 0; j < cluster_sizes[i_cluster]; j++)
			vertex_cluster.push_back(i_cluster);

	return vertex_cluster;
}
