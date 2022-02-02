#pragma once

#include<iostream>
#include"Logger.h"

struct Channel
{
	uint8_t r[5];
};

class APU
{
public:
	APU();
	~APU();

	void step(unsigned long cycleDiff);

	void writeIORegister(uint16_t address, uint8_t value);
	uint8_t readIORegister(uint16_t address);

private:
	Channel m_channels[4];
	uint8_t NR50 = 0;
	uint8_t NR51 = 0;
	uint8_t NR52 = 0;
};