#include <string>
#include <ostream>
#include <stdexcept>
#include "Instruction.h"
#include "util.h"
#include "CPU.h"

//obrisi ovo
#include <iostream>

Instruction::Instruction(const std::string& line, CPU& cpu, int lineNumber) : cpu(cpu),lineNumber(lineNumber)
{
	auto parts = splitString(trimCopy(removeAfterCopy(line,'#')));//[0] = instrukcija, [1] = dst, [2] = src/const+komentar
	if (parts.size() < 1)
		throw std::invalid_argument("Neispravna instrukcija potpuno");
	name = trim(removeAfter(parts[0],'#'));
	if (name[0] == '_') {
		this->instructionType = InstructionType::Label;
		return;//samo labela i cao
	}
	if (name[0] == '*') {
		this->instructionType = InstructionType::Breakpoint;
		return;
	}
	instructionType = InstructionType::Regular;//zasad pretpostavi ovo, kasnije cemo srediti ako bude skok ili ekstenzija
	if (parts.size() < 2)
		throw std::invalid_argument("Neispravna instrukcija, ocito mora imati jos barem jedan operand!");
	if (name[0] == 'J'|| name[0]=='j') {//znaci da je neki skok i ima jedan argument
		instructionType = InstructionType::Jump;
		try {
			jumpLineIndex = std::stoi(trimCopy(parts[1]));
			jumpType = JumpType::ToLine;
		}
		catch (std::exception& ex) {
			if (trimCopy(parts[1])[0] != '_')
				throw std::exception("SVE LABELE MORAJU POCINJATI SA znakom _ !");
			jumpLabel = trimCopy(parts[1]);
			jumpType = JumpType::ToLabel;
		}
		return;//gotovo, znas da je jump i znas gdje skace
	}
	if (name[0] == '$') {
		instructionType = InstructionType::Extension;
		name = name.substr(1);//kome trebaju dolari jos
		if (parts.size() > 1) {//i print i read mogu primiti samo jedan argument - ili const ili registar, nema ispisa iz memorije
			if (isRegister(trimCopy(parts[1]), cpu.getRegisterCount())) {
				destType = OperandType::Register;
				destStringValue = std::move(trimCopy(parts[1]));
			}
			else
			{
				destType = OperandType::StringConstant;
				destStringValue = parts[1];
			}
		}
		return;//nemoj da radis nista dalje
	}
	auto dest = trim(parts[1]);
	auto destInfo = skontajStaJeOperandTipITo(dest);
	assignInfo(std::move(destInfo), this->destType, this->destIntValue, this->destStringValue);
	if (instructionType == InstructionType::Regular && (destType == OperandType::IntegerConstant || destType == OperandType::StringConstant))//Ne moze u konstante nista
		throw std::invalid_argument("Ne moze ti konstanta biti destination... nema toga");
	if (parts.size() >= 3) {
		auto srcInfo = skontajStaJeOperandTipITo(trim(parts[2]));
		assignInfo(std::move(srcInfo), this->srcType, this->srcIntValue, this->srcStringValue);
	}
	else {
		srcType = OperandType::Missing;
		srcIntValue = 0;
		srcStringValue = "";
	}
}

static inline uint64_t countSetBits(uint64_t value){
	uint64_t result = 0;
	do
		result += value & 1;
	while (value >>= 1);
	return result;
};

