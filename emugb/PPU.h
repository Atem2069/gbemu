#pragma once

#include"Bus.h"
#include"InterruptManager.h"
#include"Logger.h"
#include"Common/vec3.h"
#include"Config.h"
#include"dmgRegisters.h"

struct Pixel
{
	bool shouldDraw;
	bool priority;
	uint8_t colIndex;
	uint8_t paletteIndex;
};

class PPU
{
public:
	PPU(std::shared_ptr<Bus>& bus, std::shared_ptr<InterruptManager>& interruptManager);
	~PPU();

	void step(unsigned long cycleCount);

	vec3* getDisplay();
private:
	int m_displayMode = 0;
	std::shared_ptr<Bus> m_bus;
	std::shared_ptr<InterruptManager> m_interruptManager;
	unsigned long m_lastCycleCount = 0;

	uint8_t lastLY = 0;
	uint8_t m_windowLineCount = 0;

	bool m_HDMAInProgress = false;
	uint16_t m_hdmaTransferBytes = 0;

	vec3 m_backBuffer[160*144] = {};	//scratchpad buffer used while rendering frame
	vec3 m_dispBuffer[160*144] = {};	//buffer that is only updated per vblank, which is returned to the display handler

	Pixel m_backgroundFIFO[160] = {};
	Pixel m_spriteFIFO[160] = {};

	void m_renderBackgroundScanline(uint8_t line);
	void m_renderWindowScanline(uint8_t line);
	void m_renderSprites(uint8_t line);
	void m_plotPixel(int x, int y, uint8_t colIdx, uint8_t paletteIdx, bool useObjPalette);
	unsigned int m_getColourFromPaletteIdx(uint8_t idx, uint8_t palette);

	//void m_HDMATransfer();

	bool m_getDisplayEnabled();
	bool m_getTileDataSelect();
	bool m_getBackgroundTileMapDisplaySelect();
	bool m_getWindowTileMapDisplaySelect();
	bool m_getMasterPriority();
	bool m_getWindowEnabled();
	bool m_getSpritesEnabled();
	bool m_spriteIs8x8();

};