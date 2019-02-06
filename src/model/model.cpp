#include "model.h"
#include "../graph/io/readFromGml.h"
#include "../graph/io/writeToGml.h"
#include "../exceptions/formatException.h"
#include "../graph/graphHelper.h"

Model Model::read_from_gml(std::istream& istr)
{
	Model m;
	auto [g,vertex_attributes,edge_attributes,graph_attributes] = ::read_from_gml(istr);
	m.g = g;

	// if undirected add edges in inverse direction
	if(graph_attributes.count("directed") && graph_attributes["directed"] == "0")
	{
		add_inverse_edges(g);
		set_increasing_indices(g);
	}

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
	if(graph_attributes.count("directed"))
	{
		m._is_directed = graph_attributes["directed"] == "1";
		graph_attributes.erase("directed");
	}

	m.global_properties = graph_attributes;

	return m;
}

void Model::write_to_gml(std::ostream& ostr)
{
	std::map<std::string, VertexPropertyMap<std::string>> vertex_attributes;
	std::map<std::string, VertexPropertyMap<std::string>> edge_attributes;
	std::map<std::string, std::string> graph_attributes;

	vertex_attributes["is_bot"] = convert<bool,std::string>(g,_is_bot);
	vertex_attributes["inner_confidence"] = convert<double,std::string>(g,_inner_confidence);
	vertex_attributes["expression_threshold"] = convert<double,std::string>(g,_expression_threshold);

	VertexPropertyMap<std::string> valence_strings(boost::num_vertices(g), boost::get(boost::vertex_index, g));
	for (auto v : vertices(g)) {
		valence_strings[v] = _valence[v] == GREEN ? "green" : "red";
	}
	vertex_attributes["valence"] = valence_strings;

	graph_attributes = global_properties;
	graph_attributes["directed"] = _is_directed ? "1" : "0";
	graph_attributes["bot_influence"] = std::to_string(_bot_influence);
	graph_attributes["alpha"] = std::to_string(_alpha);

	::write_to_gml(ostr, g, vertex_attributes, {}, graph_attributes);
}
