#include"MMU.h"

MBC3::MBC3(std::array<uint8_t, 256> m_firmware, std::vector<uint8_t> ROM)
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

	m_rtcStart = std::chrono::high_resolution_clock::now();
	m_isInBIOS = true;
}

MBC3::~MBC3()
{

}

uint8_t MBC3::read(uint16_t address)
{
	if (m_isInBIOS && address < 0x100)
		return m_BIOS[address];	//return boot code if still in BIOS 

	if (address >= 0xE000 && address <= 0xFDFF)	//handle echo ram - essentially don't read from this range
		address -= 0x2000;

	if (address >= 0x4000 && address < 0x8000 && m_bankNumber)
	{
		int offset = ((int)address) - 0x4000;
		return m_ROMBanks[m_bankNumber][offset];
	}

	if (address >= 0xA000 && address <= 0xBFFF && m_ramBankNumber >= 0x08)
	{
		if(!m_rtcLatched)
			m_rtcLast = std::chrono::high_resolution_clock::now();	//only update rtc if not latched
		double timeDiff = std::chrono::duration<double, std::milli>(m_rtcLast - m_rtcStart).count() / 1000.0;	//time diff in seconds
		if (m_ramBankNumber == 8)
			return (uint8_t)(std::fmod(timeDiff, 60));
		else if (m_ramBankNumber == 9)
			return (uint8_t)(std::fmod(timeDiff/60, 60));
		return 0;	//hour,days is redundant.

	}


	return m_memory[address];
}

void MBC3::write(uint16_t address, uint8_t value)
{
	if (address >= 0xE000 && address <= 0xFDFF)	//same as read, don't write to this range
		address -= 0x2000;

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

	if (address >= 0x2000 && address <= 0x3FFF)
	{
		m_bankNumber =  value;
		return;
	}

	if (address >= 0x6000 && address <= 0x7fff)
	{
		if (m_lastLatchWrite == 0 && value == 1)
			m_rtcLatched = !m_rtcLatched;
		m_lastLatchWrite = value;
		return;
	}

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

void MBC3::m_DMATransfer(uint8_t base)
{
	uint16_t newAddr = ((unsigned int)base << 8);
	for (unsigned int i = 0; i < 0xA0; i++)
	{
		write(0xFE00 + i, read(newAddr + i));
	}
}
