// license:BSD-3-Clause
// copyright-holders:K.Wilkins,Couriersud,Derrick Renaud,Frank Palazzolo,Jonathan Gevaryahu
#ifndef MAME_SOUND_FLT_BIQUAD_H
#define MAME_SOUND_FLT_BIQUAD_H

#pragma once

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> filter_biquad_device

class filter_biquad_device : public device_t, public device_sound_interface
{
public:
	enum class biquad_type : int
	{
		LOWPASS1P = 0,
		HIGHPASS1P,
		LOWPASS,
		HIGHPASS,
		BANDPASS,
		NOTCH,
		PEAK,
		LOWSHELF,
		HIGHSHELF
	};

	filter_biquad_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	// set up the filter with the specified filter parameters and return a pointer to the new device
	filter_biquad_device& setup(biquad_type type, double fc, double q, double gain);
	// update an existing instance with new filter parameters
	void update_params(biquad_type type, double fc, double q, double gain);

	// helper setup functions to create common filters representable by biquad filters
	// Sallen-Key low-pass
	filter_biquad_device& opamp_sk_lowpass_setup(double r1, double r2, double r3, double r4, double c1, double c2);
	// TODO when needed: Sallen-Key band-pass
	// TODO when needed: Sallen-Key high-pass

	// Multiple-Feedback low-pass
	filter_biquad_device& opamp_mfb_lowpass_setup(double r1, double r2, double r3, double c1, double c2);

	// Multiple-Feedback band-pass
	filter_biquad_device& opamp_mfb_bandpass_setup(double r1, double r2, double r3, double c1, double c2);

	// Multiple-Feedback high-pass
	filter_biquad_device& opamp_mfb_highpass_setup(double r1, double r2, double c1, double c2, double c3);


protected:
	// device-level overrides
	virtual void device_start() override;

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, std::vector<read_stream_view> const &inputs, std::vector<write_stream_view> &outputs) override;

private:
	void recalc();
	void step();

	sound_stream*  m_stream;
	biquad_type    m_type;
	int            m_last_sample_rate;
	double         m_fc;
	double         m_q;
	double         m_gain;

	stream_buffer::sample_t m_input;
	double m_w0, m_w1, m_w2; /* w[k], w[k-1], w[k-2], current and previous intermediate values */
	stream_buffer::sample_t m_output;
	double m_a1, m_a2; /* digital filter coefficients, denominator */
	double m_b0, m_b1, m_b2;  /* digital filter coefficients, numerator */
};

DECLARE_DEVICE_TYPE(FILTER_BIQUAD, filter_biquad_device)

#endif // MAME_SOUND_FLT_BIQUAD_H
