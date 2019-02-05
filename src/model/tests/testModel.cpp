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

TEST_CASE("Performing one step should lead to 0 being silenced.","[io]") 
{
	auto filepath = instance_path("/ejis_paper_example.gml");
	std::ifstream file(filepath);

	REQUIRE(file.is_open());

	auto m = Model::read_from_gml(file);

	REQUIRE(m.confidence(0) == Approx(0.635149));
	REQUIRE(m.is_silenced(0) == false);

	perform_one_step(m);

	REQUIRE(m.inner_confidence(0) == Approx(1.0));
	REQUIRE(m.confidence(0) == Approx(0.4621172));
	REQUIRE(m.is_silenced(0) == true);
}
