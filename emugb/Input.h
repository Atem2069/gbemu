#pragma once

//gets input from display (because GetAsyncKeyState will destroy CPU thread), and then sets the ff00 joypad register correctly
#include"Logger.h"
#include"MMU.h"

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
	InputManager(MMU* mmu);
	~InputManager();

	void tick(InputState curInputState);

private:
	MMU* m_mmu;
};