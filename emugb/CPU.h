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

	void m_pushToStack(uint16_t val);	//helper functions for pushing/popping items to/from stack
	uint16_t m_popFromStack();

	//Instructions (all prefixed with _)



	/*All 8-bit unprefixed instructions*/
	void _loadImmPairRegister(Register& reg);
	void _loadImmRegister(uint8_t& reg);
	void _loadImmFromRegister(uint8_t& regA, uint8_t& regB);
	void _loadDirectFromPairRegister(uint8_t& regA, Register& regB);
	void _loadDirectFromPairRegisterInc(uint8_t& regA, Register& regB);
	void _loadDirectFromPairRegisterDec(uint8_t& regA, Register& regB);

	void _storeRegisterAtPairRegister(Register& regA, uint8_t& regB);
	void _storeRegisterAtPairRegisterInc(Register& regA, uint8_t& regB);
	void _storeRegisterAtPairRegisterDec(Register& regA, uint8_t& regB);

	void _storePairRegisterAtAddress(Register& reg);

	void _storeOperandAtPairAddress(Register& reg);

	void _storeRegisterInHRAM(uint8_t& regDst, uint8_t& regSrc);	// LD (C), A
	void _loadFromHRAM(uint8_t& regDst, uint8_t& regSrcIdx);		// LD A, (C)

	void _incrementPairRegister(Register& reg);
	void _incrementRegister(uint8_t& reg);


	void _decrementRegister(uint8_t& reg);

	void _addPairRegisters(Register& regA, Register& regB);

	void _jumpRelative();
	void _jumpRelativeIfZeroNotSet();
	void _jumpRelativeIfZeroSet();
	void _jumpRelativeIfCarryNotSet();
	void _jumpRelativeIfCarrySet();

	void _jumpAbsolute();
	void _jumpAbsoluteIfZeroNotSet();
	void _jumpAbsoluteIfZeroSet();
	void _jumpAbsoluteIfCarryNotSet();
	void _jumpAbsoluteIfCarrySet();

	void _call();
	void _callIfZeroNotSet();
	void _callIfZeroSet();
	void _callIfCarryNotSet();
	void _callIfCarrySet();

	void _return();
	void _returnIfZeroNotSet();
	void _returnIfZeroSet();
	void _returnIfCarryNotSet();
	void _returnIfCarrySet();
	void _returnFromInterrupt();


	void _setCarryFlag();
	void _flipCarryFlag();


	void _addRegisters(uint8_t& regA, uint8_t& regB);
	void _addPairAddress(uint8_t& regA, Register& regB);
	void _addRegistersCarry(uint8_t& regA, uint8_t& regB);
	void _addPairAddressCarry(uint8_t& regA, Register& regB);
	void _addValue(uint8_t& reg);
	void _addValueCarry(uint8_t& reg);

	void _subRegister(uint8_t& reg);
	void _subRegisterCarry(uint8_t& reg);
	void _subPairAddress(Register& reg);
	void _subPairAddressCarry(Register& reg);
	void _subValue();
	void _subValueCarry();

	void _andRegister(uint8_t& reg);
	void _andPairAddress(Register& reg);
	void _andValue();
	void _xorRegister(uint8_t& reg);
	void _xorPairAddress(Register& reg);
	void _xorValue();
	void _orRegister(uint8_t& reg);
	void _orPairAddress(Register& reg);
	void _orValue();
	void _compareRegister(uint8_t& reg);
	void _comparePairAddress(Register& reg);
	void _compareValue();

	void _pushPairRegister(Register& reg);
	void _popToPairRegister(Register& reg);

	//some misc instructions
	void _disableInterrupts();
	void _enableInterrupts();
	void _stop();
	void _halt();
	void _resetToVector(uint8_t vectorIdx);
	void _adjustBCD();
	void _complement();	//take one's complement
	void _RLA();	//RLA, RLCA, RRA, RRCA all affect flags differently, so have to be implemented separately
	void _RLCA();
	void _RRA();
	void _RRCA();



	/*All 16-bit prefixed instructions*/
	
};