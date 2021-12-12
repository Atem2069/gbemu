#pragma once

//gets input from display (because GetAsyncKeyState will destroy CPU thread), and then sets the ff00 joypad register correctly
#include"Logger.h"
#include"MMU.h"
#include"InterruptManager.h"

struct InputState
{
	bool up;
	bool down;
	bool left;
	bool right;
	bool a;
	bool b;
	bool start;
	bool select;
};


const uint16_t REG_JOYPAD = 0xFF00;

class InputManager
{
public:
	InputManager(std::shared_ptr<MMU>& mmu, std::shared_ptr<InterruptManager>& interruptManager);
	~InputManager();

	void tick(InputState curInputState);

private:
	std::shared_ptr<MMU> m_mmu;
	std::shared_ptr<InterruptManager> m_interruptManager;
};