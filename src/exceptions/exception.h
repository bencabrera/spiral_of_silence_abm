#pragma once

#include <stdexcept>

struct Exception : public std::runtime_error
{
	using std::runtime_error::runtime_error;
};
