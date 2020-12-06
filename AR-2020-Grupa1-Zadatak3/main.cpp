#include <iostream>
#include "Flags.h"
#include "CPU.h"

#include "Instruction.h"

int main() {
	Flags flags;
	flags.CF = 1;
	flags.DF = 1;
	flags.OF = 1;
	CPU c(26);
	c.printState();

}