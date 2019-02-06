#include "modelProperties.hpp"

Vertex most_central_vertex(const Model& m)
{
	Vertex max_v = 0;
	std::size_t max_degree = boost::degree(max_v,m.graph());
	for (auto v : vertices(m.graph())) {
		if(boost::degree(v,m.graph()) > max_degree)	{
			max_degree = boost::degree(v,m.graph());
			max_v = v;
		}
	}	

	return max_v;
}
