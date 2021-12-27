#include"PPU.h"

PPU::PPU(std::shared_ptr<Bus>& bus, std::shared_ptr<InterruptManager>& interruptManager)
{
	m_bus = bus;
	m_interruptManager = interruptManager;

}

PPU::~PPU()
{

}

void PPU::step(unsigned long cycleCount)
{
	if (!m_getDisplayEnabled())
	{
		m_bus->write(REG_LY, 0);
		m_bus->write(REG_STAT, m_bus->read(REG_STAT) & 0b11111100);
		return;
	}

	unsigned long cycleDiff = cycleCount - m_lastCycleCount;
	uint8_t curLine = m_bus->read(REG_LY);
	uint8_t status = m_bus->read(REG_STAT);

	//reading out flags which can trigger STAT interrupt
	bool OAMSTAT = (status >> 5) & 0b1;
	bool HBLankSTAT = (status >> 3) & 0b1;	//Triggered when entering hblank
	bool VBlankSTAT = (status >> 4) & 0b1;	//Triggered when entering vblank
	bool LYCSTAT = (status >> 6) & 0b1;		//Triggered due to some comparison between LY and LYC


	switch (m_displayMode)
	{
	case 2:
		if (cycleDiff >= 20)
		{
			m_lastCycleCount = cycleCount;
			m_displayMode = 3;
			if (OAMSTAT)
				m_interruptManager->requestInterrupt(InterruptType::STAT);
			status |= 0b00000011;	//set lower two bits to 3 (11)
		}
		break;
	case 3:
		if (cycleDiff >= 43)
		{
			m_lastCycleCount = cycleCount;
			m_displayMode = 0;
			if (HBLankSTAT)
				m_interruptManager->requestInterrupt(InterruptType::STAT);
			status &= 0b11111100;	//set lower two bits to 0 (00)
		}
		break;
	case 0:  //hblank
		if (cycleDiff >= 51)
		{
			m_lastCycleCount = cycleCount;

			m_renderBackgroundScanline(curLine);
			m_renderWindowScanline(curLine);
			m_renderSprites(curLine);

			for (int i = 0; i < 160; i++)
			{
				unsigned int pixelIdx = (curLine * 160) + i;
				unsigned int bgIndex = m_backgroundFIFO[i];
				unsigned int spriteIndex = m_spriteFIFO[i];
				//m_backBuffer[pixelIdx] = m_getColourFromPaletteIdx(bgIndex, m_bus->read(0xFF47));

				uint8_t backgroundPaletteIdx = m_backgroundPaletteIndices[i];
				uint16_t col = m_bus->readBgColor(backgroundPaletteIdx, bgIndex);

				int red = (col & 0b0000000000011111);
				int green = (col & 0b0000001111100000) >> 5;
				int blue = (col & 0b0111110000000000) >> 10;

				vec3 res = { (float)red / 32.0f,(float)green / 32.0f,(float)blue / 32.0f };

				m_backBuffer[pixelIdx] = res;

				if (spriteIndex != 0)
				{
					//m_backBuffer[pixelIdx] = m_getColourFromPaletteIdx(spriteIndex, m_bus->read(m_spritePaletteIndices[i]));
					uint8_t spritePaletteIdx = m_spritePaletteIndices[i];
					uint16_t col = m_bus->readObjColor(spritePaletteIdx, spriteIndex);
					int red = (col & 0b0000000000011111);
					int green = (col & 0b0000001111100000) >> 5;
					int blue = (col & 0b0111110000000000) >> 10;

					vec3 res = { (float)red / 32.0f,(float)green / 32.0f,(float)blue / 32.0f };
					m_backBuffer[pixelIdx] = res;
				}

				m_spriteFIFO[i] = 0;
				m_backgroundFIFO[i] = 0;
			}

			curLine++;
			if (curLine == 144)
			{
				//enter vblank
				m_displayMode = 1;
				status &= 0b11111100; status |= 0b00000001;
				m_interruptManager->requestInterrupt(InterruptType::VBlank);
				if (VBlankSTAT)
					m_interruptManager->requestInterrupt(InterruptType::STAT);
			}
			else
			{
				if (m_bus->getHDMA())
				{
					m_bus->acknowledgeHDMA();
					m_HDMAInProgress = true;
				}
				if (m_HDMAInProgress)
				{
					uint8_t transferLength = m_bus->read(REG_HDMA5) & 0b01111111;
					if (m_hdmaTransferBytes >= transferLength)
					{
						m_bus->finishHDMA();
						m_HDMAInProgress = false;
					}
					else
					{
						uint8_t srcHigh = m_bus->read(REG_HDMA1);
						uint8_t srcLow = m_bus->read(REG_HDMA2) & 0xF0;
						uint16_t src = (srcHigh << 8) | srcLow;

						uint8_t dstHigh = m_bus->read(REG_HDMA3) & 0x0F;
						uint8_t dstLow = m_bus->read(REG_HDMA4) & 0xF0;
						uint16_t dst = (dstHigh << 8) | dstLow;
						for (int i = 0; i < 16; i++)
						{
							m_bus->write(dst + m_hdmaTransferBytes + i, m_bus->read(src + m_hdmaTransferBytes + i));
						}
						m_hdmaTransferBytes += 16;
					}
				}
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
				m_windowLineCount = 0;
				memcpy((void*)m_dispBuffer, (void*)m_backBuffer, 160 * 144 * sizeof(vec3));	//copy over backbuffer to display buffer
			}
		}
		break;
	}

	if (LYCSTAT)
	{
		uint8_t LYC = m_bus->read(REG_LYC);
		if (curLine == LYC && curLine != lastLY)
		{
			status |= 0b00000100;	//set LYC coincidence flag
			m_interruptManager->requestInterrupt(InterruptType::STAT);
		}
	}

	lastLY = curLine;
	m_bus->write(REG_STAT, status);	//update ppu registers (status and cur line)
	m_bus->write(REG_LY, curLine);
}

