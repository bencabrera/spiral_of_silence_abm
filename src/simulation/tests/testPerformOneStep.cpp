#include "../../../libs/catch/catch.hpp"
#include "../../tests/test_helpers.hpp"
#include "../../model/model.h"
#include "../performOneStep.h"
#include <fstream>

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
