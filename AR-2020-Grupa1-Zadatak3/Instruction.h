#pragma once
#include <string>
#include <memory>
#include "Flags.h"
#include <map>
#include <functional>
#include "Register.h"

class CPU;

enum class OperandType { Register, DirectAddress, IndirectAddress, StringConstant, IntegerConstant, Missing };
const char* opTypeToString(OperandType ot);

enum class InstructionType { Regular, Jump, Extension, Label, Breakpoint };
const char* instructionTypeToString(InstructionType it);

enum class JumpType { ToLine, ToLabel };
const char* jumpTypeToString(JumpType jt);

struct OperandInfo {
	OperandType type;
	std::string stringValue;
	uint64_t intValue;
};

class Instruction
{
private:
	std::string name;

	InstructionType instructionType;
	OperandType		destType, srcType;
	JumpType		jumpType;

	uint64_t destIntValue, srcIntValue;
	std::string destStringValue, srcStringValue;

	int jumpLineIndex;
	std::string jumpLabel;

	int lineNumber;

	CPU& cpu;
public:

	Instruction(const std::string& line, CPU& cpu, int lineNumber);

	void execute();
	bool isBreakpoint() const { return instructionType == InstructionType::Breakpoint; }
	const std::string& getName() const { return name; }

private:

	int getDestSize();
	int getSrcSize();

	uint64_t getDestValue();

	uint64_t getSrcValue();
	
	
	//Poenta ovoga je da se, pri citanju iz memorije, procita onoliko bajtova koliki je registar 
	uint64_t getFromMemoryByRegisterSize(const Register& reg, int address);

	void writeToDest(uint64_t value);

	/// <summary>
	/// Upisuje value u memoriju negdje
	/// </summary>
	/// <param name="reg">Registar koji se koristi za odredjivanje velicine bloka</param>
	/// <param name="address">Adresa u memoriji GDJE se upisuje</param>
	/// <param name="value">Vrijednost koja se upisuje na tu adresu u memoriji</param>
	void writeToMemoryByRegisterSize(const Register& reg, int address, uint64_t value);

	OperandInfo skontajStaJeOperandTipITo(const std::string& operand);
	void assignInfo(OperandInfo&& info, OperandType& type, uint64_t& intValue, std::string& stringValue);

	friend std::ostream& operator<<(std::ostream& os, const Instruction& i);
};
