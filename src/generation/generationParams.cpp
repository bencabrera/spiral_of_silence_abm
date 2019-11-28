#include "generationParams.h"

#include "../exceptions/formatException.h"
#include <boost/algorithm/string/replace.hpp>

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
	if(obj.count("bot_influence"))
		params.bot_influence = obj["bot_influence"].get<double>();

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
	obj["bot_influence"] = bot_influence;

	ostr << obj;
}

std::vector<GenerationParams> build_param_space_from_json(std::istream& istr)
{
	json obj;
	istr >> obj;

	return build_param_space_from_json(obj);
}

// {
	// "expression": "PreferentialAttachment(M,1)",
	// "templates": [
		// {
			// "var": "{{M}}",
			// "from": 1,
			// "to": 3,
			// "step": 1
		// }
	// ]
// },

std::string print_double_without_zeroes(double d)
{
	double intpart;
	if(std::modf(d,&intpart) == 0.0)
		return std::to_string(static_cast<int>(d));
	else
		return std::to_string(d);
}

std::string substitute_values(std::string expression, json templates, std::vector<double> cur_values) 
{
	std::size_t i_var = 0;

	for (auto obj : templates) {
		boost::replace_all(expression, std::string("{{") + obj["var"].get<std::string>() + "}}", print_double_without_zeroes(cur_values[i_var]));
		i_var++;
	}	

	return expression;
}

json build_param_list_from_template(json tpl) 
{
	if(tpl.find("expression") == tpl.end() || tpl.find("templates") == tpl.end())
		throw FormatException("Template object is missing 'expression' or 'templates'.");
	if(!tpl["expression"].is_string())
		throw FormatException("Template's 'expression' is not a string.");
	if(!tpl["templates"].is_array())
		throw FormatException("Template's 'templates' is not an array.");

	json rtn;

	std::string expression = tpl["expression"];
	auto var_params = tpl["templates"];
	std::vector<double> cur_values(var_params.size());
	std::vector<double> from_values(var_params.size());
	std::vector<double> to_values(var_params.size());
	std::vector<double> step_values(var_params.size());
	std::size_t i_param = 0;
	for (auto p : var_params) {
		cur_values[i_param] = p["from"].get<double>();
		from_values[i_param] = p["from"].get<double>();
		to_values[i_param] = p["to"].get<double>();
		step_values[i_param] = p["step"].get<double>();

		i_param++;
	}
	rtn.push_back(substitute_values(expression, var_params, cur_values));
	
	i_param = 0;
	while(i_param < cur_values.size()) {
		if(cur_values[i_param]+step_values[i_param] <= to_values[i_param]) {
			cur_values[i_param] += step_values[i_param];
			rtn.push_back(substitute_values(expression, var_params, cur_values));
		} else {
			for(std::size_t i = 0; i <= i_param; i++)
				cur_values[i] = from_values[i];

			i_param++;
		}

	}

	return rtn;
}

std::vector<GenerationParams> build_param_space_from_json(json obj)
{
	const std::vector<std::string> properties = {"n_user", "n_bots", "expression_threshold_init", "inner_confidence_init", "network_model", "bot_attachment_method", "is_directed", "ratio_valence_green", "bot_influence"};

	// check that all properties are arrays
	for (std::string p : properties) {
		if(!obj[p].is_array() && !obj[p].is_object())
			throw FormatException("Property '" + p + "' is neither an array, nor an template object.");

		// if obj[p] an object we have to first build an array by parsing the template and plugging in the values
		if(obj[p].is_object()) {
			obj[p] = build_param_list_from_template(obj[p]);			
		}
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
