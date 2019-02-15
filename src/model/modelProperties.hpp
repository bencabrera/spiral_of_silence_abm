#pragma once

#include "model.h"

template<typename Pred>
std::size_t count_vertices_with_predicate(const Model& m, Pred f)
{
	std::size_t n = 0;
	for (auto v : vertices(m.graph())) {
		if(f(v,m))
			n++;
	}

	return n;
}

template<typename Pred>
std::vector<std::size_t> count_vertices_with_predicate_by_clusters(const Model& m, Pred f)
{
	std::vector<std::size_t> count_by_cluster(m.n_clusters(), 0);

	for (auto v : vertices(m.graph())) {
		if(f(v,m) && m.cluster(v) != BOT_CLUSTER_INDEX)
			count_by_cluster[m.cluster(v)]++;
	}

	return count_by_cluster;
}

template<typename Pred>
double average_degree_with_predicate(const Model& m, Pred f)
{
	double rtn = 0.0;
	std::size_t n = 0;

	for (auto v : vertices(m.graph()))
		if(f(v,m))
		{
			rtn += boost::degree(v,m.graph());
			n++;
		}

	return rtn / n;
}

// how often does each color appear in subset of vertices with highest degree
std::pair<std::size_t, std::size_t> count_colors_in_top_degrees(const Model& m, double ratio_size_subset);

Vertex most_central_vertex(const Model& m);
