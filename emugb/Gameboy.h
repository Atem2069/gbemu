#pragma once

#include<iostream>
#include<fstream>
#include"MMU.h"
#include"CPU.h"
class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void run();
private:
	MMU* m_mmu;
	CPU* m_cpu;

	bool m_shouldRun = true;
};