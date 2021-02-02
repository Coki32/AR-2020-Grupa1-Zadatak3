#include <string>
#include <iostream>
#include <fstream>

#include "CPU.h"
#include "util.h"
#include <sstream>

template <typename T>
static void LogIfDebug(const T& ss)
{
#if _DEBUG
	std::cout << "[DEBUG]: " << ss << std::endl;
#endif
}

CPU::CPU(CpuOptions options) :debug(options.debug), avanturista(options.avanturista) {
	if (options.numberOfRegisters < 1 || options.numberOfRegisters>26)
		throw std::exception("Number of registers must be an integer in range [1, 26]");
	this->numberOfRegisters = options.numberOfRegisters;
	if(avanturista && debug)
		LogIfDebug("Ok, avanturista, srecno s debagovanjem");
	for (int i = 0; i < options.numberOfRegisters; ++i) {
		auto name = std::string("r").append(1, 'a' + i).append("x");
		registers.emplace(name, Register(std::string("r").append(1, 'a' + i).append("x")));
	}
	heap = std::vector<uint8_t>(options.heapSize);
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
		if (line[0] == '#')
			continue;
		lines.push_back(line);
		instructions.emplace_back(line,*this,i);
		i++;
	}
	return true;
}

void CPU::printState()
{
	for (auto it = registers.begin(); it != registers.end(); ++it) {
		std::cout << it->second.asHex(false) << std::endl;
	}
}

void CPU::jumpToLine(int line)
{
	currentInstruction = line;
}

void CPU::jumpToLabel(const std::string& label)
{
	int labelIndex = 0;
	auto is = std::find_if(instructions.begin(), instructions.end(), [&label](const Instruction& i) {
		return label == i.getName(); 
	});
	LogIfDebug((std::stringstream("Za ") << label << " skacem na idx: " << (is - instructions.begin())).str());
	if (is != instructions.end())
		jumpToLine(is - instructions.begin());
	else
		LogIfDebug("Imas skok na labelu koja ne postoji, to se ignorise");
}

Register& CPU::getRegister(const std::string& name)
{
	if(name[0]=='r')
		return registers.at(name);
	else {
		auto r = std::string("r").append(name.substr(1));
		LogIfDebug(std::string("Dohvatam ").append(r));
		return registers.at(r);
	}
}

void CPU::printInstructions()
{
	for (int i = 0; i < lines.size(); ++i)
		std::cout << "\"" << lines[i] << "\" => " << instructions[i] << std::endl;
}

void CPU::run()
{
	bool shouldPause = false;
	for (currentInstruction = 0; currentInstruction < instructions.size(); ++currentInstruction) {
		auto& instruction = instructions[currentInstruction];
		if (!debug)
			instruction.execute();
		else {
			LogIfDebug(instruction);
			if (instruction.isBreakpoint()) {
				LogIfDebug("Naisao na breakpoint");
				shouldPause = true;
				continue;//nemoj ni izvrsavati breakpoint
			}
			else if (shouldPause) {//prikazi meni pa u meniju odluci kako nastavlja
				showDebugMenu(instruction, shouldPause);
			}
			else//a ako nije trebao da stane ide normalno
				instruction.execute();
		}
	}
}

/// <param name="lowerBound">inclusive lower bound</param>
/// <param name="upperBound">inclusive upper bound</param>
/// <returns>a number in range [lowerBound, upperBound]</returns>
static int limitedInput(int lowerBound, int upperBound) {
	int c;
	do {
		std::cout << "Unesi broj [" << lowerBound << ", " << upperBound << "]: ";
		std::cin >> c;
	} while (c < lowerBound || c > upperBound);
	return c;
}

static int avanturistaInput(int upperBound) {
	int c;
	do {
		std::cout << "Unesi broj [do " << upperBound << "]: ";
		std::cin >> c;
	} while (c < 0);
	return c % (upperBound+1);
}

void CPU::showDebugMenu(Instruction& instruction, bool& shouldPause)//mrsko mi je da pravim jos 83 metode, ova ce biti milion linija
{
	int c;
	do {
		std::cout << instruction << std::endl;//ispisi i instrukciju da znas o cemu se radi
		std::cout << R"(		<==========DEBUG==========>
		Opcije za debagovanje:
			0 - izvrsi ovu jednu
			1 - izvrsi sve do sljedeceg breakpoint-a
			2 - prikazi registre (sve, pa se vrati u ovaj meni)
			3 - prikazi memoriju (bice podmeni, pa se vrati u ovaj meni))" << std::endl;

		c = avanturista ? avanturistaInput(3) : limitedInput(0, 3);
		switch (c)
		{
		case 0:
			shouldPause = true;
			instruction.execute();
			break;
		case 1:
			shouldPause = false;
			instruction.execute();
			break;
		case 2:
			printState();
			break;
		case 3:
			memoryDebugMenu();
			break;
		}
	} while (c == 2 || c == 3);
}

void CPU::memoryDebugMenu()
{
	std::cout << R"(		<==========MEMORIJA==========>
		Memoriju mozes pregledati heksadecimalno na dva nacina:
			0 - povratak nazad
			1 - jedna adresa (dohvati 8 bajta)
			2 - opseg adresa (dohvati barem 1 bajt))" << std::endl;
	int c = avanturista ? avanturistaInput(2) : limitedInput(0, 2);
	switch (c)
	{
	case 1: {
		int adresa = limitedInput(0, heap.size()-8);
		for (int i = 0; i < 8; ++i)
			std::cout << toHexString(heap[adresa + i]) << " ";
		std::cout<<std::endl;
	}
		break;
	case 2: {
		int donja = limitedInput(0, heap.size() - 1);
		int gornja = limitedInput(donja + 1, heap.size());
		for (int i = donja, c = 0; i <= gornja; ++i, ++c) {
			std::cout << toHexString(heap[i]) << " ";
			if (c % 8 == 0)
				std::cout << std::endl;
		}
	}
		break;
	}
}

uint64_t CPU::readFromMemoryByRegisterSize(int address, const Register& reg)
{
	switch (reg.getSize())
	{
	case RegisterSize::Full:
		return readFromMemory<uint64_t>(address);
	case RegisterSize::Half:
		return readFromMemory<uint32_t>(address);
	case RegisterSize::Quarter:
		return readFromMemory<uint16_t>(address);
	case RegisterSize::Byte:
		return readFromMemory<uint8_t>(address);
	default:
		throw std::exception("Ladno nemoguce?");
	}

}
