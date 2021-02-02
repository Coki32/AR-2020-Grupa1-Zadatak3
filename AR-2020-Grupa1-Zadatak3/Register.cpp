#include <iomanip>
#include <string>
#include <sstream>
#include "Register.h"
#include "util.h"

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

std::string Register::asHex(bool noname)
{
    return noname ? toHexString(full) : std::string("[").append(name).append("]: ").append(toHexString(full));
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
        break;
    case RegisterSize::Half:
        half = (uint32_t)value;
        break;
    case RegisterSize::Quarter:
        quarter = (uint16_t)value;
        break;
    case RegisterSize::Byte:
        byte = (uint8_t)value;
        break;
    }

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

int RegisterSizeToBits(RegisterSize rsize)
{
    switch (rsize)
    {
    case RegisterSize::Full:
        return 64;
    case RegisterSize::Half:
        return 32;
    case RegisterSize::Quarter:
        return 16;
    case RegisterSize::Byte:
        return 8;
    default:
        return 64;//AAAAKO ikad ikako dodje ovjd,e a nece
    }
}
