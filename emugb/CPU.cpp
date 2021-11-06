#include"CPU.h"

CPU::CPU(MMU* mmu)
{
	m_mmu = mmu;
	AF = {}; BC = {}; DE = {}; HL = {}, SP = {};
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
	case 0x0: m_cycleCount++; break;
	case 0x1: _loadImmPairRegister(BC); break;
	case 0x2: _storeRegisterAtPairRegister(BC, AF.high); break;
	case 0x3: _incrementPairRegister(BC); break;
	case 0x4: _incrementRegister(BC.high); break;
	case 0x5: _decrementRegister(BC.high); break;
	case 0x6: _loadImmRegister(BC.high); break;
	case 0x7: _RLCA(); break;
	case 0x8: _storePairRegisterAtAddress(SP); break;
	case 0x9: _addPairRegisters(HL, BC); break;
	case 0xA: _loadDirectFromPairRegister(AF.high, BC); break;
	case 0xB:  _decrementPairRegister(BC); break;
	case 0xC: _incrementRegister(BC.low); break;
	case 0xD: _decrementRegister(BC.low); break;
	case 0xE: _loadImmRegister(BC.low); break;
	case 0xF: _RRCA(); break;
	case 0x10: _stop(); break;
	case 0x11: _loadImmPairRegister(DE); break;
	case 0x12: _storeRegisterAtPairRegister(DE, AF.high); break;
	case 0x13: _incrementPairRegister(DE); break;
	case 0x14: _incrementRegister(DE.high); break;
	case 0x15: _decrementRegister(DE.high); break;
	case 0x16: _loadImmRegister(DE.high); break;
	case 0x17: _RLA(); break;
	case 0x18: _jumpRelative(); break;
	case 0x19: _addPairRegisters(HL, DE); break;
	case 0x1A: _loadDirectFromPairRegister(AF.high, DE); break;
	case 0x1B: _decrementPairRegister(DE); break;
	case 0x1C: _incrementRegister(DE.low); break;
	case 0x1D: _decrementRegister(DE.low); break;
	case 0x1E: _loadImmRegister(DE.low); break;
	case 0x1F: _RRA(); break;
	case 0x20: _jumpRelativeIfZeroNotSet(); break;
	case 0x21: _loadImmPairRegister(HL); break;
	case 0x22: _storeRegisterAtPairRegisterInc(HL, AF.high); break;
	case 0x23: _incrementPairRegister(HL); break;
	case 0x24: _incrementRegister(HL.high); break;
	case 0x25: _decrementRegister(HL.high); break;
	case 0x26: _loadImmRegister(HL.high); break;
	case 0x27: _adjustBCD(); break;
	case 0x28: _jumpRelativeIfZeroSet(); break;
	case 0x29: _addPairRegisters(HL, HL); break;
	case 0x2A: _loadDirectFromPairRegisterInc(AF.high, HL); break;
	case 0x2B: _decrementPairRegister(HL); break;
	case 0x2C: _incrementRegister(HL.low); break;
	case 0x2D: _decrementRegister(HL.low); break;
	case 0x2E: _loadImmRegister(HL.low); break;
	case 0x2F: _complement(); break;
	case 0x30: _jumpRelativeIfCarryNotSet(); break;
	case 0x31: _loadImmPairRegister(SP); break;
	case 0x32: _storeRegisterAtPairRegisterDec(HL, AF.high); break;
	case 0x33: _incrementPairRegister(SP); break;
	case 0x34: _incrementPairAddress(HL); break;
	case 0x35: _decrementPairAddress(HL); break;
	case 0x36: _storeOperandAtPairAddress(HL); break;
	case 0x37: _setCarryFlag(); break;
	case 0x38: _jumpRelativeIfCarrySet(); break;
	case 0x39: _addPairRegisters(HL, SP); break;
	case 0x3A: _loadDirectFromPairRegisterDec(AF.high, HL); break;
	case 0x3B: _decrementPairRegister(SP); break;
	case 0x3C: _incrementRegister(AF.high); break;
	case 0x3D: _decrementRegister(AF.high); break;
	case 0x3E: _loadImmRegister(AF.high); break;
	case 0x3F: _flipCarryFlag(); break;
	case 0x40: _loadImmFromRegister(BC.high, BC.high); break;
	case 0x41: _loadImmFromRegister(BC.high, BC.low); break;
	case 0x42: _loadImmFromRegister(BC.high, DE.high); break;
	case 0x43: _loadImmFromRegister(BC.high, DE.low); break;
	case 0x44: _loadImmFromRegister(BC.high, HL.high); break;
	case 0x45: _loadImmFromRegister(BC.high, HL.low); break;
	case 0x46: _loadDirectFromPairRegister(BC.high, HL); break;
	case 0x47: _loadImmFromRegister(BC.high, AF.high); break;
	case 0x48: _loadImmFromRegister(BC.low, BC.high); break;
	case 0x49: _loadImmFromRegister(BC.low, BC.low); break;
	case 0x4A: _loadImmFromRegister(BC.low, DE.high); break;
	case 0x4B: _loadImmFromRegister(BC.low, DE.low); break;
	case 0x4C: _loadImmFromRegister(BC.low, HL.high); break;
	case 0x4D: _loadImmFromRegister(BC.low, HL.low); break;
	case 0x4E: _loadDirectFromPairRegister(BC.low, HL); break;
	case 0x4F: _loadImmFromRegister(BC.low, AF.high); break;
	case 0x50: _loadImmFromRegister(DE.high, BC.high); break;
	case 0x51: _loadImmFromRegister(DE.high, BC.low); break;
	case 0x52: _loadImmFromRegister(DE.high, DE.high); break;
	case 0x53: _loadImmFromRegister(DE.high, DE.low); break;
	case 0x54: _loadImmFromRegister(DE.high, HL.high); break;
	case 0x55: _loadImmFromRegister(DE.high, HL.low); break;
	case 0x56: _loadDirectFromPairRegister(DE.high, HL); break;
	case 0x57: _loadImmFromRegister(DE.high, AF.high); break;
	case 0x58: _loadImmFromRegister(DE.low, BC.high); break;
	case 0x59: _loadImmFromRegister(DE.low, BC.low); break;
	case 0x5A: _loadImmFromRegister(DE.low, DE.high); break;
	case 0x5B: _loadImmFromRegister(DE.low, DE.low); break;
	case 0x5C: _loadImmFromRegister(DE.low, HL.high); break;
	case 0x5D: _loadImmFromRegister(DE.low, HL.low); break;
	case 0x5E: _loadDirectFromPairRegister(DE.low, HL); break;
	case 0x5F: _loadImmFromRegister(DE.low, AF.high); break;
	case 0x60: _loadImmFromRegister(HL.high, BC.high); break;
	case 0x61: _loadImmFromRegister(HL.high, BC.low); break;
	case 0x62: _loadImmFromRegister(HL.high, DE.high); break;
	case 0x63: _loadImmFromRegister(HL.high, DE.low); break;
	case 0x64: _loadImmFromRegister(HL.high, HL.high); break;
	case 0x65: _loadImmFromRegister(HL.high, HL.low); break;
	case 0x66: _loadDirectFromPairRegister(HL.high, HL); break;
	case 0x67: _loadImmFromRegister(HL.high, AF.high); break;
	case 0x68: _loadImmFromRegister(HL.low, BC.high); break;
	case 0x69: _loadImmFromRegister(HL.low, BC.low); break;
	case 0x6A: _loadImmFromRegister(HL.low, DE.high); break;
	case 0x6B: _loadImmFromRegister(HL.low, DE.low); break;
	case 0x6C: _loadImmFromRegister(HL.low, HL.high); break;
	case 0x6D: _loadImmFromRegister(HL.low, HL.low); break;
	case 0x6E: _loadDirectFromPairRegister(HL.low, HL); break;
	case 0x6F: _loadImmFromRegister(HL.low, AF.high); break;
	case 0x70: _storeRegisterAtPairRegister(HL, BC.high); break;
	case 0x71: _storeRegisterAtPairRegister(HL, BC.low); break;
	case 0x72: _storeRegisterAtPairRegister(HL, DE.high); break;
	case 0x73: _storeRegisterAtPairRegister(HL, DE.low); break;
	case 0x74: _storeRegisterAtPairRegister(HL, HL.high); break;
	case 0x75: _storeRegisterAtPairRegister(HL, HL.low); break;
	case 0x76: _halt(); break;
	case 0x77: _storeRegisterAtPairRegister(HL, AF.high); break;
	case 0x78: _loadImmFromRegister(AF.high, BC.high); break;
	case 0x79: _loadImmFromRegister(AF.high, BC.low); break;
	case 0x7A: _loadImmFromRegister(AF.high, DE.high); break;
	case 0x7B: _loadImmFromRegister(AF.high, DE.low); break;
	case 0x7C: _loadImmFromRegister(AF.high, HL.high); break;
	case 0x7D: _loadImmFromRegister(AF.high, HL.low); break;
	case 0x7E: _loadDirectFromPairRegister(AF.high, HL); break;
	case 0x7F: _loadImmFromRegister(AF.high, AF.high); break;
	case 0x80: _addRegisters(AF.high, BC.high); break;
	case 0x81: _addRegisters(AF.high, BC.low); break;
	case 0x82: _addRegisters(AF.high, DE.high); break;
	case 0x83: _addRegisters(AF.high, DE.low); break;
	case 0x84: _addRegisters(AF.high, HL.high); break;
	case 0x85: _addRegisters(AF.high, HL.low); break;
	case 0x86: _addPairAddress(AF.high, HL); break;
	case 0x87: _addRegisters(AF.high, AF.high); break;
	case 0x88: _addRegistersCarry(AF.high, BC.high); break;
	case 0x89: _addRegistersCarry(AF.high, BC.low); break;
	case 0x8A: _addRegistersCarry(AF.high, DE.high); break;
	case 0x8B: _addRegistersCarry(AF.high, DE.low); break;
	case 0x8C: _addRegistersCarry(AF.high, HL.high); break;
	case 0x8D: _addRegistersCarry(AF.high, HL.low); break;
	case 0x8E: _addPairAddressCarry(AF.high, HL); break;
	case 0x8F: _addRegistersCarry(AF.high, AF.high); break;
	case 0x90: _subRegister(BC.high); break;
	case 0x91: _subRegister(BC.low); break;
	case 0x92: _subRegister(DE.high); break;
	case 0x93: _subRegister(DE.low); break;
	case 0x94: _subRegister(HL.high); break;
	case 0x95: _subRegister(HL.low); break;
	case 0x96: _subPairAddress(HL); break;
	case 0x97: _subRegister(AF.high); break;
	case 0x98: _subRegisterCarry(BC.high); break;
	case 0x99: _subRegisterCarry(BC.low); break;
	case 0x9A: _subRegisterCarry(DE.high); break;
	case 0x9B: _subRegisterCarry(DE.low); break;
	case 0x9C: _subRegisterCarry(HL.high); break;
	case 0x9D: _subRegisterCarry(HL.low); break;
	case 0x9E: _subPairAddressCarry(HL); break;
	case 0x9F: _subRegisterCarry(AF.high); break;
	case 0xA0: _andRegister(BC.high); break;
	case 0xA1: _andRegister(BC.low); break;
	case 0xA2: _andRegister(DE.high); break;
	case 0xA3: _andRegister(DE.low); break;
	case 0xA4: _andRegister(HL.high); break;
	case 0xA5: _andRegister(HL.low); break;
	case 0xA6: _andPairAddress(HL); break;
	case 0xA7: _andRegister(AF.high); break;
	case 0xA8: _xorRegister(BC.high); break;
	case 0xA9: _xorRegister(BC.low); break;
	case 0xAA: _xorRegister(DE.high); break;
	case 0xAB: _xorRegister(DE.low); break;
	case 0xAC: _xorRegister(HL.high); break;
	case 0xAD: _xorRegister(HL.low); break;
	case 0xAE: _xorPairAddress(HL); break;
	case 0xAF: _xorRegister(AF.high); break;
	case 0xB0: _orRegister(BC.high); break;
	case 0xB1: _orRegister(BC.low); break;
	case 0xB2: _orRegister(DE.high); break;
	case 0xB3: _orRegister(DE.low); break;
	case 0xB4: _orRegister(HL.high); break;
	case 0xB5: _orRegister(HL.low); break;
	case 0xB6: _orPairAddress(HL); break;
	case 0xB7: _orRegister(AF.high); break;
	case 0xB8: _compareRegister(BC.high); break;
	case 0xB9: _compareRegister(BC.low); break;
	case 0xBA: _compareRegister(DE.high); break;
	case 0xBB: _compareRegister(DE.low); break;
	case 0xBC: _compareRegister(HL.high); break;
	case 0xBD: _compareRegister(HL.low); break;
	case 0xBE: _comparePairAddress(HL); break;
	case 0xBF: _compareRegister(AF.high); break;
	case 0xC0: _returnIfZeroNotSet(); break;
	case 0xC1: _popToPairRegister(BC); break;
	case 0xC2: _jumpAbsoluteIfZeroNotSet(); break;
	case 0xC3: _jumpAbsolute(); break;
	case 0xC4: _callIfZeroNotSet(); break;
	case 0xC5: _pushPairRegister(BC); break;
	case 0xC6: _addValue(AF.high); break;
	case 0xC7: _resetToVector(0); break;
	case 0xC8: _returnIfZeroSet(); break;
	case 0xC9: _return(); break;
	case 0xCA: _jumpAbsoluteIfZeroSet(); break;
	case 0xCB: m_executePrefixedInstruction(); break;	//Special 16-bit prefixed instruction decoder called instead
	case 0xCC: _callIfZeroSet(); break;
	case 0xCD: _call(); break;
	case 0xCE: _addValueCarry(AF.high); break;
	case 0xCF: _resetToVector(1); break;
	case 0xD0: _returnIfCarryNotSet(); break;
	case 0xD1: _popToPairRegister(DE); break;
	case 0xD2: _jumpAbsoluteIfCarryNotSet(); break;
//	case 0xD3: break;									Invalid opcode decoding D3
	case 0xD4: _callIfCarryNotSet(); break;
	case 0xD5: _pushPairRegister(DE); break;
	case 0xD6: _subValue(); break;
	case 0xD7: _resetToVector(2); break;
	case 0xD8: _returnIfCarrySet(); break;
	case 0xD9: _returnFromInterrupt(); break;
	case 0xDA: _jumpAbsoluteIfCarrySet(); break;
//	case 0xDB: break;                                   Invalid opcode decoding DB
	case 0xDC: _callIfCarrySet(); break;
//	case 0xDD: break;                                   Invalid opcode decoding DD
	case 0xDE: _subValueCarry(); break;
	case 0xDF: _resetToVector(3); break;
	case 0xE0: _storeRegisterInHRAMImm(AF.high); break;
	case 0xE1: _popToPairRegister(HL); break;
	case 0xE2: _storeRegisterInHRAM(BC.low, AF.high); break;
//	case 0xE3: break;                                   Invalid opcode decoding E3
//	case 0xE4: break;                                   Invalid opcode decoding E4
	case 0xE5: _pushPairRegister(HL); break;
	case 0xE6: _andValue(); break;
	case 0xE7: _resetToVector(4); break;
	case 0xE8: _addSignedValueToPairRegister(SP); break;
	case 0xE9: PC = HL.reg; m_cycleCount++;  break;	//lazy implementation but instruction is 1-cycle anyway
	case 0xEA: _storeRegisterAtAddress(AF.high); break;
//	case 0xEB: break;                                   Invalid opcode decoding EB
//	case 0xEC: break;                                   Invalid opcode decoding EC
//	case 0xED: break;                                   Invalid opcode decoding ED
	case 0xEE: _xorValue(); break;
	case 0xEF: _resetToVector(5); break;
	case 0xF0: _loadFromHRAMImm(AF.high); break;
	case 0xF1: _popToPairRegister(AF); break;
	case 0xF2: _loadFromHRAM(AF.high, BC.low); break;
	case 0xF3: _disableInterrupts(); break;
//	case 0xF4: break;                                   Invalid opcode decoding F4
	case 0xF5: _pushPairRegister(AF); break;
	case 0xF6: _orValue(); break;
	case 0xF7: _resetToVector(6); break;
	case 0xF8: _loadHLStackIdx(); break;
	case 0xF9: SP.reg = HL.reg; m_cycleCount += 2; break;	
	case 0xFA: _loadRegisterFromAddress(AF.high); break;
	case 0xFB: _enableInterrupts(); break;
//	case 0xFC: break;                                   Invalid opcode decoding FC
//	case 0xFD: break;                                   Invalid opcode decoding FD
	case 0xFE: _compareValue(); break;
	case 0xFF: _resetToVector(7); break;
	default: Logger::getInstance()->msg(LoggerSeverity::Error, "Invalid opcode " + std::to_string(opcode));
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

void CPU::m_pushToStack(uint16_t val)
{
	uint8_t highByte = (val & 0xFF00) >> 8;
	uint8_t lowByte = (val & 0x00FF);

	m_mmu->write(SP.reg - 1, highByte);
	m_mmu->write(SP.reg - 2, lowByte);
	SP.reg -= 2;
}

uint16_t CPU::m_popFromStack()
{
	uint8_t lowByte = m_mmu->read(SP.reg);
	uint8_t highByte = m_mmu->read(SP.reg + 1);
	SP.reg += 2;
	uint16_t val = (((uint16_t)highByte) << 8) | (uint16_t)lowByte;
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
	regA = regB;
	m_cycleCount += 1;
}

void CPU::_loadDirectFromPairRegister(uint8_t& regA, Register& regB)
{
	regA = m_mmu->read(regB.reg);
	m_cycleCount += 2;
}

void CPU::_loadDirectFromPairRegisterInc(uint8_t& regA, Register& regB)
{
	regA = m_mmu->read(regB.reg);
	regB.reg++;
	m_cycleCount += 2;
}

void CPU::_loadDirectFromPairRegisterDec(uint8_t& regA, Register& regB)
{
	regA = m_mmu->read(regB.reg);
	regB.reg--;
	m_cycleCount += 2;
}

void CPU::_storeRegisterAtPairRegister(Register& regA, uint8_t& regB)
{
	m_mmu->write(regA.reg, regB);
	m_cycleCount += 2;
}

void CPU::_storeRegisterAtPairRegisterInc(Register& regA, uint8_t& regB)
{
	m_mmu->write(regA.reg, regB);
	regA.reg++;
	m_cycleCount += 2;
}

void CPU::_storeRegisterAtPairRegisterDec(Register& regA, uint8_t& regB)
{
	m_mmu->write(regA.reg, regB);
	regA.reg--;
	m_cycleCount += 2;
}

void CPU::_storePairRegisterAtAddress(Register& reg)
{
	uint8_t memLow = m_fetch();
	uint8_t memHigh = m_fetch();
	uint16_t addr = ((uint16_t)memHigh << 8) | memLow;

	m_mmu->write(addr, reg.low);
	m_mmu->write(addr + 1, reg.high);
	m_cycleCount += 5;
}

void CPU::_storeRegisterAtAddress(uint8_t& reg)
{
	uint8_t memLow = m_fetch();
	uint8_t memHigh = m_fetch();
	uint16_t addr = ((uint16_t)memHigh << 8) | memLow;
	m_mmu->write(addr, reg);
	m_cycleCount += 4;
}

void CPU::_loadRegisterFromAddress(uint8_t& reg)
{
	uint8_t memLow = m_fetch();
	uint8_t memHigh = m_fetch();
	uint16_t addr = ((uint16_t)memHigh << 8) | memLow;
	reg = m_mmu->read(addr);
	m_cycleCount += 4;
}

void CPU::_storeOperandAtPairAddress(Register& reg)
{
	m_mmu->write(reg.reg, m_fetch());
	m_cycleCount += 3;
}

void CPU::_storeRegisterInHRAM(uint8_t& regDst, uint8_t& regSrc)
{
	m_mmu->write(0xFF00 + regDst, regSrc);	//destination register contains index from 0xFF00 to write to HRAM
	m_cycleCount += 2;
}

void CPU::_loadFromHRAM(uint8_t& regDst, uint8_t& regSrcIdx)
{
	regDst = m_mmu->read(0xFF00 + regSrcIdx);
	m_cycleCount += 2;
}

void CPU::_storeRegisterInHRAMImm(uint8_t& reg)
{
	uint16_t addr = (uint16_t)m_fetch();
	m_mmu->write(0xFF00 + addr, reg);
	m_cycleCount += 3;
}

void CPU::_loadFromHRAMImm(uint8_t& reg)
{
	uint16_t addr = (uint16_t)m_fetch();
	reg = m_mmu->read(0xFF00 + addr);
	m_cycleCount += 3;
}

void CPU::_incrementPairRegister(Register& reg)
{
	reg.reg++;
	m_cycleCount += 2;
}

void CPU::_incrementPairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	 
	m_setHalfCarryFlag(((val & 0x0F) + (1 & 0x0F)) > 0x0F);
	val += 1;
	m_setZeroFlag(!val);
	m_setSubtractFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 3;
}

