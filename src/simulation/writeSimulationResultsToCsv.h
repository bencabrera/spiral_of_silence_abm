#pragma once

#include "simulateUntilStable.h"
#include "../model/modelProperties.hpp"

#include <fstream>

constexpr char GLOBAL_DISTRIBUTION_HEADER[] = "n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red,";
constexpr char CLUSTER_DISTRIBUTION_HEADER[] = "n_cluster_green_dominates,n_cluster_red_dominates,n_speaking_green_by_cluster,n_speaking_red_by_cluster,n_silenced_green_by_cluster,n_silenced_red_by_cluster,";

void write_csv_header(std::ostream& csv_file, const Model& m);

std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> write_global_distribution(std::ostream& csv_file, const Model& m);
void write_cluster_distribution(std::ostream& csv_file, const Model& m);
void write_cluster_distribution(std::ostream& csv_file, const Model& m, std::size_t n_speaking_green, std::size_t n_speaking_red, std::size_t n_silenced_green, std::size_t n_silenced_red);
void write_simulation_results_to_csv(std::ostream& csv_file, const SimulationResults& simulation_results, const Model& m);
