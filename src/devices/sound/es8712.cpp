// license:BSD-3-Clause
// copyright-holders:Quench, David Graves, R. Belmont
/**********************************************************************************************
 *
 *  ES8712 Sound Controller chip for MSM5205/6585 and 74157-type TTL pair
 *  Chip is branded by Excellent Systems, probably OEM'd.
 *
 *  Samples are currently looped, but whether they should and how, is unknown.
 *  Interface to the chip is also not 100% clear.
 *
 *  Current implementation is based from :
 *  gcpinbal.cpp, by David Graves & R. Belmont.
 *  splitted by cam900
 *
 *  It seems that the ES8712 is actually a programmable counter which can stream
 *  ADPCM samples when hooked up to a ROM and a M5205 or M6585 (whose VCK signal
 *  can drive the counter). Neither of these seem to be used in conjunction with
 *  the ES8712 on Dream 9 and Dream 9 Final, which suggests it may have been used
 *  for an entirely different purpose there (perhaps to do with video timing).
 *
 **********************************************************************************************/


#include "emu.h"
#include "es8712.h"


// device type definition
DEFINE_DEVICE_TYPE(ES8712, es8712_device, "es8712", "Excellent Systems ES8712 Sound Controller")


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  es8712_device - constructor
//-------------------------------------------------

es8712_device::es8712_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, ES8712, tag, owner, clock)
	, device_rom_interface(mconfig, *this)
	, m_adpcm_select(*this, "adpcm_select")
	, m_msm(*this, finder_base::DUMMY_TAG)
	, m_reset_handler(*this)
	, m_msm_write_cb(*this)
	, m_playing(0)
	, m_base_offset(0)
	, m_start(0)
	, m_end(0)
	, m_adpcm_trigger(0)
{ }


//-------------------------------------------------
//  device_add_mconfig - device-specific machine
//  configuration addiitons
//-------------------------------------------------

void es8712_device::device_add_mconfig(machine_config &config)
{
	HCT157(config, m_adpcm_select, 0); // TODO : gcpinbal case, differs per games?
	m_adpcm_select->out_callback().set(FUNC(es8712_device::msm_w));
}


//-------------------------------------------------
//  device_start - start emulation of an ES8712 chip
//-------------------------------------------------

void es8712_device::device_start()
{
	es8712_state_save_register();
}


//-------------------------------------------------
//  device_reset - stop emulation of an ES8712-compatible chip
//-------------------------------------------------

void es8712_device::device_reset()
{
	if (m_playing)
	{
		m_playing = 0;
	}
	if (m_msm.found())
		m_msm->reset_w(1);

	m_reset_handler(CLEAR_LINE);
}


//-------------------------------------------------
//  state save support for MAME
//-------------------------------------------------

void es8712_device::es8712_state_save_register()
{
	save_item(NAME(m_playing));

	save_item(NAME(m_base_offset));

	save_item(NAME(m_start));
	save_item(NAME(m_end));

	save_item(NAME(m_adpcm_trigger));
}


//-------------------------------------------------
//  play -- Begin playing the addressed sample
//-------------------------------------------------

void es8712_device::play()
{
	assert(m_msm.found());

	if (m_start < m_end)
	{
		if (!m_playing)
		{
			logerror("Playing sample range %06x-%06x\n", m_start, m_end);
			m_playing = 1;
			m_msm->reset_w(0);
			m_reset_handler(CLEAR_LINE);
			m_base_offset = m_start;
		}
	}
	/* invalid samples go here */
	else
	{
		logerror("Request to play invalid sample range %06x-%06x\n", m_start, m_end);

		if (m_playing)
		{
			/* update the stream */
			m_playing = 0;
			m_reset_handler(ASSERT_LINE);
		}
	}
}



/**********************************************************************************************

     write -- generic data write functions

***********************************************************************************************/

/**********************************************************************************************
 *
 *  offset  Start       End
 *          0hmmll  -  0HMMLL
 *    00    ----ll
 *    01    --mm--
 *    02    0h----
 *    03               ----LL
 *    04               --MM--
 *    05               0H----
 *    06           Go!
 *
 * Offsets are written in the order -> 00, 02, 01, 03, 05, 04, 06
 * Offset 06 is written with the same value as offset 04.
 *
***********************************************************************************************/

void es8712_device::write(offs_t offset, uint8_t data)
{
	switch (offset)
	{
		case 0: case 1: case 2:
		{
			uint8_t shift = offset * 8;
			m_start &= ~(0xff << shift);
			m_start |= data << shift;
			m_start &= 0xfffff;
			break;
		}

		case 3: case 4: case 5:
		{
			uint8_t shift = (offset - 3) * 8;
			m_end &= ~(0xff << shift);
			m_end |= data << shift;
			m_end &= 0xfffff;
			break;
		}

		case 6:
			play();
			break;

		default:
			break;
	}
}

uint8_t es8712_device::read(offs_t offset)
{
	// busy state (other bits unknown)
	if (offset == 0)
		return m_playing ? 1 : 0;

	return 0;
}

void es8712_device::msm_w(offs_t offset, uint8_t data)
{
	m_msm_write_cb(offset, data);
}

void es8712_device::msm_int(int state)
{
	if (!state || !m_playing)
		return;

	if (m_base_offset >= 0x100000 || m_base_offset > m_end)
	{
		m_playing = 0;
		m_msm->reset_w(1);
		m_reset_handler(ASSERT_LINE);
		//m_base_offset = m_start;
		m_adpcm_trigger = 0;
	}
	else
	{
		m_adpcm_select->ab_w(read_byte(m_base_offset));
		m_adpcm_select->select_w(m_adpcm_trigger);
		m_adpcm_trigger ^= 1;
		if (m_adpcm_trigger == 0)
			m_base_offset++;
	}
}
