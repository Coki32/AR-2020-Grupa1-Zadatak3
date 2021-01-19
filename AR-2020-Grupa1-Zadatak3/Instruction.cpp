#include <string>
#include <ostream>
#include <stdexcept>
#include "Instruction.h"
#include "util.h"
#include "CPU.h"

//obrisi ovo
#include <iostream>

Instruction::Instruction(const std::string& line, CPU& cpu) : cpu(cpu)
{
	auto parts = splitString(trimCopy(removeAfterCopy(line,'#')));//[0] = instrukcija, [1] = dst, [2] = src/const+komentar
	if (parts.size() <= 1)
		throw std::invalid_argument("Neispravna instrukcija potpuno");
	name = trim(removeAfter(parts[0],'#'));
	auto dest = trim(parts[1]);
	instructionType = InstructionType::Regular;//zasad pretpostavi ovo, kasnije cemo srediti ako bude skok ili ekstenzija
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
	assignInfo(std::move(destInfo), this->destType, this->destIntValue, this->destStringValue);
	if (instructionType == InstructionType::Regular && (destType == OperandType::IntegerConstant || destType == OperandType::StringConstant))//Ne moze u konstante nista
		throw std::invalid_argument("Ne moze ti konstanta biti destination... nema toga");
	if (parts.size() >= 3) {
		auto srcInfo = skontajStaJeOperandTipITo(trim(parts[2]));
		assignInfo(std::move(srcInfo), this->srcType, this->srcIntValue, this->srcStringValue);
	}

}

static inline uint64_t countSetBits(uint64_t value){
	uint64_t result = 0;
	do
		result += value & 1;
	while (value >>= 1);
	return result;
};

Flags Instruction::execute()
{
	using MapType = std::map<std::string, std::function<void(CPU&)>>;
	const MapType microcode = MapType(
	{
		{"add", [this](CPU& c){
			if (destType != OperandType::Register)//mora samo u registre da sabira
				throw std::exception("Ne mozes sacuvati rezultat operacije direktno u adresu!");
			Register& dest = cpu.getRegister(destStringValue);//treba za upis kasnije
			uint64_t _destValue = getDestValue();
			uint64_t _srcValue = getSrcValue();
			uint64_t newValue = _destValue + _srcValue;
			dest.setBySize(newValue);
			auto& flags = c.getFlags();
			flags.OF = (newValue < _destValue) || (newValue < _srcValue);
			flags.PF = countSetBits(newValue) % 2 == 0;
			flags.ZF = newValue == 0;
			flags.SF = (newValue >> 63) & 1;
		}},
		{"sub", [this](CPU& c){
			
		}},
		{"mov", [this](CPU& c) {
			if (destType == OperandType::IntegerConstant || destType == OperandType::StringConstant)
				throw std::exception("Ma kako ti to nesto pomjeras u konstantu...");
			if (destType == OperandType::Register) {
				Register& dest = cpu.getRegister(destStringValue);
				dest.setBySize(getSrcValue());
			}
			else if (destType == OperandType::IndirectAddress) {//memorija je
				Register& dest = cpu.getRegister(destStringValue);
				if (srcType == OperandType::Register)
					writeToMemoryByRegisterSize(cpu.getRegister(srcStringValue), dest.getBySize(), cpu.getRegister(srcStringValue).getBySize());
				
			}
		}}
	});
	if (microcode.find(name) != microcode.end())
		microcode.at(name)(cpu);
	return Flags();
}

uint64_t Instruction::getDestValue()
{
	switch (destType)
	{
	case OperandType::Register:
		return cpu.getRegister(destStringValue).getBySize();
		break;
	case OperandType::DirectAddress:
		throw std::exception("Ne mozes GET na dest ako je dest memorija, samo SET!");
		break;
	case OperandType::IndirectAddress:
		throw std::exception("Ne mozes GET na dest ako je dest memorija, samo SET!");
		break;
	case OperandType::StringConstant:
		throw std::exception("Kako si uopste dosao do ovoga!?");//trebalo bi da execute() pukne prije nego sto dodje ovdje
		break;
	case OperandType::IntegerConstant:
		throw std::exception("Kako si uopste dosao do ovoga!?");//trebalo bi da execute() pukne prije nego sto dodje ovdje
		break;
	default:
		return 0;
		break;
	}
}

