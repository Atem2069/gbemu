#include"CPU.h"

CPU::CPU(MMU* mmu)
{
	m_mmu = mmu;
	AF = {}; BC = {}; DE = {}; HL = {};
	PC = 0;
}

CPU::~CPU()
{
	//todo
}

void CPU::step()
{
	if (!m_halted)
	{
		Logger::getInstance()->msg(LoggerSeverity::Info, "Fetch instruction PC=" + std::to_string((int)PC));
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
	case 0x0: break;
	case 0x1: break;
	case 0x2: break;
	case 0x3: break;
	case 0x4: break;
	case 0x5: break;
	case 0x6: break;
	case 0x7: break;
	case 0x8: break;
	case 0x9: break;
	case 0xA: break;
	case 0xB: break;
	case 0xC: break;
	case 0xD: break;
	case 0xE: break;
	case 0xF: break;
	case 0x10: break;
	case 0x11: break;
	case 0x12: break;
	case 0x13: break;
	case 0x14: break;
	case 0x15: break;
	case 0x16: break;
	case 0x17: break;
	case 0x18: break;
	case 0x19: break;
	case 0x1A: break;
	case 0x1B: break;
	case 0x1C: break;
	case 0x1D: break;
	case 0x1E: break;
	case 0x1F: break;
	case 0x20: break;
	case 0x21: break;
	case 0x22: break;
	case 0x23: break;
	case 0x24: break;
	case 0x25: break;
	case 0x26: break;
	case 0x27: break;
	case 0x28: break;
	case 0x29: break;
	case 0x2A: break;
	case 0x2B: break;
	case 0x2C: break;
	case 0x2D: break;
	case 0x2E: break;
	case 0x2F: break;
	case 0x30: break;
	case 0x31: break;
	case 0x32: break;
	case 0x33: break;
	case 0x34: break;
	case 0x35: break;
	case 0x36: break;
	case 0x37: break;
	case 0x38: break;
	case 0x39: break;
	case 0x3A: break;
	case 0x3B: break;
	case 0x3C: break;
	case 0x3D: break;
	case 0x3E: break;
	case 0x3F: break;
	case 0x40: break;
	case 0x41: break;
	case 0x42: break;
	case 0x43: break;
	case 0x44: break;
	case 0x45: break;
	case 0x46: break;
	case 0x47: break;
	case 0x48: break;
	case 0x49: break;
	case 0x4A: break;
	case 0x4B: break;
	case 0x4C: break;
	case 0x4D: break;
	case 0x4E: break;
	case 0x4F: break;
	case 0x50: break;
	case 0x51: break;
	case 0x52: break;
	case 0x53: break;
	case 0x54: break;
	case 0x55: break;
	case 0x56: break;
	case 0x57: break;
	case 0x58: break;
	case 0x59: break;
	case 0x5A: break;
	case 0x5B: break;
	case 0x5C: break;
	case 0x5D: break;
	case 0x5E: break;
	case 0x5F: break;
	case 0x60: break;
	case 0x61: break;
	case 0x62: break;
	case 0x63: break;
	case 0x64: break;
	case 0x65: break;
	case 0x66: break;
	case 0x67: break;
	case 0x68: break;
	case 0x69: break;
	case 0x6A: break;
	case 0x6B: break;
	case 0x6C: break;
	case 0x6D: break;
	case 0x6E: break;
	case 0x6F: break;
	case 0x70: break;
	case 0x71: break;
	case 0x72: break;
	case 0x73: break;
	case 0x74: break;
	case 0x75: break;
	case 0x76: break;
	case 0x77: break;
	case 0x78: break;
	case 0x79: break;
	case 0x7A: break;
	case 0x7B: break;
	case 0x7C: break;
	case 0x7D: break;
	case 0x7E: break;
	case 0x7F: break;
	case 0x80: break;
	case 0x81: break;
	case 0x82: break;
	case 0x83: break;
	case 0x84: break;
	case 0x85: break;
	case 0x86: break;
	case 0x87: break;
	case 0x88: break;
	case 0x89: break;
	case 0x8A: break;
	case 0x8B: break;
	case 0x8C: break;
	case 0x8D: break;
	case 0x8E: break;
	case 0x8F: break;
	case 0x90: break;
	case 0x91: break;
	case 0x92: break;
	case 0x93: break;
	case 0x94: break;
	case 0x95: break;
	case 0x96: break;
	case 0x97: break;
	case 0x98: break;
	case 0x99: break;
	case 0x9A: break;
	case 0x9B: break;
	case 0x9C: break;
	case 0x9D: break;
	case 0x9E: break;
	case 0x9F: break;
	case 0xA0: break;
	case 0xA1: break;
	case 0xA2: break;
	case 0xA3: break;
	case 0xA4: break;
	case 0xA5: break;
	case 0xA6: break;
	case 0xA7: break;
	case 0xA8: break;
	case 0xA9: break;
	case 0xAA: break;
	case 0xAB: break;
	case 0xAC: break;
	case 0xAD: break;
	case 0xAE: break;
	case 0xAF: break;
	case 0xB0: break;
	case 0xB1: break;
	case 0xB2: break;
	case 0xB3: break;
	case 0xB4: break;
	case 0xB5: break;
	case 0xB6: break;
	case 0xB7: break;
	case 0xB8: break;
	case 0xB9: break;
	case 0xBA: break;
	case 0xBB: break;
	case 0xBC: break;
	case 0xBD: break;
	case 0xBE: break;
	case 0xBF: break;
	case 0xC0: break;
	case 0xC1: break;
	case 0xC2: break;
	case 0xC3: break;
	case 0xC4: break;
	case 0xC5: break;
	case 0xC6: break;
	case 0xC7: break;
	case 0xC8: break;
	case 0xC9: break;
	case 0xCA: break;
	case 0xCB: break;
	case 0xCC: break;
	case 0xCD: break;
	case 0xCE: break;
	case 0xCF: break;
	case 0xD0: break;
	case 0xD1: break;
	case 0xD2: break;
	case 0xD3: break;
	case 0xD4: break;
	case 0xD5: break;
	case 0xD6: break;
	case 0xD7: break;
	case 0xD8: break;
	case 0xD9: break;
	case 0xDA: break;
	case 0xDB: break;
	case 0xDC: break;
	case 0xDD: break;
	case 0xDE: break;
	case 0xDF: break;
	case 0xE0: break;
	case 0xE1: break;
	case 0xE2: break;
	case 0xE3: break;
	case 0xE4: break;
	case 0xE5: break;
	case 0xE6: break;
	case 0xE7: break;
	case 0xE8: break;
	case 0xE9: break;
	case 0xEA: break;
	case 0xEB: break;
	case 0xEC: break;
	case 0xED: break;
	case 0xEE: break;
	case 0xEF: break;
	case 0xF0: break;
	case 0xF1: break;
	case 0xF2: break;
	case 0xF3: break;
	case 0xF4: break;
	case 0xF5: break;
	case 0xF6: break;
	case 0xF7: break;
	case 0xF8: break;
	case 0xF9: break;
	case 0xFA: break;
	case 0xFB: break;
	case 0xFC: break;
	case 0xFD: break;
	case 0xFE: break;
	case 0xFF: break;
	}
}

