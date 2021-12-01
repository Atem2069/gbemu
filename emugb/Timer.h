#pragma once

#include"Logger.h"
#include"MMU.h"
#include"InterruptManager.h"

#include<chrono>

const uint16_t REG_DIV = 0xFF04;	//DIV  timer. incremented at 16384Hz
const uint16_t REG_TIMA = 0xFF05;	//Incremented at some rate according to FF07, and when it overflows triggers an interrupt
const uint16_t REG_TMA = 0xFF06;	//Loaded when TIMA overflows
const uint16_t REG_TAC = 0xFF07;	//Control register - sets if timer is enabled, as well as clock rate

class Timer
{
public:
	Timer(MMU* mmu, InterruptManager* interruptManager);
	~Timer();

	void tick();

private:
	MMU* m_mmu;
	InterruptManager* m_interruptManager;
	std::chrono::steady_clock::time_point m_lastTime, m_divLastTime;
};