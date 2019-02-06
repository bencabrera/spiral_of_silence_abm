#include "generationParams.h"

#include "../exceptions/formatException.h"

using namespace nlohmann;

GenerationParams GenerationParams::parse_from_json(std::istream& istr)
{
	json obj;
	istr >> obj;

	return parse_from_json(obj);
}

GenerationParams GenerationParams::parse_from_json(json obj)
{
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
	if(obj.count("ratio_valence_green"))
		params.ratio_valence_green = obj["ratio_valence_green"].get<double>();

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
	obj["ratio_valence_green"] = ratio_valence_green;

	ostr << obj;
}

std::vector<GenerationParams> build_param_space_from_json(std::istream& istr)
{
	json obj;
	istr >> obj;

	return build_param_space_from_json(obj);
}

std::vector<GenerationParams> build_param_space_from_json(json obj)
{
	const std::vector<std::string> properties = {"n_user", "n_bots", "expression_threshold_init", "inner_confidence_init", "network_model", "bot_attachment_method", "is_directed", "ratio_valence_green"};

	// check that all properties are arrays
	for (std::string p : properties) {
		if(!obj[p].is_array())
			throw FormatException("Property '" + p + "' is not an array.");
	}

	std::vector<std::size_t> positions(properties.size(), 0);

	std::vector<std::size_t> lengths;
	for (std::string p : properties) {
		lengths.push_back(obj[p].size());
	}

	std::vector<GenerationParams> rtn;

	json sub_obj;
	for(std::size_t i_prop = 0; i_prop < properties.size(); i_prop++)
		sub_obj[properties[i_prop]] = obj[properties[i_prop]][positions[i_prop]];

	rtn.push_back(GenerationParams::parse_from_json(sub_obj));

	std::size_t i = 0;
	while(i < properties.size())
	{
		while(positions[i] == lengths[i]-1 && i < properties.size())
			i++;

		if(i == properties.size())
			break;

		positions[i]++;

		for(std::size_t j = 0; j < i; j++)
			positions[j] = 0;

		i = 0;

		// fill empty json obj with current vals
		json sub_obj;
		for(std::size_t i_prop = 0; i_prop < properties.size(); i_prop++)
			sub_obj[properties[i_prop]] = obj[properties[i_prop]][positions[i_prop]];

		rtn.push_back(GenerationParams::parse_from_json(sub_obj));
	}

	return rtn;
}