void CPU::m_executePrefixedInstruction()
{
	uint8_t opcode = m_fetch();
	switch (opcode)
	{
	case 0x0: break;
	case 0x1: break;
	case 0x2: break;
	case 0x3: break;
	case 0x4: break;
	case 0x5: break;
	case 0x6: break;
	case 0x7: break;
	case 0x8: break;
	case 0x9: break;
	case 0xA: break;
	case 0xB: break;
	case 0xC: break;
	case 0xD: break;
	case 0xE: break;
	case 0xF: break;
	case 0x10: break;
	case 0x11: break;
	case 0x12: break;
	case 0x13: break;
	case 0x14: break;
	case 0x15: break;
	case 0x16: break;
	case 0x17: break;
	case 0x18: break;
	case 0x19: break;
	case 0x1A: break;
	case 0x1B: break;
	case 0x1C: break;
	case 0x1D: break;
	case 0x1E: break;
	case 0x1F: break;
	case 0x20: break;
	case 0x21: break;
	case 0x22: break;
	case 0x23: break;
	case 0x24: break;
	case 0x25: break;
	case 0x26: break;
	case 0x27: break;
	case 0x28: break;
	case 0x29: break;
	case 0x2A: break;
	case 0x2B: break;
	case 0x2C: break;
	case 0x2D: break;
	case 0x2E: break;
	case 0x2F: break;
	case 0x30: break;
	case 0x31: break;
	case 0x32: break;
	case 0x33: break;
	case 0x34: break;
	case 0x35: break;
	case 0x36: break;
	case 0x37: break;
	case 0x38: break;
	case 0x39: break;
	case 0x3A: break;
	case 0x3B: break;
	case 0x3C: break;
	case 0x3D: break;
	case 0x3E: break;
	case 0x3F: break;
	case 0x40: break;
	case 0x41: break;
	case 0x42: break;
	case 0x43: break;
	case 0x44: break;
	case 0x45: break;
	case 0x46: break;
	case 0x47: break;
	case 0x48: break;
	case 0x49: break;
	case 0x4A: break;
	case 0x4B: break;
	case 0x4C: break;
	case 0x4D: break;
	case 0x4E: break;
	case 0x4F: break;
	case 0x50: break;
	case 0x51: break;
	case 0x52: break;
	case 0x53: break;
	case 0x54: break;
	case 0x55: break;
	case 0x56: break;
	case 0x57: break;
	case 0x58: break;
	case 0x59: break;
	case 0x5A: break;
	case 0x5B: break;
	case 0x5C: break;
	case 0x5D: break;
	case 0x5E: break;
	case 0x5F: break;
	case 0x60: break;
	case 0x61: break;
	case 0x62: break;
	case 0x63: break;
	case 0x64: break;
	case 0x65: break;
	case 0x66: break;
	case 0x67: break;
	case 0x68: break;
	case 0x69: break;
	case 0x6A: break;
	case 0x6B: break;
	case 0x6C: break;
	case 0x6D: break;
	case 0x6E: break;
	case 0x6F: break;
	case 0x70: break;
	case 0x71: break;
	case 0x72: break;
	case 0x73: break;
	case 0x74: break;
	case 0x75: break;
	case 0x76: break;
	case 0x77: break;
	case 0x78: break;
	case 0x79: break;
	case 0x7A: break;
	case 0x7B: break;
	case 0x7C: break;
	case 0x7D: break;
	case 0x7E: break;
	case 0x7F: break;
	case 0x80: break;
	case 0x81: break;
	case 0x82: break;
	case 0x83: break;
	case 0x84: break;
	case 0x85: break;
	case 0x86: break;
	case 0x87: break;
	case 0x88: break;
	case 0x89: break;
	case 0x8A: break;
	case 0x8B: break;
	case 0x8C: break;
	case 0x8D: break;
	case 0x8E: break;
	case 0x8F: break;
	case 0x90: break;
	case 0x91: break;
	case 0x92: break;
	case 0x93: break;
	case 0x94: break;
	case 0x95: break;
	case 0x96: break;
	case 0x97: break;
	case 0x98: break;
	case 0x99: break;
	case 0x9A: break;
	case 0x9B: break;
	case 0x9C: break;
	case 0x9D: break;
	case 0x9E: break;
	case 0x9F: break;
	case 0xA0: break;
	case 0xA1: break;
	case 0xA2: break;
	case 0xA3: break;
	case 0xA4: break;
	case 0xA5: break;
	case 0xA6: break;
	case 0xA7: break;
	case 0xA8: break;
	case 0xA9: break;
	case 0xAA: break;
	case 0xAB: break;
	case 0xAC: break;
	case 0xAD: break;
	case 0xAE: break;
	case 0xAF: break;
	case 0xB0: break;
	case 0xB1: break;
	case 0xB2: break;
	case 0xB3: break;
	case 0xB4: break;
	case 0xB5: break;
	case 0xB6: break;
	case 0xB7: break;
	case 0xB8: break;
	case 0xB9: break;
	case 0xBA: break;
	case 0xBB: break;
	case 0xBC: break;
	case 0xBD: break;
	case 0xBE: break;
	case 0xBF: break;
	case 0xC0: break;
	case 0xC1: break;
	case 0xC2: break;
	case 0xC3: break;
	case 0xC4: break;
	case 0xC5: break;
	case 0xC6: break;
	case 0xC7: break;
	case 0xC8: break;
	case 0xC9: break;
	case 0xCA: break;
	case 0xCB: break;
	case 0xCC: break;
	case 0xCD: break;
	case 0xCE: break;
	case 0xCF: break;
	case 0xD0: break;
	case 0xD1: break;
	case 0xD2: break;
	case 0xD3: break;
	case 0xD4: break;
	case 0xD5: break;
	case 0xD6: break;
	case 0xD7: break;
	case 0xD8: break;
	case 0xD9: break;
	case 0xDA: break;
	case 0xDB: break;
	case 0xDC: break;
	case 0xDD: break;
	case 0xDE: break;
	case 0xDF: break;
	case 0xE0: break;
	case 0xE1: break;
	case 0xE2: break;
	case 0xE3: break;
	case 0xE4: break;
	case 0xE5: break;
	case 0xE6: break;
	case 0xE7: break;
	case 0xE8: break;
	case 0xE9: break;
	case 0xEA: break;
	case 0xEB: break;
	case 0xEC: break;
	case 0xED: break;
	case 0xEE: break;
	case 0xEF: break;
	case 0xF0: break;
	case 0xF1: break;
	case 0xF2: break;
	case 0xF3: break;
	case 0xF4: break;
	case 0xF5: break;
	case 0xF6: break;
	case 0xF7: break;
	case 0xF8: break;
	case 0xF9: break;
	case 0xFA: break;
	case 0xFB: break;
	case 0xFC: break;
	case 0xFD: break;
	case 0xFE: break;
	case 0xFF: break;
	}
}

