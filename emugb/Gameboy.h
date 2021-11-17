#pragma once

#include<iostream>
#include<fstream>
#include"MMU.h"
#include"CPU.h"
#include"PPU.h"
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

	bool m_shouldRun = true;
};