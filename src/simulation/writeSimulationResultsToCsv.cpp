#include "writeSimulationResultsToCsv.h"

void write_csv_header(std::ostream& csv_file, const Model& m)
{
	for (auto [k,v] : m.all_global_property_strs()) {
		csv_file << k << ",";
	}

	csv_file << "n_individuals, n_edges, n_users, n_bots,";
	csv_file << GLOBAL_DISTRIBUTION_HEADER;
	csv_file << "ticks_until_stable, most_central_valence, most_central_silenced_start, most_central_silenced_end,";
	csv_file << "percent_0_color_flips, percent_1_color_flips, percent_2_color_flips, percent_more_color_flips,";
	csv_file << "percent_accuracy_below_50, percent_accuracy_50_to_60, percent_accuracy_60_to_70, percent_accuracy_70_to_80, percent_accuracy_80_to_90, percent_accuracy_above_90,avg_accuracy,";
	csv_file << CLUSTER_DISTRIBUTION_HEADER;
	csv_file << "avg_degree_green,avg_degree_red,";
	csv_file << "n_top_degree_green,n_top_degree_red";
	csv_file << std::endl;
}

void write_cluster_distribution(std::ostream& csv_file, const Model& m) {
	// n_speaking_green
	auto n_speaking_green = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v) && !m.is_bot(v); });
	csv_file << n_speaking_green << ",";
	// n_speaking_red
	auto n_speaking_red = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v) && !m.is_bot(v); });
	csv_file << n_speaking_red << ",";
	// n_silenced_green
	auto n_silenced_green = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && m.is_silenced(v) && !m.is_bot(v); });
	csv_file << n_silenced_green << ",";
	// n_silenced_red
	auto n_silenced_red = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && m.is_silenced(v) && !m.is_bot(v); });
	csv_file << n_silenced_red << ",";

	write_cluster_distribution(csv_file, m, n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red);
}

std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> write_global_distribution(std::ostream& csv_file, const Model& m) {
	std::tuple<std::size_t, std::size_t, std::size_t, std::size_t> rtn;

	// n_speaking_green
	std::get<0>(rtn) = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v) && !m.is_bot(v); });
	csv_file << std::get<0>(rtn) << ",";
	// n_speaking_red
	std::get<1>(rtn) = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v) && !m.is_bot(v); });
	csv_file << std::get<1>(rtn) << ",";
	// n_silenced_green
	std::get<2>(rtn) = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && m.is_silenced(v) && !m.is_bot(v); });
	csv_file << std::get<2>(rtn) << ",";
	// n_silenced_red
	std::get<3>(rtn) = count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && m.is_silenced(v) && !m.is_bot(v); });
	csv_file << std::get<3>(rtn) << ",";

	return rtn;
}

