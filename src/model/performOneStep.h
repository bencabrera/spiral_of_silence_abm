#pragma once

#include "model.h"

struct StepResults 
{
	std::size_t n_humans_not_changed_confidence = 0;
};

StepResults perform_one_step(Model& m);

