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
class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void run();
private:
	MMU* m_mmu;
	CPU* m_cpu;
	PPU* m_ppu;
	InterruptManager* m_interruptManager;
	InputManager* m_inputManager;

	InputState m_inputState = {};

	bool m_shouldRun = true;

	void displayWorker();
};