void CPU::_incrementRegister(uint8_t& reg)
{
	m_setHalfCarryFlag(((reg & 0x0F) + (1 & 0x0F)) > 0x0F);
	reg += 1;
	m_setZeroFlag(!reg);
	m_setSubtractFlag(false);
	m_cycleCount++;
}
void CPU::_decrementPairRegister(Register& reg)
{
	reg.reg--;
	m_cycleCount += 2;
}

void CPU::_decrementPairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);

	val--;
	m_setHalfCarryFlag((val & 0x0f) == 0x0f);
	m_setZeroFlag(!val);
	m_setSubtractFlag(true);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 3;
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

void CPU::_addSignedValueToPairRegister(Register& reg)
{
	int8_t val = (int8_t)m_fetch();
	uint16_t regInitial = reg.reg;
	reg.reg += val;
	m_setZeroFlag(false);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(((regInitial ^ val ^ (reg.reg & 0xFFFF)) & 0x10) == 0x10);	//black magic 16-bit half carry
	m_setCarryFlag(((regInitial ^ val ^ (reg.reg & 0xFFFF)) & 0x100) == 0x100);
	m_cycleCount += 4;
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
	uint8_t addrLow = m_fetch();
	uint8_t addrHigh = m_fetch();
	uint16_t addr = (((uint16_t)addrHigh) << 8) | (uint16_t)addrLow;

	m_pushToStack(PC);
	PC = addr;

	m_cycleCount += 6;
}

