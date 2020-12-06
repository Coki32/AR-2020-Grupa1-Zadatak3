#include <string>
#include <iostream>
#include <fstream>

#include "CPU.h"


CPU::CPU(int numberOfRegisters) {
	if (numberOfRegisters < 1 || numberOfRegisters>26)
		throw std::exception("Number of registers must be an integer in range [1, 26]");
	this->numberOfRegisters = numberOfRegisters;
	registers = std::make_unique<std::unique_ptr<Register>[]>(numberOfRegisters);
	for (int i = 0; i < numberOfRegisters; ++i)
		registers[i] = std::make_unique<Register>(std::string("r").append(1, 'a' + i).append("x"));
}

bool CPU::executeFile(const std::string& filePath)
{
	std::ifstream ifile;
	ifile.open(filePath, std::ios::in);
	if (!ifile.is_open())
		return false;
	else {
		bool ret = executeFile(ifile);
		ifile.close();
		return ret;
	}
}

bool CPU::executeFile(const std::ifstream& file)
{
	return false;
}

void CPU::printState()
{
	for (int i = 0; i < numberOfRegisters; ++i) {
		std::cout << registers[i]->asHex() << std::endl;
	}
}
