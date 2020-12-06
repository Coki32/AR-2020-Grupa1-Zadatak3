#pragma once
#include <cstdint>
#include <string>
class Flags
{
public:
	union 
	{
		uint64_t all;
		struct
		{
			uint64_t CF : 1; //carry 1 bio prenos ili posudjeno kod oduzimanja 
			uint64_t _unused : 1;
			uint64_t PF : 1; //parity: 1 ako ima paran broj jedinica, 0 neparan
			uint64_t _unused2 : 1;
			uint64_t AF : 1; //
			uint64_t _unused3 : 1;
			uint64_t ZF : 1; //zero: ako je rezultat prosle operacije bio 0
			uint64_t SF : 1; //sign: bit znaka, kopija najviseg bita rezultata
			uint64_t TF : 1; //trap: ako je 1 generise izuzetak kasnije, meni ne treba
			uint64_t IF : 1; //interrupt: 1 obradjuje maskirajuce prekide, 0 ignorise, meni ne treba
			uint64_t DF : 1; //direction: 1 sadrzaj RSI i RDI se DEKREMENTUJE, 0 inkrementuje
			uint64_t OF : 1; //overflow: kod oznacenih brojeva signalizuje da se desio overflow, ignorise kod neoznacenih
		};
	};

public:
	Flags() noexcept : all(0) {}

	std::string asHex() const;
};

