#pragma once
#include <cstdint>
#include <string>

#include "IReadWritable.h"

enum class RegisterSize { Full, Half, Quarter, Byte };

class Register : public virtual IReadWritable
{
protected:
	union
	{
		uint64_t full;
		uint32_t half;
		uint16_t quarter;
		uint8_t  byte;
	};
	std::string name;
	RegisterSize size;
public:
	Register(const std::string& name) noexcept;
	Register(const Register& other);
	Register(Register&& other);

	Register& operator=(const Register& other);
	Register& operator=(Register&& other);

	std::string asHex();

	RegisterSize getSize() const { return size; }
	RegisterSize getSize() { return size; }


	uint64_t getBySize() const;
	void setBySize(uint64_t value);

	// Inherited via IReadWritable
	virtual uint64_t getAsFull() const override;
	virtual uint32_t getAsHalf() const override;
	virtual uint16_t getAsQuarter() const override;
	virtual uint8_t getAsByte() const override;

	virtual void setAsFull(uint64_t value) override;
	virtual void setAsHalf(uint32_t value) override;
	virtual void setAsQuarter(uint16_t value) override;
	virtual void setAsByte(uint8_t value) override;

private:
	void moveOther(Register&& other);
	void copyOther(const Register& other);
};

