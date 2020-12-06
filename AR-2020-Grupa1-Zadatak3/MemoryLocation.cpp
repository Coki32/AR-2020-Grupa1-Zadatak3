#include "MemoryLocation.h"

MemoryLocation& MemoryLocation::operator=(const MemoryLocation& other) {
	if (&other == this)
		return *this;
	value = other.value;
	return *this;
}

MemoryLocation& MemoryLocation::operator=(const Register& reg) {
	value = reg.getAsFull();
	return *this;
}

#pragma region spam read/write metoda
uint64_t MemoryLocation::getAsFull() const
{
	return value;
}

uint32_t MemoryLocation::getAsHalf() const
{
	return (uint32_t)value;
}

uint16_t MemoryLocation::getAsQuarter() const
{
	return (uint16_t)value;
}

uint8_t MemoryLocation::getAsByte() const
{
	return (uint8_t)value;
}

void MemoryLocation::setAsFull(uint64_t value)
{
	this->value = value;
}

void MemoryLocation::setAsHalf(uint32_t value)
{
	this->value = (uint64_t)value;
}

void MemoryLocation::setAsQuarter(uint16_t value)
{
	this->value = (uint64_t)value;
}

void MemoryLocation::setAsByte(uint8_t value)
{
	this->value = (uint64_t)value;
}
#pragma endregion