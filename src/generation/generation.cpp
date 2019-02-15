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
	auto res = graph_generation(g, params, mt);

	// assign clusters
	model._vertex_cluster = VertexPropertyMap<std::size_t>(boost::num_vertices(g), boost::get(boost::vertex_index, g));
	std::size_t max_cluster = 0;
	for (auto v : vertices(g)) {
		model._vertex_cluster[v] = res.vertex_cluster[v];
		max_cluster = std::max(max_cluster, res.vertex_cluster[v]);
	}
	model._n_clusters = max_cluster+1;

	// add bots to network
	std::vector<Vertex> bots;
	if(params.n_bots > 0)
	{
		auto [method,param_strs] = parse_method_call(params.bot_attachment_method);
		
		// get number of connections each bot should make
		std::size_t m_bots = 0;
		double gamma_bots = 1;
		if(param_strs.size() < 2)
		{
			// use same params as PreferentialAttachment
			auto [network_method,network_param_strs] = parse_method_call(params.network_model);
			if(network_method == "PreferentialAttachment" && network_param_strs.size() == 2)
			{
				m_bots = std::stoul(network_param_strs[0]);
				gamma_bots = std::stod(network_param_strs[1]);
			}
			else
				throw FormatException("Model for bot attachment needs m and gamma parameter and network model is not PreferentialAttachment.");
		}
		else 
		{
			m_bots = std::stoul(param_strs[0]);
			gamma_bots = std::stod(param_strs[1]);
		}

		if(method == "PreferentialAttachment")
			bots = add_bots_via_preferential_attachment(g, m_bots, gamma_bots, params.n_bots, mt);
		else 
			throw FormatException("Bot attachment method not recognized.");

		model._is_bot = VertexPropertyMap<bool>(boost::num_vertices(g), boost::get(boost::vertex_index, g));
		for (auto v : vertices(g)) 
			model._is_bot[v] = false;
		for (auto v : bots) 
		{
			model._is_bot[v] = true;
			model._vertex_cluster[v] = BOT_CLUSTER_INDEX;
		}
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