int CPU::getCycleCount() { return m_cycleCount; }

bool CPU::m_getZeroFlag() { return (AF.low & 0b10000000) >> 7; }
void CPU::m_setZeroFlag(bool value)
{
	if (!value)
		AF.low &= 0b01111111;
	else
		AF.low |= 0b10000000;
}

bool CPU::m_getCarryFlag() { return (AF.low & 0b00010000) >> 4; }
void CPU::m_setCarryFlag(bool value)
{
	if (!value)
		AF.low &= 0b11101111;
	else
		AF.low |= 0b00010000;
}

bool CPU::m_getHalfCarryFlag() { return (AF.low & 0b00100000) >> 5; }
void CPU::m_setHalfCarryFlag(bool value)
{
	if (!value)
		AF.low &= 0b11011111;
	else
		AF.low |= 0b00100000;
}

bool CPU::m_getSubtractFlag() { return (AF.low & 0b01000000) >> 6; }
void CPU::m_setSubtractFlag(bool value)
{
	if (!value)
		AF.low &= 0b10111111;
	else
		AF.low |= 0b01000000;
}

uint8_t CPU::m_fetch()
{
	uint8_t val = m_mmu->read(PC);
	PC++;
	return val;
}

void CPU::_loadImmPairRegister(Register& reg)
{
	reg.low = m_fetch();
	reg.high = m_fetch();
	m_cycleCount += 3;
}

