#pragma once

#include<iostream>
#include<vector>

#include"Logger.h"

class MMU
{
public:
	MMU(std::vector<uint8_t> m_firmware, std::vector<uint8_t> m_ROM);	//initialize with BIOS code and game ROM
	~MMU();

	void updateROM(std::vector<uint8_t> newROM);						//Reset memory and write new ROM

	uint8_t read(uint16_t address);										//Read 8-bit value
	void write(uint16_t address, uint8_t value);						//Write 8-bit value


private:
	std::vector<uint8_t> m_memory;
	std::vector<uint8_t> m_BIOS;
	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
};