#pragma once

#include<iostream>
#include<vector>

#include"Logger.h"

class MMU
{
public:
	MMU() {}
	MMU(std::vector<uint8_t> m_firmware, std::vector<uint8_t> m_ROM);	//initialize with BIOS code and game ROM
	~MMU();

	virtual void updateROM(std::vector<uint8_t> newROM);						//Reset memory and write new ROM

	virtual uint8_t read(uint16_t address);										//Read 8-bit value
	virtual void write(uint16_t address, uint8_t value);						//Write 8-bit value

private:
	std::vector<uint8_t> m_memory;
	std::vector<uint8_t> m_BIOS;
	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
	void m_DMATransfer(uint8_t base);

};

class MBC1 : public MMU
{
public:
	MBC1(std::vector<uint8_t> m_firmware, std::vector<uint8_t> m_ROM);
	~MBC1();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
private:
	uint8_t m_bankNumber = 0, m_higherBankBits = 0;
	bool m_bankSwitchRequired = false;
	bool m_RAMBanking = false;
	std::vector<uint8_t> m_memory;
	std::vector<uint8_t> m_ROM;	//ROM in its entirety for bank switching.
	std::vector<uint8_t> m_BIOS;
	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
	void m_DMATransfer(uint8_t base);
	void m_bankSwitch();
};