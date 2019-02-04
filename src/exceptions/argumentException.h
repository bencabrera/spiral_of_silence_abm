#pragma once

#include <stdexcept>

#include "exception.h"

struct ArgumentException : public Exception
{
	using Exception::Exception;
};
