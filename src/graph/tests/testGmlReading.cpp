#include "../../../libs/catch/catch.hpp"
#include "../../tests/test_helpers.hpp"
#include "../io/readFromGml.h"
#include <fstream>

namespace {
	bool edge_indices_are_unique(const Digraph& g)
	{
		const auto& edge_idx_map = boost::get(boost::edge_index, g);

		std::set<std::size_t> visited_edge_indices;
		for(auto e : edges(g))
		{
			if(visited_edge_indices.find(edge_idx_map[e]) != visited_edge_indices.end())
				return false; // same edge index was already found
			else
				visited_edge_indices.insert(edge_idx_map[e]);
		}

		return true;
	}
}

TEST_CASE("Gml reading should work.","[io]") 
{
	auto filepath = instance_path("/gmlReading/1.gml");
	std::ifstream file(filepath);

	REQUIRE(file.is_open());

	auto [g, vertex_labels, edge_labels] = read_from_gml(file);

	REQUIRE(edge_indices_are_unique(g));

	REQUIRE(boost::num_vertices(g) == 5);
	REQUIRE(boost::num_edges(g) == 6);

	REQUIRE(boost::edge(0,1,g).second);
	REQUIRE(boost::edge(0,2,g).second);
	REQUIRE(boost::edge(1,2,g).second);
	REQUIRE(boost::edge(1,3,g).second);
	REQUIRE(boost::edge(2,3,g).second);
	REQUIRE(boost::edge(3,4,g).second);

	REQUIRE(vertex_labels.size() == 1);
	REQUIRE(vertex_labels["label"][0] == "A");
	REQUIRE(vertex_labels["label"][1] == "B");
	REQUIRE(vertex_labels["label"][2] == "C");
	REQUIRE(vertex_labels["label"][3] == "D");
	REQUIRE(vertex_labels["label"][4] == "E");

	REQUIRE(edge_labels.size() == 2);
	REQUIRE(edge_labels["label"][boost::edge(0,1,g).first] == "a");
	REQUIRE(edge_labels["label"][boost::edge(0,2,g).first] == "b");
	REQUIRE(edge_labels["label"][boost::edge(1,2,g).first] == "c");
	REQUIRE(edge_labels["label"][boost::edge(1,3,g).first] == "d");
	REQUIRE(edge_labels["label"][boost::edge(2,3,g).first] == "e");
	REQUIRE(edge_labels["label"][boost::edge(3,4,g).first] == "f");

	REQUIRE(edge_labels["weight"][boost::edge(0,1,g).first] == "0");
	REQUIRE(edge_labels["weight"][boost::edge(0,2,g).first] == "1");
	REQUIRE(edge_labels["weight"][boost::edge(1,2,g).first] == "2");
	REQUIRE(edge_labels["weight"][boost::edge(1,3,g).first] == "3");
	REQUIRE(edge_labels["weight"][boost::edge(2,3,g).first] == "4");
	REQUIRE(edge_labels["weight"][boost::edge(3,4,g).first] == "5");
}
