#pragma once

#include "../../graph/graph.h"

#include <random>
#include <boost/graph/random.hpp>

void generate_uniform_attachment_directed(Graph& g, const std::size_t n, const std::size_t m, std::mt19937& rand_gen);
