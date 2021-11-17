#include"PPU.h"

PPU::PPU(MMU* mmu)
{
	m_mmu = mmu;

}

PPU::~PPU()
{

}

void PPU::step(unsigned long cycleCount)
{
	if (!m_getDisplayEnabled())
		return;

	unsigned long cycleDiff = cycleCount - m_lastCycleCount;
	uint8_t curLine = m_mmu->read(REG_LY);
	uint8_t status = m_mmu->read(REG_STAT);

	switch (m_displayMode)
	{
	case 2:
		if (cycleDiff >= 20)
		{
			m_lastCycleCount = cycleCount;
			m_displayMode = 3;
			status |= 0b00000011;	//set lower two bits to 3 (11)
		}
		break;
	case 3:
		if (cycleDiff >= 43)
		{
			m_lastCycleCount = cycleCount;
			m_displayMode = 0;
			status &= 0b11111100;	//set lower two bits to 0 (00)
		}
		break;
	case 0:
		if (cycleDiff >= 51)
		{
			m_lastCycleCount = cycleCount;
			curLine++;
			m_renderScanline(curLine);
			if (curLine == 144)
			{
				//enter vblank
				m_displayMode = 1;
				status &= 0b11111100; status |= 0b00000001;
				//todo: request vblank interrupt
			}
			else
			{
				//go back to OAM search otherwise and continue scanning lines
				m_displayMode = 2;
				status &= 0b11111100; status |= 0b00000010;
			}
		}
		break;
	case 1:
		if (cycleDiff >= 114)
		{
			m_lastCycleCount = cycleCount;
			curLine++;
			if (curLine > 154)
			{
				m_displayMode = 2;
				status &= 0b11111100; status |= 0b00000010;
				curLine = 0;
				//render sprites
				memcpy((void*)m_dispBuffer, (void*)m_backBuffer, 160 * 144 * sizeof(vec3));	//copy over backbuffer to display buffer
			}
		}
		break;
	}

	m_mmu->write(REG_STAT, status);	//update ppu registers (status and cur line)
	m_mmu->write(REG_LY, curLine);
}

void PPU::m_renderScanline(uint8_t line)
{
	if (line < 0 || line > 143)
		return;

	uint8_t scrollY = 0; //(m_mmu->read(REG_SCY)) % 256;		//these wrap around (tilemap in memory is 256x256, only a 160x144 portion is actually rendered)
	uint8_t scrollX = 0;// (m_mmu->read(REG_SCX)) % 256;

	uint16_t m_backgroundBase = (m_getTileMapDisplaySelect()) ? 0x9C00 : 0x9800;

	m_backgroundBase += ((line + scrollY) / 8) * 32;	//Divide by 8 using floor division to get correct row number. Then multiply by 32 because there exist 32 tiles per row

	for (uint16_t column = 0; column < 160; column++)
	{
		uint8_t m_tileMapXCoord = (column + scrollX) % 256;
		uint16_t m_curBackgroundAddress = m_backgroundBase + (m_tileMapXCoord / 8); //divide x coord by 8 similarly, to put it into tile coords from pixel coords
		uint16_t m_tileIndex = m_mmu->read(m_curBackgroundAddress);	//now we have tile index which we can lookup in the tile data map

		uint16_t tileMemLocation = (m_getTileDataSelect()) ? 0x8000 : 0x8800;
		tileMemLocation += (m_tileIndex * 16);
		//tile is 16 bytes long. each 2 bytes specifies a specific row.
		tileMemLocation += ((line + scrollY) % 8) * 2;	//so we do modulus of scrolled y coord to find out the row, then multiply by two for alignment

		uint8_t tileData1 = m_mmu->read(tileMemLocation);		//extract two bytes that make up the tile
		uint8_t tileData2 = m_mmu->read(tileMemLocation + 1);

		//now extract the bit we're after

		uint8_t bitIndex = (column + scrollX) % 8;
		uint8_t higher = (tileData1 >> (7 - bitIndex)) & 0b1;
		uint8_t lower = (tileData2 >> (7 - bitIndex)) & 0b1;
		uint8_t colorId = (higher << 1) | lower;

		//todo: implement programmable palette maybe?
		vec3 color = {};
		switch (colorId)
		{
		case 0b00: color = { 0.f,0.f,0.f }; break;
		case 0b01:color = { 0.37f,0.37f,0.37f }; break;
		case 0b10:color = { 0.75f,0.75f,0.75f }; break;
		case 0b11:color = { 1.0f,1.0f,1.0f }; break;
		}

		//now get coordinate that we are writing to (simple pointer arithmetic)
		int pixelIdx = (line * 160) + column;
		m_backBuffer[pixelIdx] = color;
	}

}

bool PPU::m_getDisplayEnabled()
{
	return ((m_mmu->read(REG_LCDC)) >> 7) & 0b00000001;
}

bool PPU::m_getTileDataSelect()
{
	return ((m_mmu->read(REG_LCDC)) >> 4) & 0b00000001;
}

bool PPU::m_getTileMapDisplaySelect()
{
	return ((m_mmu->read(REG_LCDC)) >> 3) & 0b00000001;
}