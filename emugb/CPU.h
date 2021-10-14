#pragma once

#include<iostream>

#include"Logger.h"
#include"MMU.h"

union Register
{
	uint16_t reg;
	struct
	{
		uint8_t low;
		uint8_t high;
	};
};

class CPU
{
public:
	CPU(MMU* mmu);
	~CPU();

	void step();

	int getCycleCount();

	bool m_getZeroFlag();
	void m_setZeroFlag(bool value);

	bool m_getCarryFlag();
	void m_setCarryFlag(bool value);

	bool m_getHalfCarryFlag();
	void m_setHalfCarryFlag(bool value);

	bool m_getSubtractFlag();
	void m_setSubtractFlag(bool value);

private:

	uint8_t m_fetch();

	void m_executeInstruction();		//standard opcodes
	void m_executePrefixedInstruction(); //opcodes prefixed with 0xCB

	std::string m_lastInstruction = "";

	MMU* m_mmu;
	int m_cycleCount = 0;

	Register AF, BC, DE, HL, SP;	//General purpose registers, flags, and stack pointer
	uint16_t PC = 0;		//PC (Program Counter) - can be implemented as single uint16.
	bool m_halted = false;

	//Instructions (all prefixed with _)
	void _loadImmPairRegister(Register& reg);
	void _loadImmRegister(uint8_t& reg);

	void _storeRegisterAtPairRegister(Register& regA, uint8_t& regB);
	void _storePairRegisterAtAddress(Register& reg);

	void _incrementPairRegister(Register& reg);
	void _incrementRegister(uint8_t& reg);


	void _decrementRegister(uint8_t& reg);

};