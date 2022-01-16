#pragma once

// Blip_Wave is a synthesizer for adding a *single* waveform to a Blip_Buffer.
// A wave is built from a series of delays and new amplitudes. This provides a
// simpler interface than Blip_Synth, nothing more.
template<int quality, int range>
class Blip_Wave {
	Blip_Synth<quality, range> synth;
	long time_;
	int last_amp;
	void init() { time_ = 0; last_amp = 0; }
public:
	// Start wave at time 0 and amplitude 0
	Blip_Wave() { init(); }
	Blip_Wave(double volume) { init(); this->volume(volume); }

	// See Blip_Synth for description
	void volume(double v) { synth.volume(v); }
	void volume_unit(double v) { synth.volume_unit(v); }
	void treble_eq(const blip_eq_t& eq) { synth.treble_eq(eq); }
	Blip_Buffer* output() const { return synth.output(); }
	void output(Blip_Buffer* b) { synth.output(b); if (!b) time_ = last_amp = 0; }

	// Current time in frame
	long time() const { return time_; }
	void time(long t) { time_ = t; }

	// Current amplitude of wave
	int amplitude() const { return last_amp; }
	void amplitude(int);

	// Move forward by 't' time units
	void delay(long t) { time_ += t; }

	// End time frame of specified duration. Localize time to new frame.
	// If wave hadn't been run to end of frame, start it at beginning of new frame.
	void end_frame(long duration)
	{
		time_ -= duration;
		if (time_ < 0)
			time_ = 0;
	}
};

template<int quality, int range>
void Blip_Wave<quality, range>::amplitude(int amp) {
	int delta = amp - last_amp;
	last_amp = amp;
	synth.offset_inline(time_, delta);
}
