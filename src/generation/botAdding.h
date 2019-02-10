#pragma once

#include "../graph/graph.h"
#include <random>

std::vector<Vertex> add_bots_via_preferential_attachment(Graph& g, std::size_t m, double gamma, std::size_t n_bots, std::mt19937& mt);
