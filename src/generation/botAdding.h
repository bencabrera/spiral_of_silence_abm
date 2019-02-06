#pragma once

#include "../graph/graph.h"
#include <random>

std::vector<Vertex> add_bots_via_barabasi_albert(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt);
std::vector<Vertex> add_bots_via_inverse_barabasi_albert(Graph& g, std::size_t m, std::size_t n_bots, std::mt19937& mt);
std::vector<Vertex> add_bots_uniformly(Graph& g, std::size_t n_bots, std::mt19937& mt, std::size_t m = 1);
