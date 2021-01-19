#pragma once
#include <string>
#include <memory>
#include "Flags.h"
#include <map>
#include <functional>
#include "Register.h"

class CPU;

enum class OperandType { Register, DirectAddress, IndirectAddress, StringConstant, IntegerConstant };
const char* opTypeToString(OperandType ot);

enum class InstructionType { Regular, Jump, Extension };
const char* instructionTypeToString(InstructionType it);

enum class JumpType { ToLine, ToLabel };
const char* jumpTypeToString(JumpType jt);

struct OperandInfo {
	OperandType type;
	std::string stringValue;
	int intValue;
};

class Instruction
{
private:
	std::string name;

	InstructionType instructionType;
	OperandType		destType, srcType;
	JumpType		jumpType;

	int destIntValue, srcIntValue;
	std::string destStringValue, srcStringValue;

	int jumpLineIndex;
	std::string jumpLabel;


	CPU& cpu;
public:

	Instruction(const std::string& line, CPU& cpu);

	Flags execute();

private:

	uint64_t getDestValue();

	uint64_t getSrcValue();
	
	
	//Poenta ovoga je da se, pri citanju iz memorije, procita onoliko bajtova koliki je registar 
	uint64_t getFromMemoryByRegisterSize(const Register& reg, int address);

	//i jos ova muka da se u memoriju upise onoliko koliki je registar stvarno
	void writeToMemoryByRegisterSize(const Register& reg, int address, uint64_t value);

	OperandInfo skontajStaJeOperandTipITo(const std::string& operand);
	void assignInfo(OperandInfo&& info, OperandType& type, int& intValue, std::string& stringValue);

	friend std::ostream& operator<<(std::ostream& os, const Instruction& i);
};
