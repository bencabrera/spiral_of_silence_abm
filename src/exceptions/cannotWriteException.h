#pragma once

#include <stdexcept>

#include "exception.h"

struct CannotWriteException : public Exception
{
	using Exception::Exception;
};
