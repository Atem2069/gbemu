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
	//todo
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