#pragma once

#include <functional>

#include "../model/model.h"

struct SimulationResults {
	std::size_t ticks_until_stable = 0;
	bool most_central_silenced_start = false;
	bool most_central_silenced_end = false;
	Valence most_central_valence;
	VertexPropertyMap<std::size_t> times_flipped;
	VertexPropertyMap<double> perception_error;
};

SimulationResults simulate_until_stable(Model& m, double epsilon = 1e-4, std::function<void(Model&)> step_callback = std::function<void(Model&)>());
