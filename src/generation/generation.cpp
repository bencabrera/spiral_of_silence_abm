#include "generation.h"

#include "../exceptions/formatException.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "preferentialAttachment/preferentialAttachment.hpp"

namespace {
	std::pair<std::string, std::vector<std::string>> parse_method_call(std::string str)
	{
		std::size_t pos_opening_parenthesis = str.find_first_of('(');
		std::size_t pos_closing_parenthesis = str.find_first_of(')');

		// if no parameter specified -> return method name and empty parameter set
		if(pos_opening_parenthesis == std::string::npos || pos_closing_parenthesis == std::string::npos)
			return {str,{}};	

		std::string method_str = str.substr(0,pos_opening_parenthesis);
		std::string params_str = str.substr(pos_opening_parenthesis+1, pos_closing_parenthesis - pos_opening_parenthesis - 1);

		std::vector<std::string> params;
		boost::split(params, params_str, boost::is_any_of(","));

		return {method_str,params};
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
}

Model generate(const GenerationParams params, std::mt19937& mt)
{
	Model m;	

	// build graph via network model
	Graph g;
	{
		auto [method,param_strs] = parse_method_call(params.network_model);
		if(method == "BarabasiAlbert")
		{
			if(param_strs.size() != 1)
				throw FormatException("BarabasiAlbert model need m parameter");
			std::size_t m = std::stoul(param_strs[0]);
			GraphGeneration::generate_preferential_attachment_directed(g, params.n_user, m, mt);
		}
	}
	m.g = g;

	// initialize expression thresholds
	m._expression_threshold = VertexPropertyMap<double>(boost::num_vertices(m.g), boost::get(boost::vertex_index, m.g));
	for (auto v : vertices(m.g)) {
		m._expression_threshold[v] = draw_from_distribution(params.expression_threshold_init,mt);
	}

	// initialize inner confidences
	m._inner_confidence = VertexPropertyMap<double>(boost::num_vertices(m.g), boost::get(boost::vertex_index, m.g));
	for (auto v : vertices(m.g)) {
		m._inner_confidence[v] = draw_from_distribution(params.inner_confidence_init,mt);
	}

	m._valence = VertexPropertyMap<Valence>(boost::num_vertices(m.g), boost::get(boost::vertex_index, m.g));
	std::uniform_real_distribution<double> unif_0_1(0,1);
	for (auto v : vertices(m.g)) {
		if(unif_0_1(mt) < params.ratio_valence_green)
			m._valence[v] = GREEN;
		else
			m._valence[v] = RED;
	}

	return m;
}
