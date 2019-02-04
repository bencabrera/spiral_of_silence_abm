#include "../../../libs/catch/catch.hpp"
#include "../../tests/test_helpers.hpp"
#include <fstream>
#include <sstream>

#include "../io/readFromGml.h"
#include "../io/writeToGml.h"

TEST_CASE("Gml writing and reading again should yield same graph","[io]") 
{
	auto filepath = instance_path("/gmlReading/2.gml");
	std::ifstream file(filepath);

	REQUIRE(file.is_open());

	auto [g, vertex_labels, edge_labels] = read_from_gml(file);

	std::stringstream ss;
	write_to_gml(ss, g, vertex_labels, edge_labels);
	ss.seekg(0);

	std::ofstream file_test("out.gml");
	file_test << ss.str();
	
	auto [g2, vertex_labels2, edge_labels2] = read_from_gml(ss);

	REQUIRE(boost::num_vertices(g) == boost::num_vertices(g2));
	REQUIRE(boost::num_edges(g) == boost::num_edges(g2));

	REQUIRE(boost::edge(0,1,g2).second);
	REQUIRE(boost::edge(0,2,g2).second);
	REQUIRE(boost::edge(1,2,g2).second);
	REQUIRE(boost::edge(1,3,g2).second);
	REQUIRE(boost::edge(2,3,g2).second);
	REQUIRE(boost::edge(3,4,g2).second);

	REQUIRE(vertex_labels.size() == vertex_labels2.size());
	REQUIRE(vertex_labels["label"][0] == vertex_labels2["label"][0]);
	REQUIRE(vertex_labels["label"][1] == vertex_labels2["label"][1]);
	REQUIRE(vertex_labels["label"][2] == vertex_labels2["label"][2]);
	REQUIRE(vertex_labels["label"][3] == vertex_labels2["label"][3]);
	REQUIRE(vertex_labels["label"][4] == vertex_labels2["label"][4]);

	REQUIRE(edge_labels.size() == edge_labels2.size());
	REQUIRE(edge_labels["label"][boost::edge(0,1,g).first] == edge_labels2["label"][boost::edge(0,1,g2).first]);
	REQUIRE(edge_labels["label"][boost::edge(0,2,g).first] == edge_labels2["label"][boost::edge(0,2,g2).first]);
	REQUIRE(edge_labels["label"][boost::edge(1,2,g).first] == edge_labels2["label"][boost::edge(1,2,g2).first]);
	REQUIRE(edge_labels["label"][boost::edge(1,3,g).first] == edge_labels2["label"][boost::edge(1,3,g2).first]);
	REQUIRE(edge_labels["label"][boost::edge(2,3,g).first] == edge_labels2["label"][boost::edge(2,3,g2).first]);
	REQUIRE(edge_labels["label"][boost::edge(3,4,g).first] == edge_labels2["label"][boost::edge(3,4,g2).first]);

	REQUIRE(edge_labels["weight"][boost::edge(0,1,g).first] == edge_labels2["weight"][boost::edge(0,1,g2).first]);
	REQUIRE(edge_labels["weight"][boost::edge(0,2,g).first] == edge_labels2["weight"][boost::edge(0,2,g2).first]);
	REQUIRE(edge_labels["weight"][boost::edge(1,2,g).first] == edge_labels2["weight"][boost::edge(1,2,g2).first]);
	REQUIRE(edge_labels["weight"][boost::edge(1,3,g).first] == edge_labels2["weight"][boost::edge(1,3,g2).first]);
	REQUIRE(edge_labels["weight"][boost::edge(2,3,g).first] == edge_labels2["weight"][boost::edge(2,3,g2).first]);
	REQUIRE(edge_labels["weight"][boost::edge(3,4,g).first] == edge_labels2["weight"][boost::edge(3,4,g2).first]);
}
