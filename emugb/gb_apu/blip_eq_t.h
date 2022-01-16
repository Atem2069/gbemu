#pragma once

// Low-pass equalization parameters (see notes.txt)
class blip_eq_t {
public:
	blip_eq_t(double t = 0)
		: treble(t), cutoff(0), sample_rate(44100) {}
	blip_eq_t(double t, long c, long sr)
		: treble(t), cutoff(c), sample_rate(sr) {}
	double treble;
	long cutoff;
	long sample_rate;
};