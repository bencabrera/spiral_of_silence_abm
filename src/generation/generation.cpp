#include "generation.h"

#include "../exceptions/formatException.h"

#include "../graph/graphHelper.h"
#include "botAdding.h"
#include "helpers.h"
#include "graphGeneration.h"

Model generate(const GenerationParams params, std::mt19937& mt)
{
	Model model;	

	// build graph via network model
	auto& g = model.g;
	graph_generation(model.g, params, mt);

	// add bots to network
	if(params.n_bots > 0)
	{
		std::vector<Vertex> bots;
		auto [method,param_strs] = parse_method_call(params.network_model);
		
		// get number of connections each bot should make
		std::size_t m = 0;
		if(param_strs.size() < 1)
		{
			auto [network_method,network_param_strs] = parse_method_call(params.network_model);
			if(network_method == "BA" && network_param_strs.size() == 1)
				m = std::stoul(network_param_strs[0]);
			else
				throw FormatException("Model for bot attachment needs m parameter and network model is not BA.");
		}
		else
			m = std::stoul(param_strs[0]);

		if(method == "BA")
		{
			bots = add_bots_via_barabasi_albert(g, m, params.n_bots, mt);
		}
		else if(method == "InverseBA")
		{
			bots = add_bots_via_inverse_barabasi_albert(g, m, params.n_bots, mt);
		}
		else if(method == "Uniform")
		{
			bots = add_bots_uniformly(g, m, params.n_bots, mt);
		}
		else 
			throw FormatException("Bot attachment method not recognized.");

		model._is_bot = VertexPropertyMap<bool>(boost::num_vertices(g), boost::get(boost::vertex_index, g));
		for (auto v : vertices(g)) 
			model._is_bot[v] = false;
		for (auto v : bots) 
			model._is_bot[v] = true;
	}

	if(!params.is_directed)
		add_inverse_edges(g);
	set_increasing_indices(g);

	// initialize expression thresholds
	model._expression_threshold = VertexPropertyMap<double>(boost::num_vertices(g), boost::get(boost::vertex_index, g));
	for (auto v : vertices(g)) {
		if(model.is_bot(v))
			model._expression_threshold[v] = 0.0;
		else
			model._expression_threshold[v] = draw_from_distribution(params.expression_threshold_init,mt);
	}

	// initialize inner confidences
	model._inner_confidence = VertexPropertyMap<double>(boost::num_vertices(g), boost::get(boost::vertex_index, g));
	for (auto v : vertices(g)) {
		model._inner_confidence[v] = draw_from_distribution(params.inner_confidence_init,mt);
	}

	model._valence = VertexPropertyMap<Valence>(boost::num_vertices(g), boost::get(boost::vertex_index, g));
	std::uniform_real_distribution<double> unif_0_1(0,1);
	for (auto v : vertices(g)) {
		if(model.is_bot(v))
		{
			model._valence[v] = RED;
			continue;
		}

		if(unif_0_1(mt) < params.ratio_valence_green)
			model._valence[v] = GREEN;
		else
			model._valence[v] = RED;
	}

	model._is_directed = params.is_directed;

	model._bot_influence = params.bot_influence;

	// add some properties refering to how model was generated
	model.global_properties["expression_threshold_init_method"] = params.expression_threshold_init;
	model.global_properties["inner_confidence_init_method"] = params.inner_confidence_init;
	model.global_properties["network_model"] = params.network_model;
	model.global_properties["bot_attachment_method"] = params.bot_attachment_method;

	return model;
}