void Instruction::execute()
{
	if (instructionType == InstructionType::Label || instructionType == InstructionType::Breakpoint)//to se ne izvrsava
		return;
	auto jump = [this](CPU& c) {
		switch (jumpType)
		{
		case JumpType::ToLine:
			c.jumpToLine(jumpLineIndex);
			break;
		case JumpType::ToLabel:
			c.jumpToLabel(jumpLabel);
			break;
		}
	};
	using MapType = std::map<std::string, std::function<void(CPU&)>>;
	const MapType microcode = MapType(
		{
			{"add", [this](CPU& c) {
				if (destType != OperandType::Register)//mora samo u registre da sabira
					throw std::exception("Ne mozes sacuvati rezultat operacije direktno u adresu!");
				Register& dest = cpu.getRegister(destStringValue);//treba za upis kasnije
				uint64_t _destValue = dest.getBySize();
				uint64_t _srcValue = getSrcValue();
				uint64_t newValue = _destValue + _srcValue;
				dest.setBySize(newValue);
				auto& flags = c.getFlags();
				flags.OF = (newValue < _destValue) || (newValue < _srcValue);//ako se omota okolo, zbir ce biti manji od barem jednog od ova dva
				flags.PF = countSetBits(newValue) % 2 == 0;
				flags.ZF = newValue == 0;
				flags.SF = (newValue >> (RegisterSizeToBits(dest.getSize()) - 1)) & 1;
			}},
			{"sub", [this](CPU& c) {
				if (destType != OperandType::Register)//mora samo iz registra da oduzima
					throw std::exception("Moras oduzimati od registra!");
				Register& dest = cpu.getRegister(destStringValue);
				uint64_t _destValue = dest.getBySize();
				uint64_t _srcValue = getSrcValue();
				uint64_t newValue = _destValue - _srcValue;
				dest.setBySize(newValue);
				auto& flags = c.getFlags();
				flags.ZF = newValue == 0;
				flags.SF = (newValue >> (RegisterSizeToBits(dest.getSize()) - 1)) & 1;
				flags.PF = countSetBits(newValue) % 2 == 0;
				flags.OF = _srcValue > _destValue;//!?!?!?!
			}},
			{"mov", [this](CPU& c) {
				if (destType == OperandType::IntegerConstant || destType == OperandType::StringConstant)
					throw std::exception("Ma kako ti to nesto pomjeras u konstantu...");
				if (destType == OperandType::Register) {
					Register& dest = cpu.getRegister(destStringValue);
					dest.setBySize(getSrcValue());
				}
				else if (destType == OperandType::IndirectAddress || destType == OperandType::DirectAddress) {//U nekakvu MEMORIJU
					int address = 0;
					if (destType == OperandType::IndirectAddress)
						address = cpu.getRegister(destStringValue).getBySize();
					else
						address = destIntValue;

					if (srcType == OperandType::Register)//IZ REGISTRA
						writeToMemoryByRegisterSize(cpu.getRegister(srcStringValue), address, cpu.getRegister(srcStringValue).getBySize());
					else if (srcType == OperandType::DirectAddress)//IZ DIREKTNE MEMORIJE
						c.writeToMemory<uint64_t>(address, cpu.readFromMemory<uint64_t>(getSrcValue()));
					else if (srcType == OperandType::IndirectAddress)
						//kaze pisi u memoriju  ( velicine dest, na adresu dest, ono sto nadjes u memoriji na adresu koji sadrzi registar src)
						c.writeToMemory<uint64_t>(address, cpu.readFromMemory<uint64_t>(cpu.getRegister(srcStringValue).getBySize()));
					else if (srcType == OperandType::IntegerConstant)//posljednje, mozes i konstante u memoriju pisati lol
						//writeToMemoryByRegisterSize(dest, address, getSrcValue());//gotovo veselje, else error
						c.writeToMemory<uint64_t>(address, getSrcValue());
					else
						throw new std::exception("Ne mozes string u memoriju upisati, nema toga!");
				}
				else
					throw std::exception("Cek je l ovo moguce? da si napisao mov bez operanada? Sta kako gdje...");
			}},
			{"and", [this](CPU& c) {
				if (destType != OperandType::Register)
					throw std::exception("Odrediste mora biti registar!");
				auto& destReg = c.getRegister(destStringValue);
				auto& flags = c.getFlags();
				flags.OF = 0;
				flags.CF = 0;
				uint64_t destSize = RegisterSizeToBits(destReg.getSize());
				uint64_t _destValue = destReg.getBySize();
				uint64_t _srcValue = getSrcValue();
				uint64_t newValue = _destValue & _srcValue;
				flags.CF = (newValue >> (destSize - 1)) & 1;
				flags.ZF = newValue == 0;
				flags.PF = countSetBits(newValue & (((uint64_t)-1) >> (64 - destSize))) % 2 == 0;
				destReg.setBySize(newValue);				
			}},
			{"or", [this](CPU& c) {
				if (destType != OperandType::Register)
					throw std::exception("Odrediste mora biti registar!");
				auto& destReg = c.getRegister(destStringValue);
				auto& flags = c.getFlags();
				flags.OF = 0;
				flags.CF = 0;
				uint64_t destSize = RegisterSizeToBits(destReg.getSize());
				uint64_t _destValue = destReg.getBySize();
				uint64_t _srcValue = getSrcValue();
				uint64_t newValue = _destValue | _srcValue;
				flags.CF = (newValue >> (destSize - 1)) & 1;
				flags.ZF = newValue == 0;
				flags.PF = countSetBits(newValue & (((uint64_t)-1) >> (64 - destSize))) % 2 == 0;
				destReg.setBySize(newValue);
			}},
			{ "not", [this](CPU& c) {
				if (destType != OperandType::Register)
					throw std::exception("Odrediste mora biti registar!");
				auto& destReg = c.getRegister(destStringValue);
				auto& flags = c.getFlags();
				flags.OF = 0;
				flags.CF = 0;
				uint64_t destSize = RegisterSizeToBits(destReg.getSize());
				uint64_t _destValue = destReg.getBySize();
				uint64_t newValue = ~_destValue;
				flags.CF = (newValue >> (destSize - 1)) & 1;
				flags.ZF = newValue == 0;
				flags.PF = countSetBits(newValue & (((uint64_t)-1) >> (64 - destSize))) % 2 == 0;
				destReg.setBySize(newValue);
			} },
			{ "cmp", [this](CPU& c) {//Ovo je ladno kopiran SUB bez upisa vrijednosti, ne salim se. Tako rece neki sajt da ide flags za CMP
				if (destType != OperandType::Register)
					throw std::exception("Moras oduzimati od registra!");
				Register& dest = cpu.getRegister(destStringValue);
				uint64_t _destValue = dest.getBySize();
				uint64_t _srcValue = getSrcValue();
				uint64_t newValue = _destValue - _srcValue;
				auto& flags = c.getFlags();
				flags.ZF = newValue == 0;
				flags.SF = (newValue >> (RegisterSizeToBits(dest.getSize()) - 1)) & 1;
				flags.PF = countSetBits(newValue) % 2 == 0;
				flags.OF = _srcValue > _destValue;//!?!?!?!
			} },
			{ "jmp", jump},
			{ "je", [this,&jump](CPU& c) {//jump if equal
				if (c.getFlags().ZF == 1)
					jump(c);
			}},
			{ "jne", [this,&jump](CPU& c) {//jump if not equal
				if (c.getFlags().ZF == 0)
					jump(c);
			}},
			{ "jge", [this,&jump](CPU& c) {//jump if greater or equal
				if (c.getFlags().SF == 0)
					jump(c);
			}},
			{ "jl", [this,&jump](CPU& c) {//jump if less
				if (c.getFlags().SF != 0)
					jump(c);
			}},
			{ "jpe", [this,&jump](CPU& c) {//jump if parity even
				if (c.getFlags().PF == 1)
					jump(c);
			}},
			{ "jpo", [this,&jump](CPU& c) {//jump if parity odd
				if (c.getFlags().PF == 1)
					jump(c);
			}},
			{ "jz", [this,&jump](CPU& c) {//jump if zero
				if (c.getFlags().ZF == 1)
					jump(c);
			}},
			{ "jnz", [this,&jump](CPU& c) {//jump if not zero
				if (c.getFlags().ZF == 0)
					jump(c);
			}},
			{ "print", [this](CPU& c) {
				if (destType == OperandType::Register)
					if (c.isHexOutput())
						std::cout << c.getRegister(destStringValue).asHex();
					else
						std::cout << c.getRegister(destStringValue).getBySize();
				else if (trimCopy(destStringValue)[0] == '[') {//mozda je registar
					auto trimmed = trimCopy(destStringValue);//uzasno, znam
					if (trimmed[trimmed.length() - 1] == ']')
						if (isRegister(trimmed.substr(1, trimmed.length() - 2), c.getRegisterCount()))
							std::cout << c.readFromMemory<uint64_t>(c.getRegister(trimmed.substr(1, trimmed.length() - 2)).getBySize());
					}
				else
					std::cout << destStringValue;
			} },
			{ "println", [this](CPU& c) {
				if (destType == OperandType::Register)
					if (c.isHexOutput())
						std::cout << c.getRegister(destStringValue).asHex();
					else
						std::cout << c.getRegister(destStringValue).getBySize();
				else if (trimCopy(destStringValue)[0] == '[') {//mozda je registar
					auto trimmed = trimCopy(destStringValue);//uzasno, znam
					if (trimmed[trimmed.length() - 1] == ']')
						if (isRegister(trimmed.substr(1, trimmed.length() - 2), c.getRegisterCount()))
							std::cout << c.readFromMemory<uint64_t>(c.getRegister(trimmed.substr(1, trimmed.length() - 2)).getBySize());
					}
				else
					std::cout << destStringValue;
				std::cout << std::endl;
			} },
			{ "read", [this](CPU& c) {
				//ovo treba da procita u DST nesto sa tastature, to ce MORATI biti int nekakav
				if (destType != OperandType::Register)
					throw std::exception("Ne mozes citati u memoriju pravo!");
				auto& dest = c.getRegister(destStringValue);
				uint64_t noviInt = 0;
				std::string input;
				std::cin >> input;
				if (input.length()>2 && (input[1] == 'x' || input[1] == 'X'))
					noviInt = std::stoull(input.substr(2), nullptr, 16);
				else
					noviInt = std::stoull(input);
				dest.setBySize(noviInt);
			}}
		});
	if (microcode.find(name) != microcode.end())
		microcode.at(name)(cpu);
}

