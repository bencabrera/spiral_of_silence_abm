#include <fstream>
#include <iostream>

#include "exceptions/argumentException.h"
#include "generation/iterativeAttachment.h"

#include "graph/graphHelper.h"

std::vector<double> avg_dist_from_runs(const std::size_t n_vertices, const std::vector<std::vector<std::size_t>> dists)
{
	std::vector<double> dist(n_vertices,0.0);

	for(std::size_t i_dist = 0; i_dist < dists.size(); i_dist++)
		for(std::size_t i = 0; i < dists[i_dist].size(); i++)
			dist[i] += dists[i_dist][i];

	double sum = 0.0;
	for (auto& n : dist) 
		sum += n;

	for (auto& n : dist) 
		n = n / sum;

	return dist;
}

int main(int argc, const char** argv)
{
	if(argc != 5)
		throw ArgumentException("Please provide four arguments: [n_vertices] [m] [n_runs] [csv_file].");

	const std::size_t n_vertices = std::stoul(argv[1]);
	const std::size_t m = std::stoul(argv[2]);
	const std::size_t n_runs = std::stoul(argv[3]);
	const std::string out_file_path = argv[4];

	std::ofstream csv_file(out_file_path);

	csv_file << "model,n,p" << std::endl;

	std::random_device rd;
	std::mt19937 mt(rd());

	// preferential attachment
	{
		std::vector<std::vector<std::size_t>> dists;
		for(std::size_t i_run = 0; i_run < n_runs; i_run++)
		{
			Graph g;
			generate_iterative_attachment_directed(g, n_vertices, m, 1.0, mt);

			dists.push_back(compute_degree_distribution(g));
		}

		auto dist = avg_dist_from_runs(n_vertices,dists);

		for(std::size_t n = 0; n < dist.size(); n++)
		{
			csv_file << "PA," << n << "," << dist[n] << std::endl;
		}
	}

	// uniform attachment
	{
		std::vector<std::vector<std::size_t>> dists;
		for(std::size_t i_run = 0; i_run < n_runs; i_run++)
		{
			Graph g;
			generate_iterative_attachment_directed(g, n_vertices, m, 0.0, mt);

			dists.push_back(compute_degree_distribution(g));
		}

		auto dist = avg_dist_from_runs(n_vertices,dists);

		for(std::size_t n = 0; n < dist.size(); n++)
		{
			csv_file << "UA," << n << "," << dist[n] << std::endl;
		}
	}

	// inverse preferential attachment
	{
		std::vector<std::vector<std::size_t>> dists;
		for(std::size_t i_run = 0; i_run < n_runs; i_run++)
		{
			Graph g;
			generate_iterative_attachment_directed(g, n_vertices, m, -1.0, mt);

			dists.push_back(compute_degree_distribution(g));
		}

		auto dist = avg_dist_from_runs(n_vertices,dists);

		for(std::size_t n = 0; n < dist.size(); n++)
		{
			csv_file << "IPA," << n << "," << dist[n] << std::endl;
		}
	}
}
