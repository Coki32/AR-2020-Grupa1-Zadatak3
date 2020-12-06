#include "Register.h"

std::string Register::asHex()
{
    return std::string("[").append(name).append("]: ").append(std::to_string(full));
}

#pragma region Smece get/set as nesto
uint64_t Register::getAsFull() const
{
    return full;
}

uint32_t Register::getAsHalf() const
{
    return half;
}

uint16_t Register::getAsQuarter() const
{
    return quarter;
}

uint8_t Register::getAsByte() const
{
    return byte;
}

void Register::setAsFull(uint64_t value)
{
    full = value;
}

void Register::setAsHalf(uint32_t value)
{
    half = value;
}

void Register::setAsQuarter(uint16_t value)
{
    quarter = value;
}

void Register::setAsByte(uint8_t value)
{
    byte = value;
}
#pragma endregion