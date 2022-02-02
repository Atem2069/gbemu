#include"APU.h"

APU::APU()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Init APU..");
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 5; j++)
			m_channels[i].r[j] = {};
	}

}

APU::~APU()
{

}

void APU::step(unsigned long cycleDiff)
{
	//todo
}

void APU::writeIORegister(uint16_t address, uint8_t value)
{
	if (address >= 0xFF10 && address <= 0xFF14)
		m_channels[0].r[address - 0xFF10] = value;
	if (address >= 0xFF15 && address <= 0xFF19)
		m_channels[1].r[address - 0xFF15] = value;
	if (address >= 0xFF1A && address <= 0xFF1E)
		m_channels[2].r[address - 0xFF1A] = value;
	if (address >= 0xFF1F && address <= 0xFF23)
		m_channels[3].r[address - 0xFF1F] = value;
	if (address == 0xFF24)
		NR50 = value;
	if (address == 0xFF25)
		NR51 = value;
	if (address == 0xFF26)
		NR52 = value;
	//todo: wave table read/writes
}

uint8_t APU::readIORegister(uint16_t address)
{
	if (address >= 0xFF10 && address <= 0xFF14)
		return m_channels[0].r[address - 0xFF10];
	if (address >= 0xFF15 && address <= 0xFF19)
		return m_channels[1].r[address - 0xFF15];
	if (address >= 0xFF1A && address <= 0xFF1E)
		return m_channels[2].r[address - 0xFF1A];
	if (address >= 0xFF1F && address <= 0xFF23)
		return m_channels[3].r[address - 0xFF1F];
	if (address == 0xFF24)
		return NR50;
	if (address == 0xFF25)
		return NR51;
	if (address == 0xFF26)
		return 0;	//HACK: when length counters are properly implemented this is no longer necessary.
	return 0xFF;
}