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

//cartridge header constants
static uint16_t CART_TITLE = 0x0134;
static uint16_t CART_TYPE = 0x0147;
static uint16_t CART_ROMSIZE = 0x148;
static uint16_t CART_RAMSIZE = 0x149;



class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void run();
private:

	void m_initialise();
	void m_destroy();
	bool m_loadCartridge(std::string name, MMU** mmu);

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