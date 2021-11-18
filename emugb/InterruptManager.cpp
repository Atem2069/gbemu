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
	}

	m_mmu->write(REG_IFLAGS, intFlags);
}

InterruptType InterruptManager::getActiveInterrupt()
{
	if (!interruptsEnabled)
		return InterruptType::None;

	uint8_t intFlags = m_mmu->read(REG_IFLAGS);
	uint8_t enabledInterrupts = m_mmu->read(REG_IE);

	uint8_t activeInterrupts = intFlags & enabledInterrupts;

	if ((activeInterrupts & 0b1))	//todo bad code rewrite
	{
		intFlags &= 0b11111110;
		m_mmu->write(REG_IFLAGS, intFlags);
		return InterruptType::VBlank;
	}
}

void InterruptManager::enableInterrupts()
{
	interruptsEnabled = true;
}

void InterruptManager::disableInterrupts()
{
	interruptsEnabled = false;
}