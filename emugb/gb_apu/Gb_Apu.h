
// Nintendo Game Boy PAPU sound chip emulator

// Gb_Snd_Emu 0.1.4. Copyright (C) 2003-2005 Shay Green. GNU LGPL license.

#pragma once

#include "Gb_Oscs.h"

class Gb_Apu {
public:
	Gb_Apu();
	~Gb_Apu();
	
	// Set overall volume of all oscillators, where 1.0 is full volume
	void volume( double );
	
	// Set treble equalization
	void treble_eq( const blip_eq_t& );
	
	// Reset oscillators and internal state
	void reset();
	
	// Assign all oscillator outputs to specified buffer(s). If buffer
	// is NULL, silence all oscillators.
	void output( Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right );
	
	// Assign single oscillator output to buffer(s). Valid indicies are 0 to 3,
	// which refer to Square 1, Square 2, Wave, and Noise.
	// If buffer is NULL, silence oscillator.
	enum { osc_count = 4 };
	void osc_output( int index, Blip_Buffer* center, Blip_Buffer* left, Blip_Buffer* right );
	
	// Reads and writes at addr must satisfy start_addr <= addr <= end_addr
	enum { start_addr = 0xff10 };
	enum { end_addr   = 0xff3f };
	enum { register_count = end_addr - start_addr + 1 };
	
	// Write 'data' to address at specified time
	void write_register(uint16_t, uint8_t data );
	
	// Read from address at specified time
	uint8_t read_register(uint16_t);
	
	// Run all oscillators up to specified time, end current time frame, then
	// start a new frame at time 0. Return true if any oscillators added
	// sound to one of the left/right buffers, false if they only added
	// to the center buffer.
	bool end_frame( long );

	void update_time(long);
	
private:
	// noncopyable
	Gb_Apu( const Gb_Apu& );
	Gb_Apu& operator = ( const Gb_Apu& );
	
	Gb_Osc*     oscs [osc_count];
	long   next_frame_time;
	long   last_time;
	int         frame_count;
	bool        stereo_found;
	
	Gb_Square   square1;
	Gb_Square   square2;
	Gb_Wave     wave;
	Gb_Noise    noise;
	uint8_t regs [register_count];
	Gb_Square::Synth square_synth; // shared between squares
	Gb_Wave::Synth   other_synth;  // shared between wave and noise
	
	long m_timeArg = 0;

	void run_until( long );
};