void CPU::_callIfZeroNotSet()
{
	if (!m_getZeroFlag())
		_call();
	else
		m_cycleCount += 3;
}

void CPU::_callIfZeroSet()
{
	if (m_getZeroFlag())
		_call();
	else
		m_cycleCount += 3;
}

void CPU::_callIfCarryNotSet()
{
	if (!m_getCarryFlag())
		_call();
	else
		m_cycleCount += 3;
}

void CPU::_callIfCarrySet()
{
	if (m_getCarryFlag())
		_call();
	else
		m_cycleCount += 3;
}

void CPU::_return()
{
	uint16_t returnAddress = m_popFromStack();
	PC = returnAddress;
	m_cycleCount += 4;
}

void CPU::_returnIfZeroNotSet()
{
	if (!m_getZeroFlag())
	{
		_return();
		m_cycleCount++;	//conditional RET takes one more cycle than unconditional RET
	}
	else
		m_cycleCount += 2;
}

void CPU::_returnIfZeroSet()
{
	if (m_getZeroFlag())
	{
		_return();
		m_cycleCount++;	//conditional RET takes one more cycle than unconditional RET
	}
	else
		m_cycleCount += 2;
}

void CPU::_returnIfCarryNotSet()
{
	if (!m_getCarryFlag())
	{
		_return();
		m_cycleCount++;	//conditional RET takes one more cycle than unconditional RET
	}
	else
		m_cycleCount += 2;
}