uint64_t Instruction::getSrcValue()
{
	switch (srcType)
	{
	case OperandType::Register:
		return cpu.getRegister(srcStringValue).getBySize();
		break;
	case OperandType::DirectAddress:
		return getFromMemoryByRegisterSize(cpu.getRegister(destStringValue), srcIntValue);
		break;
	case OperandType::IndirectAddress: {
		auto& reg = cpu.getRegister(srcStringValue);
		return getFromMemoryByRegisterSize(reg, reg.getBySize());
	}
	case OperandType::StringConstant:
		throw std::exception("Ma za koju operaciju si vidio da je string konstanta izvor... nema manipulacije stringovima");
		break;
	case OperandType::IntegerConstant:
		return srcIntValue;
		break;
	default:
		return 0;
		break;
	}
}

uint64_t Instruction::getFromMemoryByRegisterSize(const Register& reg, int address)
{
	switch (reg.getSize())
	{
	case RegisterSize::Full:
		return cpu.readFromMemory<uint64_t>(address);
		break;
	case RegisterSize::Half:
		return cpu.readFromMemory<uint32_t>(address);
		break;
	case RegisterSize::Quarter:
		return cpu.readFromMemory<uint16_t>(address);
		break;
	case RegisterSize::Byte:
		return cpu.readFromMemory<uint8_t>(address);
		break;
	default:
		return 0;
		break;
	}
}

void Instruction::writeToMemoryByRegisterSize(const Register& reg, int address, uint64_t value)
{
	switch (reg.getSize())
	{
	case RegisterSize::Full:
		cpu.writeToMemory(address, value);
		break;
	case RegisterSize::Half:
		cpu.writeToMemory(address, (uint32_t)value);
		break;
	case RegisterSize::Quarter:
		cpu.writeToMemory(address, (uint16_t)value);
		break;
	case RegisterSize::Byte:
		cpu.writeToMemory(address, (uint8_t)value);
		break;
	//nema ni defaulta, ako ne znas sta ces - nemoj nista
	}
}

OperandInfo Instruction::skontajStaJeOperandTipITo(const std::string& operand)
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
			info.stringValue = std::move(dest);
		}
	}
	else if (isRegister(dest, cpu.getRegisterCount()))//prvi je registar, ok
	{
		info.type = OperandType::Register;
		info.stringValue = std::move(dest);
	}
	else {//nije memorija... nije registar... znaci da mora biti nekakva konstanta. E sad zbog ispisa mora i konstanta moci da bude string...
		try {
			info.intValue = std::stoi(dest);
			info.type = OperandType::IntegerConstant;
		}
		catch (std::exception& ex) {//nije broj ocito
			info.stringValue = std::move(dest);
			info.type = OperandType::StringConstant;
		}
	}
	return info;
}

void Instruction::assignInfo(OperandInfo&& info, OperandType& type, int& intValue, std::string& stringValue)
{
	type = info.type;
	switch (type) {
	case OperandType::StringConstant:
		stringValue = std::move(info.stringValue);
		break;
	case OperandType::IntegerConstant:
		intValue = info.intValue;
		break;
	case OperandType::DirectAddress:
		intValue = info.intValue;
		break;
	case OperandType::IndirectAddress:
		stringValue = std::move(info.stringValue);
		break;
	case OperandType::Register:
		stringValue = std::move(info.stringValue);
		break;
	}
}

const char* opTypeToString(OperandType ot) {
	switch (ot)
	{
	case OperandType::Register:
		return "registar";
		break;
	case OperandType::DirectAddress:
		return "direktna memorija";
		break;
	case OperandType::IndirectAddress:
		return "indirektna memorija";
		break;
	case OperandType::StringConstant:
		return "string konstanta";
		break;
	case OperandType::IntegerConstant:
		return "int konstanta";
		break;
	default:
		return "S T A  I  K A K O";
		break;
	}
}

const char* instructionTypeToString(InstructionType it)
{
	switch (it)
	{
	case InstructionType::Regular:
		return "obicna";
		break;
	case InstructionType::Jump:
		return "skok";
		break;
	case InstructionType::Extension:
		return "ekstenzija";
		break;
	default:
		return "vrlo vjerovatno nemoguce";
		break;
	}
}

const char* jumpTypeToString(JumpType jt)
{
	switch (jt)
	{
	case JumpType::ToLine:
		return "na liniju";
		break;
	case JumpType::ToLabel:
		return "na labelu";
		break;
	default:
		return "nema sanse";
		break;
	}
}

std::ostream& operator<<(std::ostream& os, const Instruction& i)
{
	return os << "[" << i.name << "](" << instructionTypeToString(i.instructionType) << ") u"
		<< " (" << opTypeToString(i.destType) << ") [" << i.destIntValue << ", " << i.destStringValue << "]  "
		<< "iz (" << opTypeToString(i.srcType) << ")[" << i.srcIntValue << ", " << i.srcStringValue << "]";
}