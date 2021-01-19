#include <string>
#include <iostream>
#include <fstream>

#include "CPU.h"


CPU::CPU(int numberOfRegisters) {
	if (numberOfRegisters < 1 || numberOfRegisters>26)
		throw std::exception("Number of registers must be an integer in range [1, 26]");
	this->numberOfRegisters = numberOfRegisters;

	for (int i = 0; i < numberOfRegisters; ++i) {
		auto name = std::string("r").append(1, 'a' + i).append("x");
		registers.emplace(name,Register(std::string("r").append(1, 'a' + i).append("x")));
	}
	heap = std::vector<uint8_t>(CPU::HEAP_SIZE);
}

bool CPU::readFile(const std::string& filePath)
{
	std::fstream file;
	file.open(filePath, std::ios::in);
	if (!file.is_open())
		return false;
	else {
		bool ret = readFile(file);
		file.close();
		return ret;
	}
}

bool CPU::readFile(std::fstream& file)
{
	if (!file.is_open() || !(file.flags() & std::ios::in))
		return false;
	std::string line;
	int i = 0;
	while (std::getline(file, line)) {
		lines.push_back(line);
		instructions.emplace_back(line,*this);
	}
	return true;
}

void CPU::printState()
{
	for (auto it = registers.begin(); it != registers.end(); ++it) {
		std::cout << it->second.asHex() << std::endl;
	}
}

Register& CPU::getRegister(const std::string& name)
{
	return registers.at(name);
}

void CPU::printInstructions()
{
	for (int i = 0; i < lines.size(); ++i)
		std::cout << "\"" << lines[i] << "\" => " << instructions[i] << std::endl;
}

void CPU::run()
{
	for (currentInstruction = 0; currentInstruction < instructions.size(); ++currentInstruction)
		instructions[currentInstruction].execute();
}
