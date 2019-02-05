#include "readFromGml.h"

#include <boost/algorithm/string/trim.hpp>
#include <regex>
#include <iostream>
#include <map>
#include <string>

#include "../../exceptions/formatException.h"

namespace {
	std::map<std::string, std::string> break_into_key_value_pairs(std::string str)
	{
		enum State {
			KEY_NEXT,
			KEY_READING,
			VALUE_NEXT,
			VALUE_READING
		} state;

		bool in_string = false;
		std::map<std::string, std::string> rtn;

		state = KEY_NEXT;
		std::string key, value;
		for(auto c : str)
		{
			if(c == ' ' || c == '\t' || c == '\n' || c == '\r')
			{
				if(state == KEY_NEXT)
					continue;
				else if(state == KEY_READING)
				{
					if(in_string)
						key += c;
					else
						state = VALUE_NEXT;	
				}
				else if(state == VALUE_NEXT)
				{
					if(in_string)
						value += c;
				}
				else if(state == VALUE_READING)
				{
					if(in_string)
						value += c;
					else
					{
						rtn[key] = value;
						key.clear();
						value.clear();
						state = KEY_NEXT;
					}
				}
			}
			else if(c == '"')
			{
				if(state == KEY_NEXT)
				{
					if(key.size() == 0)
					{
						in_string = true;
						state = KEY_READING;
					}
					else
						throw FormatException("Key in GML file may only contain '\"' at the beginning or the end.");
				}
				else if(state == KEY_READING)
					if(in_string)
					{
						in_string = false;
						state = VALUE_NEXT;
					}
					else
						throw FormatException("Key in GML file may not contain '\"'.");
				else if(state == VALUE_NEXT)
				{
					if(value.size() == 0)
					{
						in_string = true;
						state = VALUE_READING;
					}
					else
						throw FormatException("Value in GML file may only contain '\"' at the beginning or the end.");
				}
				else if(state == VALUE_READING)
				{
					if(in_string)
					{
						rtn[key] = value;
						key.clear();
						value.clear();
						in_string = false;
						state = KEY_NEXT;
					}
					else
						throw FormatException("Value in GML file may only contain '\"' in the beginning or in the end.");
				}
			}
			else
			{
				if(state == KEY_NEXT)
				{
					key += c;
					state = KEY_READING;
				}
				else if(state == KEY_READING)
					key += c;
				else if(state == VALUE_NEXT)
				{
					value += c;
					state = VALUE_READING;
				}
				else if(state == VALUE_READING)
					value += c;
			}
		}

		if(state == KEY_READING || state == VALUE_NEXT)
			throw FormatException("Malformed key value pairs.");
		if(state == VALUE_READING)
		{
			if(in_string)
				throw FormatException("Missing final closing '\"' in key value pair.");
			else
				rtn[key] = value;
		}

		return rtn;
	}
}

std::tuple<Graph,std::map<std::string, VertexPropertyMap<std::string>>, std::map<std::string, EdgePropertyMap<std::string>>, std::map<std::string,std::string>> read_from_gml(std::istream& istr)
{
	const std::string text(std::istreambuf_iterator<char>(istr), {});

	const std::regex graph_attributes_regex("graph\\s*\\[([^\\[]+)");
	const std::regex node_regex("node *\\[([^\\]]+)");
	const std::regex edge_regex("edge *\\[([^\\]]+)");

	Graph graph;
	std::map<std::string, std::size_t> v_map;
	std::map<std::string, VertexPropertyMap<std::string>> vertex_labels;
	std::map<std::string, EdgePropertyMap<std::string>> edge_labels;

	// get graph attributes
	std::map<std::string,std::string> graph_labels;
	{
		std::sregex_iterator dir_it_begin(text.begin(), text.end(), graph_attributes_regex);
		std::sregex_iterator dir_it_end;
		if(dir_it_begin != dir_it_end)
		{
			std::string val = (*dir_it_begin)[1];
			std::string clean = std::regex_replace(val, std::regex("graph\\s*\\["), "");
			clean = std::regex_replace(clean, std::regex("node"), "");
			clean = std::regex_replace(clean, std::regex("\\n\\s+"), "\n");
			boost::trim(clean);
			graph_labels = break_into_key_value_pairs(clean);
		}
	}

	// directedness
	bool directed = false;
	if(graph_labels.count("directed"))
	{
		directed = graph_labels["directed"] == "1";
		graph_labels.erase("directed");
	}

	// extract nodes
	std::sregex_iterator outer_node_it_begin(text.begin(), text.end(), node_regex);
	std::sregex_iterator outer_node_it_end;
	for(auto it = outer_node_it_begin; it != outer_node_it_end; it++)
	{
		std::string node_text = (*it)[1];
		auto key_values = break_into_key_value_pairs(node_text);

		if(key_values.count("id") == 0)
			throw FormatException("Node " + std::to_string(v_map.size()) + " is missing id attribute.");

		Vertex v;
		std::string id_str = key_values["id"];
		auto tmpIt = v_map.find(id_str);
		if(tmpIt == v_map.end())
		{
			v = boost::add_vertex(graph);
			v_map.insert({ id_str, v });
		}
		else
			throw FormatException("Node with id " + id_str + " is defined multiple times.");

		key_values.erase("id");

		if(key_values.size() > 0)
		{
			for(auto p : key_values)
				vertex_labels[p.first][v] = p.second;
		}
	}

	if(boost::num_vertices(graph) == 0)
		throw FormatException("No vertices were read.");

	std::sregex_iterator outer_edge_it_begin(text.begin(), text.end(), edge_regex);
	std::size_t i_edge = 0;
	for(auto it = outer_edge_it_begin; it != outer_node_it_end; it++)
	{
		std::string edge_text = (*it)[1];
		auto key_values = break_into_key_value_pairs(edge_text);

		if(key_values.count("source") == 0 || key_values.count("target") == 0)
			throw FormatException("Edge " + std::to_string(i_edge) + " is missing either source or target attribute.");

		auto source_str = key_values.at("source");
		auto target_str = key_values.at("target");
		if(v_map.count(source_str) == 0 || v_map.count(target_str) == 0)
			throw FormatException("Either source or target of edge " + std::to_string(i_edge) + " not found as id of a node.");

		auto s = v_map[source_str];
		auto t = v_map[target_str];

		auto [e,added] = boost::add_edge(s,t,graph);
		if(!added)
			throw FormatException("Edge could not be added.");
		boost::put(boost::edge_index, graph, e, i_edge++);

		// if is undirected also add edge in other direction
		if(!directed)
		{
			auto [e2,added2] = boost::add_edge(t,s,graph);
			if(added2)
				boost::put(boost::edge_index, graph, e2, i_edge++);
		}

		key_values.erase("id");
		key_values.erase("source");
		key_values.erase("target");

		if(key_values.size() > 0)
		{
			for(auto p : key_values)
				edge_labels[p.first][e] = p.second;
		}
	}

	return {graph, vertex_labels, edge_labels,graph_labels};
}
