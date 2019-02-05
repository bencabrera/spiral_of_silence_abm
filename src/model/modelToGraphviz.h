#pragma once

#include "model.h"

struct GraphvizOptions {
	bool show_confidence = false;
	bool show_expression_threshold = false;
	bool show_inner_confidence = false;
	bool show_bots = false;
};

void draw_to_graphviz(std::ostream& ostr, const Model& m, GraphvizOptions options = GraphvizOptions());
