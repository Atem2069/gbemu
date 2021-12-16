#include"MMU.h"

MMU::MMU(std::array<uint8_t,256> m_firmware, std::vector<uint8_t> m_ROM)
{
	m_BIOS = m_firmware;
	std::copy(m_ROM.begin(), m_ROM.begin() + 32768, m_memory.begin());
	for (int i = 32768; i < 65536; i++)
	{
		m_memory[i] = 0xFF;
	}
	m_isInBIOS = true;
}

MMU::~MMU()
{
	//todo
}

/*void MMU::updateROM(std::vector<uint8_t> newROM)
{
	//machine is going to be reset, so re-enable BIOS
	m_isInBIOS = true;
	m_memory.insert(m_memory.begin(), newROM.begin(), newROM.begin() + 0x7FFF);
}*/

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

	if (address == 0xFF01 && Config::getInstance()->getValue<bool>("serialDebug"))
	{
		std::cout << (char)value;
	}


	if (address == 0xFF46)
		return m_DMATransfer(value);

	if (address == 0xFF50)
	{
		Logger::getInstance()->msg(LoggerSeverity::Info, "Exiting BIOS..");
		m_isInBIOS = false;	//FF50 is BIOS select register, treat all writes as disabling the BIOS
	}

	m_memory[address] = value;
}

void MMU::m_DMATransfer(uint8_t base)
{
	uint16_t newAddr = ((unsigned int)base << 8);
	for (unsigned int i = 0; i < 0xA0; i++)
	{
		write(0xFE00 + i, read(newAddr + i));
	}
}