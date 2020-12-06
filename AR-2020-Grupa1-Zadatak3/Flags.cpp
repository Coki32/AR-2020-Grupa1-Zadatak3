#include "Flags.h"

std::string Flags::asHex() const
{
	return std::string("[FLAGS]: ").append(std::to_string(all));
}
