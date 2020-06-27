// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Driver for Williams/Midway X-unit games.

**************************************************************************/

#include "emu.h"
#include "cpu/tms34010/tms34010.h"
#include "cpu/m6809/m6809.h"
#include "audio/dcs.h"
#include "includes/midtunit.h"
#include "includes/midxunit.h"
#include "midwayic.h"

#define LOG_IO      (1 << 0)
#define LOG_UART    (1 << 1)
#define LOG_UNKNOWN (1 << 2)
#define LOG_SOUND   (1 << 3)

#define VERBOSE     (0)
#include "logmacro.h"


/*************************************
 *
 *  CMOS reads/writes
 *
 *************************************/

uint16_t midxunit_state::midxunit_cmos_r(offs_t offset)
{
	return m_nvram[offset];
}

void midxunit_state::midxunit_cmos_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(m_nvram+offset);
}


/*************************************
 *
 *  General I/O writes
 *
 *************************************/

void midxunit_state::midxunit_io_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	int oldword, newword;

	offset = (offset / 2) % 8;
	oldword = m_iodata[offset];
	newword = oldword;
	COMBINE_DATA(&newword);

	switch (offset)
	{
		case 2:
			/* watchdog reset */
//          watchdog_reset_w(0,0);
			break;

		default:
			// Gun Outputs for RevX
			// Note: The Gun for the Coin slot you use is supposed to rumble when you insert coins, and it doesn't for P3.
			// Perhaps an Input is hooked up wrong.
			m_gun_recoil[0] = BIT(data, 0);
			m_gun_recoil[1] = BIT(data, 1);
			m_gun_recoil[2] = BIT(data, 2);
			m_gun_led[0] = BIT(~data, 4);
			m_gun_led[1] = BIT(~data, 5);
			m_gun_led[2] = BIT(~data, 6);

			LOGMASKED(LOG_IO, "%s: I/O write to %d = %04X\n", machine().describe_context(), offset, data);
			break;
	}
	m_iodata[offset] = newword;
}


void midxunit_state::midxunit_unknown_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	int offs = offset / 0x40000;

	if (offs == 1 && ACCESSING_BITS_0_7)
		m_dcs->reset_w(~data & 2);

	if (ACCESSING_BITS_0_7 && offset % 0x40000 == 0)
		LOGMASKED(LOG_UNKNOWN, "%s: midxunit_unknown_w @ %d = %02X\n", machine().describe_context(), offs, data & 0xff);
}


WRITE_LINE_MEMBER(midxunit_state::adc_int_w)
{
	m_adc_int = (state != CLEAR_LINE);
}



/*************************************
 *
 *  General I/O reads
 *
 *************************************/

uint16_t midxunit_state::midxunit_status_r()
{
	/* low bit indicates whether the ADC is done reading the current input */
	return (m_midway_serial_pic->status_r() << 1) | (m_adc_int ? 1 : 0);
}



/*************************************
 *
 *  Revolution X UART
 *
 *************************************/

WRITE_LINE_MEMBER(midxunit_state::midxunit_dcs_output_full)
{
	/* only signal if not in loopback state */
	if (m_uart[1] != 0x66)
		m_maincpu->set_input_line(1, state ? ASSERT_LINE : CLEAR_LINE);
}


uint16_t midxunit_state::midxunit_uart_r(offs_t offset)
{
	int result = 0;

	/* convert to a byte offset */
	if (offset & 1)
		return 0;
	offset /= 2;

	/* switch off the offset */
	switch (offset)
	{
		case 0: /* register 0 must return 0x13 in order to pass the self test */
			result = 0x13;
			break;

		case 1: /* register 1 contains the status */

			/* loopback case: data always ready, and always ok to send */
			if (m_uart[1] == 0x66)
				result |= 5;

			/* non-loopback case: bit 0 means data ready, bit 2 means ok to send */
			else
			{
				int temp = midxunit_sound_state_r();
				result |= (temp & 0x800) >> 9;
				result |= (~temp & 0x400) >> 10;
				machine().scheduler().synchronize();
			}
			break;

		case 3: /* register 3 contains the data read */

			/* loopback case: feed back last data wrtten */
			if (m_uart[1] == 0x66)
				result = m_uart[3];

			/* non-loopback case: read from the DCS system */
			else
				result = midxunit_sound_r();
			break;

		case 5: /* register 5 seems to be like 3, but with in/out swapped */

			/* loopback case: data always ready, and always ok to send */
			if (m_uart[1] == 0x66)
				result |= 5;

			/* non-loopback case: bit 0 means data ready, bit 2 means ok to send */
			else
			{
				int temp = midxunit_sound_state_r();
				result |= (temp & 0x800) >> 11;
				result |= (~temp & 0x400) >> 8;
				machine().scheduler().synchronize();
			}
			break;

		default: /* everyone else reads themselves */
			result = m_uart[offset];
			break;
	}

	LOGMASKED(LOG_UART, "%s: UART R @ %X = %02X\n", machine().describe_context(), offset, result);
	return result;
}