void write_cluster_distribution(std::ostream& csv_file, const Model& m, std::size_t n_speaking_green, std::size_t n_speaking_red, std::size_t n_silenced_green, std::size_t n_silenced_red) {
	if(m.n_clusters() == 1)
	{
		csv_file << ((n_speaking_green >= n_speaking_red) ? "1" : "0") << ",";
		csv_file << ((n_speaking_green >= n_speaking_red) ? "0" : "1") << ",";
		csv_file << n_speaking_green << ",";
		csv_file << n_speaking_red << ",";
		csv_file << n_silenced_green << ",";
		csv_file << n_silenced_red << ",";
	}
	else
	{
		auto n_green_speaking_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v) && !m.is_bot(v); });
		auto n_red_speaking_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v) && !m.is_bot(v); });

		std::size_t n_cluster_green_dominates = 0, n_cluster_red_dominates = 0;
		for(std::size_t i_cluster = 0; i_cluster < m.n_clusters(); i_cluster++)
		{
			if(n_green_speaking_by_cluster[i_cluster] >= n_red_speaking_by_cluster[i_cluster])
				n_cluster_green_dominates++;
			else
				n_cluster_red_dominates++;
		}

		csv_file << n_cluster_green_dominates << "," << n_cluster_red_dominates << ",";

		// n_speaking_green_by_cluster
		auto n_speaking_green_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && !m.is_silenced(v) && !m.is_bot(v); });
		csv_file << "\"";
		csv_file << n_speaking_green_by_cluster[0];
		for (std::size_t i = 1; i < n_speaking_green_by_cluster.size(); i++) 
			csv_file << "," << n_speaking_green_by_cluster[i];
		csv_file << "\",";

		// n_speaking_red_by_cluster
		auto n_speaking_red_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && !m.is_silenced(v) && !m.is_bot(v); });
		csv_file << "\"";
		csv_file << n_speaking_red_by_cluster[0];
		for (std::size_t i = 1; i < n_speaking_red_by_cluster.size(); i++) 
			csv_file << "," << n_speaking_red_by_cluster[i];
		csv_file << "\",";

		// n_silenced_green_by_cluster
		auto n_silenced_green_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN && m.is_silenced(v) && !m.is_bot(v); });
		csv_file << "\"";
		csv_file << n_silenced_green_by_cluster[0];
		for (std::size_t i = 1; i < n_silenced_green_by_cluster.size(); i++) 
			csv_file << "," << n_silenced_green_by_cluster[i];
		csv_file << "\",";

		// n_silenced_red_by_cluster
		auto n_silenced_red_by_cluster = count_vertices_with_predicate_by_clusters(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED && m.is_silenced(v) && !m.is_bot(v); });
		csv_file << "\"";
		csv_file << n_silenced_red_by_cluster[0];
		for (std::size_t i = 1; i < n_silenced_red_by_cluster.size(); i++) 
			csv_file << "," << n_silenced_red_by_cluster[i];
		csv_file << "\",";
	}
}

void write_simulation_results_to_csv(std::ostream& csv_file, const SimulationResults& simulation_results, const Model& m)
{
	// TODO: is this correct handling
	if(simulation_results.ticks_until_stable > 100000)
		return;

	const std::size_t n_vertices = boost::num_vertices(m.graph());
	const std::size_t n_edges = boost::num_edges(m.graph());

	for (auto [k,v] : m.all_global_property_strs())
		csv_file << "\"" << v << "\",";

	// n_individuals
	csv_file << n_vertices << ",";
	csv_file << n_edges << ",";
	// n_users
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return !m.is_bot(v); }) << ",";
	// n_bots
	csv_file << count_vertices_with_predicate(m, [](auto v, const Model& m) -> bool { return m.is_bot(v); }) << ",";

	auto [n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red] = write_global_distribution(csv_file, m);

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

	csv_file << (static_cast<double>(n_0_color_flips) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_1_color_flips) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_2_color_flips) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_more_color_flips) / n_vertices) << ",";


	// accuracy
	double mean_accuracy = 0;
	std::size_t n_accuracy_below_50 = 0, n_accuracy_50_to_60 = 0, n_accuracy_60_to_70 = 0, n_accuracy_70_to_80 = 0, n_accuracy_80_to_90 = 0, n_accuracy_above_90 = 0;
	for (auto v : vertices(m.graph()))
	{
		double perception_accuracy = 1 - simulation_results.perception_error[v];
		mean_accuracy += perception_accuracy;

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
	mean_accuracy = mean_accuracy / boost::num_vertices(m.graph());

	csv_file << (static_cast<double>(n_accuracy_below_50) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_50_to_60) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_60_to_70) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_70_to_80) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_80_to_90) / n_vertices) << ",";
	csv_file << (static_cast<double>(n_accuracy_above_90) / n_vertices) << ",";
	csv_file << mean_accuracy << ",";

	// dominating color in clusters
	write_cluster_distribution(csv_file, m, n_speaking_green, n_speaking_red, n_silenced_green, n_silenced_red);

	// avg_degree_green
	auto avg_degree_green = average_degree_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == GREEN; });
	csv_file << avg_degree_green << ",";

	// avg_degree_red
	auto avg_degree_red = average_degree_with_predicate(m, [](auto v, const Model& m) -> bool { return m.valence(v) == RED; });
	csv_file << avg_degree_red << ",";

	// n_top_degree_green & n_top_degree_red
	auto n_top_degree_color = count_colors_in_top_degrees(m, 0.01);
	csv_file << n_top_degree_color.first << "," << n_top_degree_color.second << "";

	csv_file << std::endl;
}
