#pragma once

#include <random>
#include <string>
#include <chrono>

std::pair<std::string, std::vector<std::string>> parse_method_call(std::string str);
double draw_from_distribution(const std::string param, std::mt19937& mt);
std::vector<std::vector<double>> parse_matrix(std::string str);