void PPU::m_renderBackgroundScanline(uint8_t line)	
{
	if (line < 0 || line > 143 || !m_getBackgroundEnabled())
		return;
	if (Config::getInstance()->getValue<bool>("ppuDebugOverride") && !Config::getInstance()->getValue<bool>("background"))
		return;
	uint8_t scrollY = (m_bus->read(REG_SCY));// % 256;		//these wrap around (tilemap in memory is 256x256, only a 160x144 portion is actually rendered)
	uint8_t scrollX = (m_bus->read(REG_SCX));// % 256;
	uint16_t m_backgroundBase = (m_getBackgroundTileMapDisplaySelect()) ? 0x9c00 : 0x9800;
	//get tilemap base addr
	m_backgroundBase += (((line + scrollY) % 256) / 8) * 32;	//Divide by 8 using floor division to get correct row number. Then multiply by 32 because there exist 32 tiles per row

	for (uint16_t column = 0; column < 160; column++)
	{
		uint8_t m_tileMapXCoord = (column + (scrollX)) % 256;
		uint16_t m_curBackgroundAddress = m_backgroundBase + (m_tileMapXCoord / 8); //divide x coord by 8 similarly, to put it into tile coords from pixel coords
		uint8_t m_tileIndex = m_bus->readVRAM(0,m_curBackgroundAddress);	//now we have tile index which we can lookup in the tile data map

		uint8_t m_tileAttributes = m_bus->readVRAM(1, m_curBackgroundAddress);
		uint8_t tileBank = (m_tileAttributes & 0b00001000) >> 3;
		uint8_t tilePalette = (m_tileAttributes & 0b00000111);

		uint16_t tileMemLocation = (m_getTileDataSelect()) ? 0x8000 : 0x8800;
		if (!m_getTileDataSelect())
			m_tileIndex += 128;
		tileMemLocation += (m_tileIndex * 16);
		//tile is 16 bytes long. each 2 bytes specifies a specific row.
		tileMemLocation += ((line + scrollY) % 8) * 2;	//so we do modulus of scrolled y coord to find out the row, then multiply by two for alignment

		uint8_t tileData1 = m_bus->readVRAM(tileBank,tileMemLocation);		//extract two bytes that make up the tile
		uint8_t tileData2 = m_bus->readVRAM(tileBank,tileMemLocation + 1);
		m_backgroundFIFO[column] = m_plotPixel(column, line, scrollX, tileData1, tileData2);
		m_backgroundPaletteIndices[column] = tilePalette;
	}

}

