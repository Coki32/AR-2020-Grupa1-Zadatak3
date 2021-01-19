#include "Register.h"

Register::Register(const std::string& name) noexcept :full(0), name(name)
{
    int length = name.length();
    if (length == 2) {
        size = name[1] == 'x' ? RegisterSize::Quarter : RegisterSize::Byte;
    }
    else
        size = name[0] == 'r' ? RegisterSize::Full : RegisterSize::Half;
}

Register::Register(const Register& other)
{
    copyOther(other);
}

Register::Register(Register&& other)
{
    moveOther(std::move(other));
}

Register& Register::operator=(const Register& other)
{
    copyOther(other);
    return *this;
}

Register& Register::operator=(Register&& other)
{
    moveOther(std::move(other));
    return *this;
}

std::string Register::asHex()
{
    return std::string("[").append(name).append("]: ").append(std::to_string(full));
}

#pragma region Smece get/set as nesto
uint64_t Register::getBySize() const
{
    switch (size)
    {
    case RegisterSize::Full:
        return full;
        break;
    case RegisterSize::Half:
        return half;
        break;
    case RegisterSize::Quarter:
        return quarter;
        break;
    case RegisterSize::Byte:
        return byte;
        break;
    default:
        return 0;
        break;
    }
}
void Register::setBySize(uint64_t value)
{
    switch (size)
    {
    case RegisterSize::Full:
        full = value;
        //return false;
        break;
    case RegisterSize::Half:
        half = (uint32_t)value;
        //return value>UINT32_MAX
        break;
    case RegisterSize::Quarter:
        quarter = (uint16_t)value;
        break;
    case RegisterSize::Byte:
        byte = (uint8_t)value;
        break;
    //nema defaulta
    }

}
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
void Register::moveOther(Register&& other)
{
    if (this != &other) {
        name = std::move(other.name);
        full = std::move(other.full);
        size = std::move(other.size);
    }
}
void Register::copyOther(const Register& other)
{
    if (this != &other) {
        name = other.name;
        full = other.full;
        size = other.size;
    }
}
#pragma endregion