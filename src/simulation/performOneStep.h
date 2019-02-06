#pragma once

#include "../model/model.h"

struct StepResults 
{
	std::size_t n_humans_not_changed_confidence = 0;
	std::vector<Vertex> flipped_vertices;
	VertexPropertyMap<double> deltas;
};

StepResults perform_one_step(Model& m, double epsilon = 1.0);

