#pragma once

#include"Logger.h"
#include"MMU.h"
#include"InterruptManager.h"
#include"Config.h"
#include"dmgRegisters.h"
#include<chrono>



class Timer
{
public:
	Timer(std::shared_ptr<MMU>& mmu, std::shared_ptr<InterruptManager>& interruptManager);
	~Timer();

	void tick(unsigned long cycleCount);

private:
	unsigned long lastCycleCount = 0;
	unsigned long divLastCycleCount = 0;
	std::shared_ptr<MMU> m_mmu;
	std::shared_ptr<InterruptManager> m_interruptManager;
	std::chrono::steady_clock::time_point m_lastTime, m_divLastTime;
};