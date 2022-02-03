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

	uint8_t m_dutyTable[4] =	//hardware predefined square wave duties
	{	
		0b00000001,				//12.5%
		0b00000011,				//25%
		0b00001111,				//50%
		0b11111100				//75%
	};

	//some other misc channel 2 stuff
	int chan2_freqTimer = 0;
	uint8_t chan2_waveDutyPosition = 0;
	int chan2_amplitude = 0;
	uint8_t chan2_lengthCounter = 0;

	//chan 3:
	uint8_t chan3_lengthCounter = 0;

	//frame sequencer
	unsigned long frameSeq_cycleDiff = 0;
};