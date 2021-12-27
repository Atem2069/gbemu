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
	Bus(std::array<uint8_t, 256> bootRom, std::vector<uint8_t> ROM);
	~Bus();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
private:
	std::shared_ptr<MMU> m_mbc;
	bool m_isInBootRom = true;
	void m_DMATransfer(uint8_t base);

	std::array<uint8_t, 256> m_bootRom;

	std::array<uint8_t, 8192> m_workRAM;
	std::array<uint8_t, 8192> m_VRAM;
	std::array<uint8_t, 160> m_OAM;
	std::array<uint8_t, 128> m_IORegisters;
	std::array<uint8_t, 128> m_HRAM;
};