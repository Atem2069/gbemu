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
#include"BIOS/biosdump.h"
#include"Config.h"
#include"dmgRegisters.h"
#include"gb_apu/Gb_Apu.h"
#include"gb_apu/Multi_Buffer.h"

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

	std::shared_ptr<Bus> m_bus;
	std::shared_ptr<CPU> m_cpu;
	std::shared_ptr<PPU> m_ppu;
	std::shared_ptr<InterruptManager> m_interruptManager;
	std::shared_ptr<InputManager> m_inputManager;
	std::shared_ptr<Timer> m_timer;

	std::shared_ptr<Gb_Apu> m_apu;

	InputState m_inputState = {};

	bool m_shouldRun = true;
	bool m_initialized = false;

	std::chrono::time_point<std::chrono::high_resolution_clock> m_lastTime;
	unsigned int m_cyclesSinceLastVblank = 0;

	void displayWorker();
};