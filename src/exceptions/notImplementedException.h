#pragma once

#include <stdexcept>

#include "exception.h"

struct NotImplementedException : public Exception
{
	using Exception::Exception;
};
