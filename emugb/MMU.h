#pragma once

#include<iostream>
#include<vector>
#include<array>
#include<chrono>

#include"Logger.h"
#include"Config.h"
#include"dmgRegisters.h"
#include"debugStates.h"
class MMU
{
public:
	MMU() {}
	MMU(std::array<uint8_t,256> m_firmware, std::vector<uint8_t> m_ROM);	//initialize with BIOS code and game ROM
	~MMU();


	virtual uint8_t read(uint16_t address);										//Read 8-bit value
	virtual void write(uint16_t address, uint8_t value);						//Write 8-bit value

private:
	std::array<uint8_t, 65536> m_memory = {};
	std::array<uint8_t,256> m_BIOS;
	bool m_isInBIOS = true;		
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
	uint8_t m_ramBankNumber = 0;
	bool m_RAMBanking = false;
	bool m_shouldSave = false;
	std::string m_saveName;
	std::array<uint8_t,65536> m_memory;
	std::vector<std::array<uint8_t, 16384>> m_ROMBanks;
	std::array<std::array<uint8_t, 8192>,4> m_RAMBanks;
	std::array<uint8_t,256> m_BIOS;
	bool m_isInBIOS = true;	
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

	bool m_isInBIOS = true;		
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
	std::array<uint8_t, 65536> m_memory;
	std::vector<std::array<uint8_t, 16384>> m_ROMBanks;
	std::array<uint8_t, 256> m_BIOS;
	bool m_isInBIOS = true;		
	void m_DMATransfer(uint8_t base);
};