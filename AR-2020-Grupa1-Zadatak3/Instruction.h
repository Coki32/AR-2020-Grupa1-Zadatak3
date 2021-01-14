#pragma once
#include <string>
#include <memory>
#include "Flags.h"
#include "IReadWritable.h"
#include <map>
#include "Register.h"
class CPU;

class Instruction
{
public:

	enum class OperandType		{ Register, DirectAddress, IndirectAddress, StringConstant, IntegerConstant };
	enum class InstructionType	{ Regular, Jump, Extension };
	enum class JumpType			{ ToLine, ToLabel };
	
private:

	struct OperandInfo {
		OperandType type;
		std::string stringValue;
		int intValue;
	};

	std::string name;

	InstructionType instructionType;
	OperandType		op1Type, op2Type;
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

	OperandInfo skontajStaJeOperandTipITo(const std::string& operand);
	void assignInfo(OperandInfo info, OperandType& type, int& intValue, std::string& stringValue);

	friend std::ostream& operator<<(std::ostream& os, const Instruction& i);
};
