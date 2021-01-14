#include <string>
#include <ostream>
#include <stdexcept>
#include "Instruction.h"
#include "util.h"
#include "CPU.h"

Instruction::Instruction(const std::string& line, CPU& cpu) : cpu(cpu)
{
	auto parts = splitString(trimCopy(removeAfterCopy(line,'#')));//[0] = instrukcija, [1] = dst, [2] = src/const+komentar
	if (parts.size() <= 1)
		throw std::invalid_argument("Neispravna instrukcija potpuno");
	name = trim(removeAfter(parts[0],'#'));
	auto dest = trim(parts[1]);
	if (name[0] == 'J') {//znaci da je neki skok i ima jedan argument
		instructionType = InstructionType::Jump;
		try {
			jumpLineIndex = std::stoi(dest);
			jumpType = JumpType::ToLine;
		}
		catch (std::exception& ex) {
			jumpLabel = dest;
			jumpType = JumpType::ToLabel;
		}
	}
	auto destInfo = skontajStaJeOperandTipITo(dest);
	assignInfo(destInfo, this->op1Type, this->destIntValue, this->destStringValue);
	if (instructionType == InstructionType::Regular && (op1Type == OperandType::IntegerConstant || op1Type == OperandType::StringConstant))
		throw std::invalid_argument("Ne moze ti konstanta biti destination... nema toga");
	if (parts.size() >= 3) {
		auto srcInfo = skontajStaJeOperandTipITo(trim(parts[2]));
		assignInfo(srcInfo, this->op2Type, this->srcIntValue, this->srcStringValue);
	}

}

Flags Instruction::execute()
{
	return Flags();
}

Instruction::OperandInfo Instruction::skontajStaJeOperandTipITo(const std::string& operand)
{
	OperandInfo info;
	auto dest = std::string(operand);//ladno mi se ne da da mijenjam svuda u kopiranom bloku, i jos je const...
	if (dest[0] == '[' && dest[dest.length() - 1] == ']')// memorija je
	{
		dest = dest.substr(1, dest.length() - 2);//sredi odmah
		try {
			info.intValue = std::stoi(dest);
			info.type = OperandType::DirectAddress;
		}
		catch (std::exception& ex) {//znaci nije int unutar zagrada, provjeri da li je registar
			if (!isRegister(dest, cpu.getRegisterCount()))
				throw std::invalid_argument("Neispravno ime registra!");
			info.type = OperandType::IndirectAddress;
			info.stringValue = dest;
		}
	}
	else if (isRegister(dest, cpu.getRegisterCount()))//prvi je registar, ok
	{
		info.type = OperandType::Register;
		info.stringValue = dest;
	}
	else {//nije memorija... nije registar... znaci da mora biti nekakva konstanta. E sad zbog ispisa mora i konstanta moci da bude string...
		try {
			info.intValue = std::stoi(dest);
			info.type = OperandType::IntegerConstant;
		}
		catch (std::exception& ex) {//nije broj ocito
			info.stringValue = dest;
			info.type = OperandType::StringConstant;
		}
	}
	return info;
}

void Instruction::assignInfo(Instruction::OperandInfo info, Instruction::OperandType& type, int& intValue, std::string& stringValue)
{
	type = info.type;
	switch (type) {
	case OperandType::StringConstant:
		stringValue = info.stringValue;
		break;
	case OperandType::IntegerConstant:
		intValue = info.intValue;
		break;
	case OperandType::DirectAddress:
		intValue = info.intValue;
		break;
	case OperandType::IndirectAddress:
		break;
		stringValue = info.stringValue;
	case OperandType::Register:
		stringValue = info.stringValue;
		break;
	}
}


std::ostream& operator<<(std::ostream& os, const Instruction& i)
{
	return os << "[" << i.name << "](" << (int)i.instructionType << ") iz (" << (int)i.op1Type << ") [" << i.destIntValue << ", " << i.destStringValue << "]  u (" << (int)i.op2Type << ")[" << i.srcIntValue << ", " << i.srcStringValue << "]";
}
