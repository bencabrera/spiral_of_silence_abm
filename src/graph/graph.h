#pragma once

#include <boost/graph/adjacency_list.hpp>
#include <boost/range/iterator_range.hpp>

using VertexIndexProperty = boost::property<boost::vertex_index_t, std::size_t>;
using EdgeIndexProperty = boost::property<boost::edge_index_t, std::size_t>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::bidirectionalS, VertexIndexProperty, EdgeIndexProperty, boost::vecS>;

using Vertex = Graph::vertex_descriptor;
using Edge = Graph::edge_descriptor;

// external vertex properties
template<typename T>
using VertexPropertyMap = boost::vector_property_map<T, typename boost::property_map<Graph, boost::vertex_index_t>::const_type>;

// external edge edge properties (we have to use vector_property_map because edge_descriptor is not std::size_t, even if edgeList = vecS)
template<typename T>
using EdgePropertyMap = boost::vector_property_map<T, typename boost::property_map<Graph, boost::edge_index_t>::const_type>; 

boost::iterator_range<Graph::vertex_iterator> vertices(const Graph& g);
boost::iterator_range<Graph::edge_iterator> edges(const Graph& g);
boost::iterator_range<Graph::in_edge_iterator> in_edges(const Vertex& v, const Graph& g);
boost::iterator_range<Graph::out_edge_iterator> out_edges(const Vertex& v, const Graph& g);

template<typename TFrom,typename TTo>
TTo conv(TFrom v);

template<typename T1, typename T2> VertexPropertyMap<T2> convert(const Graph& g, const VertexPropertyMap<T1>& map);

template<typename T1, typename T2> EdgePropertyMap<T2> convert(const Graph& g, const EdgePropertyMap<T1>& map);
