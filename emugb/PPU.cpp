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
			//m_HDMATransfer();
			m_bus->HDMATransfer();
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
				m_plotPixel(i, curLine, 0, 0, false);
				Pixel backgroundPixel = m_backgroundFIFO[i];
				Pixel spritePixel = m_spriteFIFO[i];
				if(backgroundPixel.shouldDraw)
					m_plotPixel(i, curLine, backgroundPixel.colIndex, backgroundPixel.paletteIndex, false);

				if (spritePixel.shouldDraw)
				{
					if ((!backgroundPixel.priority && spritePixel.priority) || (backgroundPixel.priority && backgroundPixel.colIndex == 0) || backgroundPixel.colIndex == 0)
						m_plotPixel(i, curLine, spritePixel.colIndex, spritePixel.paletteIndex, true);
				}

				m_backgroundFIFO[i] = {};
				m_spriteFIFO[i] = {};
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
	if (line < 0 || line > 143 || (!m_getMasterPriority() && m_bus->getInCompatibilityMode()))	//In CGB mode: LCDC bit 0 is master priority. In DMG mode, it is bg enable/disable
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

		uint8_t tileBank = 0;
		uint8_t tilePalette = 0;
		bool yFlip = false;
		bool xFlip = false;
		bool backgroundPriority = false;

		if (!m_bus->getInCompatibilityMode())
		{
			uint8_t m_tileAttributes = m_bus->readVRAM(1, m_curBackgroundAddress);
			tileBank = (m_tileAttributes & 0b00001000) >> 3;
			tilePalette = (m_tileAttributes & 0b00000111);
			yFlip = (m_tileAttributes & 0b01000000) >> 6;
			xFlip = (m_tileAttributes & 0b00100000) >> 5;
			backgroundPriority = (m_tileAttributes & 0b10000000) >> 7;
			if (!m_getMasterPriority())
				backgroundPriority = false;
		}

		uint16_t tileMemLocation = (m_getTileDataSelect()) ? 0x8000 : 0x8800;
		if (!m_getTileDataSelect())
			m_tileIndex += 128;
		tileMemLocation += (m_tileIndex * 16);
		//tile is 16 bytes long. each 2 bytes specifies a specific row.
		if (!yFlip)
			tileMemLocation += ((line + scrollY) % 8) * 2;	//so we do modulus of scrolled y coord to find out the row, then multiply by two for alignment
		else
			tileMemLocation += (7 - ((line + scrollY) % 8)) * 2;
		uint8_t tileData1 = m_bus->readVRAM(tileBank,tileMemLocation);		//extract two bytes that make up the tile
		uint8_t tileData2 = m_bus->readVRAM(tileBank,tileMemLocation + 1);
		uint8_t scr_column = column + (scrollX % 8);
		uint8_t colLower = (tileData1 >> (7 - (scr_column % 8))) & 0b1;
		uint8_t colHigher = (tileData2 >> (7 - (scr_column % 8))) & 0b1;

		if (xFlip)
		{
			colLower = (tileData1 >> (scr_column % 8)) & 0b1;
			colHigher = (tileData2 >> (scr_column % 8)) & 0b1;
		}

		uint8_t colIdx = (colHigher << 1) | colLower;

		if(m_bus->getInCompatibilityMode())
			colIdx = m_getColourFromPaletteIdx(colIdx, m_bus->read(0xFF47));

		Pixel px = {};
		px.colIndex = colIdx;
		px.paletteIndex = tilePalette;
		px.priority = backgroundPriority;
		px.shouldDraw = true;
		m_backgroundFIFO[column] = px;
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


		uint8_t tileBank = 0;
		uint8_t tilePalette = 0;
		bool xFlip = false;
		bool yFlip = false;
		bool windowPriority = false;

		if (!m_bus->getInCompatibilityMode())
		{
			uint8_t m_tileAttributes = m_bus->readVRAM(1, m_curTilemapAddress);
			tileBank = (m_tileAttributes & 0b00001000) >> 3;
			tilePalette = (m_tileAttributes & 0b00000111);
			yFlip = (m_tileAttributes & 0b01000000) >> 6;
			xFlip = (m_tileAttributes & 0b00100000) >> 5;
			windowPriority = (m_tileAttributes & 0b10000000) >> 7;
		}

		uint16_t tileMemLocation = (m_getTileDataSelect()) ? 0x8000 : 0x8800;
		if (!m_getTileDataSelect())
			m_tileIndex += 128;
		tileMemLocation += (m_tileIndex * 16);
		//tile is 16 bytes long. each 2 bytes specifies a specific row.
		if(!yFlip)
		tileMemLocation += (m_windowLineCount % 8) * 2;	//so we do modulus of scrolled y coord to find out the row, then multiply by two for alignment
		else
			tileMemLocation += (7 - (m_windowLineCount % 8)) * 2;

		uint8_t tileData1 = m_bus->readVRAM(tileBank,tileMemLocation);		//extract two bytes that make up the tile
		uint8_t tileData2 = m_bus->readVRAM(tileBank,tileMemLocation + 1);
		int pixelIdx = std::min((plotLine * 160) + (column+winX),23039);	//visual studio warns of false buffer overrun warning, idk why
		uint8_t colLower = (tileData1 >> (7 - (column % 8))) & 0b1;
		uint8_t colHigher = (tileData2 >> (7 - (column % 8))) & 0b1;

		if (xFlip)
		{
			colLower = (tileData1 >> (column % 8)) & 0b1;
			colHigher = (tileData2 >> (column % 8)) & 0b1;
		}

		uint8_t colIdx = (colHigher << 1) | colLower;
		if(m_bus->getInCompatibilityMode())
			colIdx = m_getColourFromPaletteIdx(colIdx, m_bus->read(0xFF47));
		
		Pixel px = {};
		px.colIndex = colIdx;
		px.paletteIndex = tilePalette;
		px.priority = windowPriority;
		px.shouldDraw = true;
		m_backgroundFIFO[std::min(column + winX, 159)] = px;
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
		uint8_t paletteIdx = (attributes & 0b00000111);
		auto tileBank = (attributes & 0b00001000) >> 3;

		uint8_t paletteData = 0;

		if (m_bus->getInCompatibilityMode())
		{
			tileBank = 0;
			uint16_t obpAddr = (((attributes & 0b00010000) >> 4)==1) ? 0xFF49 : 0xFF48;
			uint8_t grayscalePalette = m_bus->read(obpAddr);
			paletteData = grayscalePalette;
		}

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
		uint8_t byte1 = m_bus->readVRAM(tileBank,addr);
		uint8_t byte2 = m_bus->readVRAM(tileBank,addr + 1);
		//process bytes and draw to screen
		for (int k = 0; k < 8; k++)
		{
			if (x + k < 0)
				continue;
			int column = std::min(x + k, 159);
			if (m_posAtPixel[column] <= x && m_bus->getInCompatibilityMode())			//DMG: x-coord priority
				continue;
			if (m_spriteFIFO[column].shouldDraw && !m_bus->getInCompatibilityMode())	//CGB: OAM priority
				continue;

			int pixelIdx = (line * 160) + column;
			uint8_t byteShift = (7 - (k % 8));
			if (xFlip)
				byteShift = (k % 8);
			uint8_t colLower = (byte1 >> byteShift) & 0b1;
			uint8_t colHigher = (byte2 >> byteShift) & 0b1;
			uint8_t colIdx = (colHigher << 1) | colLower;

			if (colIdx == 0)
				continue;

			if (m_bus->getInCompatibilityMode())
			{
				uint8_t tmpcolIdx = m_getColourFromPaletteIdx(colIdx, paletteData);
				colIdx = tmpcolIdx;
				paletteIdx = 0;
			}

			Pixel px = {};
			px.colIndex = colIdx;
			px.paletteIndex = paletteIdx;
			px.priority = !spritePriority;
			px.shouldDraw = true;
			m_spriteFIFO[column] = px;
			m_posAtPixel[column] = x;			//if sprite pixel is not blank, update pos data (otherwise transparency breaks in sprite-sprite interactions)
		}

		renderedSpriteCount++;
	}
}

void PPU::m_plotPixel(int x, int y, uint8_t colIndex, uint8_t paletteIndex, bool useObjPalette)
{
	uint16_t col = 0;
	if (!useObjPalette)									//there exist two palette memory areas - one for just background/window tiles, and one for sprites
		col = m_bus->readBgColor(paletteIndex, colIndex);
	else
		col = m_bus->readObjColor(paletteIndex, colIndex);

	int red = (col & 0b0000000000011111);
	red = (red << 3) | (red >> 2);
	int green = (col & 0b0000001111100000) >> 5;
	green = (green << 3) | (green >> 2);
	int blue = (col & 0b0111110000000000) >> 10;
	blue = (blue << 3) | (blue >> 2);

	vec3 res = { (float)red / 255.0f,(float)green / 255.0f,(float)blue / 255.0f };
	int pixelIdx = (y*160)+x;
	m_backBuffer[pixelIdx] = res;
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

bool PPU::m_getMasterPriority()
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