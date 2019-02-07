#include "generation.h"

#include "../exceptions/formatException.h"

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include "preferentialAttachment/preferentialAttachment.hpp"
#include "erdoesRenyi/erdoesRenyi.hpp"

#include "../graph/graphHelper.h"
#include "botAdding.h"

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
	Model model;	

	// build graph via network model
	Graph g;
	{
		auto [method,param_strs] = parse_method_call(params.network_model);
		if(method == "BarabasiAlbert")
		{
			if(param_strs.size() != 1)
				throw FormatException("BarabasiAlbert model needs m parameter");
			std::size_t m = std::stoul(param_strs[0]);
			generate_preferential_attachment_directed(g, params.n_user, m, mt);
		}
		else if(method == "ErdoesRenyi")
		{
			if(param_strs.size() != 1)
				throw FormatException("ErdoesRenyi model needs p parameter");
			double p = std::stod(param_strs[0]);
			if(params.is_directed)
				generate_erdoes_renyi_directed(g, params.n_user, p, mt);
			else
				generate_erdoes_renyi_undirected(g, params.n_user, p, mt);
		}
		else
			throw FormatException("Network model not recognized.");
	}

	// add bots to network
	if(params.n_bots > 0)
	{
		std::vector<Vertex> bots;
		auto [method,param_strs] = parse_method_call(params.network_model);
		if(method == "BarabasiAlbert")
		{
			std::size_t m = 3;
			if(param_strs.size() < 1)
			{
				auto [network_method,network_param_strs] = parse_method_call(params.network_model);
				if(network_method == "BarabasiAlbert" && network_param_strs.size() == 1)
					m = std::stoul(network_param_strs[0]);
				else
					throw FormatException("BarabasiAlbert model for bot attachment needs m parameter and network model is not BarabasiAlbert.");
			}
			else
				m = std::stoul(param_strs[0]);

			bots = add_bots_via_barabasi_albert(g, m, params.n_bots, mt);
		}
		if(method == "Uniform")
		{
			std::size_t m = 1;
			if(param_strs.size() > 0)
				m = std::stoul(param_strs[0]);

			bots = add_bots_uniformly(g, params.n_bots, mt, m);
		}

		model._is_bot = VertexPropertyMap<bool>(boost::num_vertices(model.g), boost::get(boost::vertex_index, model.g));
		for (auto v : vertices(g)) 
			model._is_bot[v] = false;
		for (auto v : bots) 
			model._is_bot[v] = true;
	}

	if(!params.is_directed)
		add_inverse_edges(g);
	set_increasing_indices(g);

	model.g = g;

	// initialize expression thresholds
	model._expression_threshold = VertexPropertyMap<double>(boost::num_vertices(model.g), boost::get(boost::vertex_index, model.g));
	for (auto v : vertices(model.g)) {
		if(model.is_bot(v))
			model._expression_threshold[v] = 0.0;
		else
			model._expression_threshold[v] = draw_from_distribution(params.expression_threshold_init,mt);
	}

	// initialize inner confidences
	model._inner_confidence = VertexPropertyMap<double>(boost::num_vertices(model.g), boost::get(boost::vertex_index, model.g));
	for (auto v : vertices(model.g)) {
		model._inner_confidence[v] = draw_from_distribution(params.inner_confidence_init,mt);
	}

	model._valence = VertexPropertyMap<Valence>(boost::num_vertices(model.g), boost::get(boost::vertex_index, model.g));
	std::uniform_real_distribution<double> unif_0_1(0,1);
	for (auto v : vertices(model.g)) {
		if(unif_0_1(mt) < params.ratio_valence_green)
			model._valence[v] = GREEN;
		else
			model._valence[v] = RED;
	}

	model._is_directed = params.is_directed;

	// add some properties refering to how model was generated
	model.global_properties["expression_threshold_init_method"] = params.expression_threshold_init;
	model.global_properties["inner_confidence_init_method"] = params.inner_confidence_init;
	model.global_properties["network_model"] = params.network_model;
	model.global_properties["bot_attachment_method"] = params.bot_attachment_method;

	return model;
}
