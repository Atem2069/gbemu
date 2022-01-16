
// Blip_Buffer 0.3.4. http://www.slack.net/~ant/libs/

#include "Blip_Buffer.h"

#include <string>
#include <cstddef>
#include <cmath>
#include <algorithm>

/* Copyright (C) 2003-2005 Shay Green. This module is free software; you
can redistribute it and/or modify it under the terms of the GNU Lesser
General Public License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version. This
module is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
more details. You should have received a copy of the GNU Lesser General
Public License along with this module; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

#include BLARGG_SOURCE_BEGIN

void Blip_Buffer::clear( )
{
	offset_ = 0;
	reader_accum = 0;
	if ( !buffer_.empty() )
		std::fill_n(buffer_.begin(), buffer_size_ + widest_impulse_, sample_offset_);
}

void Blip_Buffer::set_sample_rate( long new_rate, int msec ) noexcept
{
	unsigned new_size = (std::numeric_limits<unsigned>::max() >> BLIP_BUFFER_ACCURACY) + 1 - widest_impulse_ - 64;
	if ( msec != blip_default_length )
	{
		new_size = (new_rate * (msec + 1) + 999) / 1000;
	}
	
	if ( buffer_size_ != new_size )
	{
		buffer_size_ = 0;
		offset_ = 0;
		
		int const count_clocks_extra = 2;
		buffer_.resize(new_size + widest_impulse_ + count_clocks_extra);
	}
	
	buffer_size_ = new_size;
	length_ = new_size * 1000 / new_rate - 1;
	
	samples_per_sec = new_rate;
	if ( clocks_per_sec )
		clock_rate( clocks_per_sec ); // recalculate factor
	
	bass_freq( bass_freq_ ); // recalculate shift
	
	clear();
}

unsigned long Blip_Buffer::clock_rate_factor( long clock_rate ) const
{
	return (unsigned long) std::floor(
			(double) samples_per_sec / clock_rate * (1L << BLIP_BUFFER_ACCURACY) + 0.5 );
}

void Blip_Buffer::bass_freq( int freq )
{
	bass_freq_ = freq;
	if ( freq == 0 )
	{
		bass_shift = 31; // 32 or greater invokes undefined behavior elsewhere
		return;
	}
	bass_shift = 1 + (int) std::floor( 1.442695041 * std::log( 0.124 * samples_per_sec / freq ) );
	if ( bass_shift < 0 )
		bass_shift = 0;
	if ( bass_shift > 24 )
		bass_shift = 24;
}

void Blip_Buffer::remove_samples( long count )
{
	if ( !count ) // optimization
		return;
	
	remove_silence( count );
	
	// Allows synthesis slightly past time passed to end_frame(), as long as it's
	// not more than an output sample.
	// to do: kind of hacky, could add run_until() which keeps track of extra synthesis
	int const copy_extra = 1;
	
	// copy remaining samples to beginning and clear old samples
	const long remain = samples_avail() + widest_impulse_ + copy_extra;
	if ( count >= remain )
		std::move(buffer_.cbegin() + count, buffer_.cbegin() + count + remain, buffer_.begin());
	else
		std::copy_n(buffer_.cbegin() + count, remain, buffer_.begin());
	std::fill_n(buffer_.begin() + remain, count, sample_offset_);
}
