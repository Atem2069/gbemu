#include"Timer.h"

Timer::Timer(std::shared_ptr<Bus>& bus, std::shared_ptr<InterruptManager>& interruptManager)
{
	m_bus = bus;
	m_interruptManager = interruptManager;
	m_lastTime = std::chrono::high_resolution_clock::now();
	m_divLastTime = m_lastTime;

	m_bus->write(REG_TAC, 0);
	m_bus->write(REG_TIMA, 0);
	m_bus->write(REG_TMA, 0);
	m_bus->write(REG_DIV, 0);
}

Timer::~Timer()
{

}

void Timer::tick(unsigned long cycleCount)
{

	unsigned long cycleDiff = cycleCount - lastCycleCount;
	unsigned long divCycleDiff = cycleCount - divLastCycleCount;
	//for div register:
	if (divCycleDiff >= 64)
	{
		uint8_t ticks = divCycleDiff / 64;
		m_bus->write(0xFF04, m_bus->read(REG_DIV) + ticks);
		divLastCycleCount = cycleCount;
	}

	uint8_t timerControl = m_bus->read(REG_TAC);
	if ((timerControl >> 2) & 0b1)
	{
		unsigned long tickThreshold = 0;
		switch (timerControl & 0b11)
		{
		case 0b00:tickThreshold = (1024 / 4); break;
		case 0b01:tickThreshold = (16 / 4); break;
		case 0b10:tickThreshold = (64 / 4); break;
		case 0b11:tickThreshold = (256 / 4); break;
		}

		while (cycleDiff >= tickThreshold)
		{
			uint8_t ticks = 1;
			uint8_t curTimerValue = m_bus->read(REG_TIMA);
			uint8_t newTimerValue = curTimerValue + ticks;
			if (curTimerValue==255)
			{
				m_interruptManager->requestInterrupt(InterruptType::Timer);
				m_bus->write(REG_TIMA, m_bus->read(REG_TMA));
			}
			else
				m_bus->write(REG_TIMA, newTimerValue);
			cycleDiff -= tickThreshold;
			lastCycleCount = cycleCount;
		}
	}
	else
		lastCycleCount = cycleCount;

}