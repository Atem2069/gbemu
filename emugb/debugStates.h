#pragma once

#include<iostream>

struct CPUState
{
	uint16_t AF;
	uint16_t BC;
	uint16_t DE;
	uint16_t HL;
	uint16_t SP;
	uint16_t PC;
	bool IME;
	bool halfCarry;
	bool carry;
	bool zero;
	bool subtract;
};

struct MMUState
{
	bool inBIOS;
	int romBankNumber;
	int ramBankNumber;
};