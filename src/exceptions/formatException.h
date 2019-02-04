#pragma once

#include <stdexcept>

#include "exception.h"

struct FormatException : public Exception
{
	using Exception::Exception;
};
