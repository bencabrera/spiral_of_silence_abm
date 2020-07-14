#include <catch/catch.hpp>
#include "../../tests/test_helpers.hpp"
#include "../generationParams.h"
#include "../generation.h"
#include <fstream>

TEST_CASE("Undirected model graphs should be have all edge in both directions", "[generation]")
{
	GenerationParams params;
	params.is_directed = false;

	std::random_device rd;
	std::mt19937 mt(rd());

	auto m = generate(params, mt);

	const auto g = m.graph();

	for (auto e : edges(g)) 
		REQUIRE(boost::edge(boost::target(e,g),boost::source(e,g),g).second);
}

TEST_CASE("Directed models graphs should be one edge that is one in one direction", "[generation]")
{
	GenerationParams params;
	params.is_directed = true;

	std::random_device rd;
	std::mt19937 mt(rd());

	auto m = generate(params, mt);

	const auto g = m.graph();

	bool all_both_directions = true;
	for (auto e : edges(g)) 
		all_both_directions = all_both_directions && boost::edge(boost::target(e,g),boost::source(e,g),g).second;

	REQUIRE(all_both_directions == false);
}
