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

struct IOState
{
	uint8_t JOYPAD;
	uint8_t DIV;
	uint8_t TIMA;
	uint8_t TMA;
	uint8_t TAC;
	uint8_t IFLAGS;
	uint8_t STAT;
	uint8_t SCX;
	uint8_t SCY;
	uint8_t LY;
	uint8_t LYC;
	uint8_t DMA;
	uint8_t WX;
	uint8_t WY;
	uint8_t IE;
};

struct MMUState
{
	bool inBIOS;
	int romBankNumber;
	int ramBankNumber;
};