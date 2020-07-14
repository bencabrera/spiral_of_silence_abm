#include <catch/catch.hpp>
#include "../../tests/test_helpers.hpp"
#include "../io/readFromGml.h"
#include "../io/writeToGml.h"
#include <fstream>

namespace {
	bool edge_indices_are_unique(const Graph& g)
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

TEST_CASE("Reading graph should work correctly", "[io]")
{
	auto filepath = instance_path("/tests/graphIo1.gml");
	std::ifstream file(filepath);
	REQUIRE(file.is_open());
	auto [g, vertex_attributes, edge_attributes, graph_attributes] = read_from_gml(file);

	REQUIRE(edge_indices_are_unique(g));

	REQUIRE(boost::num_vertices(g) == 5);
	REQUIRE(boost::num_edges(g) == 4);

	REQUIRE(boost::edge(1,0,g).second);
	REQUIRE(boost::edge(2,0,g).second);
	REQUIRE(boost::edge(3,0,g).second);
	REQUIRE(boost::edge(4,0,g).second);

	REQUIRE(vertex_attributes.count("prop1") > 0);
	REQUIRE(vertex_attributes.count("prop2") > 0);
	REQUIRE(vertex_attributes.count("prop3") > 0);

	REQUIRE(edge_attributes.count("weight") > 0);

	REQUIRE(graph_attributes.count("directed") > 0);
	REQUIRE(graph_attributes.count("prop1") > 0);
	REQUIRE(graph_attributes.count("prop4") > 0);

	REQUIRE(vertex_attributes["prop1"][0] == "11");
	REQUIRE(vertex_attributes["prop1"][1] == "12");
	REQUIRE(vertex_attributes["prop1"][2] == "323");
	REQUIRE(vertex_attributes["prop1"][3] == "rev");
	REQUIRE(vertex_attributes["prop1"][4] == "1");

	REQUIRE(edge_attributes["weight"][boost::edge(1,0,g).first] == "1");
	REQUIRE(edge_attributes["weight"][boost::edge(2,0,g).first] == "10");
	REQUIRE(edge_attributes["weight"][boost::edge(3,0,g).first] == "231");
	REQUIRE(edge_attributes["weight"][boost::edge(4,0,g).first] == "981");

	REQUIRE(graph_attributes["prop1"] == "val1");
	REQUIRE(graph_attributes["prop4"] == "283");
}

TEST_CASE("Reading graph writing it and reading again should yield same graph","[io]") 
{
	auto filepath = instance_path("/tests/graphIo1.gml");
	std::ifstream file(filepath);
	REQUIRE(file.is_open());
	auto [g, vertex_attributes, edge_attributes, graph_attributes] = read_from_gml(file);

	std::stringstream ss;
	write_to_gml(ss,g,true,vertex_attributes,edge_attributes,graph_attributes);
	ss.seekg(0);

	auto [g2, vertex_attributes2, edge_attributes2, graph_attributes2] = read_from_gml(ss);

	REQUIRE(boost::num_vertices(g) == boost::num_vertices(g2));
	REQUIRE(boost::num_edges(g) == boost::num_edges(g2));

	REQUIRE(boost::edge(1,0,g2).second);
	REQUIRE(boost::edge(2,0,g2).second);
	REQUIRE(boost::edge(3,0,g2).second);
	REQUIRE(boost::edge(4,0,g2).second);

	REQUIRE(vertex_attributes2.count("prop1") > 0);
	REQUIRE(vertex_attributes2.count("prop2") > 0);
	REQUIRE(vertex_attributes2.count("prop3") > 0);

	REQUIRE(edge_attributes2.count("weight") > 0);

	REQUIRE(graph_attributes2.count("directed") > 0);
	REQUIRE(graph_attributes2.count("prop1") > 0);
	REQUIRE(graph_attributes2.count("prop4") > 0);

	REQUIRE(vertex_attributes["prop1"][0] == vertex_attributes2["prop1"][0]);
	REQUIRE(vertex_attributes["prop1"][1] == vertex_attributes2["prop1"][1]);
	REQUIRE(vertex_attributes["prop1"][2] == vertex_attributes2["prop1"][2]);
	REQUIRE(vertex_attributes["prop1"][3] == vertex_attributes2["prop1"][3]);
	REQUIRE(vertex_attributes["prop1"][4] == vertex_attributes2["prop1"][4]);

	REQUIRE(edge_attributes["weight"][boost::edge(1,0,g).first] == edge_attributes2["weight"][boost::edge(1,0,g).first]);
	REQUIRE(edge_attributes["weight"][boost::edge(2,0,g).first] == edge_attributes2["weight"][boost::edge(2,0,g).first]);
	REQUIRE(edge_attributes["weight"][boost::edge(3,0,g).first] == edge_attributes2["weight"][boost::edge(3,0,g).first]);
	REQUIRE(edge_attributes["weight"][boost::edge(4,0,g).first] == edge_attributes2["weight"][boost::edge(4,0,g).first]);

	REQUIRE(graph_attributes["prop1"] == graph_attributes2["prop1"]);
	REQUIRE(graph_attributes["prop4"] == graph_attributes2["prop4"]);
}
