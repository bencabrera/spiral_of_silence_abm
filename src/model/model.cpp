#include "model.h"
#include "../graph/io/readFromGml.h"
#include "../exceptions/formatException.h"

Model Model::read_from_gml(std::istream& istr)
{
	Model m;
	auto [g,vertex_attributes,edge_attributes,graph_attributes] = ::read_from_gml(istr);
	m.g = g;

	const std::vector<std::string> mandatory_properties = {"valence", "expression_threshold", "inner_confidence"};
	for (auto p : mandatory_properties) {
		if(!vertex_attributes.count(p))
			throw FormatException("The mandatory property '" + p + "' does not exist in input graph.");
	}

	m._valence = VertexPropertyMap<Valence>(boost::num_vertices(m.g), boost::get(boost::vertex_index, m.g));
	for (auto v : vertices(g)) {
		m._valence[v] = vertex_attributes["valence"][v] == "green" ? GREEN : RED;
	}

	m._expression_threshold = convert<std::string,double>(g,vertex_attributes["expression_threshold"]);
	m._inner_confidence = convert<std::string,double>(g,vertex_attributes["inner_confidence"]);

	if(vertex_attributes.count("is_bot"))
		m._is_bot = convert<std::string,bool>(g,vertex_attributes["is_bot"]);
	else
	{
		m._is_bot = VertexPropertyMap<bool>(boost::num_vertices(m.g), boost::get(boost::vertex_index, m.g));
		for (auto v : vertices(g)) {
			m._is_bot[v] = false;
		}
	}

	if(graph_attributes.count("bot_influence"))
	{
		m._bot_influence = std::stod(graph_attributes["bot_influence"]);
		graph_attributes.erase("bot_influence");
	}
	if(graph_attributes.count("alpha"))
	{
		m._alpha = std::stod(graph_attributes["alpha"]);
		graph_attributes.erase("alpha");
	}

	m.global_properties = graph_attributes;

	return m;
}
