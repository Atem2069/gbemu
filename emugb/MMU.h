#pragma once

#include<iostream>
#include<vector>
#include<array>
#include<chrono>

#include"Logger.h"
#include"Config.h"

class MMU
{
public:
	MMU() {}
	MMU(std::array<uint8_t,256> m_firmware, std::vector<uint8_t> m_ROM);	//initialize with BIOS code and game ROM
	~MMU();

	//virtual void updateROM(std::vector<uint8_t> newROM);						//Reset memory and write new ROM

	virtual uint8_t read(uint16_t address);										//Read 8-bit value
	virtual void write(uint16_t address, uint8_t value);						//Write 8-bit value

private:
	std::array<uint8_t, 65536> m_memory = {};
	std::array<uint8_t,256> m_BIOS;
	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
	void m_DMATransfer(uint8_t base);

};

class MBC1 : public MMU
{
public:
	MBC1(std::array<uint8_t,256> m_firmware, std::vector<uint8_t> m_ROM);
	~MBC1();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
private:
	uint8_t m_bankNumber = 0, m_higherBankBits = 0;
	bool m_bankSwitchRequired = false;
	bool m_RAMBanking = false;
	std::array<uint8_t,65536> m_memory;
	//std::vector<uint8_t> m_ROM;	//ROM in its entirety for bank switching.
	std::vector<std::array<uint8_t, 16384>> m_ROMBanks;
	std::array<uint8_t,256> m_BIOS;
	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
	void m_DMATransfer(uint8_t base);
};

class MBC3 : public MMU
{
public:
	MBC3(std::array<uint8_t, 256> m_firmware, std::vector<uint8_t> m_ROM);
	~MBC3();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
private:
	uint16_t m_bankNumber = 0;
	uint8_t m_ramBankNumber = 0;
	uint8_t m_lastLatchWrite = 0xff;	//writing 0 then 1 toggles latching/unlatching rtc
	bool m_rtcLatched = false;
	std::chrono::steady_clock::time_point m_rtcStart, m_rtcLast;
	std::array<uint8_t, 65536> m_memory;
	std::vector<std::array<uint8_t, 16384>> m_ROMBanks;
	std::array<uint8_t, 256> m_BIOS;

	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
	void m_DMATransfer(uint8_t base);
};

class MBC5 : public MMU
{
public:
	MBC5(std::array<uint8_t, 256> m_firmware, std::vector<uint8_t> m_ROM);
	~MBC5();

	uint8_t read(uint16_t address);
	void write(uint16_t address, uint8_t value);
private:
	uint16_t m_bankNumber = 0;
	uint16_t m_bankNumberHighBit = 0;
	uint8_t m_ramBankNumber = 0;
	bool m_bankSwitchRequired = false;
	std::array<uint8_t, 65536> m_memory;
	//std::vector<uint8_t> m_ROM;	//ROM in its entirety for bank switching.
	std::vector<std::array<uint8_t, 16384>> m_ROMBanks;
	std::array<uint8_t, 256> m_BIOS;
	bool m_isInBIOS = true;		//initially bios is enabled so 0x0-0xFF is mapped to BIOS rom. When exiting the BIOS 0xFF50 is written to.. explained further in write()
	void m_DMATransfer(uint8_t base);
};