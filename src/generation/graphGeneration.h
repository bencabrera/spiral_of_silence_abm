#include "../graph/graph.h"
#include "../model/model.h"
#include "generationParams.h"

struct GraphGenerationResults {
	std::vector<std::size_t> vertex_cluster;
};

std::vector<std::size_t> generate_general_stochastic_block_model_undirected(Graph& g, const std::size_t n, const std::vector<double> priors, double community_connectivity, const std::string network_model, std::mt19937& mt);

GraphGenerationResults graph_generation(Graph& g, const GenerationParams params, std::mt19937& mt);
