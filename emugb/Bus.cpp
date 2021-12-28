#include"Bus.h"

Bus::Bus(std::vector<uint8_t> bootRom, std::vector<uint8_t> ROM)
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
	{
		for (int j = 0; j < m_VRAM[i].size(); j++)
			m_VRAM[i][j] = 0xFF;
	}
	for (int i = 0; i < m_workRAM.size(); i++)
	{
		for (int j = 0; j < m_workRAM[i].size(); j++)
			m_workRAM[i][j] = 0xFF;
	}
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
	if (address >= 0x200 && address <= 0x08ff && m_isInBootRom)
		return m_bootRom[address];
	if (address <= 0x7FFF || (address >= 0xA000 && address <= 0xBFFF))
		return m_mbc->read(address);
	if (address >= 0x8000 && address <= 0x9FFF)
		return m_VRAM[m_VRAMBank][address - 0x8000];
	if (address >= 0xC000 && address <= 0xFDFF)
	{
		if (address >= 0xE000)
			address -= 0x2000;
		if (address <= 0xCFFF)
			return m_workRAM[0][address - 0xC000];	//bank 0 is always fixed (c000-cfff)
		else
			return m_workRAM[m_WRAMBank][address - 0xD000];	//rest is bank switched (1-7)
	}
	if (address >= 0xFE00 && address <= 0xFE9F)
		return m_OAM[address - 0xFE00];
	if (address >= 0xFF00 && address <= 0xFF7F)
	{
		if (address == REG_BGPD)
			return m_paletteMemory[m_paletteIndex];
		if (address == REG_OBPD)
			return m_objPaletteMemory[m_objPaletteIndex];
		return m_IORegisters[address - 0xFF00];
	}
	if (address >= 0xFF80 && address <= 0xFFFF)
		return m_HRAM[address - 0xFF80];

	return 0xFF;	//ideally open bus behaviour would be emulated, but like one game depends on it - so it doesn't matter

}

void Bus::write(uint16_t address, uint8_t value)
{
	if (address <= 0x7FFF || (address >= 0xA000 && address <= 0xBFFF))
		m_mbc->write(address,value);
	if (address >= 0x8000 && address <= 0x9FFF)
		m_VRAM[m_VRAMBank][address - 0x8000] = value;
	if (address >= 0xC000 && address <= 0xFDFF)
	{
		if (address >= 0xE000)
			address -= 0x2000;
		if (address <= 0xCFFF)
			m_workRAM[0][address - 0xC000] = value;
		else
			m_workRAM[m_WRAMBank][address - 0xD000] = value;
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

		if (address == REG_SVBK)
		{
			m_WRAMBank = value & 0b00000111;	//set wram bank
			if (m_WRAMBank == 0)
				m_WRAMBank = 1;
		}
		if (address == REG_VBK)
			m_VRAMBank = value & 0b1;			//set vram bank

		if (address == 0xFF46)
			m_DMATransfer(value);

		if (address == REG_HDMA5)
		{
			if ((value >> 7) & 0b1)
				m_HDMARequested = true;
			else if (((value >> 7) & 0b1) == 0 && m_HDMAInProgress)
				m_HDMAInProgress = false;
			else
				m_GDMATransfer(value & 0b01111111);
		}

		if (address == REG_BGPI)
		{
			if ((value >> 7) & 0b1)
				m_incrementPaletteIndex = true;
			else
				m_incrementPaletteIndex = false;

			m_paletteIndex = value & 0b00111111;
		}

		if (address == REG_BGPD)
		{
			m_paletteMemory[m_paletteIndex] = value;
			if (m_incrementPaletteIndex)
				m_paletteIndex += 1;
		}

		if (address == REG_OBPI)
		{
			if ((value >> 7) & 0b1)
				m_incrementObjPaletteIndex = true;
			else
				m_incrementObjPaletteIndex = false;

			m_objPaletteIndex = value & 0b00111111;
		}

		if (address == REG_OBPD)
		{
			m_objPaletteMemory[m_objPaletteIndex] = value;
			if (m_incrementObjPaletteIndex)
				m_objPaletteIndex += 1;
		}

		if (address == REG_KEY0)
		{
			if (value == 0x04)
			{
				Logger::getInstance()->msg(LoggerSeverity::Info, "Entering compatibility mode. CGB features are disabled.");
				m_isInCompatibilityMode = true;
			}
		}

		m_IORegisters[address - 0xFF00] = value;
	}
	if (address >= 0xFF80 && address <= 0xFFFF)
		m_HRAM[address - 0xFF80] = value;
}

uint8_t Bus::readVRAM(uint8_t bank, uint16_t address)
{
	return m_VRAM[bank][address - 0x8000];
}

void Bus::writeVRAM(uint8_t bank, uint16_t address, uint8_t value)
{
	m_VRAM[bank][address - 0x8000] = value;
}

uint16_t Bus::readBgColor(uint8_t paletteIndex, uint8_t colorIndex)
{
	uint8_t idx = (paletteIndex * 8) + (colorIndex * 2);	//8 bytes per palette, 2 bytes per color

	uint8_t low = m_paletteMemory[idx];
	uint8_t high = m_paletteMemory[idx + 1];
	uint16_t col = (high << 8) | low;
	return col;
}

uint16_t Bus::readObjColor(uint8_t paletteIndex, uint8_t colorIndex)
{
	uint8_t idx = (paletteIndex * 8) + (colorIndex * 2);	//8 bytes per palette, 2 bytes per color

	uint8_t low = m_objPaletteMemory[idx];
	uint8_t high = m_objPaletteMemory[idx + 1];
	uint16_t col = (high << 8) | low;
	return col;
}

bool Bus::getHDMA()
{
	return m_HDMARequested;
}

bool Bus::getHDMAInProgress()
{
	return m_HDMAInProgress;
}

void Bus::acknowledgeHDMA()
{
	m_HDMARequested = false;
	m_HDMAInProgress = true;
	m_IORegisters[REG_HDMA5 - 0xFF00] &= 0b01111111;	//DMA in progress (bit 7 cleared)
}

void Bus::finishHDMA()
{
	m_HDMAInProgress = false;
	m_IORegisters[REG_HDMA5 - 0xFF00] |= 0b10000000;	//DMA completed (bit 7 set)
}

bool Bus::getInCompatibilityMode()
{
	return m_isInCompatibilityMode;
}

void Bus::m_DMATransfer(uint8_t base)
{
	uint16_t newAddr = ((unsigned int)base << 8);
	for (unsigned int i = 0; i < 0xA0; i++)
	{
		write(0xFE00 + i, read(newAddr + i));
	}
}

void Bus::m_GDMATransfer(uint8_t length)
{
	uint8_t srcHigh = read(REG_HDMA1);
	uint8_t srcLow = read(REG_HDMA2) & 0xF0;
	uint16_t src = (srcHigh << 8) | srcLow;

	uint8_t dstHigh = read(REG_HDMA3) & 0b00011111;
	uint8_t dstLow = read(REG_HDMA4) & 0xF0;
	uint16_t dst = 0x8000 + ((dstHigh << 8) | dstLow);

	std::cout << std::hex << src << " " << dst << std::endl;

	for (int i = 0; i < (int)(length+1)*16; i++)
		write(dst + i, read(src + i));

	m_IORegisters[REG_HDMA5 - 0xFF00] |= 0b10000000;
}