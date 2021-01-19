#include <iostream>
#include "Flags.h"
#include "CPU.h"
#include "util.h"
#include "Instruction.h"

int main() {
	Flags flags;
	flags.CF = 1;
	flags.DF = 1;
	flags.OF = 1;
	CPU c(4);
	c.printState();
	c.readFile("Test.aself");
	c.writeToMemory(2, (uint32_t)0xAABBCCDD);
	std::cout << "Na adresi 2 je upisano " << c.readFromMemory<uint8_t>(2) << std::endl;
	c.printInstructions();
	c.run();
	c.printState();
}