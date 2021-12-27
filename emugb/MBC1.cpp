#include"MMU.h"

MBC1::MBC1(std::array<uint8_t,256> m_firmware, std::vector<uint8_t> ROM)
{
	m_BIOS = m_firmware;
	std::copy(ROM.begin(), ROM.begin() + 32768, m_memory.begin());
	for (int i = 32768; i < 65536; i++)
	{
		m_memory[i] = 0xFF;
	}

	for (int i = 0; i < ROM.size() - 16384; i += 16384)
	{
		std::array<uint8_t, 16384> curROMBank;
		std::copy(ROM.begin() + i, ROM.begin() + i + 16384, curROMBank.begin());
		m_ROMBanks.push_back(curROMBank);
	}


	uint8_t cartType = m_memory[CART_TYPE];
	if (cartType == 3)
		m_shouldSave = true;

	if (m_shouldSave)
	{
		//load save file into RAM bank 0 (only if cart has battery)
		m_saveName = Config::getInstance()->getValue<std::string>("RomName");
		m_saveName.resize(m_saveName.size() - 2);
		m_saveName += "sav";
		std::ifstream ramReadHandle(m_saveName, std::ios::in | std::ios::binary);
		if (!ramReadHandle)
		{
			Logger::getInstance()->msg(LoggerSeverity::Info, "No save file exists for current ROM - file will be created upon unload!");
		}
		else
		{
			ramReadHandle >> std::noskipws;
			int i = 0;
			while (!ramReadHandle.eof())
			{
				unsigned char curByte;
				ramReadHandle.read((char*)&curByte, sizeof(uint8_t));
				m_RAMBanks[0][i] = (uint8_t)curByte;
				i++;
			}

			ramReadHandle.close();
		}
	}

	m_isInBIOS = true;
}

MBC1::~MBC1()
{
	if (m_shouldSave)
	{
		Logger::getInstance()->msg(LoggerSeverity::Info, "Saving game memory..");
		std::ofstream ramWriteHandle(m_saveName, std::ios::out | std::ios::binary);
		ramWriteHandle.write((const char*)m_RAMBanks[0].data(), m_RAMBanks[0].size());
		ramWriteHandle.close();
	}
}

uint8_t MBC1::read(uint16_t address)
{

	if (address >= 0x4000 && address < 0x8000 && m_bankNumber)
	{
		int offset = ((int)address) - 0x4000;
		return m_ROMBanks[m_bankNumber][offset];
	}

	if (address >= 0xa000 && address <= 0xbfff)
	{
		if (!m_SRAMEnabled)
			return 0xFF;
		int offset = ((int)address - 0xa000);
		return m_RAMBanks[m_ramBankNumber][offset];
	}
	
	if (address >= 0x4000 && address < 0x8000 && m_bankNumber == 0)
		return m_ROMBanks[1][(int)address - 0x4000];
	return m_ROMBanks[0][address];

}

void MBC1::write(uint16_t address, uint8_t value)
{

	if (address >= 0x0000 && address <= 0x1fff)
	{
		if ((value & 0x0f) == 0x0A)
			m_SRAMEnabled = true;
		else
			m_SRAMEnabled = false;
		return;
	}

	if (address >= 0x4000 && address <= 0x5fff)
	{
		if (!m_RAMBanking)
			m_higherBankBits = (value & 0b00000011);
		else
			m_ramBankNumber = (value & 0b00000011);
		return;
	}

	if (address >= 0x6000 && address <= 0x7fff)
	{
		if ((value & 0b1) == 0)
		{
			m_RAMBanking = false;
			m_ramBankNumber = 0;
		}
		if ((value & 0b1) == 1)
			m_RAMBanking = true;

		return;
	}

	if (address >= 0x2000 && address <= 0x3FFF)
	{
		value = value & 0b00011111;
		if (!value)
			value = 1;
		m_bankNumber = (m_higherBankBits << 5) | value;
		return;
	}

	if (address >= 0xa000 && address <= 0xbfff)
	{
		int offset = ((int)address - 0xa000);
		m_RAMBanks[m_ramBankNumber][offset] = value;
	}

	//MMUState curState = { m_isInBIOS,m_bankNumber,m_ramBankNumber };
	//Config::getInstance()->setValue<MMUState>("MMUState", curState);
}
