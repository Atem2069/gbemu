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
	if (address >= 0xE000 && address <= 0xFDFF)	//same as read, don't write to this range
		address -= 0x2000;

	if (address == 0xFF46)
	{
		//todo: DMA transfer
	}

	if (address == 0xFF50)
		m_isInBIOS = false;	//FF50 is BIOS select register, treat all writes as disabling the BIOS

	m_memory[address] = value;
}