#include"Input.h"

InputManager::InputManager(std::shared_ptr<Bus>& bus, std::shared_ptr<InterruptManager>& interruptManager)
{
	m_bus = bus;
	m_interruptManager = interruptManager;
}

InputManager::~InputManager()
{

}

void InputManager::tick(InputState curInputState)
{
	uint8_t joypad = m_bus->read(REG_JOYPAD) & 0b11110000;
	bool dirKeys = (joypad >> 5) & 0b1;
	if (!dirKeys)
	{
		//button keys are being checked
		joypad |= (curInputState.a ? 0b00000001 : 0b0);
		joypad |= (curInputState.b ? 0b00000010 : 0b0);
		joypad |= (curInputState.select ? 0b00000100 : 0b0);
		joypad |= (curInputState.start ? 0b00001000 : 0b0);
		
	}
	else
	{
		//direction keys 
		joypad |= (curInputState.right ? 0b00000001 : 0b0);
		joypad |= (curInputState.left ? 0b00000010 : 0b0);
		joypad |= (curInputState.up ? 0b00000100 : 0b0);
		joypad |= (curInputState.down ? 0b00001000 : 0b0);

	}
	joypad ^= 0b00001111;	//flip bits (1=not pressed, 0=pressed)

	m_bus->write(REG_JOYPAD, joypad);
}