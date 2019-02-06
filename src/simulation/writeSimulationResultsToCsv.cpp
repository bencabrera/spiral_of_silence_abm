#include "writeSimulationResultsToCsv.h"

void write_simulation_results_to_csv(std::string csv_path, const SimulationResults& simulation_results, const Model& m)
{
	bool header = true;
	std::ifstream csv_file_test(csv_path);
	if(csv_file_test.good())
		header = false;
	csv_file_test.close();

	std::ofstream csv_file(csv_path, std::ios_base::app);
	if(header)
	{
		for (auto [k,v] : m.all_global_property_strs()) {
			csv_file << k << ",";
		}

		csv_file << "n_individuals, n_users, n_bots, n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red, ticks_until_stable, most_central_valence, most_central_silenced_start, most_central_silenced_end" << std::endl;
	}

	for (auto [k,v] : m.all_global_property_strs()) {
		csv_file << v << ",";
	}

	// n_individuals
	csv_file << boost::num_vertices(m.graph()) << ",";
	// n_users
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return !m.is_bot(v); }) << ",";
	// n_bots
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.is_bot(v); }) << ",";

	// n_speaking_green
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v); }) << ",";
	// n_speaking_red
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v); }) << ",";
	// n_silenced_green
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && m.is_silenced(v); }) << ",";
	// n_silenced_red
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && m.is_silenced(v); }) << ",";

	// ticks_until_stable
	csv_file << simulation_results.ticks_until_stable << ",";

	// most_central_valence
	csv_file << simulation_results.most_central_valence << ",";

	// most_central_silenced_start
	csv_file << simulation_results.most_central_silenced_start << ",";

	// most_central_silenced_end
	csv_file << simulation_results.most_central_silenced_end;

	csv_file << std::endl;

	// "percent_0_color_flips",
	// "percent_1_color_flips",
	// "percent_2_color_flips",
	// "percent_more_color_flips",
	// "percent_accuracy_below_50",
	// "percent_accuracy_50_to_60",
	// "percent_accuracy_60_to_70",
	// "percent_accuracy_70_to_80",
	// "percent_accuracy_80_to_90",
	// "percent_accuracy_above_90",
}