int Instruction::getDestSize()
{
	switch (destType)
	{
	case OperandType::Register:
		return RegisterSizeToBits(cpu.getRegister(destStringValue).getSize());
	case OperandType::DirectAddress:
		return 64;
	case OperandType::IndirectAddress:
		return 64;
	default:
		return 0;
	}
}

int Instruction::getSrcSize()
{
	switch (srcType)
	{
	case OperandType::Register:
		return RegisterSizeToBits(cpu.getRegister(srcStringValue).getSize());
	case OperandType::DirectAddress:
		return 64;
	case OperandType::IndirectAddress:
		return 64;
	default:
		return 0;
	}
}

uint64_t Instruction::getDestValue()
{
	switch (destType)
	{
	case OperandType::Register:
		return cpu.getRegister(destStringValue).getBySize();
		break;
	case OperandType::DirectAddress:
		return cpu.readFromMemory<uint64_t>(destIntValue);
		break;
	case OperandType::IndirectAddress:
		return cpu.readFromMemoryByRegisterSize(cpu.getRegister(destStringValue).getBySize(), cpu.getRegister(destStringValue));
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

void Instruction::writeToDest(uint64_t value)
{
	switch (destType)
	{
	case OperandType::Register:
		cpu.getRegister(destStringValue).setBySize(value);
		break;
	case OperandType::DirectAddress:
		break;
	case OperandType::IndirectAddress:
		break;
	case OperandType::StringConstant:
		throw std::exception("Ne mozes upisati u const");
	case OperandType::IntegerConstant:
		throw std::exception("Ne mozes upisati u const");
	case OperandType::Missing:
		throw std::exception("Upis u operand kog nema...");
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
			if (dest[1] == 'x' || dest[1] == 'X')
				info.intValue = std::stoull(dest.substr(2), nullptr, 16);
			else
				info.intValue = std::stoull(dest);
			info.type = OperandType::IntegerConstant;
		}
		catch (std::exception& ex) {//nije broj ocito
			info.stringValue = std::move(dest);
			info.type = OperandType::StringConstant;
		}
	}
	return info;
}

void Instruction::assignInfo(OperandInfo&& info, OperandType& type, uint64_t& intValue, std::string& stringValue)
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
	case OperandType::DirectAddress:
		return "direktna memorija";
	case OperandType::IndirectAddress:
		return "indirektna memorija";
	case OperandType::StringConstant:
		return "string konstanta";
	case OperandType::IntegerConstant:
		return "int konstanta";
	default://ovo je ono missing
		return "";
	}
}

const char* instructionTypeToString(InstructionType it)
{
	switch (it)
	{
	case InstructionType::Regular:
		return "obicna";
	case InstructionType::Jump:
		return "skok";
	case InstructionType::Extension:
		return "ekstenzija";
	case InstructionType::Label:
		return "labela";
	case InstructionType::Breakpoint:
		return "breakpoint";
	default:
		return "vrlo vjerovatno nemoguce";
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
	return os << "[" << i.name << "{ "<<i.lineNumber<<" }](" << instructionTypeToString(i.instructionType) << ") u"
		<< " (" << opTypeToString(i.destType) << ") [" << i.destIntValue << ", " << i.destStringValue << "]  "
		<< "iz (" << opTypeToString(i.srcType) << ")[" << i.srcIntValue << ", " << i.srcStringValue << "]";
}