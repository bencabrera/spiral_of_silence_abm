#pragma once

#include "simulateUntilStable.h"
#include "../model/modelProperties.hpp"

#include <fstream>

void write_simulation_results_to_csv(std::string csv_path, const SimulationResults& simulation_results, const Model& m);
