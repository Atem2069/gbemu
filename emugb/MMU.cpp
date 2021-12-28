#include"MMU.h"

MMU::MMU(std::vector<uint8_t> m_ROM)
{
	/*m_BIOS = m_firmware;
	std::copy(m_ROM.begin(), m_ROM.begin() + 32768, m_memory.begin());
	for (int i = 32768; i < 65536; i++)
	{
		m_memory[i] = 0xFF;
	}
	m_isInBIOS = true;*/

	std::copy(m_ROM.begin(), m_ROM.begin() + 32768, m_memory.begin());
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
	//Logger::getInstance()->msg(LoggerSeverity::Warn, "Write to cartridge space on unmapped ROM! Potential game bug.");
}
