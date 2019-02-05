#include "../../../libs/catch/catch.hpp"
#include "../../tests/test_helpers.hpp"
#include "../model.h"
#include "../performOneStep.h"
#include <fstream>

TEST_CASE("Reading paper example should work and values should match.","[io]") 
{
	auto filepath = instance_path("/ejis_paper_example.gml");
	std::ifstream file(filepath);

	REQUIRE(file.is_open());

	auto m = Model::read_from_gml(file);

	REQUIRE(boost::num_vertices(m.graph()) == 5);
	REQUIRE(boost::num_edges(m.graph()) == 4);

	REQUIRE(m.bot_influence() == 0.8);
	REQUIRE(m.alpha() == 1);
	REQUIRE(m.global_properties["prop1"] == "bla");

	REQUIRE(m.expression_threshold(0) == Approx(0.6));
	REQUIRE(m.expression_threshold(1) == Approx(0.1));
	REQUIRE(m.expression_threshold(2) == Approx(0.1));
	REQUIRE(m.expression_threshold(3) == Approx(0.1));
	REQUIRE(m.expression_threshold(4) == Approx(0.1));

	REQUIRE(m.inner_confidence(0) == Approx(1.5));
	REQUIRE(m.inner_confidence(1) == Approx(1.5));
	REQUIRE(m.inner_confidence(2) == Approx(1.5));
	REQUIRE(m.inner_confidence(3) == Approx(1.5));
	REQUIRE(m.inner_confidence(4) == Approx(1.5));

	REQUIRE(m.confidence(0) == Approx(0.635149));
	REQUIRE(m.confidence(1) == Approx(0.635149));
	REQUIRE(m.confidence(2) == Approx(0.635149));
	REQUIRE(m.confidence(3) == Approx(0.635149));
	REQUIRE(m.confidence(4) == Approx(0.635149));

	REQUIRE(m.valence(0) == GREEN);
	REQUIRE(m.valence(1) == GREEN);
	REQUIRE(m.valence(2) == RED);
	REQUIRE(m.valence(3) == RED);
	REQUIRE(m.valence(4) == RED);

	REQUIRE(m.is_silenced(0) == false);
	REQUIRE(m.is_silenced(1) == false);
	REQUIRE(m.is_silenced(2) == false);
	REQUIRE(m.is_silenced(3) == false);
	REQUIRE(m.is_silenced(4) == false);
}

TEST_CASE("Reading and writing and reading again should yield same model.","[io]") 
{
	auto filepath = instance_path("/tests/ejis_paper_example_undirected.gml");
	std::ifstream file(filepath);

	REQUIRE(file.is_open());

	auto m = Model::read_from_gml(file);

	std::stringstream ss;
	m.write_to_gml(ss);
	ss.seekg(0);

	auto m2 = Model::read_from_gml(ss);

	REQUIRE(boost::num_vertices(m.graph()) == boost::num_vertices(m2.graph()));
	REQUIRE(boost::num_edges(m.graph()) == boost::num_edges(m2.graph()));

	REQUIRE(m.bot_influence() == m2.bot_influence());
	REQUIRE(m.alpha() == m2.alpha());
	REQUIRE(m.global_properties["prop1"] == m2.global_properties["prop1"]);

	REQUIRE(m.expression_threshold(0) == m2.expression_threshold(0));
	REQUIRE(m.expression_threshold(1) == m2.expression_threshold(1));
	REQUIRE(m.expression_threshold(2) == m2.expression_threshold(2));
	REQUIRE(m.expression_threshold(3) == m2.expression_threshold(3));
	REQUIRE(m.expression_threshold(4) == m2.expression_threshold(4));

	REQUIRE(m.inner_confidence(0) == m2.inner_confidence(0));
	REQUIRE(m.inner_confidence(1) == m2.inner_confidence(1));
	REQUIRE(m.inner_confidence(2) == m2.inner_confidence(2));
	REQUIRE(m.inner_confidence(3) == m2.inner_confidence(3));
	REQUIRE(m.inner_confidence(4) == m2.inner_confidence(4));

	REQUIRE(m.confidence(0) == m2.confidence(0));
	REQUIRE(m.confidence(1) == m2.confidence(1));
	REQUIRE(m.confidence(2) == m2.confidence(2));
	REQUIRE(m.confidence(3) == m2.confidence(3));
	REQUIRE(m.confidence(4) == m2.confidence(4));

	REQUIRE(m.valence(0) == m2.valence(0));
	REQUIRE(m.valence(1) == m2.valence(1));
	REQUIRE(m.valence(2) == m2.valence(2));
	REQUIRE(m.valence(3) == m2.valence(3));
	REQUIRE(m.valence(4) == m2.valence(4));

	REQUIRE(m.is_silenced(0) == m2.is_silenced(0));
	REQUIRE(m.is_silenced(1) == m2.is_silenced(1));
	REQUIRE(m.is_silenced(2) == m2.is_silenced(2));
	REQUIRE(m.is_silenced(3) == m2.is_silenced(3));
	REQUIRE(m.is_silenced(4) == m2.is_silenced(4));
}

