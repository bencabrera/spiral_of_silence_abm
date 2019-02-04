#pragma once

#include "../graph.h"

void write_to_gml(std::ostream& ostr, const Graph& g, std::map<std::string, VertexPropertyMap<std::string>> vertex_labels = {}, std::map<std::string, EdgePropertyMap<std::string>> edge_labels = {});
