#pragma once

#include <random>

#include "../graph/graph.h"

void generate_erdoes_renyi_undirected(Graph& g, const std::size_t n, const double p, std::mt19937& rand_gen);

void generate_erdoes_renyi_directed(Graph& g, const std::size_t n, const double p, std::mt19937& rand_gen);
