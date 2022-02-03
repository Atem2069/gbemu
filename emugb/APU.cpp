#include"APU.h"

APU::APU()
{
	Logger::getInstance()->msg(LoggerSeverity::Info, "Init APU..");
	for (int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 5; j++)
			m_channels[i].r[j] = {};
	}

}

APU::~APU()
{

}

void APU::step(unsigned long cycleDiff)
{
	//sort out duty position
	chan2_freqTimer -= (cycleDiff * 4);	//cycle diff is measured in m-cycles, but frequency timer decrements per t-cycle.
	if (chan2_freqTimer <= 0)			//todo: account for when the timer goes negative (subtract difference)
	{
		uint8_t freqLow = m_channels[1].r[3];
		uint8_t freqHigh = m_channels[1].r[4] & 0b00000111;
		int newFreq = ((int)freqHigh << 8) | freqLow;
		chan2_freqTimer = (2048 - newFreq) * 4;

		chan2_waveDutyPosition++;		//new duty selected when frequency timer is reloaded.
	}

	//frame sequencer: 2048 m-cycles.
	frameSeq_cycleDiff += cycleDiff;
	if (frameSeq_cycleDiff >= 2048)
		frameSeq_cycleDiff -= 2048;

	if (frameSeq_cycleDiff % 2 == 0)
	{
		//clock length counters
		bool chan2_lengthEnabled = (m_channels[1].r[4] >> 6) & 0b1;
		if (chan2_lengthEnabled)
		{
			if (chan2_lengthCounter != 0)
				chan2_lengthCounter--;
			if (chan2_lengthCounter == 0)
			{
				NR52 &= 0b11111101;	//clear channel 2 enabled bit
			}
		}

		bool chan3_lengthEnabled = (m_channels[2].r[4] >> 6) & 0b1;
		if (chan3_lengthEnabled)
		{
			if (chan3_lengthCounter != 0)
				chan3_lengthCounter--;
			if (chan3_lengthCounter == 0)
			{
				NR52 &= 0b11111011;	//clear chan 3 bit
			}
		}
	}

	//have to clock other components (see nightshade)
	//volume envelope: 64 hz
	//sweep: 128 hz


	//after doing all timing and stuff, get amplitudes
	//Channel 2:
	int chan2_dutyIdx = (m_channels[1].r[1] & 0b11000000) >> 6;
	chan2_amplitude = (m_dutyTable[chan2_dutyIdx] >> chan2_waveDutyPosition) & 0b1;


	//mixing
	mixer_cycleDiff += (44100 * cycleDiff);
	if (mixer_cycleDiff >= (1048576))
	{
		mixer_cycleDiff -= 1048576;
		sampleIndex = (sampleIndex + 1) % 735;	//wrap around
		if (chan2_amplitude == 1)				//dumb hack lol, fix
			samples[sampleIndex] = 0xFFFF;
		else
			samples[sampleIndex] = 0x0000;
	}
}

void APU::playSamples()
{
	//todo
}

void APU::writeIORegister(uint16_t address, uint8_t value)
{
	if (address >= 0xFF10 && address <= 0xFF14)
		m_channels[0].r[address - 0xFF10] = value;
	if (address >= 0xFF15 && address <= 0xFF19)
	{
		if (address - 0xFF15 == 1)
			chan2_lengthCounter = 64 - (value & 0b00111111);
		m_channels[1].r[address - 0xFF15] = value;
	}
	if (address >= 0xFF1A && address <= 0xFF1E)
	{
		if (address - 0xFF1A == 1)
			chan3_lengthCounter = 256 - value;	//chan 3 uses full 8-bit value for length load
		if ((address - 0xFF1A) == 0 && ((value >> 7) & 0b1) ==0)	//DAC=0 disables channel in NR52
			NR52 &= 0b11111011;
		m_channels[2].r[address - 0xFF1A] = value;
	}
	if (address >= 0xFF1F && address <= 0xFF23)
		m_channels[3].r[address - 0xFF1F] = value;
	if (address == 0xFF24)
		NR50 = value;
	if (address == 0xFF25)
		NR51 = value;
	if (address == 0xFF26)
		NR52 = value;
	//todo: wave table read/writes
}

uint8_t APU::readIORegister(uint16_t address)
{
	if (address >= 0xFF10 && address <= 0xFF14)
		return m_channels[0].r[address - 0xFF10];
	if (address >= 0xFF15 && address <= 0xFF19)
		return m_channels[1].r[address - 0xFF15];
	if (address >= 0xFF1A && address <= 0xFF1E)
		return m_channels[2].r[address - 0xFF1A];
	if (address >= 0xFF1F && address <= 0xFF23)
		return m_channels[3].r[address - 0xFF1F];
	if (address == 0xFF24)
		return NR50;
	if (address == 0xFF25)
		return NR51;
	if (address == 0xFF26)
		return NR52;	//HACK: when length counters are properly implemented this is no longer necessary.
	return 0xFF;
}