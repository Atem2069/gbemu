#include"CPU.h"

CPU::CPU(MMU* mmu)
{
	m_mmu = mmu;

}

CPU::~CPU()
{
	//todo
}

void CPU::step()
{
	if (!m_halted)
	{
		m_executeInstruction();
	}
	else
		m_cycleCount++;

	//handle interrupts
	//tick timer
	//synch timing (todo later)
}

void CPU::m_executeInstruction()
{
	uint8_t opcode = m_fetch();
	switch (opcode)
	{
	default: std::cout << "Not implemented." << std::endl; break;
	}
}

void CPU::m_executePrefixedInstruction()
{
	uint8_t opcode = m_fetch();
	switch (opcode)
	{
	default:std::cout << "Not implemented." << std::endl; break;
	}
}

int CPU::getCycleCount() { return m_cycleCount; }

bool CPU::m_getZeroFlag() { return (F & 0b10000000) >> 7; }
void CPU::m_setZeroFlag(bool value)
{
	if (!value)
		F &= 0b01111111;
	else
		F |= 0b10000000;
}

bool CPU::m_getCarryFlag() { return (F & 0b00010000) >> 4; }
void CPU::m_setCarryFlag(bool value)
{
	if (!value)
		F &= 0b11101111;
	else
		F |= 0b00010000;
}

bool CPU::m_getHalfCarryFlag() { return (F & 0b00100000) >> 5; }
void CPU::m_setHalfCarryFlag(bool value)
{
	if (!value)
		F &= 0b11011111;
	else
		F |= 0b00100000;
}

bool CPU::m_getSubtractFlag() { return (F & 0b01000000) >> 6; }
void CPU::m_setSubtractFlag(bool value)
{
	if (!value)
		F &= 0b10111111;
	else
		F |= 0b01000000;
}

uint8_t CPU::m_fetch()
{
	uint8_t val = m_mmu->read(PC);
	PC++;
	return val;
}