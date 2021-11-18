#include"MMU.h"

MMU::MMU(std::vector<uint8_t> m_firmware, std::vector<uint8_t> m_ROM)
{
	m_BIOS = m_firmware;
	m_memory.resize(0xFFFF);
	m_memory.insert(m_memory.begin(), m_ROM.begin(), m_ROM.begin() + 0x7FFF);	//ROM is mapped 0-7fff (first 2 banks).
	m_isInBIOS = true;
}

MMU::~MMU()
{
	//todo
}

void MMU::updateROM(std::vector<uint8_t> newROM)
{
	//machine is going to be reset, so re-enable BIOS
	m_isInBIOS = true;
	m_memory.insert(m_memory.begin(), newROM.begin(), newROM.begin() + 0x7FFF);
}

uint8_t MMU::read(uint16_t address)
{
	if (m_isInBIOS && address < 0x100)
		return m_BIOS[address];	//return boot code if still in BIOS 

	if (address >= 0xE000 && address <= 0xFDFF)	//handle echo ram - essentially don't read from this range
		address -= 0x2000;

	return m_memory[address];
}

void MMU::write(uint16_t address, uint8_t value)
{

	if (address <= 0x7FFF)
	{
		Logger::getInstance()->msg(LoggerSeverity::Error, "Invalid memory write to address " + std::to_string((int)address));
		return;
	}

	if (address >= 0xE000 && address <= 0xFDFF)	//same as read, don't write to this range
		address -= 0x2000;

	if (address == 0xFF01)	//weird debug output
	{
		//std::cout << (char)value;
	}

	if (address == 0xFF46)
	{
		if (!m_OAMLock)
			return m_DMATransfer(value);
		else
			Logger::getInstance()->msg(LoggerSeverity::Warn, "DMA attempted while OAM is locked for reading. Ignoring...");
	}

	if (address == 0xFF50)
	{
		Logger::getInstance()->msg(LoggerSeverity::Info, "Exiting BIOS..");
		m_isInBIOS = false;	//FF50 is BIOS select register, treat all writes as disabling the BIOS
	}

	m_memory[address] = value;
}

void MMU::setOAMLocked(bool locked)
{
	m_OAMLock = locked;
}

void MMU::m_DMATransfer(uint8_t base)
{
	uint16_t newAddr = ((unsigned int)base << 8);
	for (unsigned int i = 0; i < 0xA0; i++)
	{
		write(0xFE00 + i, read(newAddr + i));
	}
}