void CPU::_returnIfCarrySet()
{
	if (m_getCarryFlag())
	{
		_return();
		m_cycleCount++;	//conditional RET takes one more cycle than unconditional RET
	}
	else
		m_cycleCount += 2;
}

void CPU::_returnFromInterrupt()
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "RETI not implemented! Interrupts aren't currently supported.");
	m_cycleCount += 4;
}

void CPU::_setCarryFlag()
{
	m_setCarryFlag(true);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 1;
}

void CPU::_flipCarryFlag()
{
	m_setCarryFlag(!m_getCarryFlag());
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 1;
}



void CPU::_addRegisters(uint8_t& regA, uint8_t& regB)
{
	m_setHalfCarryFlag(((regA & 0x0F) + (regB & 0x0F)) > 0x0F);
	m_setCarryFlag(((int)regA + (int)regB) > 0xFF);
	m_setSubtractFlag(false);

	regA += regB;
	m_setZeroFlag(regA == 0);

	m_cycleCount += 1;
}

void CPU::_addPairAddress(uint8_t& regA, Register& regB)
{
	uint8_t val = m_mmu->read(regB.reg);

	m_setHalfCarryFlag(((regA & 0x0F) + (val & 0x0F)) > 0x0F);
	m_setCarryFlag(((int)regA + (int)val) > 0xFF);
	m_setSubtractFlag(false);

	regA += val;
	m_setZeroFlag(regA == 0);

	m_cycleCount += 2;
}

