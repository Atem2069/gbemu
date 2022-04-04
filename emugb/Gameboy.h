#pragma once

#include<iostream>
#include<thread>
#include<fstream>
#include<chrono>
#include<mutex>
#include"Bus.h"
#include"CPU.h"
#include"PPU.h"
#include"Display.h"
#include"InterruptManager.h"
#include"Input.h"
#include"Timer.h"
#include"APU.h"
#include"BIOS/biosdump.h"
#include"Config.h"
#include"dmgRegisters.h"

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void run();
private:
	std::mutex m_bufAccessLock;	//thread accesses shared ptrs that can be deleted, so lock when reading or deleting
	void m_initialise();
	void m_destroy();
	bool m_loadCartridge(std::string name, std::shared_ptr<Bus>& bus);

	const uint8_t m_headerChecksumData[48] = { 0xCE, 0xED, 0x66, 0x66, 0xCC, 0x0D, 0x00, 0x0B, 0x03, 0x73, 0x00, 0x83, 0x00, 0x0C, 0x00, 0x0D,	//used for validating ROM nintendo logo
											0x00, 0x08, 0x11, 0x1F, 0x88, 0x89, 0x00, 0x0E, 0xDC, 0xCC, 0x6E, 0xE6, 0xDD, 0xDD, 0xD9, 0x99,
											0xBB, 0xBB, 0x67, 0x63, 0x6E, 0x0E, 0xEC, 0xCC, 0xDD, 0xDC, 0x99, 0x9F, 0xBB, 0xB9, 0x33, 0x3E };


	std::shared_ptr<Bus> m_bus;
	std::shared_ptr<CPU> m_cpu;
	std::shared_ptr<PPU> m_ppu;
	std::shared_ptr<InterruptManager> m_interruptManager;
	std::shared_ptr<InputManager> m_inputManager;
	std::shared_ptr<Timer> m_timer;
	std::shared_ptr<APU> m_apu;

	InputState m_inputState = {};

	bool m_shouldRun = true;
	bool m_initialized = false;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
	unsigned int m_cyclesSinceLastVblank = 0;

	void displayWorker();
};