#pragma once

#include "Blip_Impulse_.h"
#include "Blip_Buffer.h"

#include <algorithm>
#include <cmath>

void Blip_Impulse_::init(uint32_t* imps, int w, int r, int fb)
{
	fine_bits = fb;
	width = w;
	impulses = (uint16_t*)imps;
	generate = true;
	volume_unit_ = -1.0;
	res = r;

	impulse = &impulses[width * res * 2 * (fine_bits ? 2 : 1)];
	offset = 0;
}

void Blip_Impulse_::scale_impulse(int unit, uint16_t* imp_in) const
{
	const long offset = ((long)unit << impulse_bits) - impulse_offset * unit +
		(1 << (impulse_bits - 1));
	uint16_t* imp = imp_in;
	uint16_t* fimp = impulse;
	for (int n = res / 2 + 1; n--; )
	{
		int error = unit;
		for (int nn = width; nn--; )
		{
			long a = ((long)*fimp++ * unit + offset) >> impulse_bits;
			error -= a - unit;
			*imp++ = (uint16_t)a;
		}

		// add error to middle
		imp[-width / 2 - 1] += (uint16_t)error;
	}

	if (res > 2)
	{
		// second half is mirror-image
		const uint16_t* rev = imp - width - 1;
		for (int nn = (res / 2 - 1) * width - 1; nn--; )
			*imp++ = *--rev;
		*imp++ = (uint16_t)unit;
	}

	// copy to odd offset
	*imp++ = (uint16_t)unit;
	std::copy_n(imp_in, res * width - 1, imp);

	/*
	for ( int i = 0; i < res; i++ )
	{
		for ( int j = 0; j < width; j++ )
			printf( "%6d,", imp_in [i * width + j] - 0x8000 );
		printf( "\n" );
	}*/
}

void Blip_Impulse_::fine_volume_unit()
{
	// to do: find way of merging in-place without temporary buffer

	uint16_t temp[Blip_Buffer::max_res * 2 * Blip_Buffer::widest_impulse_];
	scale_impulse((offset & 0xffff) << fine_bits, temp);
	uint16_t* imp2 = impulses + res * 2 * width;
	scale_impulse(offset & 0xffff, imp2);

	// merge impulses
	uint16_t* imp = impulses;
	uint16_t* src2 = temp;
	for (int n = res / 2 * 2 * width; n--; )
	{
		*imp++ = *imp2++;
		*imp++ = *imp2++;
		*imp++ = *src2++;
		*imp++ = *src2++;
	}
}

void Blip_Impulse_::volume_unit(double new_unit)
{
	if (new_unit == volume_unit_)
		return;

	if (generate)
		treble_eq(blip_eq_t(-8.87, 8800, 44100));

	volume_unit_ = new_unit;

	offset = 0x10001 * (unsigned long)std::floor(volume_unit_ * 0x10000 + 0.5);

	if (fine_bits)
		fine_volume_unit();
	else
		scale_impulse(offset & 0xffff, impulses);
}

void Blip_Impulse_::treble_eq(const blip_eq_t& new_eq)
{
	if (!generate && new_eq.treble == eq.treble && new_eq.cutoff == eq.cutoff &&
		new_eq.sample_rate == eq.sample_rate)
		return; // already calculated with same parameters

	generate = false;
	eq = new_eq;

	double treble = pow(10.0, 1.0 / 20 * eq.treble); // dB (-6dB = 0.50)
	if (treble < 0.000005)
		treble = 0.000005;

	const double treble_freq = 22050.0; // treble level at 22 kHz harmonic
	const double sample_rate = eq.sample_rate;
	const double pt = treble_freq * 2 / sample_rate;
	double cutoff = eq.cutoff * 2 / sample_rate;
	if (cutoff >= pt * 0.95 || cutoff >= 0.95)
	{
		cutoff = 0.5;
		treble = 1.0;
	}

	// DSF Synthesis (See T. Stilson & J. Smith (1996),
	// Alias-free digital synthesis of classic analog waveforms)

	// reduce adjacent impulse interference by using small part of wide impulse
	const double n_harm = 4096;
	const double rolloff = pow(treble, 1.0 / (n_harm * pt - n_harm * cutoff));
	const double rescale = 1.0 / pow(rolloff, n_harm * cutoff);

	const double pow_a_n = rescale * pow(rolloff, n_harm);
	const double pow_a_nc = rescale * pow(rolloff, n_harm * cutoff);

	double total = 0.0;
	const double to_angle = pi / 2 / n_harm / Blip_Buffer::max_res;

	float buf[Blip_Buffer::max_res * (Blip_Buffer::widest_impulse_ - 2) / 2];
	const int size = Blip_Buffer::max_res * (width - 2) / 2;
	for (int i = size; i--; )
	{
		const double angle = (i * 2 + 1) * to_angle;

		// equivalent
		//double y =     dsf( angle, n_harm * cutoff, 1.0 );
		//y -= rescale * dsf( angle, n_harm * cutoff, rolloff );
		//y += rescale * dsf( angle, n_harm,          rolloff );

		const double cos_angle = cos(angle);
		const double cos_nc_angle = cos(n_harm * cutoff * angle);
		const double cos_nc1_angle = cos((n_harm * cutoff - 1.0) * angle);

		const double b = 2.0 - 2.0 * cos_angle;
		const double a = 1.0 - cos_angle - cos_nc_angle + cos_nc1_angle;

		const double d = 1.0 + rolloff * (rolloff - 2.0 * cos_angle);
		const double c = pow_a_n * rolloff * cos((n_harm - 1.0) * angle) -
			pow_a_n * cos(n_harm * angle) -
			pow_a_nc * rolloff * cos_nc1_angle +
			pow_a_nc * cos_nc_angle;

		// optimization of a / b + c / d
		double y = (a * d + c * b) / (b * d);

		// fixed window which affects wider impulses more
		if (width > 12)
		{
			const double window = cos(n_harm / 1.25 / Blip_Buffer::widest_impulse_ * angle);
			y *= window * window;
		}

		total += (float)y;
		buf[i] = (float)y;
	}

	// integrate runs of length 'max_res'
	const double factor = impulse_amp * 0.5 / total; // 0.5 accounts for other mirrored half
	uint16_t* imp = impulse;
	const int step = Blip_Buffer::max_res / res;
	int offset = res > 1 ? Blip_Buffer::max_res : Blip_Buffer::max_res / 2;
	for (int n = res / 2 + 1; n--; offset -= step)
	{
		for (int w = -width / 2; w < width / 2; w++)
		{
			double sum = 0;
			for (int i = Blip_Buffer::max_res; i--; )
			{
				int index = w * Blip_Buffer::max_res + offset + i;
				if (index < 0)
					index = -index - 1;
				if (index < size)
					sum += buf[index];
			}
			*imp++ = (uint16_t)std::floor(sum * factor + (impulse_offset + 0.5));
		}
	}

	// rescale
	const double unit = volume_unit_;
	if (unit >= 0)
	{
		volume_unit_ = -1;
		volume_unit(unit);
	}
}
