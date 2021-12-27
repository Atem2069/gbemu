#include"MMU.h"

MBC5::MBC5(std::vector<uint8_t> ROM)
{
	for (int i = 0; i < ROM.size() - 16384; i += 16384)
	{
		std::array<uint8_t, 16384> curROMBank;
		std::copy(ROM.begin() + i, ROM.begin() + i + 16384, curROMBank.begin());
		m_ROMBanks.push_back(curROMBank);
	}
}

MBC5::~MBC5()
{

}

uint8_t MBC5::read(uint16_t address)
{

	if (address >= 0x4000 && address < 0x8000 && m_bankNumber)
	{
		int offset = ((int)address) - 0x4000;
		return m_ROMBanks[m_bankNumber][offset];
	}
	if (address >= 0x4000 && address < 0x8000 && m_bankNumber == 0)
		return m_ROMBanks[1][(int)address - 0x4000];

	if (address >= 0xA000 && address <= 0xBFFF)
		return m_RAMBanks[0][address - 0xA000];

	return m_ROMBanks[0][address];
}

void MBC5::write(uint16_t address, uint8_t value)
{
	if (address >= 0x0000 && address <= 0x1fff)
	{
		//Logger::getInstance()->msg(LoggerSeverity::Error, "Attempt to enable RAM in MBC. Unsupported");
		return;
	}

	if (address >= 0x4000 && address <= 0x5fff)
	{
		m_ramBankNumber = value;
		return;
	}


	if (address >= 0x2000 && address <= 0x2FFF)
	{
		m_bankNumber = (m_bankNumberHighBit << 8) | value;
		return;
	}

	if (address >= 0x3000 && address <= 0x3FFF)
	{
		m_bankNumberHighBit = value & 0b1;
	}

	if (address >= 0xA000 && address <= 0xBFFF)
		m_RAMBanks[0][address - 0xA000] = value;
	//MMUState curState = { m_isInBIOS,m_bankNumber,m_ramBankNumber };
	//Config::getInstance()->setValue<MMUState>("MMUState", curState);
}

