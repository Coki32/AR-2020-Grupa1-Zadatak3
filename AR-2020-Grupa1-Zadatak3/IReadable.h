#pragma once
#include <cstdint>

struct IReadable
{
	virtual uint64_t getAsFull() const = 0;
	virtual uint32_t getAsHalf() const = 0;
	virtual uint16_t getAsQuarter() const = 0;
	virtual uint8_t getAsByte() const = 0;
};

