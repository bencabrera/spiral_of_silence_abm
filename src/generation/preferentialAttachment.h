#pragma once

#include "../graph/graph.h"

#include <random>

void generate_iterative_attachment_directed(Graph& g, const std::size_t n, const std::size_t m, double gamma, std::mt19937& rand_gen);