void PPU::m_renderWindowScanline(uint8_t line)
{
	if (line < 0 || line > 143 || !m_getWindowEnabled())
		return;
	if (Config::getInstance()->getValue<bool>("ppuDebugOverride") && !Config::getInstance()->getValue<bool>("window"))
		return;
	int8_t winX = m_bus->read(REG_WX)-7;	//winx register starts at 7
	uint8_t winY = m_bus->read(REG_WY);

	if (line < winY)
		return;

	if (winX > 160 || winX < -7)
		return;
	if (winX < 0 && winX >-7)		//std::min doesn't like int8_t, and cast to int feels wrong
		winX = 0;

	
	uint8_t plotLine = line;
	line -= winY;

	uint16_t m_windowBase = (m_getWindowTileMapDisplaySelect()) ? 0x9c00 : 0x9800;
	//get tilemap base addr
	m_windowBase += ((m_windowLineCount) / 8) * 32;	//Divide by 8 using floor division to get correct row number. Then multiply by 32 because there exist 32 tiles per row

	for (uint16_t column = 0; (column+winX) < 160; column++)
	{
		uint16_t m_curTilemapAddress = m_windowBase + (column / 8); //divide x coord by 8 similarly, to put it into tile coords from pixel coords
		uint8_t m_tileIndex = m_bus->readVRAM(0,m_curTilemapAddress);	//now we have tile index which we can lookup in the tile data map

		uint8_t m_tileAttributes = m_bus->readVRAM(1, m_curTilemapAddress);
		uint8_t tileBank = (m_tileAttributes & 0b00001000) >> 3;
		uint8_t tilePalette = (m_tileAttributes & 0b00000111);

		uint16_t tileMemLocation = (m_getTileDataSelect()) ? 0x8000 : 0x8800;
		if (!m_getTileDataSelect())
			m_tileIndex += 128;
		tileMemLocation += (m_tileIndex * 16);
		//tile is 16 bytes long. each 2 bytes specifies a specific row.
		tileMemLocation += (m_windowLineCount % 8) * 2;	//so we do modulus of scrolled y coord to find out the row, then multiply by two for alignment

		uint8_t tileData1 = m_bus->readVRAM(tileBank,tileMemLocation);		//extract two bytes that make up the tile
		uint8_t tileData2 = m_bus->readVRAM(tileBank,tileMemLocation + 1);
		int pixelIdx = std::min((plotLine * 160) + (column+winX),23039);	//visual studio warns of false buffer overrun warning, idk why
		uint8_t colLower = (tileData1 >> (7 - (column % 8))) & 0b1;
		uint8_t colHigher = (tileData2 >> (7 - (column % 8))) & 0b1;
		uint8_t colIdx = (colHigher << 1) | colLower;
		m_backgroundFIFO[std::min(column + winX,159)] = colIdx;
		m_backgroundPaletteIndices[std::min(column + winX, 259)] = tilePalette;
	}
	m_windowLineCount += 1;
}

