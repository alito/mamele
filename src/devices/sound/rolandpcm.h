// license:BSD-3-Clause
// copyright-holders:Valley Bell
#ifndef MAME_SOUND_ROLANDPCM_H
#define MAME_SOUND_ROLANDPCM_H

#pragma once

class rolandpcm_device : public device_t, public device_sound_interface, public device_rom_interface
{
public:
	rolandpcm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	u8 read(offs_t offset);
	void write(offs_t offset, u8 data);

protected:
	// device-level overrides
	virtual void device_start() override;

	// sound stream update overrides
	virtual void sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples) override;

	// device_rom_interface overrides
	virtual void rom_bank_updated() override;

	static int16_t decode_sample(int8_t data);
	static int16_t sample_interpolate(int16_t smp1, int16_t smp2, uint16_t frac);

private:
	static constexpr unsigned NUM_CHANNELS = 32;

	struct pcm_channel
	{
		pcm_channel() { }

		// registers
		uint16_t mode = 0;
		uint16_t bank = 0;
		uint16_t step = 0;      // 2.14 fixed point (0x4000 equals 32000 Hz)
		uint16_t volume = 0;
		uint32_t start = 0;     // start address (18.14 fixed point)
		uint16_t end = 0;       // end offset (high word)
		uint16_t loop = 0;      // loop offset (high word)
		// work variables
		bool enable = false;
		int8_t play_dir = 0;    // playing direction, -1 [backwards] / 0 [stopped] / +1 [forwards]
		uint32_t addr = 0;      // current address
		int16_t smpl_cur = 0;   // current sample
		int16_t smpl_nxt = 0;   // next sample
	};

	uint32_t m_clock;                   // clock
	uint32_t m_rate;                    // sample rate (usually 32000 Hz)
	sound_stream* m_stream;             // stream handle
	pcm_channel m_chns[NUM_CHANNELS];   // channel memory
	uint8_t m_sel_chn;                  // selected channel
};

DECLARE_DEVICE_TYPE(ROLANDPCM, rolandpcm_device)

#endif // MAME_SOUND_ROLANDPCM_H
