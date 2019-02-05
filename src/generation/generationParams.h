#pragma once

#include "../model/model.h"

class GenerationParams {
	public:
		std::size_t n_user = 1000;
		std::size_t n_bots = 10;

		std::string expression_threshold_init = "Unif(0,1)";	
		std::string inner_confidence_init = "Unif(0,1)";	
		std::string network_model = "BarabasiAlbert(3)";
		std::string bot_attachment_method = "PreferentialAttachment";

		bool is_directed = false;

		static GenerationParams parse_from_json(std::istream& istr);
		void write_to_json(std::ostream& ostr);
};
