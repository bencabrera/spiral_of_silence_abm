#pragma once

#include "graph.h"

void set_increasing_indices(Graph& g);

void add_inverse_edges(Graph& g);

std::vector<std::size_t> compute_degree_distribution(Graph& g);
