// license:BSD-3-Clause
// copyright-holders:Allard van der Bas
/***************************************************************************

    Wiping sound driver (quick hack of the Namco sound driver)

    used by wiping.cpp and clshroad.cpp

***************************************************************************/

#include "emu.h"
#include "audio/wiping.h"

DEFINE_DEVICE_TYPE(WIPING_CUSTOM, wiping_sound_device, "wiping_sound", "Wiping Custom Sound")

wiping_sound_device::wiping_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, WIPING_CUSTOM, tag, owner, clock),
	device_sound_interface(mconfig, *this),
	m_last_channel(nullptr),
	m_sound_prom(nullptr),
	m_sound_rom(nullptr),
	m_num_voices(0),
	m_sound_enable(0),
	m_stream(nullptr)
{
	memset(m_channel_list, 0, sizeof(wp_sound_channel)*MAX_VOICES);
	memset(m_soundregs, 0, sizeof(uint8_t)*0x4000);
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void wiping_sound_device::device_start()
{
	wp_sound_channel *voice;

	/* get stream channels */
	m_stream = stream_alloc(0, 1, clock()/2);

	/* allocate a buffer to mix into - 1 second's worth should be more than enough */
	m_mixer_buffer.resize(clock()/2);

	/* extract globals from the interface */
	m_num_voices = 8;
	m_last_channel = m_channel_list + m_num_voices;

	m_sound_rom = machine().root_device().memregion("samples")->base();
	m_sound_prom = machine().root_device().memregion("soundproms")->base();

	/* start with sound enabled, many games don't have a sound enable register */
	m_sound_enable = 1;

	/* reset all the voices */
	for (voice = m_channel_list; voice < m_last_channel; voice++)
	{
		voice->frequency = 0;
		voice->volume = 0;
		voice->wave = &m_sound_prom[0];
		voice->counter = 0;
	}

	save_item(NAME(m_soundregs));

	for (int i = 0; i < MAX_VOICES; i++)
	{
		save_item(NAME(m_channel_list[i].frequency), i);
		save_item(NAME(m_channel_list[i].counter), i);
		save_item(NAME(m_channel_list[i].volume), i);
		save_item(NAME(m_channel_list[i].oneshot), i);
		save_item(NAME(m_channel_list[i].oneshotplaying), i);
	}
}

/********************************************************************************/

void wiping_sound_device::sound_w(offs_t offset, uint8_t data)
{
	wp_sound_channel *voice;
	int base;

	/* update the streams */
	m_stream->update();

	/* set the register */
	m_soundregs[offset] = data;

	/* recompute all the voice parameters */
	if (offset <= 0x3f)
	{
		for (base = 0, voice = m_channel_list; voice < m_last_channel; voice++, base += 8)
		{
			voice->frequency = m_soundregs[0x02 + base] & 0x0f;
			voice->frequency = voice->frequency * 16 + ((m_soundregs[0x01 + base]) & 0x0f);
			voice->frequency = voice->frequency * 16 + ((m_soundregs[0x00 + base]) & 0x0f);

			voice->volume = m_soundregs[0x07 + base] & 0x0f;
			if (m_soundregs[0x5 + base] & 0x0f)
			{
				voice->wave = &m_sound_rom[128 * (16 * (m_soundregs[0x5 + base] & 0x0f)
						+ (m_soundregs[0x2005 + base] & 0x0f))];
				voice->oneshot = 1;
			}
			else
			{
				voice->wave = &m_sound_rom[16 * (m_soundregs[0x3 + base] & 0x0f)];
				voice->oneshot = 0;
				voice->oneshotplaying = 0;
			}
		}
	}
	else if (offset >= 0x2000)
	{
		voice = &m_channel_list[(offset & 0x3f)/8];
		if (voice->oneshot)
		{
			voice->counter = 0;
			voice->oneshotplaying = 1;
		}
	}
}

//-------------------------------------------------
//  sound_stream_update - handle a stream update
//-------------------------------------------------

void wiping_sound_device::sound_stream_update(sound_stream &stream, std::vector<read_stream_view> const &inputs, std::vector<write_stream_view> &outputs)
{
	auto &buffer = outputs[0];
	wp_sound_channel *voice;
	short *mix;
	int i;

	/* if no sound, we're done */
	if (m_sound_enable == 0)
	{
		buffer.fill(0);
		return;
	}

	/* zap the contents of the mixer buffer */
	std::fill_n(&m_mixer_buffer[0], buffer.samples(), 0);

	/* loop over each voice and add its contribution */
	for (voice = m_channel_list; voice < m_last_channel; voice++)
	{
		int f = 16*voice->frequency;
		int v = voice->volume;

		/* only update if we have non-zero volume and frequency */
		if (v && f)
		{
			const uint8_t *w = voice->wave;
			int c = voice->counter;

			mix = &m_mixer_buffer[0];

			/* add our contribution */
			for (i = 0; i < buffer.samples(); i++)
			{
				int offs;

				c += f;

				if (voice->oneshot)
				{
					if (voice->oneshotplaying)
					{
						offs = (c >> 15);
						if (w[offs>>1] == 0xff)
						{
							voice->oneshotplaying = 0;
						}

						else
						{
							/* use full byte, first the high 4 bits, then the low 4 bits */
							if (offs & 1)
								*mix++ += ((w[offs>>1] & 0x0f) - 8) * v;
							else
								*mix++ += (((w[offs>>1]>>4) & 0x0f) - 8) * v;
						}
					}
				}
				else
				{
					offs = (c >> 15) & 0x1f;

					/* use full byte, first the high 4 bits, then the low 4 bits */
					if (offs & 1)
						*mix++ += ((w[offs>>1] & 0x0f) - 8) * v;
					else
						*mix++ += (((w[offs>>1]>>4) & 0x0f) - 8) * v;
				}
			}

			/* update the counter for this voice */
			voice->counter = c;
		}
	}

	/* mix it down */
	mix = &m_mixer_buffer[0];
	for (i = 0; i < buffer.samples(); i++)
		buffer.put_int(i, *mix++, 128 * MAX_VOICES);
}
