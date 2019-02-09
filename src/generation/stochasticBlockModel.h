#pragma once

#include "../graph/graph.h"

#include <random>

void generate_stochastic_block_model_undirected(Graph& g, const std::size_t n, const std::vector<double> priors, std::vector<std::vector<double>> community_connectivity, std::mt19937& mt);

void generate_stochastic_block_model_directed(Graph& g, const std::size_t n, const std::vector<double> priors, std::vector<std::vector<double>> community_connectivity, std::mt19937& mt);
