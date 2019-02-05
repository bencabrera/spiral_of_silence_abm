#pragma once

#include "../graph.h"

std::tuple<Graph,std::map<std::string, VertexPropertyMap<std::string>>, std::map<std::string, EdgePropertyMap<std::string>>, std::map<std::string,std::string>> read_from_gml(std::istream& istr);