void CPU::_addRegistersCarry(uint8_t& regA, uint8_t& regB)
{
	uint8_t lastCarryFlag = m_getCarryFlag();	//save carry flag
	m_setHalfCarryFlag(((regA & 0x0F) + (regB & 0x0F) + (lastCarryFlag & 0x0F)) > 0x0F);
	m_setCarryFlag(((int)regA + (int)regB + (int)lastCarryFlag) > 0xFF);
	m_setSubtractFlag(false);

	regA += regB + lastCarryFlag;
	m_setZeroFlag(regA == 0);
	m_cycleCount += 1;
}

void CPU::_addPairAddressCarry(uint8_t& regA, Register& regB)
{
	uint8_t val = m_mmu->read(regB.reg);
	uint8_t lastCarryFlag = m_getCarryFlag();
	m_setHalfCarryFlag(((regA & 0x0F) + (val & 0x0F) + (lastCarryFlag & 0x0F)) > 0x0F);
	m_setCarryFlag(((int)regA + (int)val + (int)lastCarryFlag) > 0xFF);
	m_setSubtractFlag(false);

	regA += val + lastCarryFlag;
	m_setZeroFlag(regA == 0);
	m_cycleCount += 2;

	m_cycleCount += 2;
}

void CPU::_addValue(uint8_t& reg)
{
	uint8_t val = m_fetch();

	m_setHalfCarryFlag(((reg & 0x0F) + (val & 0x0F)) > 0x0F);
	m_setCarryFlag(((int)reg + (int)val) > 0xFF);
	m_setSubtractFlag(false);

	reg += val;
	m_setZeroFlag(reg == 0);
	
	m_cycleCount += 2;
}

