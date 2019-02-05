#include "writeToGml.h"

namespace {
	constexpr auto INDENT = "\t";

	template<typename TPropertyMap, typename TIndex>
	void write_properties(std::ostream& ostr, std::map<std::string, TPropertyMap> properties, TIndex i, std::size_t indentation = 0)
	{
		bool first = true;
		for(auto p : properties)
		{
			const auto& key = p.first;
			const auto& val = p.second[i];
			if(val.empty())
				continue;
			
			if(!first)
				ostr << std::endl;
			first = false;

			for(std::size_t i_indent = 0; i_indent < indentation; i_indent++)
				ostr << INDENT;

			bool contains_space = val.find(' ') != std::string::npos;
			ostr << key << " " << (contains_space ? "\"" : "") << val << (contains_space ? "\"" : "");
		}
	}
}

void write_to_gml(std::ostream& ostr, const Graph& g, std::map<std::string, VertexPropertyMap<std::string>> vertex_labels, std::map<std::string, EdgePropertyMap<std::string>> edge_labels, std::map<std::string,std::string> graph_labels)
{
	ostr << "graph [" << std::endl;

	for (auto [k,v] : graph_labels) {
		ostr << INDENT << k << " " << v << std::endl;	
	}

	for(auto v : vertices(g))
	{
		ostr << INDENT << "node [" << std::endl;
		ostr << INDENT << INDENT << "id " << v << std::endl;
		write_properties(ostr, vertex_labels, v, 2);
		ostr << std::endl << INDENT << "]" << std::endl;
	}

	for(auto e : edges(g))
	{
		ostr << INDENT << "edge [" << std::endl;
		ostr << INDENT << INDENT << "source " << boost::source(e,g) << std::endl;
		ostr << INDENT << INDENT << "target " << boost::target(e,g) << std::endl;
		write_properties(ostr, edge_labels, e, 2);
		ostr << std::endl << INDENT << "]" << std::endl;
	}

	ostr << "]";
}
