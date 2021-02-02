#pragma once
#include <cstdint>
#include <string>

enum class RegisterSize { Full, Half, Quarter, Byte };

int RegisterSizeToBits(RegisterSize rsize);

class Register
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

	std::string asHex(bool noname = true);

	RegisterSize getSize() const { return size; }
	RegisterSize getSize() { return size; }


	uint64_t getBySize() const;
	void setBySize(uint64_t value);

private:
	void moveOther(Register&& other);
	void copyOther(const Register& other);
};

