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

Vertex most_central_vertex(const Model& m);