void CPU::_loadImmRegister(uint8_t& reg)
{
	reg = m_fetch();
	m_cycleCount += 3;
}

void CPU::_loadImmFromRegister(uint8_t& regA, uint8_t& regB)
{

}

void CPU::_loadDirectFromPairRegister(uint8_t& regA, Register& regB)
{

}

void CPU::_loadDirectFromPairRegisterInc(uint8_t& regA, Register& regB)
{

}

void CPU::_loadDirectFromPairRegisterDec(uint8_t& regA, Register& regB)
{

}

void CPU::_storeRegisterAtPairRegister(Register& regA, uint8_t& regB)
{

}

void CPU::_storeRegisterAtPairRegisterInc(Register& regA, uint8_t& regB)
{

}

void CPU::_storeRegisterAtPairRegisterDec(Register& regA, uint8_t& regB)
{

}

void CPU::_storeRegisterAtPairRegister(Register& regA, uint8_t& regB)
{
	m_mmu->write(regA.reg, regB);
	m_cycleCount += 2;
}

void CPU::_storePairRegisterAtAddress(Register& reg)
{
	uint8_t memLow = m_fetch();
	uint8_t memHigh = m_fetch();
	uint16_t addr = (memHigh << 8) | memLow;

	m_mmu->write(addr, reg.low);
	m_mmu->write(addr + 1, reg.high);
	m_cycleCount += 5;
}

void CPU::_storeOperandAtPairAddress(Register& reg)
{

}

void CPU::_storeRegisterInHRAM(uint8_t& regDst, uint8_t& regSrc)
{

}

void CPU::_loadFromHRAM(uint8_t& regDst, uint8_t& regSrcIdx)
{

}

void CPU::_incrementPairRegister(Register& reg)
{
	reg.reg++;
	m_cycleCount += 2;
}
void CPU::_incrementRegister(uint8_t& reg)
{
	m_setHalfCarryFlag(((reg & 0x0F) + (1 & 0x0F)) > 0x0F);
	reg += 1;
	m_setZeroFlag(!reg);
	m_setSubtractFlag(false);
	m_cycleCount++;
}


void CPU::_decrementRegister(uint8_t& reg)
{
	reg--;
	m_setHalfCarryFlag((reg & 0x0f) == 0x0f);
	m_setZeroFlag(!reg);
	m_setSubtractFlag(true);
	m_cycleCount++;
}

void CPU::_addPairRegisters(Register& regA, Register& regB)
{
	m_setHalfCarryFlag(((regA.reg & 0xfff) + (regB.reg & 0xfff)) > 0xfff);
	m_setCarryFlag(((int)regA.reg + (int)regB.reg) > 0xffff);
	m_setSubtractFlag(false);
	regA.reg += regB.reg;
	m_cycleCount += 2;
}