void PPU::m_renderSprites(uint8_t line)
{
	//return if sprites not enabled otherwise we'll read garbage from OAM
	if (!m_getSpritesEnabled())
		return;
	if (Config::getInstance()->getValue<bool>("ppuDebugOverride") && !Config::getInstance()->getValue<bool>("sprites"))
		return;

	int m_posAtPixel[160] = {};				//store the x-origin of each sprite at each pixel coord we check
	std::fill(m_posAtPixel, m_posAtPixel + 160, 255);	//set to impossible value initially

	int renderedSpriteCount = 0, i = 0;
	while (i < 40 && renderedSpriteCount < 10)
	{
		uint16_t spriteAttribAddress = 0xFE00 + (i * 4);
		i++;
		auto y = m_bus->read(spriteAttribAddress) - 16;
		auto x = m_bus->read(spriteAttribAddress + 1) - 8;
		auto patternIdx = m_bus->read(spriteAttribAddress + 2);
		auto attributes = m_bus->read(spriteAttribAddress + 3);
		auto spritePriority = (attributes & 0b10000000) >> 7;
		auto yFlip = (attributes & 0b01000000) >> 6;
		auto xFlip = (attributes & 0b00100000) >> 5;
		//auto paletteIdx = (attributes & 0b00010000) >> 4;
		auto paletteIdx = (attributes & 0b00000111);

		uint8_t lowerBound = (m_spriteIs8x8() ? 8 : 16);

		if (!(line >= y && line < (y + lowerBound)))	//check if line is in the bounds of tile being considered
			continue;
		if (x > 160 || x < -8)
			continue;
		uint8_t lineOffset = line - y;
		uint8_t flipOffset = (m_spriteIs8x8()) ? 7 : 15;
		if (yFlip)
			lineOffset = flipOffset - lineOffset;

		if (!m_spriteIs8x8())
		{
			if (lineOffset < 8)
				patternIdx &= 0xFE;
			else
				patternIdx |= 0x01;
		}

		uint16_t addr = 0x8000 + (patternIdx * 16 + ((lineOffset % 8) * 2));
		uint8_t byte1 = m_bus->readVRAM(0,addr);
		uint8_t byte2 = m_bus->readVRAM(0,addr + 1);
		//process bytes and draw to screen
		for (int k = 0; k < 8; k++)
		{
			if (x + k < 0)
				continue;
			int column = std::min(x + k, 159);
			if (m_posAtPixel[column] <= x)		//if a sprite is already drawn, and has a lower than or equal x-origin than current sprite, then the current sprite isn't drawn there.
				continue;
			uint8_t paletteData = m_bus->read(0xFF48);
			if (paletteIdx==1)
				paletteData = m_bus->read(0xFF49);
			int pixelIdx = (line * 160) + column;
			if (spritePriority && m_backgroundFIFO[column] != 0)
				continue;
			uint8_t byteShift = (7 - (k % 8));
			if (xFlip)
				byteShift = (k % 8);
			uint8_t colLower = (byte1 >> byteShift) & 0b1;
			uint8_t colHigher = (byte2 >> byteShift) & 0b1;
			uint8_t colIdx = (colHigher << 1) | colLower;
			if (colIdx == 0)
				continue;
			m_spriteFIFO[column] = colIdx;
			m_spritePaletteIndices[column] = paletteIdx;
			m_posAtPixel[column] = x;			//if sprite pixel is not blank, update pos data (otherwise transparency breaks in sprite-sprite interactions)
		}

		renderedSpriteCount++;
	}
}

unsigned int PPU::m_plotPixel(int x, int y, int scroll, uint8_t byteHigh, uint8_t byteLow)
{
	int pixelIdx = (y * 160) + x;
	x += (scroll % 8);
	uint8_t colLower = (byteHigh >> (7 - (x%8))) & 0b1;
	uint8_t colHigher = (byteLow >> (7 - (x % 8))) & 0b1;
	uint8_t colIdx = (colHigher << 1) | colLower;
	return colIdx;
}

unsigned int PPU::m_getColourFromPaletteIdx(uint8_t idx, uint8_t palette)
{
	uint8_t colIdx = (palette >> (idx * 2)) & 0b00000011;

	return colIdx;
}

vec3* PPU::getDisplay()
{
	return m_dispBuffer;
}

bool PPU::m_getDisplayEnabled()
{
	return ((m_bus->read(REG_LCDC)) >> 7) & 0b00000001;
}

bool PPU::m_getTileDataSelect()
{
	return ((m_bus->read(REG_LCDC)) >> 4) & 0b00000001;
}

bool PPU::m_getBackgroundTileMapDisplaySelect()
{
	return ((m_bus->read(REG_LCDC)) >> 3) & 0b00000001;
}

bool PPU::m_getWindowTileMapDisplaySelect()
{
	return ((m_bus->read(REG_LCDC)) >> 6) & 0b00000001;
}

bool PPU::m_getBackgroundEnabled()
{
	return ((m_bus->read(REG_LCDC))) & 0b1;
}

bool PPU::m_getWindowEnabled()
{
	return ((m_bus->read(REG_LCDC)) >> 5) & 0b1;
}

bool PPU::m_getSpritesEnabled()
{
	return ((m_bus->read(REG_LCDC)) >> 1) & 0b1;
}

bool PPU::m_spriteIs8x8()
{
	return !(((m_bus->read(REG_LCDC)) >> 2) & 0b1);
}