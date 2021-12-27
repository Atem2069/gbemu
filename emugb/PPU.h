#pragma once

#include"Bus.h"
#include"InterruptManager.h"
#include"Logger.h"
#include"Common/vec3.h"
#include"Config.h"
#include"dmgRegisters.h"

class PPU
{
public:
	PPU(std::shared_ptr<Bus>& bus, std::shared_ptr<InterruptManager>& interruptManager);
	~PPU();

	void step(unsigned long cycleCount);

	unsigned int* getDisplay();
private:
	int m_displayMode = 0;
	//MMU* m_mmu;
	//InterruptManager* m_interruptManager;
	std::shared_ptr<Bus> m_bus;
	std::shared_ptr<InterruptManager> m_interruptManager;
	unsigned long m_lastCycleCount = 0;

	uint8_t lastLY = 0;
	uint8_t m_windowLineCount = 0;

	bool m_HDMAInProgress = false;
	uint8_t m_hdmaTransferBytes = 0;

	unsigned int m_backBuffer[160*144] = {};	//scratchpad buffer used while rendering frame
	unsigned int m_dispBuffer[160*144] = {};	//buffer that is only updated per vblank, which is returned to the display handler

	unsigned int m_backgroundFIFO[160] = {};	//background/window fifo combined into one
	unsigned int m_spriteFIFO[160] = {};
	unsigned int m_spritePaletteIndices[160] = {};

	void m_renderBackgroundScanline(uint8_t line);
	void m_renderWindowScanline(uint8_t line);
	void m_renderSprites(uint8_t line);
	unsigned int m_plotPixel(int x, int y, int scroll, uint8_t byteHigh, uint8_t byteLow);
	unsigned int m_getColourFromPaletteIdx(uint8_t idx, uint8_t palette);

	bool m_getDisplayEnabled();
	bool m_getTileDataSelect();
	bool m_getBackgroundTileMapDisplaySelect();
	bool m_getWindowTileMapDisplaySelect();
	bool m_getBackgroundEnabled();
	bool m_getWindowEnabled();
	bool m_getSpritesEnabled();
	bool m_spriteIs8x8();

};