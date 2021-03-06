#include <catch/catch.hpp>
#include "../../tests/test_helpers.hpp"
#include "../generationParams.h"
#include <fstream>

TEST_CASE("Param space should be build correctly", "[generation]")
{
	auto filepath = instance_path("/tests/example_param_space.json");
	std::ifstream file(filepath);
	REQUIRE(file.is_open());
	auto params = build_param_space_from_json(file);

	REQUIRE(params.size() == 6);

	REQUIRE(params[0].bot_attachment_method == "PreferentialAttachment(1,1)");
	REQUIRE(params[1].bot_attachment_method == "PreferentialAttachment(1,1)");
	REQUIRE(params[2].bot_attachment_method == "PreferentialAttachment(2,1)");
	REQUIRE(params[3].bot_attachment_method == "PreferentialAttachment(2,1)");
	REQUIRE(params[4].bot_attachment_method == "PreferentialAttachment(3,1)");
	REQUIRE(params[5].bot_attachment_method == "PreferentialAttachment(3,1)");

	REQUIRE(params[0].expression_threshold_init == "Unif(0,1)");
	REQUIRE(params[1].expression_threshold_init == "Unif(0,2)");
	REQUIRE(params[2].expression_threshold_init == "Unif(0,1)");
	REQUIRE(params[3].expression_threshold_init == "Unif(0,2)");
	REQUIRE(params[4].expression_threshold_init == "Unif(0,1)");
	REQUIRE(params[5].expression_threshold_init == "Unif(0,2)");
}

TEST_CASE("Param space should be build correctly when using template expression", "[generation]")
{
	auto filepath = instance_path("/tests/example_param_space_template_expr.json");
	std::ifstream file(filepath);
	REQUIRE(file.is_open());
	auto params = build_param_space_from_json(file);

	REQUIRE(params.size() == 6);

	REQUIRE(params[0].bot_attachment_method == "PreferentialAttachment(1,1)");
	REQUIRE(params[1].bot_attachment_method == "PreferentialAttachment(1,1)");
	REQUIRE(params[2].bot_attachment_method == "PreferentialAttachment(2,1)");
	REQUIRE(params[3].bot_attachment_method == "PreferentialAttachment(2,1)");
	REQUIRE(params[4].bot_attachment_method == "PreferentialAttachment(3,1)");
	REQUIRE(params[5].bot_attachment_method == "PreferentialAttachment(3,1)");

	REQUIRE(params[0].expression_threshold_init == "Unif(0,1)");
	REQUIRE(params[1].expression_threshold_init == "Unif(0,2)");
	REQUIRE(params[2].expression_threshold_init == "Unif(0,1)");
	REQUIRE(params[3].expression_threshold_init == "Unif(0,2)");
	REQUIRE(params[4].expression_threshold_init == "Unif(0,1)");
	REQUIRE(params[5].expression_threshold_init == "Unif(0,2)");
}

TEST_CASE("Param list should be build correctly from template", "[generation]")
{
	auto filepath = instance_path("/tests/example_template_expr.json");
	std::ifstream file(filepath);
	REQUIRE(file.is_open());
	nlohmann::json template_obj;
	file >> template_obj;
	
	auto list = build_param_list_from_template(template_obj);
	REQUIRE(list[0].get<std::string>() == "PreferentialAttachment(1,1)");
	REQUIRE(list[1].get<std::string>() == "PreferentialAttachment(2,1)");
	REQUIRE(list[2].get<std::string>() == "PreferentialAttachment(3,1)");
}
