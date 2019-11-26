#include "writeToGml.h"
#include <regex>

namespace {
	constexpr auto INDENT = "\t";

	bool is_number(std::string str) 
	{
		static const std::regex expression("-?[0-9]+(\\.[0-9]+)?");
		std::smatch match;
		return (std::regex_match(str, match, expression));
	}

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

			// bool in_quotes = val.find(' ') != std::string::npos;
			bool in_quotes = !is_number(val);	
			ostr << key << " " << (in_quotes ? "\"" : "") << val << (in_quotes ? "\"" : "");
		}
	}
}

void write_to_gml(std::ostream& ostr, const Graph& g, bool export_directed, std::map<std::string, VertexPropertyMap<std::string>> vertex_labels, std::map<std::string, EdgePropertyMap<std::string>> edge_labels, std::map<std::string,std::string> graph_labels)
{
	ostr << "graph [" << std::endl;

	for (auto [k,v] : graph_labels) {
		bool in_quotes = !is_number(v);	
		if(in_quotes)
			ostr << INDENT << k << " \"" << v << "\"" << std::endl;	
		else
			ostr << INDENT << k << " " << v << std::endl;	
	}

	for(auto v : vertices(g))
	{
		ostr << INDENT << "node [" << std::endl;
		ostr << INDENT << INDENT << "id " << v << std::endl;
		ostr << INDENT << INDENT << "label " << v << std::endl;
		write_properties(ostr, vertex_labels, v, 2);
		ostr << std::endl << INDENT << "]" << std::endl;
	}

	std::size_t i_edge = 0;
	for(auto e : edges(g))
	{
		auto v = boost::source(e,g);
		auto w = boost::target(e,g);
		if(!export_directed && w > v)
			continue;

		ostr << INDENT << "edge [" << std::endl;
		ostr << INDENT << INDENT << "id " << i_edge++ << std::endl;
		ostr << INDENT << INDENT << "source " << v << std::endl;
		ostr << INDENT << INDENT << "target " << w << std::endl;
		write_properties(ostr, edge_labels, e, 2);
		if(edge_labels.size() > 0)
			ostr << std::endl;
		ostr << INDENT << "]" << std::endl;
	}

	ostr << "]";
}
