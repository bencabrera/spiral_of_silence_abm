#include "graphGeneration.h"

#include "preferentialAttachment.h"
#include "erdoesRenyi.h"
#include "stochasticBlockModel.h"

#include "../exceptions/formatException.h"
#include "helpers.h"

namespace {
	std::vector<std::size_t> all_vertices_same_cluster(const Graph& g)
	{
		std::vector<std::size_t> rtn;

		for(std::size_t v = 0; v < boost::num_vertices(g); v++)
			rtn.push_back(0);

		return rtn;
	}
}

GraphGenerationResults graph_generation(Graph& g, const GenerationParams params, std::mt19937& mt)
{
	GraphGenerationResults rtn;

	auto [method,param_strs] = parse_method_call(params.network_model);
	if(method == "IterativeAttachment")
	{
		if(param_strs.size() != 2)
			throw FormatException("IterativeAttachment model needs m and gamma parameter");
		std::size_t m = std::stoul(param_strs[0]);
		double gamma = std::stod(param_strs[1]);
		generate_iterative_attachment_directed(g, params.n_user, m, gamma, mt);

		rtn.vertex_cluster = all_vertices_same_cluster(g);
	}
	else if(method == "ErdoesRenyi")
	{
		if(param_strs.size() != 1)
			throw FormatException("ER model needs p parameter");
		double p = std::stod(param_strs[0]);
		if(params.is_directed)
			generate_erdoes_renyi_directed(g, params.n_user, p, mt);
		else
			generate_erdoes_renyi_undirected(g, params.n_user, p, mt);

		rtn.vertex_cluster = all_vertices_same_cluster(g);
	}
	else if(method == "StochasticBlockModel")
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

		rtn.vertex_cluster = all_vertices_same_cluster(g); // TODO: this is wrong
	}
	else if(method == "GeneralStochasticBlockModel")
	{
		if(param_strs.size() != 3)
			throw FormatException("Stochastic Block Model model needs 3 parameters: network model & priors & connectivity matrix.");

		const auto network_model = param_strs[0];

		auto tmp_priors = parse_matrix(param_strs[1]);
		if(tmp_priors.size() != 1)
			throw FormatException("Prior vector needs to be row vector.");
		auto priors = tmp_priors[0];

		auto p = std::stod(param_strs[2]);

		rtn.vertex_cluster = generate_general_stochastic_block_model_undirected(g, params.n_user, priors, p, network_model, mt);
	}
	else
		throw FormatException("Network model not recognized.");

	assert(rtn.vertex_cluster.size() == boost::num_vertices(g));
	return rtn;
}
