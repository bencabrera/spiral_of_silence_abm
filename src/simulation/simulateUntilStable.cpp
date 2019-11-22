#include "simulateUntilStable.h"

#include "performOneStep.h"
#include "../model/modelProperties.hpp"

SimulationResults simulate_until_stable(Model& m, double epsilon, std::function<void(Model&)> step_callback)
{
	SimulationResults rtn;

	auto v_most_central = most_central_vertex(m);
	rtn.most_central_silenced_start = m.is_silenced(v_most_central);
	rtn.most_central_valence = m.valence(v_most_central);

	VertexPropertyMap<std::size_t> times_flipped(boost::num_vertices(m.graph()),boost::get(boost::vertex_index,m.graph()));
	for (auto v : vertices(m.graph()))
		times_flipped[v] = 0;

	StepResults step_results;
	do {
		if(step_callback)
			step_callback(m);
		step_results = perform_one_step(m,epsilon);

		for (auto v : step_results.flipped_vertices)
			times_flipped[v]++;

		rtn.ticks_until_stable++;
		
		if(rtn.ticks_until_stable > 100000)
			break;
	}
	while(step_results.n_humans_not_changed_confidence != 0);

	rtn.most_central_silenced_end = m.is_silenced(v_most_central);
	rtn.times_flipped = times_flipped;

	// perception error
	double n_green_speaking = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v); });
	double n_red_speaking = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v); });

	
	double global_delta_for_green = (n_green_speaking - n_red_speaking) / (n_green_speaking + n_red_speaking);
	double global_delta_for_red = (n_red_speaking - n_green_speaking) / (n_green_speaking + n_red_speaking);

	VertexPropertyMap<double> perception_error(boost::num_vertices(m.graph()),boost::get(boost::vertex_index,m.graph()));
	for (auto v : vertices(m.graph())) 
	{
		if(m.valence(v) == GREEN)
			perception_error[v] = std::abs(global_delta_for_green - step_results.deltas[v]) / 2;
		if(m.valence(v) == RED)
			perception_error[v] = std::abs(global_delta_for_red - step_results.deltas[v]) / 2;
	}

	rtn.perception_error = perception_error;

	return rtn;
}

