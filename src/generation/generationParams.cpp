#include "generationParams.h"

#include "../../libs/json/json.hpp"

using namespace nlohmann;

GenerationParams GenerationParams::parse_from_json(std::istream& istr)
{
	json obj;
	istr >> obj;

	GenerationParams params;

	if(obj.count("n_user"))
		params.n_user = obj["n_user"].get<std::size_t>();
	if(obj.count("n_bots"))
		params.n_bots = obj["n_bots"].get<std::size_t>();
	if(obj.count("expression_threshold_init"))
		params.expression_threshold_init = obj["expression_threshold_init"].get<std::string>();
	if(obj.count("inner_confidence_init"))
		params.inner_confidence_init = obj["inner_confidence_init"].get<std::string>();
	if(obj.count("network_model"))
		params.network_model = obj["network_model"].get<std::string>();
	if(obj.count("bot_attachment_method"))
		params.bot_attachment_method = obj["bot_attachment_method"].get<std::string>();
	if(obj.count("is_directed"))
		params.is_directed = obj["is_directed"].get<bool>();

	return params;
}

void GenerationParams::write_to_json(std::ostream& ostr)
{
	json obj;

	obj["n_user"] = n_user;
	obj["n_bots"] = n_bots;

	obj["expression_threshold_init"] = expression_threshold_init;
	obj["inner_confidence_init"] = inner_confidence_init;
	obj["network_model"] = network_model;
	obj["bot_attachment_method"] = bot_attachment_method;
	obj["is_directed"] = is_directed;

	ostr << obj;
}