void CPU::_jumpRelative()
{
	int8_t disp = (int8_t)m_fetch();	//cast unsigned int value to signed int value as displacement is signed/relative
	PC += disp;
	m_cycleCount += 3;
}

void CPU::_jumpRelativeIfZeroNotSet()	
{
	if (!m_getZeroFlag())
		_jumpRelative();				//if flag isn't set then 3-cycle relative jump takes place (same operation), otherwise cycles += 2
	else
		m_cycleCount += 2;
}

void CPU::_jumpRelativeIfZeroSet()
{
	if (m_getZeroFlag())
		_jumpRelative();
	else
		m_cycleCount += 2;
}

void CPU::_jumpRelativeIfCarryNotSet()
{
	if (!m_getCarryFlag())
		_jumpRelative();
	else
		m_cycleCount += 2;
}

void CPU::_jumpRelativeIfCarrySet()
{
	if (m_getCarryFlag())
		_jumpRelative();
	else
		m_cycleCount += 2;
}

void CPU::_jumpAbsolute()
{
	uint8_t byteLow = m_fetch();
	uint8_t byteHigh = m_fetch();
	uint16_t addr = ((uint16_t)byteHigh << 8) | byteLow;

	PC = addr;

	m_cycleCount += 4;
}

void CPU::_jumpAbsoluteIfZeroNotSet()
{
	if (!m_getZeroFlag())
		_jumpAbsolute();
	else
		m_cycleCount += 3;
}

void CPU::_jumpAbsoluteIfZeroSet()
{
	if (m_getZeroFlag())
		_jumpAbsolute();
	else
		m_cycleCount += 3;
}

void CPU::_jumpAbsoluteIfCarryNotSet()
{
	if (!m_getCarryFlag())
		_jumpAbsolute();
	else
		m_cycleCount += 3;
}

void CPU::_jumpAbsoluteIfCarrySet()
{
	if (m_getCarryFlag())
		_jumpAbsolute();
	else
		m_cycleCount += 3;
}

void CPU::_call()
{

}

void CPU::_callIfZeroNotSet()
{

}

void CPU::_callIfZeroSet()
{

}

void CPU::_callIfCarryNotSet()
{

}

void CPU::_callIfCarrySet()
{

}

void CPU::_returnIfZeroNotSet()
{

}

void CPU::_returnIfZeroSet()
{

}

void CPU::_returnIfCarryNotSet()
{

}

void CPU::_returnIfCarrySet()
{

}

void CPU::_return()
{

}

void CPU::_returnFromInterrupt()
{

}

void CPU::_setCarryFlag()
{

}

void CPU::_clearCarryFlag()
{

}



void CPU::_addRegisters(uint8_t& regA, uint8_t& regB)
{

}

void CPU::_addPairAddress(uint8_t& regA, Register& regB)
{

}

void CPU::_addRegistersCarry(uint8_t& regA, uint8_t& regB)
{

}

void CPU::_addPairAddressCarry(uint8_t& regA, Register& regB)
{

}

void CPU::_addValue(uint8_t& reg)
{

}

void CPU::_addValueCarry(uint8_t& reg)
{

}

void CPU::_subRegister(uint8_t& reg)
{

}

void CPU::_subRegisterCarry(uint8_t& reg)
{

}

void CPU::_subPairAddress(Register& reg)
{

}

void CPU::_subPairAddressCarry(Register& reg)
{

}

void CPU::_subValue()
{

}

void CPU::_subValueCarry()
{

}

void CPU::_andRegister(uint8_t& reg)
{

}

void CPU::_andPairAddress(Register& reg)
{

}

void CPU::_andValue()
{

}

void CPU::_xorRegister(uint8_t& reg)
{

}

void CPU::_xorPairAddress(Register& reg)
{

}

void CPU::_xorValue()
{

}

void CPU::_orRegister(uint8_t& reg)
{

}

void CPU::_orPairAddress(Register& reg)
{

}

void CPU::_orValue()
{

}

void CPU::_compareRegister(uint8_t& reg)
{

}

void CPU::_comparePairAddress(Register& reg)
{

}

void CPU::_compareValue()
{

}

void CPU::_pushPairRegister(Register& reg)
{

}

void CPU::_popToPairRegister(Register& reg)
{

}

//some misc instructions
void CPU::_disableInterrupts()
{

}

void CPU::_enableInterrupts()
{

}

void CPU::_stop()
{

}

void CPU::_halt()
{

}

void CPU::_resetToVector(uint8_t vectorIdx)
{

}

void CPU::_adjustBCD()
{

}

void CPU::_complement()
{

}

void CPU::_rotateALeft()
{

}

void CPU::_rotateALeftCarry()
{

}