#include"MMU.h"

MBC1::MBC1(std::vector<uint8_t> m_firmware, std::vector<uint8_t> ROM)
{
	m_BIOS = m_firmware;
	m_ROM.insert(m_ROM.begin(), ROM.begin(), ROM.end());
	m_memory.resize(0xFFFF);
	m_memory.insert(m_memory.begin(), ROM.begin(), ROM.begin() + 0x7FFF);	//ROM is mapped 0-7fff (first 2 banks).
	m_isInBIOS = true;
}

MBC1::~MBC1()
{

}

uint8_t MBC1::read(uint16_t address)
{
	m_bankSwitch();
	if (m_isInBIOS && address < 0x100)
		return m_BIOS[address];	//return boot code if still in BIOS 

	if (address >= 0xE000 && address <= 0xFDFF)	//handle echo ram - essentially don't read from this range
		address -= 0x2000;

	/*/if (address >= 0x4000 && address <= 0x8000 && m_bankNumber)
	{
		//return m_ROM[(uint16_t)(address + ((uint16_t)m_bankNumber * 0x4000) - 0x4000)];
		uint16_t offset = address - 0x4000;
		uint16_t bankBase = (uint16_t)(m_bankNumber) * 0x4000;
		return m_ROM[bankBase + offset];
	}*/

	return m_memory[address];
}

void MBC1::write(uint16_t address, uint8_t value)
{
	m_bankSwitch();
	if (address >= 0xE000 && address <= 0xFDFF)	//same as read, don't write to this range
		address -= 0x2000;

	if (address >= 0x0000 && address <= 0x1fff)
	{
		//Logger::getInstance()->msg(LoggerSeverity::Error, "Attempt to enable RAM in MBC. Unsupported");
		return;
	}

	if (address >= 0x4000 && address <= 0x5fff)
	{
		if (!m_RAMBanking)
		{
			m_higherBankBits = value;
			//m_bankNumber = (m_bankNumber & 0b00011111) | (value << 5);
			//m_bankSwitchRequired = true;
		}
		return;
	}

	if (address >= 0x6000 && address <= 0x7fff)
	{
		m_RAMBanking = value;
		return;
	}

	if (address >= 0x2000 && address <= 0x3FFF)
	{
		value = value & 0b00011111;
		if (!value)
			value = 1;
		m_bankNumber = (m_higherBankBits << 5) | value;
		m_bankSwitchRequired = true;
		return;
	}

	if (address == 0xFF01)	//weird debug output
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

void MBC1::m_DMATransfer(uint8_t base)
{
	uint16_t newAddr = ((unsigned int)base << 8);
	for (unsigned int i = 0; i < 0xA0; i++)
	{
		write(0xFE00 + i, read(newAddr + i));
	}
}

void MBC1::m_bankSwitch()
{
	if (!m_bankSwitchRequired)
		return;
	m_bankSwitchRequired = false;
	int bankBase = (int)m_bankNumber * 0x4000;
	for (int i = 0; i < 0x4000; i++)
	{
		m_memory[0x4000 + i] = m_ROM[bankBase + i];
	}
}