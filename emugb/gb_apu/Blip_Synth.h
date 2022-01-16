
// Blip_Synth and Blip_Wave are waveform transition synthesizers for adding
// waveforms to a Blip_Buffer.

// Blip_Buffer 0.3.4. Copyright (C) 2003-2005 Shay Green. GNU LGPL license.

#pragma once

#include "Blip_Buffer.h"
#include "Blip_Impulse_.h"

// Blip_Synth is a transition waveform synthesizer which adds band-limited
// offsets (transitions) into a Blip_Buffer. For a simpler interface, use
// Blip_Wave (below).
//
// Range specifies the greatest expected offset that will occur. For a
// waveform that goes between +amp and -amp, range should be amp * 2 (half
// that if it only goes between +amp and 0). When range is large, a higher
// accuracy scheme is used; to force this even when range is small, pass
// the negative of range (i.e. -range).
template<int quality,int range>
class Blip_Synth {
	static_assert ( 1 <= quality && quality <= 5, "Quality" );
	static_assert( -32768 <= range && range <= 32767, "Range" );
	enum {
		abs_range = (range < 0) ? -range : range,
		fine_mode = (range > 512 || range < 0),
		width = (quality < 5 ? quality * 4 : Blip_Buffer::widest_impulse_),
		res = 1 << Blip_Buffer::blip_res_bits_,
		impulse_size = width / 2 * (fine_mode + 1),
		base_impulses_size = width / 2 * (res / 2 + 1),
		fine_bits = (fine_mode ? (abs_range <= 64 ? 2 : abs_range <= 128 ? 3 :
			abs_range <= 256 ? 4 : abs_range <= 512 ? 5 : abs_range <= 1024 ? 6 :
			abs_range <= 2048 ? 7 : 8) : 0)
	};
	uint32_t  impulses [impulse_size * res * 2 + base_impulses_size];
	Blip_Impulse_ impulse;
	void init() { impulse.init( impulses, width, res, fine_bits ); }
public:
	Blip_Synth()                            { init(); }
	Blip_Synth( double volume )             { init(); this->volume( volume ); }
	
	// Configure low-pass filter (see notes.txt). Not optimized for real-time control
	void treble_eq( const blip_eq_t& eq )   { impulse.treble_eq( eq ); }
	
	// Set volume of a transition at amplitude 'range' by setting volume_unit
	// to v / range
	void volume( double v )                 { impulse.volume_unit( v * (1.0 / abs_range) ); }
	
	// Set base volume unit of transitions, where 1.0 is a full swing between the
	// positive and negative extremes. Not optimized for real-time control.
	void volume_unit( double unit )         { impulse.volume_unit( unit ); }
	
	// Default Blip_Buffer used for output when none is specified for a given call
	
	// Add an amplitude offset (transition) with a magnitude of delta * volume_unit
	// into the specified buffer (default buffer if none specified) at the
	// specified source time. Delta can be positive or negative. To increase
	// performance by inlining code at the call site, use offset_inline().
	void offset( long, int delta, Blip_Buffer* ) const;
	
	void offset_resampled( unsigned long, int delta, Blip_Buffer* ) const;
	void offset_inline( long time, int delta, Blip_Buffer* buf ) const {
		offset_resampled( time * buf->factor_ + buf->offset_, delta, buf );
	}
};

// End of public interface
	
template<int quality,int range>
inline void Blip_Synth<quality,range>::offset_resampled( unsigned long time,
		int delta, Blip_Buffer* blip_buf ) const
{
	unsigned sample_index = (time >> Blip_Buffer::BLIP_BUFFER_ACCURACY) & ~1;
	assert(( "Blip_Synth/Blip_wave: Went past end of buffer",
			sample_index < blip_buf->buffer_size_ ));
	enum { const_offset = Blip_Buffer::widest_impulse_ / 2 - width / 2 };
	uint32_t* buf = (uint32_t*) &blip_buf->buffer_ [const_offset + sample_index];
	
	enum { shift = Blip_Buffer::BLIP_BUFFER_ACCURACY - Blip_Buffer::blip_res_bits_ };
	enum { mask = res * 2 - 1 };
	const uint32_t* imp = &impulses [((time >> shift) & mask) * impulse_size];
	
	uint32_t offset = impulse.offset * delta;
	
	if constexpr ( !fine_bits )
	{
		// normal mode
		for ( int n = width / 4; n; --n )
		{
			uint32_t t0 = buf [0] - offset;
			uint32_t t1 = buf [1] - offset;
			
			t0 += imp [0] * delta;
			t1 += imp [1] * delta;
			imp += 2;
			
			buf [0] = t0;
			buf [1] = t1;
			buf += 2;
		}
	}
	else
	{
		// fine mode
		enum { sub_range = 1 << fine_bits };
		delta += sub_range / 2;
		int delta2 = (delta & (sub_range - 1)) - sub_range / 2;
		delta >>= fine_bits;
		
		for ( int n = width / 4; n; --n )
		{
			uint32_t t0 = buf [0] - offset;
			uint32_t t1 = buf [1] - offset;
			
			t0 += imp [0] * delta2;
			t0 += imp [1] * delta;
			
			t1 += imp [2] * delta2;
			t1 += imp [3] * delta;
			
			imp += 4;
			
			buf [0] = t0;
			buf [1] = t1;
			buf += 2;
		}
	}
}

template<int quality,int range>
void Blip_Synth<quality,range>::offset( long time, int delta, Blip_Buffer* buf ) const {
	offset_resampled( time * buf->factor_ + buf->offset_, delta, buf );
}
