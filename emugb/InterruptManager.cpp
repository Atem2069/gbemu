#include "InterruptManager.h"

InterruptManager::InterruptManager(MMU* mmu)
{
	m_mmu = mmu;
}

InterruptManager::~InterruptManager()
{

}

void InterruptManager::requestInterrupt(InterruptType interrupt)
{
	uint8_t intFlags = m_mmu->read(REG_IFLAGS);

	switch (interrupt)
	{
	case InterruptType::VBlank:
		intFlags |= 0b00000001;
		break;
	case InterruptType::STAT:
		intFlags |= 0b00000010;
		break;
	}

	m_mmu->write(REG_IFLAGS, intFlags);
}

InterruptType InterruptManager::getActiveInterrupt()
{
	uint8_t intFlags = m_mmu->read(REG_IFLAGS);
	uint8_t enabledInterrupts = m_mmu->read(REG_IE);

	uint8_t activeInterrupts = intFlags & enabledInterrupts;
	InterruptType chosenInterruptType = InterruptType::None;
	if ((activeInterrupts & 0b1))	//todo bad code rewrite
	{
		intFlags &= 0b11111110;
		chosenInterruptType = InterruptType::VBlank;
	}
	else if ((activeInterrupts >> 1) & 0b1)
	{
		intFlags &= 0b11111101;
		chosenInterruptType = InterruptType::STAT;
	}

	if (interruptsEnabled)				//only turn down flags if interrupts enabled
		m_mmu->write(REG_IFLAGS, intFlags);

	return chosenInterruptType;
}

void InterruptManager::enableInterrupts()
{
	interruptsEnabled = true;
}

void InterruptManager::disableInterrupts()
{
	interruptsEnabled = false;
}

bool InterruptManager::getInterruptsEnabled()
{
	return interruptsEnabled;
}