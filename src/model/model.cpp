#include "model.h"
#include "../graph/io/readFromGml.h"
#include "../exceptions/formatException.h"

Model Model::read_from_gml(std::istream& istr)
{
	throw FormatException("Reading bot_influence not implemented");
	throw FormatException("Reading alpha not implemented");

	Model m;
	auto [g,vertex_attributes,edge_attributes] = ::read_from_gml(istr);
	m.g = g;

	const std::vector<std::string> mandatory_properties = {"valence", "expression_threshold", "inner_confidence", "is_bot"};
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
	m._is_bot = convert<std::string,bool>(g,vertex_attributes["is_bot"]);

	// TODO: make two directional edges for undirectzed graph nin reading gml

	return m;
}
