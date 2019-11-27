#include "cliHelpers.h"

#include <iomanip>

Timepoint get_timestamp() 
{
	return std::chrono::steady_clock::now();
}

std::string duration_to_readable(Duration duration)
{
	std::stringstream ss;

	std::size_t milliseconds = duration.count();

	auto hours = (milliseconds / (1000*60*60));
	auto mins =  (milliseconds % (1000*60*60)) / (1000*60);
	auto seconds =  ((milliseconds % (1000*60*60)) % (1000*60)) / 1000;
	auto milli =  (((milliseconds % (1000*60*60)) % (1000*60)) % 1000);
	ss << hours << "h " << std::setw(2) << mins << "m " << std::setw(2) << seconds << "s " << std::setw(4) << milli << "ms";

	return ss.str();
}
