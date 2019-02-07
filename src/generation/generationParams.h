#pragma once

#include "../model/model.h"
#include "../../libs/json/json.hpp"

class GenerationParams {
	public:
		std::size_t n_user = 1000;
		std::size_t n_bots = 10;
		double ratio_valence_green = 0.5;

		std::string expression_threshold_init = "Unif(0,1)";	
		std::string inner_confidence_init = "Unif(0,1)";	
		std::string network_model = "BA(3)";
		std::string bot_attachment_method = "BA(1)";

		bool is_directed = false;

		static GenerationParams parse_from_json(std::istream& istr);
		static GenerationParams parse_from_json(nlohmann::json obj);
		void write_to_json(std::ostream& ostr);
};

std::vector<GenerationParams> build_param_space_from_json(std::istream& istr);
std::vector<GenerationParams> build_param_space_from_json(nlohmann::json obj);
