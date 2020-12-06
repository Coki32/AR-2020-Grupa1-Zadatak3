#pragma once
#include <string>

#include "Flags.h"
#include "IReadWritable.h"

//Obicne instrukcije
//ne oni skokovi i fazoni
class Instruction
{
	std::string name;
	IWritable* dest;
	IReadable* src;
	uint64_t constValue;

};
