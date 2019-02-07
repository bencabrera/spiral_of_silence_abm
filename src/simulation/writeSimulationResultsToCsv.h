#pragma once

#include "simulateUntilStable.h"
#include "../model/modelProperties.hpp"

#include <fstream>

void write_csv_header(std::ostream& csv_file, const Model& m);
void write_simulation_results_to_csv(std::ostream& csv_file, const SimulationResults& simulation_results, const Model& m);
