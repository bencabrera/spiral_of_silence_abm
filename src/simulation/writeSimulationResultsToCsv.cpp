#include "writeSimulationResultsToCsv.h"

void write_simulation_results_to_csv(std::string csv_path, const SimulationResults& simulation_results, const Model& m)
{
	const std::size_t n_vertices = boost::num_vertices(m.graph());

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

		csv_file << "n_individuals, n_users, n_bots, n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red, ticks_until_stable, most_central_valence, most_central_silenced_start, most_central_silenced_end, percent_0_color_flips, percent_1_color_flips, percent_2_color_flips, percent_more_color_flips, percent_accuracy_below_50, percent_accuracy_50_to_60, percent_accuracy_60_to_70, percent_accuracy_70_to_80, percent_accuracy_80_to_90, percent_accuracy_above_90" << std::endl;
	}

	for (auto [k,v] : m.all_global_property_strs()) {
		csv_file << v << ",";
	}

	// n_individuals
	csv_file << n_vertices << ",";
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
	csv_file << simulation_results.most_central_silenced_end << ",";

	// color flips
	std::size_t n_0_color_flips = 0, n_1_color_flips = 0, n_2_color_flips = 0, n_more_color_flips = 0;
	std::size_t n_accuracy_below_50 = 0, n_accuracy_50_to_60 = 0, n_accuracy_60_to_70 = 0, n_accuracy_70_to_80 = 0, n_accuracy_80_to_90 = 0, n_accuracy_above_90 = 0;
	for (auto v : vertices(m.graph()))
	{
		if(simulation_results.times_flipped[v] == 0)
			n_0_color_flips++;
		if(simulation_results.times_flipped[v] == 1)
			n_1_color_flips++;
		if(simulation_results.times_flipped[v] == 2)
			n_2_color_flips++;
		if(simulation_results.times_flipped[v] > 2)
			n_more_color_flips++;

		double perception_accuracy = 1 - simulation_results.perception_error[v];
		if(perception_accuracy < 0.5)
			n_accuracy_below_50++;
		if(perception_accuracy >= 0.5 && perception_accuracy < 0.6)
			n_accuracy_50_to_60++;
		if(perception_accuracy >= 0.6 && perception_accuracy < 0.7)
			n_accuracy_60_to_70++;
		if(perception_accuracy >= 0.7 && perception_accuracy < 0.8)
			n_accuracy_70_to_80++;
		if(perception_accuracy >= 0.8 && perception_accuracy < 0.9)
			n_accuracy_80_to_90++;
		if(perception_accuracy >= 0.9)
			n_accuracy_above_90++;
	}

	csv_file << (static_cast<double>(n_0_color_flips) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_1_color_flips) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_2_color_flips) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_more_color_flips) / n_vertices) << ",";

	csv_file << (static_cast<double>(n_accuracy_below_50) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_50_to_60) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_60_to_70) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_70_to_80) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_80_to_90) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_above_90) / n_vertices);

	csv_file << std::endl;
}
