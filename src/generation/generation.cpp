#include "generation.h"

#include "../exceptions/formatException.h"

#include "preferentialAttachment/preferentialAttachment.hpp"
#include "erdoesRenyi/erdoesRenyi.hpp"
#include "stochasticBlockModel/stochasticBlockModel.hpp"

#include "../graph/graphHelper.h"
#include "botAdding.h"
#include "helpers.h"

Model generate(const GenerationParams params, std::mt19937& mt)
{
	Model model;	

	// build graph via network model
	Graph g;
	{
		auto [method,param_strs] = parse_method_call(params.network_model);
		if(method == "BA")
		{
			if(param_strs.size() != 1)
				throw FormatException("BA model needs m parameter");
			std::size_t m = std::stoul(param_strs[0]);
			generate_preferential_attachment_directed(g, params.n_user, m, mt);
		}
		else if(method == "ER")
		{
			if(param_strs.size() != 1)
				throw FormatException("ER model needs p parameter");
			double p = std::stod(param_strs[0]);
			if(params.is_directed)
				generate_erdoes_renyi_directed(g, params.n_user, p, mt);
			else
				generate_erdoes_renyi_undirected(g, params.n_user, p, mt);
		}
		else if(method == "SBM")
		{
			if(param_strs.size() != 2)
				throw FormatException("Stochastic Block Model model needs 2 parameters: priors & connectivity matrix.");

			auto tmp_priors = parse_matrix(param_strs[0]);
			if(tmp_priors.size() != 1)
				throw FormatException("Prior vector needs to be row vector.");
			auto priors = tmp_priors[0];

			auto matrix = parse_matrix(param_strs[1]);
			if(matrix.size() != priors.size())
				throw FormatException("Connectivity matrix needs to have same dimensions as priors.");
			for(std::size_t i = 0; i < priors.size(); i++)
				if(matrix[i].size() != priors.size())
					throw FormatException("Connectivity matrix needs to have same dimensions as priors.");

			if(params.is_directed)
				generate_stochastic_block_model_directed(g, params.n_user, priors, matrix, mt);
			else
				generate_stochastic_block_model_undirected(g, params.n_user, priors, matrix, mt);
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
