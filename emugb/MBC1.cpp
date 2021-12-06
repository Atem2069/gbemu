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
	if (m_isInBIOS && address < 0x100)
		return m_BIOS[address];	//return boot code if still in BIOS 

	if (address >= 0xE000 && address <= 0xFDFF)	//handle echo ram - essentially don't read from this range
		address -= 0x2000;

	return m_memory[address];
}

void MBC1::write(uint16_t address, uint8_t value)
{
	if (address >= 0xE000 && address <= 0xFDFF)	//same as read, don't write to this range
		address -= 0x2000;

	if (address >= 0x4000 && address <= 0x5fff)
		m_bankUpperBits = (value << 5);

	if (address >= 0x6000 && address <= 0x7fff)
		std::cout << (int)value << '\n';

	if (address >= 0x2000 && address <= 0x3FFF)
	{
		value = value & 0b00011111;
		//std::cout << (int)m_bankUpperBits << '\n';
		//Logger::getInstance()->msg(LoggerSeverity::Info, "MBC1: Bank switch index=" + std::to_string((int)value));
		for (int i = 0x4000; i < 0x7fff; i++)
		{
			int bank = (value | m_bankUpperBits) * 16384;	//the cur bank
			bank += (i - 0x4000);
			int romPTR = bank;
			m_memory[i] = m_ROM[romPTR];

		}
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