#pragma once

#include"MMU.h"

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

	uint8_t A = 0, F = 0;	//AF
	uint8_t B = 0, C = 0;	//BC
	uint8_t D = 0, E = 0;	//DE
	uint8_t H = 0, L = 0;	//HL
	uint8_t S = 0, P = 0;	//SP
	uint16_t PC = 0;		//PC (Program Counter)
	bool m_halted = false;
};