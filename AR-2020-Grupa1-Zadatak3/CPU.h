#pragma once
#include <memory>
#include <exception>
#include <stack>

#include "Register.h"
#include "MemoryLocation.h"
class CPU
{
	int numberOfRegisters;
	std::unique_ptr<std::unique_ptr<Register>[]> registers;
	std::unique_ptr<MemoryLocation[]> heap;
	std::stack<MemoryLocation> stack;

public:
	CPU(int numberOfRegisters);

	bool executeFile(const std::string& filePath);
	bool executeFile(const std::ifstream& file);

	void printState();

private:
	

};

