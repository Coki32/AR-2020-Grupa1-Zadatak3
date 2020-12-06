#pragma once
#include <cstdint>

struct IWritable
{
	virtual void setAsFull(uint64_t value) = 0;
	virtual void setAsHalf(uint32_t value) = 0;
	virtual void setAsQuarter(uint16_t value) = 0;
	virtual void setAsByte(uint8_t value) = 0;
};