void midxunit_state::midxunit_uart_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	/* convert to a byte offset, ignoring MSB writes */
	if ((offset & 1) || !ACCESSING_BITS_0_7)
		return;
	offset /= 2;
	data &= 0xff;

	/* switch off the offset */
	switch (offset)
	{
		case 3: /* register 3 contains the data to be sent */

			/* loopback case: don't feed through */
			if (m_uart[1] == 0x66)
				m_uart[3] = data;

			/* non-loopback case: send to the DCS system */
			else
				midxunit_sound_w(0, data, mem_mask);
			break;

		case 5: /* register 5 write seems to reset things */
			m_dcs->data_r();
			break;

		default: /* everyone else just stores themselves */
			m_uart[offset] = data;
			break;
	}

	LOGMASKED(LOG_UART, "%s: UART W @ %X = %02X\n", machine().describe_context(), offset, data);
}



/*************************************
 *
 *  X-unit init (DCS)
 *
 *  music: ADSP2101
 *
 *************************************/

/********************** Revolution X **********************/

/*************************************
 *
 *  Machine init
 *
 *************************************/

void midxunit_state::machine_start()
{
	m_gun_recoil.resolve();
	m_gun_led.resolve();

	save_item(NAME(m_cmos_write_enable));
	save_item(NAME(m_iodata));
	save_item(NAME(m_ioshuffle));
	save_item(NAME(m_uart));
	save_item(NAME(m_security_bits));
	save_item(NAME(m_adc_int));
}

void midxunit_state::machine_reset()
{
	/* reset sound */
	m_dcs->reset_w(0);
	m_dcs->reset_w(1);

	/* reset I/O shuffling */
	for (int i = 0; i < 16; i++)
		m_ioshuffle[i] = i % 8;

	m_dcs->set_io_callbacks(write_line_delegate(*this, FUNC(midxunit_state::midxunit_dcs_output_full)), write_line_delegate(*this));
}



/*************************************
 *
 *  Security chip I/O
 *
 *************************************/

uint16_t midxunit_state::midxunit_security_r()
{
	return m_midway_serial_pic->read();
}

void midxunit_state::midxunit_security_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (ACCESSING_BITS_0_7)
		m_security_bits = data & 0x0f;
}


void midxunit_state::midxunit_security_clock_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (offset == 0 && ACCESSING_BITS_0_7)
		m_midway_serial_pic->write(((~data & 2) << 3) | m_security_bits);
}



/*************************************
 *
 *  Sound write handlers
 *
 *************************************/

uint16_t midxunit_state::midxunit_sound_r()
{
	LOGMASKED(LOG_SOUND, "%08X:Sound read\n", m_maincpu->pc());

	return m_dcs->data_r() & 0xff;
}


uint16_t midxunit_state::midxunit_sound_state_r()
{
	return m_dcs->control_r();
}


void midxunit_state::midxunit_sound_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	/* check for out-of-bounds accesses */
	if (offset)
	{
		LOGMASKED(LOG_SOUND | LOG_UNKNOWN, "%s: Unexpected write to sound (hi) = %04X\n", machine().describe_context(), data);
		return;
	}

	/* call through based on the sound type */
	if (ACCESSING_BITS_0_7)
	{
		LOGMASKED(LOG_SOUND, "%s: Sound write = %04X\n", machine().describe_context(), data);
		m_dcs->data_w(data & 0xff);
	}
}
