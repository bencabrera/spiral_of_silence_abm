#pragma once

#include "../../graph/graph.h"

#include <random>

void generate_preferential_attachment_directed(Graph& g, const std::size_t n, const std::size_t m, std::mt19937& rand_gen);
