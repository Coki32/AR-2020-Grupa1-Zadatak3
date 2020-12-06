#pragma once
#include <cstdint>

#include "Register.h"

struct MemoryLocation : public virtual IReadWritable
{
	uint64_t value;

	MemoryLocation() : MemoryLocation(0) {}
	MemoryLocation(uint64_t value) :value(value) {}
	MemoryLocation(const Register& reg) : value(reg.getAsFull()) {}
	MemoryLocation(const MemoryLocation& other) : value(other.value) {}
	MemoryLocation(MemoryLocation&& other) = delete;

	MemoryLocation& operator=(const MemoryLocation& other);
	MemoryLocation& operator=(const Register& reg);

	// Inherited via IReadWritable
	virtual uint64_t getAsFull() const override;
	virtual uint32_t getAsHalf() const override;
	virtual uint16_t getAsQuarter() const override;
	virtual uint8_t getAsByte() const override;
	virtual void setAsFull(uint64_t value) override;
	virtual void setAsHalf(uint32_t value) override;
	virtual void setAsQuarter(uint16_t value) override;
	virtual void setAsByte(uint8_t value) override;
};

