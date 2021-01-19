#pragma once
#include <memory>
#include <exception>
#include <stack>
#include <vector>

#include "Register.h"
#include "Instruction.h"

#include <map>
class CPU
{
	static const int HEAP_SIZE = 32;
	int numberOfRegisters;
	std::map<std::string, Register> registers;
	std::vector<uint8_t> heap;
	std::stack<uint64_t> stack;

	Flags flags;

	int currentInstruction = 0;
	std::vector<Instruction> instructions;
	std::vector<std::string> lines;
public:
	CPU(int numberOfRegisters = 8);

	/*
	* Read the input file and store instructions locally before execution
	*/
	bool readFile(const std::string& filePath);
	bool readFile(std::fstream& file);

	void printState();

	inline int getHeapSize() const { return heap.size(); };
	inline int getHeapSize() { return heap.size(); };

	inline int getRegisterCount() const { return numberOfRegisters; };
	inline int getRegisterCount()		{ return numberOfRegisters; };

	inline Flags& getFlags() { return flags; }
	inline const Flags& getFlags() const { return flags; }//zlu ne trebalo

	Register& getRegister(const std::string& name);

	void printInstructions();

	void run();

public:
	
	template<typename T>
	void writeToMemory(int address, T value);

	template<typename T>
	T readFromMemory(int address);

};

template<typename T>
inline void CPU::writeToMemory(int address, T value)
{
	if (address + sizeof(T) > heap.size() || address < 0)
		throw new std::out_of_range("Pristup van memorije!");
	for (int i = 0; i < sizeof(T); ++i)
		heap[address + i] = (value & (0xFF << i * 8)) >> (i * 8);
}

template<typename T>
inline T CPU::readFromMemory(int address)
{
	T result = 0;
	for (int i = 0; i < sizeof(T); ++i)
		result += heap[address + i] << (i * 8);
	return result;
}
