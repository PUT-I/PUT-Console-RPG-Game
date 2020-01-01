#pragma once

#include <exception>

class resource_exception final : public std::exception
{
public:
	resource_exception() noexcept : std::exception()
	{
	}

	explicit resource_exception(char const* const message) noexcept : std::exception(message)
	{
	}
};
