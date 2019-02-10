#include "writeSimulationResultsToCsv.h"

void write_csv_header(std::ostream& csv_file, const Model& m)
{
	for (auto [k,v] : m.all_global_property_strs()) {
		csv_file << k << ",";
	}

	csv_file << "n_individuals, n_users, n_bots, n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red,";
	csv_file << "ticks_until_stable, most_central_valence, most_central_silenced_start, most_central_silenced_end,";
	csv_file << "percent_0_color_flips, percent_1_color_flips, percent_2_color_flips, percent_more_color_flips,";
	csv_file << "percent_accuracy_below_50, percent_accuracy_50_to_60, percent_accuracy_60_to_70, percent_accuracy_70_to_80, percent_accuracy_80_to_90, percent_accuracy_above_90,";
	csv_file << "n_cluster_green_dominates,n_cluster_red_dominates,";
	csv_file << "n_speaking_green_by_cluster,n_speaking_red_by_cluster,n_silenced_green_by_cluster,n_silenced_red_by_cluster";
	csv_file << std::endl;
}

void write_simulation_results_to_csv(std::ostream& csv_file, const SimulationResults& simulation_results, const Model& m)
{
	const std::size_t n_vertices = boost::num_vertices(m.graph());

	for (auto [k,v] : m.all_global_property_strs())
		csv_file << "\"" << v << "\",";

	// n_individuals
	csv_file << n_vertices << ",";
	// n_users
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return !m.is_bot(v); }) << ",";
	// n_bots
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.is_bot(v); }) << ",";

	// n_speaking_green
	auto n_speaking_green = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v); });
	csv_file << n_speaking_green << ",";
	// n_speaking_red
	auto n_speaking_red = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v); });
	csv_file << n_speaking_red << ",";
	// n_silenced_green
	auto n_silenced_green = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && m.is_silenced(v); });
	csv_file << n_silenced_green << ",";
	// n_silenced_red
	auto n_silenced_red = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && m.is_silenced(v); });
	csv_file << n_silenced_red << ",";

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
	}

	// accuracy
	std::size_t n_accuracy_below_50 = 0, n_accuracy_50_to_60 = 0, n_accuracy_60_to_70 = 0, n_accuracy_70_to_80 = 0, n_accuracy_80_to_90 = 0, n_accuracy_above_90 = 0;
	for (auto v : vertices(m.graph()))
	{
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
	csv_file << (static_cast<double>(n_accuracy_above_90) / n_vertices) << ",";

	// dominating color in clusters
	if(m.n_clusters() == 1)
	{
		csv_file << ((n_speaking_green >= n_speaking_red) ? "1" : "0") << ",";
		csv_file << ((n_speaking_green >= n_speaking_red) ? "0" : "1") << ",";
		csv_file << n_speaking_green << ",";
		csv_file << n_speaking_red << ",";
		csv_file << n_silenced_green << ",";
		csv_file << n_silenced_red;
	}
	else
	{
		auto n_green_speaking_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v); });
		auto n_red_speaking_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v); });

		std::size_t n_cluster_green_dominates = 0, n_cluster_red_dominates = 0;
		for(std::size_t i_cluster = 0; i_cluster < m.n_clusters(); i_cluster++)
		{
			if(n_green_speaking_by_cluster[i_cluster] >= n_red_speaking_by_cluster[i_cluster])
				n_cluster_green_dominates++;
			else
				n_cluster_red_dominates++;
		}

		csv_file << n_cluster_green_dominates << "," << n_cluster_red_dominates;

		// n_speaking_green_by_cluster
		auto n_speaking_green_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v); });
		csv_file << "\"";
		csv_file << n_speaking_green_by_cluster[0];
		for (std::size_t i = 1; i < n_speaking_green_by_cluster.size(); i++) 
			csv_file << "," << n_speaking_green_by_cluster[i];
		csv_file << "\",";
			
		// n_speaking_red_by_cluster
		auto n_speaking_red_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v); });
		csv_file << "\"";
		csv_file << n_speaking_red_by_cluster[0];
		for (std::size_t i = 1; i < n_speaking_red_by_cluster.size(); i++) 
			csv_file << "," << n_speaking_red_by_cluster[i];
		csv_file << "\",";

		// n_silenced_green_by_cluster
		auto n_silenced_green_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && m.is_silenced(v); });
		csv_file << "\"";
		csv_file << n_silenced_green_by_cluster[0];
		for (std::size_t i = 1; i < n_silenced_green_by_cluster.size(); i++) 
			csv_file << "," << n_silenced_green_by_cluster[i];
		csv_file << "\",";
		
		// n_silenced_red_by_cluster
		auto n_silenced_red_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && m.is_silenced(v); });
		csv_file << "\"";
		csv_file << n_silenced_red_by_cluster[0];
		for (std::size_t i = 1; i < n_silenced_red_by_cluster.size(); i++) 
			csv_file << "," << n_silenced_red_by_cluster[i];
		csv_file << "\"";
	}

	csv_file << std::endl;
}