void CPU::_addValueCarry(uint8_t& reg)
{
	uint8_t val = m_fetch();
	uint8_t lastCarryFlag = m_getCarryFlag();

	m_setHalfCarryFlag(((reg & 0x0F) + (val & 0x0F) + (lastCarryFlag & 0x0F)) > 0x0F);
	m_setCarryFlag(((int)reg + (int)val + (int)lastCarryFlag) > 0xFF);
	m_setSubtractFlag(false);

	reg += val;
	m_setZeroFlag(reg == 0);

	m_cycleCount += 2;
}

void CPU::_subRegister(uint8_t& reg)
{
	m_setCarryFlag(AF.high < reg);
	m_setHalfCarryFlag(((AF.high & 0xf) - (reg & 0xf)) & 0x10);
	m_setSubtractFlag(true);

	AF.high -= reg;
	m_setZeroFlag(AF.high == 0);

	m_cycleCount += 1;
}

void CPU::_subRegisterCarry(uint8_t& reg)
{
	uint8_t lastCarryFlag = m_getCarryFlag();
	m_setCarryFlag(AF.high < reg);
	m_setHalfCarryFlag(((AF.high & 0xf) - (reg & 0xf) - (lastCarryFlag & 0xf)) & 0x10);
	m_setSubtractFlag(true);

	AF.high -= (reg + lastCarryFlag);
	m_setZeroFlag(AF.high == 0);

	m_cycleCount += 1;
}

void CPU::_subPairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	m_setCarryFlag(AF.high < val);
	m_setHalfCarryFlag(((AF.high & 0xf) - (val & 0xf)) & 0x10);
	m_setSubtractFlag(true);

	AF.high -= val;
	m_setZeroFlag(AF.high == 0);

	m_cycleCount += 2;
}

void CPU::_subPairAddressCarry(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t lastCarryFlag = m_getCarryFlag();
	m_setCarryFlag(AF.high < val);
	m_setHalfCarryFlag(((AF.high & 0xf) - (val & 0xf) - (lastCarryFlag & 0xf)) & 0x10);
	m_setSubtractFlag(true);

	AF.high -= (val + lastCarryFlag);
	m_setZeroFlag(AF.high == 0);

	m_cycleCount += 2;
}

void CPU::_subValue()
{
	uint8_t val = m_fetch();
	m_setCarryFlag(AF.high < val);
	m_setHalfCarryFlag(((AF.high & 0xf) - (val & 0xf)) & 0x10);
	m_setSubtractFlag(true);

	AF.high -= val;
	m_setZeroFlag(AF.high == 0);

	m_cycleCount += 2;
}

