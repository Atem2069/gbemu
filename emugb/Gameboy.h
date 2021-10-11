#pragma once

#include<iostream>
#include<fstream>
#include"MMU.h"

class GameBoy
{
public:
	GameBoy();
	~GameBoy();

	void run();
private:
	MMU* m_mmu;

	bool m_shouldRun = true;
};