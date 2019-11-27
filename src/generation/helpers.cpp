#include "helpers.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "../exceptions/formatException.h"
#include <boost/algorithm/string/trim.hpp>

std::pair<std::string, std::vector<std::string>> parse_method_call(std::string str)
{
	std::size_t pos_opening_parenthesis = str.find_first_of('(');
	std::size_t pos_closing_parenthesis = str.find_last_of(')');

	// if no parameter specified -> return method name and empty parameter set
	if(pos_opening_parenthesis == std::string::npos || pos_closing_parenthesis == std::string::npos)
		return {str,{}};	

	std::string method_str = str.substr(0,pos_opening_parenthesis);
	std::string params_str = str.substr(pos_opening_parenthesis+1, pos_closing_parenthesis - pos_opening_parenthesis - 1);

	// identify separating commas
	int bracket_depth = 0;
	std::vector<std::size_t> commas;
	for(std::size_t i = 0; i < params_str.size(); i++)
	{
		if(params_str[i] == ',' && bracket_depth == 0)
			commas.push_back(i);
		if(params_str[i] == '[' || params_str[i] == '(')
			bracket_depth++;
		if(params_str[i] == ']' || params_str[i] == ')')
			bracket_depth--;
	}

	if(commas.size() == 0)
		return {method_str,{params_str}};

	// split via separating commas
	std::vector<std::string> params;
	params.push_back(params_str.substr(0,commas.front()));
	
	for(int i = 0; i < static_cast<int>(commas.size())-1; i++)
		params.push_back(params_str.substr(commas[i]+1,commas[i+1]-commas[i]-1));

	params.push_back(params_str.substr(commas.back()+1));

	// boost::split(params, params_str, boost::is_any_of(","));

	return {method_str,params};
}

std::vector<std::vector<double>> parse_matrix(std::string str)
{
	boost::trim_if(str,boost::is_any_of(" ,[]"));

	std::vector<std::string> row_strs;
	boost::split(row_strs, str, boost::is_any_of(";"));
	std::size_t n_rows = row_strs.size();

	std::vector<std::vector<double>> matrix(n_rows);

	unsigned long long i_row = 0;
	for(auto row_str : row_strs)
	{
		boost::trim_if(row_str,boost::is_any_of(" ,"));

		std::vector<std::string> value_strs;
		boost::split(value_strs, row_str, boost::is_any_of(","));

		for(auto val : value_strs)
			matrix[i_row].push_back(std::stod(val));

		i_row++;
	}

	return matrix;
}

double draw_from_distribution(const std::string param, std::mt19937& mt)
{
	auto [method,param_strs] = parse_method_call(param);
	if(method == "Unif")
	{
		if(param_strs.size() != 2)
			throw FormatException("Unif need two parameters.");
		double lower_bound = std::stod(param_strs[0]);
		double upper_bound = std::stod(param_strs[1]);
		std::uniform_real_distribution<double> dist(lower_bound, upper_bound);
		return dist(mt);
	}
	if(method == "Exp")
	{
		if(param_strs.size() != 1)
			throw FormatException("Exp needs one parameter.");
		double mean = std::stod(param_strs[0]);
		std::exponential_distribution<double> dist(mean);
		return dist(mt);
	}

	throw FormatException("Distribution type not recognized.");
}

