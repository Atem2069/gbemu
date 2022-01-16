#pragma once

#include <cstdint>
#include <vector>

#include "Blip_Buffer.h"

// not documented yet (see Multi_Buffer.cpp for an example of use)
class Blip_Reader {
	std::vector<uint16_t>::const_iterator buf;
	long accum;
public:
	// avoid anything which might cause optimizer to put object in memory

	int begin(Blip_Buffer& blip_buf) {
		buf = blip_buf.buffer_.cbegin();
		accum = blip_buf.reader_accum;
		return blip_buf.bass_shift;
	}

	int read() const {
		return accum >> Blip_Buffer::accum_fract;
	}

	void next(int bass_shift = 9) {
		accum -= accum >> bass_shift;
		accum += ((long)*buf++ - Blip_Buffer::sample_offset_) << Blip_Buffer::accum_fract;
	}

	void end(Blip_Buffer& blip_buf) {
		blip_buf.reader_accum = accum;
	}
};

