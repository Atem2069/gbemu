#pragma once

#include "blip_eq_t.h"

#include <cstdint>

class Blip_Impulse_ {

	static constexpr int impulse_bits = 15;
	static constexpr long impulse_amp = 1L << impulse_bits;
	static constexpr long impulse_offset = impulse_amp / 2;

	static constexpr double pi = 3.1415926535897932384626433832795029L;

	blip_eq_t eq;
	double  volume_unit_;
	uint16_t*  impulses;
	uint16_t*  impulse;
	int     width;
	int     fine_bits;
	int     res;
	bool    generate;

	void fine_volume_unit();
	void scale_impulse(int unit, uint16_t*) const;
public:
	uint32_t offset;

	void init(uint32_t* impulses, int width, int res, int fine_bits = 0);
	void volume_unit(double);
	void treble_eq(const blip_eq_t&);
};
