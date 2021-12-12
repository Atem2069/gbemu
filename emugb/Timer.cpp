#include"Timer.h"

Timer::Timer(std::shared_ptr<MMU>& mmu, std::shared_ptr<InterruptManager>& interruptManager)
{
	m_mmu = mmu;
	m_interruptManager = interruptManager;
	m_lastTime = std::chrono::high_resolution_clock::now();
	m_divLastTime = m_lastTime;
}

Timer::~Timer()
{

}

void Timer::tick()
{
	auto m_curTime = std::chrono::high_resolution_clock::now();

	//first deal with DIV register, then timer register
	double divTimeDiff = std::chrono::duration<double, std::milli>(m_curTime - m_divLastTime).count();
	if (divTimeDiff >= ((1.0 / 16384.0) * 1000.0))	//DIV always updates, never disabled. Runs at 16384Hz
	{
		uint8_t curDivTimer = m_mmu->read(REG_DIV);
		m_mmu->write(0xFF04, curDivTimer + 1);

		m_divLastTime = m_curTime;
	}

	uint8_t timerControl = m_mmu->read(REG_TAC);
	if ((timerControl >> 2) & 0b1)	//bit 2: timer enabled
	{
		double timerDiff = std::chrono::duration<double, std::milli>(m_curTime - m_lastTime).count();

		double timePeriod = 0.0f;
		switch (timerControl & 0b11)
		{
		case 0:timePeriod = 1.0 / 4096.0; break;
		case 1:timePeriod = 1.0 / 262144.0; break;
		case 2:timePeriod = 1.0 / 65536.0; break;
		case 3:timePeriod = 1.0 / 16384.0; break;
		}

		if (timerDiff >= (timePeriod * 1000.0))
		{
			uint8_t curTimerValue = m_mmu->read(REG_TIMA);
			if (curTimerValue == 255)
			{
				m_interruptManager->requestInterrupt(InterruptType::Timer);
				m_mmu->write(REG_TIMA, m_mmu->read(REG_TMA));
			}
			else
				m_mmu->write(REG_TIMA, curTimerValue + 1);

			m_lastTime = m_curTime;
		}

	}

}