void CPU::_subValueCarry()
{
	uint8_t val = m_fetch();
	uint8_t lastCarryFlag = m_getCarryFlag();
	m_setCarryFlag(AF.high < val);
	m_setHalfCarryFlag(((AF.high & 0xf) - (val & 0xf) - (lastCarryFlag & 0xf)) & 0x10);
	m_setSubtractFlag(true);

	AF.high -= (val + lastCarryFlag);
	m_setZeroFlag(AF.high == 0);

	m_cycleCount += 2;
}

void CPU::_andRegister(uint8_t& reg)
{
	AF.high &= reg;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(true);
	m_setCarryFlag(false);
	m_cycleCount += 1;
}

void CPU::_andPairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	AF.high &= val;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(true);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_andValue()
{
	uint8_t val = m_fetch();
	AF.high &= val;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(true);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_xorRegister(uint8_t& reg)
{
	AF.high ^= reg;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 1;
}

void CPU::_xorPairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	AF.high ^= val;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_xorValue()
{
	uint8_t val = m_fetch();
	AF.high ^= val;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_orRegister(uint8_t& reg)
{
	AF.high |= reg;
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 1;
}

void CPU::_orPairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_orValue()
{
	uint8_t val = m_fetch();
	m_setZeroFlag(!AF.high);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_compareRegister(uint8_t& reg)
{
	m_setHalfCarryFlag((AF.high & 0xF) - (reg & 0xF) & 0x10);
	m_setCarryFlag(AF.high < reg);
	m_setZeroFlag(AF.high - reg == 0);
	m_setSubtractFlag(true);
	m_cycleCount += 1;
}

void CPU::_comparePairAddress(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	m_setHalfCarryFlag((AF.high & 0xF) - (val & 0xF) & 0x10);
	m_setCarryFlag(AF.high < val);
	m_setZeroFlag(AF.high - val == 0);
	m_setSubtractFlag(true);
	m_cycleCount += 2;
}

void CPU::_compareValue()
{
	uint8_t val = m_fetch();
	m_setHalfCarryFlag((AF.high & 0xF) - (val & 0xF) & 0x10);
	m_setCarryFlag(AF.high < val);
	m_setZeroFlag(AF.high - val == 0);
	m_setSubtractFlag(true);
	m_cycleCount += 2;
}

void CPU::_pushPairRegister(Register& reg)
{
	m_pushToStack(reg.reg);
	m_cycleCount += 4;
}

void CPU::_popToPairRegister(Register& reg)
{
	uint16_t val = m_popFromStack();
	reg.reg = val;
	m_cycleCount += 3;
}

//some misc instructions
void CPU::_disableInterrupts()
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "Disable interrupts called, however instruction is not implemented!");
}

void CPU::_enableInterrupts()
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "Enable interrupts called, however instruction is not implemented!");
}

void CPU::_stop()
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "STOP Instruction is not implemented, interpreting as no operation");
}

void CPU::_halt()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Processor halted temporarily");
	m_halted = true;
	m_cycleCount += 1;
}

void CPU::_resetToVector(uint8_t vectorIdx)
{
	uint16_t resetAddr = vectorIdx * 8;
	m_pushToStack(PC);
	PC = resetAddr;
	m_cycleCount += 4;
}

void CPU::_adjustBCD()
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "Program attempted to perform BCD adjust which is not implemented!");
}

void CPU::_loadHLStackIdx()
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "Opcode 0xF8 is not implemented!");
}

void CPU::_complement()
{
	m_setSubtractFlag(true);
	m_setHalfCarryFlag(true);
	AF.high = ~AF.high;
	m_cycleCount += 1;
}

void CPU::_RLA()
{
	uint8_t msb = (AF.high & 0b10000000) >> 7;
	AF.high <<= 1;
	uint8_t m_lastCarry = (m_getCarryFlag()) ? 0b00000001 : 0b0;
	AF.high |= m_lastCarry;
	m_setCarryFlag(msb);
	m_setSubtractFlag(false);
	m_setZeroFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 1;
}

void CPU::_RLCA()
{
	uint8_t msb = (AF.high & 0b10000000) >> 7;
	AF.high <<= 1;
	m_setCarryFlag(msb);
	m_setSubtractFlag(false);
	m_setZeroFlag(false);
	m_setHalfCarryFlag(false);
	AF.high |= msb;
	m_cycleCount += 1;
}

void CPU::_RRA()
{
	uint8_t lsb = (AF.high & 0b00000001);
	AF.high >>= 1;
	uint8_t m_lastCarry = (m_getCarryFlag()) ? 0b10000000 : 0b0;
	AF.high |= m_lastCarry;
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setZeroFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 1;
}

void CPU::_RRCA()
{
	uint8_t lsb = (AF.high & 0b00000001);
	AF.high >>= 1;
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setZeroFlag(false);
	m_setHalfCarryFlag(false);
	AF.high |= (lsb << 7);
	m_cycleCount += 1;
}

