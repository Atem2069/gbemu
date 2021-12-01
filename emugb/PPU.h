#pragma once

#include"MMU.h"
#include"InterruptManager.h"
#include"Logger.h"
#include"Common/vec3.h"


const uint16_t REG_LCDC = 0xFF40;	//LCD control register
const uint16_t REG_STAT = 0xFF41;	//LCD status register
const uint16_t REG_SCY = 0xFF42;	//Y scroll offset
const uint16_t REG_SCX = 0xFF43;	//X scroll offset
const uint16_t REG_LY = 0xFF44;		//Holds current scanline being rendered (critical for game boy bootrom to do anything)

class PPU
{
public:
	PPU(MMU* mmu, InterruptManager* interruptManager);
	~PPU();

	void step(unsigned long cycleCount);

	vec3* getDisplay();
private:
	int m_displayMode = 0;
	MMU* m_mmu;
	InterruptManager* m_interruptManager;
	unsigned long m_lastCycleCount = 0;

	vec3 m_backBuffer[160*144] = {};	//scratchpad buffer used while rendering frame
	vec3 m_dispBuffer[160*144] = {};	//buffer that is only updated per vblank, which is returned to the display handler

	void m_renderBackgroundScanline(uint8_t line);
	void m_renderWindowScanline(uint8_t line);
	void m_renderSprites(uint8_t line);
	void m_plotPixel(int x, int y, uint8_t byteHigh, uint8_t byteLow);
	vec3 m_getColourFromPaletteIdx(uint8_t idx);

	bool m_getDisplayEnabled();
	bool m_getTileDataSelect();
	bool m_getBackgroundTileMapDisplaySelect();
	bool m_getWindowTileMapDisplaySelect();
	bool m_getBackgroundEnabled();
	bool m_getWindowEnabled();
	bool m_getSpritesEnabled();

};