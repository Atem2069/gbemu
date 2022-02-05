#pragma once

#include<iostream>
#include<SDL.h>
#undef main		//sdl sucks
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

	void step(unsigned long cycleCount);
	void playSamples();

	void writeIORegister(uint16_t address, uint8_t value);
	uint8_t readIORegister(uint16_t address);

private:
	unsigned long m_lastCycleCount = 0;
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
	unsigned long frameSeq_count = 0;

	//mixing/sampling
	unsigned long mixer_cycleDiff = 0;
	uint8_t samples[1024] = {};
	uint8_t curPlayingSamples[1024] = {};
	int sampleIndex = 0;
	SDL_AudioDeviceID mixer_audioDevice = {};
};