void CPU::_RL(uint8_t& reg)
{
	uint8_t msb = (reg & 0b10000000) >> 7;
	uint8_t lastCarry = (m_getCarryFlag()) ? 0b00000001 : 0b0;
	reg <<= 1;
	reg |= lastCarry;
	m_setZeroFlag(!reg);
	m_setCarryFlag(msb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_RLC(uint8_t& reg)
{
	uint8_t msb = (reg & 0b10000000) >> 7;
	reg <<= 1;
	reg |= msb;
	m_setCarryFlag(msb);
	m_setZeroFlag(!reg);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_RL(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	
	uint8_t msb = (val & 0b10000000) >> 7;
	uint8_t lastCarry = (m_getCarryFlag()) ? 0b00000001 : 0b0;
	val <<= 1;
	val |= lastCarry;
	m_setZeroFlag(!val);
	m_setCarryFlag(msb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_RLC(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);

	uint8_t msb = (val & 0b10000000) >> 7;
	val <<= 1;
	val |= msb;
	m_setCarryFlag(msb);
	m_setZeroFlag(!val);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_RR(uint8_t& reg)
{
	uint8_t lsb = (reg & 0b00000001);
	reg >>= 1;
	uint8_t m_lastCarry = (m_getCarryFlag()) ? 0b10000000 : 0b0;
	reg |= m_lastCarry;
	m_setZeroFlag(!reg);
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_RRC(uint8_t& reg)
{
	uint8_t lsb = (reg & 0b00000001);
	reg >>= 1;
	m_setCarryFlag(lsb);
	reg |= (lsb << 7);
	m_setZeroFlag(!reg);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_RR(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t lsb = (val & 0b00000001);
	val >>= 1;
	uint8_t m_lastCarry = (m_getCarryFlag()) ? 0b10000000 : 0b0;
	val |= m_lastCarry;
	m_setZeroFlag(!val);
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_RRC(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);

	uint8_t lsb = (val & 0b00000001);
	val >>= 1;
	m_setCarryFlag(lsb);
	val |= (lsb << 7);
	m_setZeroFlag(!val);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_SLA(uint8_t& reg)
{
	uint8_t msb = (reg & 0b10000000) >> 7;
	reg <<= 1;
	m_setZeroFlag(!reg);
	m_setCarryFlag(msb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_SLA(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t msb = (val & 0b10000000) >> 7;
	val <<= 1;
	m_setZeroFlag(!val);
	m_setCarryFlag(msb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_SRA(uint8_t& reg)
{
	uint8_t lsb = (reg & 0b00000001);
	uint8_t msb = (reg & 0b00000001);
	reg >>= 1;
	reg |= msb;
	m_setZeroFlag(!reg);
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_SRA(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t lsb = (val & 0b00000001);
	uint8_t msb = (val & 0b10000000);
	val >>= 1;
	val |= msb;
	m_setZeroFlag(!val);
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_SRL(uint8_t& reg)
{
	uint8_t lsb = (reg & 0b00000001);
	reg >>= 1;
	m_setZeroFlag(!reg);
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_SRL(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t lsb = (val & 0b00000001);
	val >>= 1;
	m_setZeroFlag(!val);
	m_setCarryFlag(lsb);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_SWAP(uint8_t& reg)
{
	uint8_t low = (reg & 0x0F) << 4;
	uint8_t high = (reg & 0xF0) >> 4;

	reg = low | high;

	m_setZeroFlag(!reg);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);
	m_cycleCount += 2;
}

void CPU::_SWAP(Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t low = (val & 0x0F) << 4;
	uint8_t high = (val & 0xF0) >> 4;

	val = low | high;

	m_setZeroFlag(!val);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(false);
	m_setCarryFlag(false);

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_BIT(int idx, uint8_t& reg)
{
	uint8_t bitVal = (reg >> idx) & 0b00000001;
	m_setZeroFlag(!bitVal);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(true);
	m_cycleCount += 2;
}

void CPU::_BIT(int idx, Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t bitVal = (val >> idx) & 0b00000001;
	m_setZeroFlag(!bitVal);
	m_setSubtractFlag(false);
	m_setHalfCarryFlag(true);
	m_cycleCount += 3;
}

void CPU::_RES(int idx, uint8_t& reg)
{
	uint8_t mask = 0b00000001 << idx;
	mask ^= 0b11111111;	//invert all bits
	reg &= mask;

	m_cycleCount += 2;
}

void CPU::_RES(int idx, Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t mask = 0b00000001 << idx;
	mask ^= 0b11111111;	//invert all bits
	val &= mask;

	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}

void CPU::_SET(int idx, uint8_t& reg)
{
	uint8_t mask = 0b00000001 << idx;
	reg |= mask;
	m_cycleCount += 2;
}

void CPU::_SET(int idx, Register& reg)
{
	uint8_t val = m_mmu->read(reg.reg);
	uint8_t mask = 0b00000001 << idx;
	val |= mask;
	m_mmu->write(reg.reg, val);
	m_cycleCount += 4;
}