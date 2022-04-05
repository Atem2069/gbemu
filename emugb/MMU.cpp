#include"MMU.h"

MMU::MMU(std::vector<uint8_t> m_ROM)
{
	if (m_ROM.size()-1 != 32768)
		Logger::getInstance()->msg(LoggerSeverity::Warn, "ROM size invalid: expected 32768 bytes, got " + std::to_string(m_ROM.size()));

	//max to copy is 32768 - but if file is smaller then copy smaller amount
	int copyOffset = 32768;
	if (m_ROM.size() < 32768)
		copyOffset = m_ROM.size();

	std::copy(m_ROM.begin(), m_ROM.begin()+copyOffset, m_memory.begin());
}

MMU::~MMU()
{
	//todo
}

uint8_t MMU::read(uint16_t address)
{
	return m_memory[address];
}

void MMU::write(uint16_t address, uint8_t value)
{
	Logger::getInstance()->msg(LoggerSeverity::Warn, "Write to cartridge space on unmapped ROM! Potential game bug.");
	std::string addrDebug = std::format("Addr={:#x} Value={:#x}", address, value);
	Logger::getInstance()->msg(LoggerSeverity::Warn, addrDebug);
}
