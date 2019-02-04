#pragma once

#include <stdexcept>

#include "exception.h"

struct InternalException : public Exception
{
	using Exception::Exception;
};
