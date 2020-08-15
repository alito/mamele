// license:BSD-3-Clause
// copyright-holders:Krzysztof Strzecha, Miodrag Milanovic
/***************************************************************************

  galaxy.c

  Functions to emulate the video hardware of the Galaksija.

  20/05/2008 - Real video implementation by Miodrag Milanovic
  01/03/2008 - Update by Miodrag Milanovic to make Galaksija video work with new SVN code
***************************************************************************/

#include "emu.h"
#include "includes/galaxy.h"
#include "cpu/z80/z80.h"


TIMER_CALLBACK_MEMBER(galaxy_state::gal_video)
{
	address_space &space = m_maincpu->space(AS_PROGRAM);
	int y, x;
	if (m_interrupts_enabled == true)
	{
		uint8_t dat = BIT(m_latch_value, 2, 4);
		if ((m_gal_cnt >= 48 * 2) && (m_gal_cnt < 48 * 210))  // display on screen just m_first 208 lines
		{
			uint16_t addr = (m_maincpu->state_int(Z80_I) << 8) | m_maincpu->state_int(Z80_R) | (~m_latch_value & 0x80);
			if (!BIT(m_latch_value, 1)) // bit 2 latch represents mode
			{
				// Text mode
				if (m_first == 0 && (m_maincpu->state_int(Z80_R) & 0x1f) == 0)
				{
					// Due to a fact that on real processor latch value is set at
					// the end of last cycle we need to skip display of double
					// m_first char in each row
					m_code = 0x00;
					m_first = 1;
				}
				else
				{
					m_code = space.read_byte(addr) & 0xbf;
					m_code += (m_code & 0x80) >> 1;
					m_code = m_p_chargen[(m_code & 0x7f) +(dat << 7 )] ^ 0xff;
					m_first = 0;
				}
				y = m_gal_cnt / 48 - 2;
				x = (m_gal_cnt % 48) * 8;

				m_bitmap.pix16(y, x++ ) = BIT(m_code, 0);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 1);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 2);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 3);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 4);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 5);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 6);
				m_bitmap.pix16(y, x   ) = BIT(m_code, 7);
			}
			else
			{ // Graphics mode
				if (m_first < 4 && (m_maincpu->state_int(Z80_R) & 0x1f) == 0)
				{
					// Due to a fact that on real processor latch value is set at
					// the end of last cycle we need to skip display of 4 times
					// m_first char in each row
					m_code = 0x00;
					m_first++;
				}
				else
				{
					m_code = space.read_byte(addr) ^ 0xff;
					m_first = 0;
				}
				y = m_gal_cnt / 48 - 2;
				x = (m_gal_cnt % 48) * 8;

				/* hack - until calc of R is fixed in Z80 */
				if (x == 11 * 8 && y == 0)
					m_start_addr = addr;

				if ((x / 8 >= 11) && (x / 8 < 44))
					m_code = space.read_byte(m_start_addr + y * 32 + (m_gal_cnt % 48) - 11) ^ 0xff;
				else
					m_code = 0x00;
				/* end of hack */

				m_bitmap.pix16(y, x++ ) = BIT(m_code, 0);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 1);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 2);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 3);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 4);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 5);
				m_bitmap.pix16(y, x++ ) = BIT(m_code, 6);
				m_bitmap.pix16(y, x   ) = BIT(m_code, 7);
			}
		}
		m_gal_cnt++;
	}
}

void galaxy_state::set_timer()
{
	m_gal_cnt = 0;
	m_gal_video_timer->adjust(attotime::zero, 0, attotime::from_hz(6144000 / 8));
}

void galaxy_state::machine_start()
{
	m_gal_cnt = 0;

	m_gal_video_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(galaxy_state::gal_video),this));
	m_gal_video_timer->adjust(attotime::zero, 0, attotime::never);

	m_screen->register_screen_bitmap(m_bitmap);

	save_item(NAME(m_interrupts_enabled));
	save_item(NAME(m_latch_value));
	save_item(NAME(m_gal_cnt));
	save_item(NAME(m_code));
	save_item(NAME(m_first));
	save_item(NAME(m_start_addr));
}

uint32_t galaxy_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_gal_video_timer->adjust(attotime::zero, 0, attotime::never);
	if (m_interrupts_enabled == false)
	{
		const rectangle black_area(0, 384 - 1, 0, 208 - 1);
		m_bitmap.fill(0, black_area);
	}
	m_interrupts_enabled = false;
	copybitmap(bitmap, m_bitmap, 0, 0, 0, 0, cliprect);
	return 0;
}
