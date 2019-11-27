#pragma once

#include "../graph/graph.h"

#include <random>

std::vector<std::size_t> generate_general_stochastic_block_model_undirected(Graph& g, const std::size_t n, const std::vector<std::size_t> priors, const double inter_cluster_density_ratio, const std::string network_model, std::mt19937& mt);
