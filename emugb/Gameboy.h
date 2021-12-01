#pragma once

#include<iostream>
#include<thread>
#include<fstream>
#include<chrono>
#include"MMU.h"
#include"CPU.h"
#include"PPU.h"
#include"Display.h"
#include"InterruptManager.h"
#include"Input.h"
#include"Timer.h"
#include"BIOS/biosdump.h"
class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void run();
private:

	void m_initialise();
	void m_destroy();

	MMU* m_mmu;
	CPU* m_cpu;
	PPU* m_ppu;
	InterruptManager* m_interruptManager;
	InputManager* m_inputManager;
	Timer* m_timer;

	InputState m_inputState = {};

	bool m_shouldRun = true;

	void displayWorker();
};