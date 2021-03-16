#pragma once
#include <memory>
#include <exception>
#include <stack>
#include <vector>

#include "Register.h"
#include "Instruction.h"
#include <map>

struct CpuOptions {
	int numberOfRegisters;
	int heapSize;
	bool debug;
	bool avanturista;//srecno
	bool printsHex;
	CpuOptions(int numberOfRegisters,int heapSize, bool debug, bool avanturista, bool printsHex) :
		numberOfRegisters(numberOfRegisters), heapSize(heapSize),
		debug(debug), avanturista(avanturista), printsHex(printsHex) {}
	CpuOptions() :CpuOptions(8,1024, false, false, false) {}
};

class CPU
{
	int numberOfRegisters;
	bool debug;
	bool avanturista;
	bool printsHex;
	std::map<std::string, Register> registers;
	std::vector<uint8_t> heap;
	std::stack<uint64_t> stack;

	Flags flags;

	int currentInstruction = 0;
	std::vector<Instruction> instructions;
	std::vector<std::string> lines;
public:
	CPU(CpuOptions options);

	bool readFile(const std::string& filePath);
	bool readFile(std::fstream& file);

	void printState();

	inline int getHeapSize() const { return heap.size(); };
	inline int getHeapSize() { return heap.size(); };

	inline int getRegisterCount() const { return numberOfRegisters; };
	inline int getRegisterCount()		{ return numberOfRegisters; };

	inline Flags& getFlags() { return flags; }
	inline const Flags& getFlags() const { return flags; }//zlu ne trebalo

	inline bool isHexOutput() { return printsHex; }
	inline const bool isHexOutput() const { return printsHex; }

	void jumpToLine(int line);
	void jumpToLabel(const std::string& label);

	Register& getRegister(const std::string& name);

	void printInstructions();

	void run();

	void showDebugMenu(Instruction& instruction, bool& shouldPause);
public:

	void memoryDebugMenu();
	
	template<typename T>
	void writeToMemory(int address, T value);

	template<typename T>
	T readFromMemory(int address);

	uint64_t readFromMemoryByRegisterSize(int address, const Register& reg);

};

template<typename T>
inline void CPU::writeToMemory(int address, T value)
{
	if (address + sizeof(T) > heap.size() || address < 0)
		throw new std::out_of_range("Pristup van memorije!");
	for (int i = 0; i < sizeof(T); ++i)
		heap[address + i] = (uint8_t)(value >> (i * 8));
}

template<typename T>
inline T CPU::readFromMemory(int address)
{
	T result = 0;
	for (int i = 0; i < sizeof(T); ++i)
		result += heap[address + i] << (i * 8);
	return result;
}
