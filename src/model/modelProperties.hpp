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

Vertex most_central_vertex(const Model& m);
