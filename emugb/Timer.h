#pragma once

#include"Logger.h"
#include"Bus.h"
#include"InterruptManager.h"
#include"Config.h"
#include"dmgRegisters.h"
#include<chrono>



class Timer
{
public:
	Timer(std::shared_ptr<Bus>& bus, std::shared_ptr<InterruptManager>& interruptManager);
	~Timer();

	void tick(unsigned long cycleDiff);

private:
	unsigned long m_cycleCount;
	unsigned long m_divCycleCount;
	std::shared_ptr<Bus> m_bus;
	std::shared_ptr<InterruptManager> m_interruptManager;
	std::chrono::steady_clock::time_point m_lastTime, m_divLastTime;
};