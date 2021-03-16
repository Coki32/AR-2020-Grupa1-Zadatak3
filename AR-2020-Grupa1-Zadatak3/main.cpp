#include <iostream>
#include <algorithm>
#include <vector>
#include <memory>

#include "Flags.h"
#include "CPU.h"
#include "util.h"
#include "Instruction.h"

struct ProgramOptions {
	CpuOptions cpuOptions;
	std::string inputFilename;
	bool valid = false;
};

ProgramOptions argumenti(int argc, char** argv);
void printArgs();

int main(int argc, char** argv) {
	auto options = argumenti(argc, argv);
	if (!options.valid) {
		std::cerr << "Nedostaju ti argumenti komandne linije." << std::endl;
		printArgs();
		std::exit(-3);
	}
	CPU c(options.cpuOptions);
	c.readFile(options.inputFilename);
	c.run();
}

void printArgs() {
	std::cout << R"(Moguci (i obavezni) argumenti pri pokretanju:
	-f filename - jedini obavezni argument. Navodi ime .aself fajla koji se koristi kao ulaz za interpreter.
	-d | --debug - oznacava da li se program izvrsava u debug modu ili ne. Bez debug-a ignorisu se breakpointi.
	-a | --avanturista - oznacava da li se pri odabiru debug opcije koristi zabavni unos ili ne.
	-x | --hex - oznacava da ce output kod print i println instrukcija biti ustvari heksadecimalni broj, default: false
	-rcount N - specificira broj registara koje "procesor" koristi. Mora biti u opsegu [1,26], default: 8
	-heap M - specificira velicinu heap-a u bajtovima, default: 1024)" << std::endl;
}

ProgramOptions argumenti(int argc, char** argv) {
	ProgramOptions options;
	if (argc < 3)
		return options;
	auto prettyArgs = charPtrArrayToVector(argv, argc);
	const auto end = prettyArgs->end();
	auto argumentExists = [&prettyArgs](const std::string& arg)->auto{
		return std::find(prettyArgs->begin(), prettyArgs->end(), arg) != prettyArgs->end();
	};
	auto getArgumentIfExists = [&prettyArgs](const std::string& arg) -> auto {
		auto it = std::find(prettyArgs->begin(), prettyArgs->end(), arg);
		return it != prettyArgs->end() ? (++it) : prettyArgs->end();
	};
	options.cpuOptions.debug = argumentExists("-d") || argumentExists("--debug");
	options.cpuOptions.avanturista = argumentExists("-a") || argumentExists("--avanturista");
	options.cpuOptions.printsHex = argumentExists("-x") || argumentExists("--hex");
	if (auto rcount = getArgumentIfExists("-rcount"); rcount != end) {
		try {
			options.cpuOptions.numberOfRegisters = std::stoi(*rcount);
		}
		catch (std::exception& ex) {
			std::cerr << "Neispravan argument za -rcount, koristim default a to je 8" << std::endl;
		}
	}
	if (auto heapSize = getArgumentIfExists("-heap"); heapSize != end) {
		try {
			options.cpuOptions.heapSize = std::stoi(*heapSize);
		}
		catch (std::exception& ex) {
			std::cerr << "Neispravan argument za -heap, koristim default a to je 1024" << std::endl;
		}
	}
	
	if (auto inputFilename = getArgumentIfExists("-f"); inputFilename != end) {
		if ((*inputFilename).find(".aself") == std::string::npos) {
			std::cerr << "Podrzani su samo .aself fajlovi!" << std::endl;
			std::exit(-42);
		}
		options.inputFilename = *inputFilename;
		options.valid = true;
	}
	else {
		std::cerr << "Moras navesti ulazni program koji se koristi pomocu -f opcije!" << std::endl;
		std::exit(-1);
	}
	
	return options;
}