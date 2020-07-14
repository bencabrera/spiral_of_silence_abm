#include <catch/catch.hpp>
#include "../../tests/test_helpers.hpp"
#include <fstream>
#include "../helpers.h"
#include <iostream>


TEST_CASE("Method parsing should work correctly", "[generation]")
{
	SECTION("Basic") 
	{
		std::string str = "BA(3)";
		auto [method_str, params_strs] = parse_method_call(str);
		REQUIRE(method_str == "BA");
		REQUIRE(params_strs.size() == 1);
		REQUIRE(params_strs[0] == "3");
	}

	SECTION("Multiple parameters") 
	{
		std::string str = "BA(3,32,45.3,a,b,c)";
		auto [method_str, params_strs] = parse_method_call(str);
		REQUIRE(method_str == "BA");
		REQUIRE(params_strs.size() == 6);
		REQUIRE(params_strs[0] == "3");
		REQUIRE(params_strs[1] == "32");
		REQUIRE(params_strs[2] == "45.3");
		REQUIRE(params_strs[3] == "a");
		REQUIRE(params_strs[4] == "b");
		REQUIRE(params_strs[5] == "c");
	}

	SECTION("Matrices and vectors as parameters") 
	{
		std::string str = "BA(3,[1,2,3,4],[1,2,3;34,5,6;435,3,54])";
		auto [method_str, params_strs] = parse_method_call(str);
		REQUIRE(method_str == "BA");
		REQUIRE(params_strs.size() == 3);
		REQUIRE(params_strs[0] == "3");
		REQUIRE(params_strs[1] == "[1,2,3,4]");
		REQUIRE(params_strs[2] == "[1,2,3;34,5,6;435,3,54]");
	}
}

TEST_CASE("Matrix parsing should work correctly", "[generation]")
{
	std::string str1 = "[1.5,2.2,3]";
	auto m1 = parse_matrix(str1);
	REQUIRE(m1.size() == 1);
	REQUIRE(m1[0].size() == 3);
	REQUIRE(m1[0] == std::vector<double>{1.5,2.2,3});

	std::string str2 = "[1.5,2.1,3;34,5,6;435.3,3,54]";
	auto m2 = parse_matrix(str2);
	REQUIRE(m2.size() == 3);
	REQUIRE(m2[0].size() == 3);
	REQUIRE(m2[1].size() == 3);
	REQUIRE(m2[2].size() == 3);
	REQUIRE(m2[0] == std::vector<double>{1.5,2.1,3});
	REQUIRE(m2[1] == std::vector<double>{34,5,6});
	REQUIRE(m2[2] == std::vector<double>{435.3,3,54});
}
