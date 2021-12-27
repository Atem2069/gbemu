#pragma once

#include<iostream>
#include<vector>
#include<array>
#include<chrono>

#include"MMU.h"
#include"Logger.h"
#include"Config.h"
#include"dmgRegisters.h"
#include"cgbRegisters.h"
#include"debugStates.h"

class Bus
{
public:
	Bus(std::vector<uint8_t> bootRom, std::vector<uint8_t> ROM);
	~Bus();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);

	bool getHDMA();
	void acknowledgeHDMA();
	void finishHDMA();
private:
	std::shared_ptr<MMU> m_mbc;
	bool m_isInBootRom = true;
	bool m_HDMARequested = false;
	void m_DMATransfer(uint8_t base);
	void m_GDMATransfer(uint8_t length);

	std::vector<uint8_t> m_bootRom;

	std::array<std::array<uint8_t, 4096>, 8> m_workRAM;
	std::array<std::array<uint8_t, 8192>,2> m_VRAM;
	std::array<uint8_t, 160> m_OAM;
	std::array<uint8_t, 128> m_IORegisters;
	std::array<uint8_t, 128> m_HRAM;

	uint8_t m_WRAMBank = 1;	//default bank values
	uint8_t m_VRAMBank = 0;
};