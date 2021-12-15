#include"Input.h"

InputManager::InputManager(std::shared_ptr<MMU>& mmu, std::shared_ptr<InterruptManager>& interruptManager)
{
	m_mmu = mmu;
	m_interruptManager = interruptManager;
}

InputManager::~InputManager()
{

}

void InputManager::tick(InputState curInputState)
{
	uint8_t joypad = m_mmu->read(REG_JOYPAD) & 0b11110000;
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
	//joypad = ~joypad;	//flip bits (high=off, low=on for some reason)
	joypad ^= 0b00001111;
	//if (joypad != m_mmu->read(REG_JOYPAD))
	//	m_interruptManager->requestInterrupt(InterruptType::Joypad);

	m_mmu->write(REG_JOYPAD, joypad);
}