#pragma once

#include <exception>

class resource_exception final : public std::exception
{
public:
	resource_exception() : std::exception()
	{
	}

	explicit resource_exception(char const* const message) : std::exception(message)
	{
	}
};
