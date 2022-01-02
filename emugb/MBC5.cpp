#include"MMU.h"

MBC5::MBC5(std::vector<uint8_t> ROM)
{
	int count = 0;
	for (int i = 0; i < ROM.size() - 16384; i += 16384)
	{
		std::array<uint8_t, 16384> curROMBank;
		std::copy(ROM.begin() + i, ROM.begin() + i + 16384, curROMBank.begin());
		m_ROMBanks[count++] = curROMBank;
	}
	m_ramBankNumber = 0;
}

MBC5::~MBC5()
{

}

uint8_t MBC5::read(uint16_t address)
{

	if (address >= 0x4000 && address < 0x8000)
	{
		int offset = ((int)address) - 0x4000;
		return m_ROMBanks[m_bankNumber][offset];
	}

	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if (m_SRAMEnabled)
			return m_RAMBanks[m_ramBankNumber][address - 0xA000];
		return 0xFF;
	}

	if(address <= 0x3FFF)
		return m_ROMBanks[0][address];
}

void MBC5::write(uint16_t address, uint8_t value)
{
	if (address >= 0x0000 && address <= 0x1fff)
	{
		if (value == 0x0A)
			m_SRAMEnabled = true;
		else if (value == 0x00)
			m_SRAMEnabled = false;	//ignore all other values, only 0a enables and 00 disables
	}

	if (address >= 0x4000 && address <= 0x5fff)
	{
		m_ramBankNumber = 0;// value % 8;
	}


	if (address >= 0x2000 && address <= 0x2FFF)
	{
		m_bankNumber = (m_bankNumberHighBit << 8) | value;
	}

	if (address >= 0x3000 && address <= 0x3FFF)
	{
		m_bankNumberHighBit = value & 0b1;
	}

	if (address >= 0xA000 && address <= 0xBFFF)
	{
		if(m_SRAMEnabled)
			m_RAMBanks[m_ramBankNumber][address - 0xA000] = value;
	}
	//MMUState curState = { m_isInBIOS,m_bankNumber,m_ramBankNumber };
	//Config::getInstance()->setValue<MMUState>("MMUState", curState);
}

