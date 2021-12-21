#pragma once

#include"Logger.h"
#include"MMU.h"
#include"dmgRegisters.h"

enum class InterruptType
{
	None=0,
	VBlank=0x40,				//VBlank interrupt goes to vector 0x40
	STAT=0x48,					//LCD STAT interrupt
	Timer=0x50,					//generated regularly by interval timer
	Joypad=0x60
};


class InterruptManager
{
public:
	InterruptManager(std::shared_ptr<MMU>& mmu);
	~InterruptManager();

	void requestInterrupt(InterruptType interrupt);
	InterruptType getActiveInterrupt();

	void enableInterrupts();
	void disableInterrupts();
	bool getInterruptsEnabled();

private:
	std::shared_ptr<MMU> m_mmu;
	bool interruptsEnabled = false;
};