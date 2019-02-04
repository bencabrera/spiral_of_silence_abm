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

// std::size_t n_silenced(const Model& m);
// std::size_t n_green(const Model& m);
// std::size_t n_red(const Model& m);
// std::size_t n_green_silenced(const Model& m);
// std::size_t n_red_silenced(const Model& m);
// std::size_t n_bots(const Model& m);

