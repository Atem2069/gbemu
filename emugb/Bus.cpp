#include"Bus.h"

Bus::Bus(std::array<uint8_t, 256> bootRom, std::vector<uint8_t> ROM)
{

	m_bootRom = bootRom;

	uint8_t ROMType = ROM[CART_TYPE];
	if (ROMType == 0)
		m_mbc = std::make_shared<MMU>(ROM);
	else if (ROMType >= 0x01 && ROMType <= 0x03)
		m_mbc = std::make_shared<MBC1>(ROM);
	else if (ROMType >= 0x0F && ROMType <= 0x13)
		m_mbc = std::make_shared<MBC3>(ROM);
	else if (ROMType >= 0x19 && ROMType <= 0x1E)
		m_mbc = std::make_shared<MBC5>(ROM);

	for (int i = 0; i < m_VRAM.size(); i++)
		m_VRAM[i] = 0xFF;
	for (int i = 0; i < m_workRAM.size(); i++)
		m_workRAM[i] = 0xFF;
	for (int i = 0; i < m_IORegisters.size(); i++)
		m_IORegisters[i] = 0xFF;
	for (int i = 0; i < m_HRAM.size(); i++)
		m_HRAM[i] = 0xFF;
	for (int i = 0; i < m_OAM.size(); i++)
		m_OAM[i] = 0xFF;

}

Bus::~Bus()
{

}

uint8_t Bus::read(uint16_t address)
{
	if (address <= 0xFF && m_isInBootRom)
		return m_bootRom[address];
	if (address <= 0x7FFF || (address >= 0xA000 && address <= 0xBFFF))
		return m_mbc->read(address);
	if (address >= 0x8000 && address <= 0x9FFF)
		return m_VRAM[address - 0x8000];
	if (address >= 0xC000 && address <= 0xFDFF)
	{
		if (address >= 0xE000)
			address -= 0x2000;
		return m_workRAM[address - 0xC000];
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
		return m_OAM[address - 0xFE00];
	if (address >= 0xFF00 && address <= 0xFF7F)
		return m_IORegisters[address - 0xFF00];
	if (address >= 0xFF80 && address <= 0xFFFF)
		return m_HRAM[address - 0xFF80];

	return 0xFF;	//assume open bus

}

void Bus::write(uint16_t address, uint8_t value)
{
	if (address <= 0x7FFF || (address >= 0xA000 && address <= 0xBFFF))
		m_mbc->write(address,value);
	if (address >= 0x8000 && address <= 0x9FFF)
		m_VRAM[address - 0x8000] = value;
	if (address >= 0xC000 && address <= 0xFDFF)
	{
		if (address >= 0xE000)
			address -= 0x2000;
		m_workRAM[address - 0xC000] = value;
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
		m_OAM[address - 0xFE00] = value;
	if (address >= 0xFF00 && address <= 0xFF7F)
	{

		if (address == 0xFF50)
		{
			Logger::getInstance()->msg(LoggerSeverity::Info, "Unmapping boot ROM. .");
			m_isInBootRom = false;
		}

		if (address == 0xFF46)
			m_DMATransfer(value);

		m_IORegisters[address - 0xFF00] = value;
	}
	if (address >= 0xFF80 && address <= 0xFFFF)
		m_HRAM[address - 0xFF80] = value;
}

void Bus::m_DMATransfer(uint8_t base)
{
	uint16_t newAddr = ((unsigned int)base << 8);
	for (unsigned int i = 0; i < 0xA0; i++)
	{
		write(0xFE00 + i, read(newAddr + i));
	}
}