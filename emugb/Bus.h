#pragma once

#include<iostream>
#include<vector>
#include<array>
#include<chrono>

#include"MMU.h"
#include"APU.h"
#include"Logger.h"
#include"Config.h"
#include"dmgRegisters.h"
#include"cgbRegisters.h"
#include"debugStates.h"

class Bus
{
public:
	Bus(std::vector<uint8_t> bootRom, std::vector<uint8_t> ROM, std::shared_ptr<APU>& apu);
	~Bus();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);

	uint8_t readVRAM(uint8_t bank, uint16_t address);
	void writeVRAM(uint8_t bank, uint16_t address, uint8_t value);

	void writeIORegister(uint16_t address, uint8_t value);

	uint16_t readBgColor(uint8_t paletteIndex, uint8_t colorIndex);
	uint16_t readObjColor(uint8_t paletteIndex, uint8_t colorIndex);

	void HDMATransfer();

	bool getInCompatibilityMode();
	bool getInDoubleSpeedMode();
	void toggleDoubleSpeedMode();
private:
	std::shared_ptr<MMU> m_mbc;
	std::shared_ptr<APU> m_apu;
	bool m_isInBootRom = true;
	bool m_hdmaInProgress = false;
	void m_DMATransfer(uint8_t base);
	void m_GDMATransfer(uint8_t length);

	std::vector<uint8_t> m_bootRom;

	uint8_t m_workRAM[8][4096];
	uint8_t m_VRAM[2][8192];
	uint8_t m_OAM[160];
	uint8_t m_IORegisters[128];
	uint8_t m_HRAM[128];

	uint8_t m_paletteMemory[64];
	uint8_t m_paletteIndex = 0;
	bool m_incrementPaletteIndex = false;

	uint8_t m_objPaletteMemory[64];
	uint8_t m_objPaletteIndex = 0;
	bool m_incrementObjPaletteIndex = false;

	uint8_t m_WRAMBank = 1;	//default bank values
	uint8_t m_VRAMBank = 0;

	bool m_isInCompatibilityMode = false;
	bool m_isInDoubleSpeedMode = false;
};