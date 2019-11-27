#pragma once 

#include <chrono>
#include <string>

using Duration = std::chrono::duration<double,std::milli>;
using Timepoint = std::chrono::time_point<std::chrono::steady_clock>;

Timepoint get_timestamp();
std::string duration_to_readable(Duration duration);
