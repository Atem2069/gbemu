#pragma once

#include"Logger.h"
#include"MMU.h"

enum class InterruptType
{
	None=0,
	VBlank=0x40,				//VBlank interrupt goes to vector 0x40
	STAT=0x48,					//LCD STAT interrupt
	Timer=0x50,					//generated regularly by interval timer
	Joypad=0x60
};

const uint16_t REG_IE = 0xFFFF;
const uint16_t REG_IFLAGS = 0xFF0F;

class InterruptManager
{
public:
	InterruptManager(MMU* mmu);
	~InterruptManager();

	void requestInterrupt(InterruptType interrupt);
	InterruptType getActiveInterrupt();

	void enableInterrupts();
	void disableInterrupts();
	bool getInterruptsEnabled();

private:
	MMU* m_mmu;
	bool interruptsEnabled = false;
};