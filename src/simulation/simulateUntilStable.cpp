#include "simulateUntilStable.h"

#include "performOneStep.h"
#include "../model/modelProperties.hpp"

SimulationResults simulate_until_stable(Model& m, double epsilon, std::function<void(Model&)> step_callback)
{
	SimulationResults rtn;

	auto v_most_central = most_central_vertex(m);
	rtn.most_central_silenced_start = m.is_silenced(v_most_central);
	rtn.most_central_valence = m.valence(v_most_central);

	StepResults res;
	do {
		if(step_callback)
			step_callback(m);
		res = perform_one_step(m,epsilon);
		rtn.ticks_until_stable++;
	}
	while(res.n_humans_not_changed_confidence != 0);

	rtn.most_central_silenced_end = m.is_silenced(v_most_central);

	return rtn;
}

