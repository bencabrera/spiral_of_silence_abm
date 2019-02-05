#include "graph.h"

boost::iterator_range<Graph::vertex_iterator> vertices(const Graph& g)
{
	return boost::make_iterator_range(boost::vertices(g));
}

boost::iterator_range<Graph::edge_iterator> edges(const Graph& g)
{
	return boost::make_iterator_range(boost::edges(g));
}

boost::iterator_range<Graph::in_edge_iterator> in_edges(const Vertex& v, const Graph& g)
{
	return boost::make_iterator_range(boost::in_edges(v,g));
}

boost::iterator_range<Graph::out_edge_iterator> out_edges(const Vertex& v, const Graph& g)
{
	return boost::make_iterator_range(boost::out_edges(v,g));
}


template<> bool conv<std::string,bool>(std::string v)
{
	return v == "1" || v == "true";
}
template<> double conv<std::string,double>(std::string v)
{
	return std::stod(v);
}
template<> std::size_t conv<std::string,std::size_t>(std::string v)
{
	return std::stoul(v);
}

template<> std::string conv<bool,std::string>(bool v)
{
	std::stringstream ss;
	ss << std::boolalpha << v;

	return ss.str();
}
template<> std::string conv<double,std::string>(double v)
{
	return std::to_string(v);
}
template<> std::string conv<std::size_t,std::string>(std::size_t v)
{
	return std::to_string(v);
}

template<typename T1, typename T2> VertexPropertyMap<T2> convert(const Graph& g, const VertexPropertyMap<T1>& map)
{
	VertexPropertyMap<T2> rtn;

	for(auto v : vertices(g))
		rtn[v] = conv<T1,T2>(map[v]);

	return rtn;
}

template<typename T1, typename T2> EdgePropertyMap<T2> convert(const Graph& g, const EdgePropertyMap<T1>& map)
{
	EdgePropertyMap<T2> rtn;

	for(auto e : edges(g))
		rtn[e] = conv<T1,T2>(map[e]);

	return rtn;
}


template VertexPropertyMap<bool> convert<std::string,bool>(const Graph& g, const VertexPropertyMap<std::string>& map);
template VertexPropertyMap<double> convert<std::string,double>(const Graph& g, const VertexPropertyMap<std::string>& map);
template VertexPropertyMap<std::size_t> convert<std::string,std::size_t>(const Graph& g, const VertexPropertyMap<std::string>& map);

template VertexPropertyMap<std::string> convert<bool,std::string>(const Graph& g, const VertexPropertyMap<bool>& map);
template VertexPropertyMap<std::string> convert<double,std::string>(const Graph& g, const VertexPropertyMap<double>& map);
template VertexPropertyMap<std::string> convert<std::size_t,std::string>(const Graph& g, const VertexPropertyMap<std::size_t>& map);

template EdgePropertyMap<bool> convert<std::string,bool>(const Graph& g, const EdgePropertyMap<std::string>& map);
template EdgePropertyMap<double> convert<std::string,double>(const Graph& g, const EdgePropertyMap<std::string>& map);
template EdgePropertyMap<std::size_t> convert<std::string,std::size_t>(const Graph& g, const EdgePropertyMap<std::string>& map);

template EdgePropertyMap<std::string> convert<bool,std::string>(const Graph& g, const EdgePropertyMap<bool>& map);
template EdgePropertyMap<std::string> convert<double,std::string>(const Graph& g, const EdgePropertyMap<double>& map);
template EdgePropertyMap<std::string> convert<std::size_t,std::string>(const Graph& g, const EdgePropertyMap<std::size_t>& map);
