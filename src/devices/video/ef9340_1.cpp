// license:BSD-3-Clause
// copyright-holders:Wilbert Pol, hap
/***************************************************************************

Thomson EF9340 + EF9341 teletext graphics, this device is a combination of chips.

Minimal components:
- Thomson EF9340 "VIN"
- Thomson EF9341 "GEN"
- 2*1KB RAM, A for attributes, B for character codes

There's also an optional extended character memory, it can be RAM or ROM.
This is implemented with a callback. The datasheet explains how to hook up
1KB RAM, but it's possible to have more.

TODO:
- busy state (right now it is immediate)
- internal display timing (on g7400, most of it is done externally)
- read slice from internal ROM
- window boxing
- Y zoom

***************************************************************************/

#include "emu.h"
#include "ef9340_1.h"

#include "screen.h"

//#define VERBOSE (LOG_GENERAL)
#include "logmacro.h"


// device type definition
DEFINE_DEVICE_TYPE(EF9340_1, ef9340_1_device, "ef9340_1", "Thomson EF9340+EF9341")


ef9340_1_device::ef9340_1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, EF9340_1, tag, owner, clock)
	, device_video_interface(mconfig, *this)
	, m_charset(*this, "ef9340_1")
	, m_write_exram(*this)
	, m_read_exram(*this)
{
}


ROM_START( ef9340_1 )
	ROM_REGION( 0xA00, "ef9340_1", 0 )
	ROM_LOAD( "charset_ef9340_1.rom", 0x0000, 0x0a00, BAD_DUMP CRC(8de85988) SHA1(f8e3892234da6626eb4302e171179ada5a51fca8) ) // taken from datasheet
ROM_END


const tiny_rom_entry *ef9340_1_device::device_rom_region() const
{
	return ROM_NAME( ef9340_1 );
}


void ef9340_1_device::device_start()
{
	m_write_exram.resolve_safe();
	m_read_exram.resolve_safe(0xff);

	// Let the screen create our temporary bitmap with the screen's dimensions
	screen().register_screen_bitmap(m_tmp_bitmap);

	m_line_timer = timer_alloc(TIMER_LINE);
	m_line_timer->adjust( screen().time_until_pos(0, 0), 0, screen().scan_period() );

	m_blink_timer = timer_alloc(TIMER_BLINK);
	m_blink_timer->adjust( screen().time_until_pos(0, 0), 0, screen().frame_period() );

	// zerofill
	m_ef9341.TA = 0;
	m_ef9341.TB = 0;
	m_ef9341.busy = false;

	m_ef9340.X = 0;
	m_ef9340.Y = 0;
	m_ef9340.Y0 = 0;
	m_ef9340.R = 0;
	m_ef9340.M = 0;
	m_ef9340.blink = false;
	m_ef9340.blink_prescaler = 0;
	m_ef9340.h_parity = false;

	memset(m_ram_a, 0, sizeof(m_ram_a));
	memset(m_ram_b, 0, sizeof(m_ram_b));

	// register our state
	save_item(NAME(m_ef9341.TA));
	save_item(NAME(m_ef9341.TB));
	save_item(NAME(m_ef9341.busy));

	save_item(NAME(m_ef9340.X));
	save_item(NAME(m_ef9340.Y));
	save_item(NAME(m_ef9340.Y0));
	save_item(NAME(m_ef9340.R));
	save_item(NAME(m_ef9340.M));
	save_item(NAME(m_ef9340.blink));
	save_item(NAME(m_ef9340.blink_prescaler));
	save_item(NAME(m_ef9340.h_parity));

	save_item(NAME(m_ram_a));
	save_item(NAME(m_ram_b));
}


void ef9340_1_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	switch ( id )
	{
		case TIMER_LINE:
			ef9340_scanline(screen().vpos());
			break;

		case TIMER_BLINK:
			// blink rate is approximately 0.5s
			m_ef9340.blink_prescaler = (m_ef9340.blink_prescaler + 1) & 0x1f;
			if (m_ef9340.R & 0x40 && m_ef9340.blink_prescaler == 24)
				m_ef9340.blink_prescaler = 0;

			if (m_ef9340.blink_prescaler == 0)
				m_ef9340.blink = !m_ef9340.blink;

			break;
	}
}


uint16_t ef9340_1_device::ef9340_get_c_addr(uint8_t x, uint8_t y)
{
	if ( ( y & 0x18 ) == 0x18 )
	{
		return 0x318 | ( ( x & 0x38 ) << 2 ) | ( x & 0x07 );
	}
	if ( x & 0x20 )
	{
		return 0x300 | ( ( y & 0x07 ) << 5 ) | ( y & 0x18 ) | ( x & 0x07 );
	}
	return ( y & 0x1f ) << 5 | ( x & 0x1f );
}


void ef9340_1_device::ef9340_inc_c()
{
	m_ef9340.X++;
	if ( m_ef9340.X == 40 || m_ef9340.X == 48 || m_ef9340.X == 56 || m_ef9340.X == 64 )
	{
		m_ef9340.Y = ( m_ef9340.Y + 1 ) & 0x1f;
		if ( m_ef9340.Y == 24 )
		{
			m_ef9340.Y = 0;
		}
		m_ef9340.X = 0;
	}
}


void ef9340_1_device::ef9341_write( uint8_t command, uint8_t b, uint8_t data )
{
	LOG("ef9341 %s write, t%s, data %02X\n", command ? "command" : "data", b ? "B" : "A", data );

	if ( command )
	{
		if ( b )
		{
			m_ef9341.TB = data;
			m_ef9341.busy = true;
			switch( m_ef9341.TB & 0xE0 )
			{
			case 0x00:  /* Begin row */
				m_ef9340.X = 0;
				m_ef9340.Y = m_ef9341.TA & 0x1F;
				break;
			case 0x20:  /* Load Y */
				m_ef9340.Y = m_ef9341.TA & 0x1F;
				break;
			case 0x40:  /* Load X */
				m_ef9340.X = m_ef9341.TA & 0x3F;
				break;
			case 0x60:  /* INC C */
				ef9340_inc_c();
				break;
			case 0x80:  /* Load M */
				m_ef9340.M = m_ef9341.TA;
				break;
			case 0xA0:  /* Load R */
				m_ef9340.R = m_ef9341.TA;
				break;
			case 0xC0:  /* Load Y0 */
				m_ef9340.Y0 = m_ef9341.TA & 0x3F;
				break;
			case 0xE0:  /* Not interpreted */
				break;
			}
			m_ef9341.busy = false;
		}
		else
		{
			m_ef9341.TA = data;
		}
	}
	else
	{
		if ( b )
		{
			uint16_t addr = ef9340_get_c_addr( m_ef9340.X, m_ef9340.Y ) & 0x3ff;

			m_ef9341.TB = data;
			m_ef9341.busy = true;
			switch ( m_ef9340.M & 0xE0 )
			{
				case 0x00:  /* Write */
					m_ram_a[addr] = m_ef9341.TA;
					m_ram_b[addr] = m_ef9341.TB;
					ef9340_inc_c();
					break;

				case 0x40:  /* Write without increment */
					m_ram_a[addr] = m_ef9341.TA;
					m_ram_b[addr] = m_ef9341.TB;
					break;

				case 0x80:  /* Write slice */
					{
						uint8_t a = m_ram_a[addr];
						uint8_t b = m_ram_b[addr];
						uint8_t slice = m_ef9340.M & 0x0f;

						if (b >= 0xa0)
							m_write_exram(a << 12 | b << 4 | slice, m_ef9341.TA);

						// Increment slice number
						m_ef9340.M = ( m_ef9340.M & 0xf0) | ( ( slice + 1 ) % 10 );
					}
					break;

				default:    /* Illegal or Read command */
					break;
			}
			m_ef9341.busy = false;
		}
		else
		{
			m_ef9341.TA = data;
		}
	}
}


uint8_t ef9340_1_device::ef9341_read( uint8_t command, uint8_t b )
{
	uint8_t   data;

	LOG("ef9341 %s read, t%s\n", command ? "command" : "data", b ? "B" : "A" );

	if ( command )
	{
		if ( b )
		{
			data = 0;
		}
		else
		{
			data = (m_ef9341.busy) ? 0x80 : 0;
		}
	}
	else
	{
		if ( b )
		{
			uint16_t addr = ef9340_get_c_addr( m_ef9340.X, m_ef9340.Y ) & 0x3ff;

			data = m_ef9341.TB;
			m_ef9341.busy = true;
			switch ( m_ef9340.M & 0xE0 )
			{
				case 0x20:  /* Read */
					m_ef9341.TA = m_ram_a[addr];
					m_ef9341.TB = m_ram_b[addr];
					ef9340_inc_c();
					break;

				case 0x60:  /* Read without increment */
					m_ef9341.TA = m_ram_a[addr];
					m_ef9341.TB = m_ram_b[addr];
					break;

				case 0xA0:  /* Read slice */
					{
						uint8_t a = m_ram_a[addr];
						uint8_t b = m_ram_b[addr];
						uint8_t slice = m_ef9340.M & 0x0f;

						m_ef9341.TA = 0xff;
						m_ef9341.TB = 0xff;

						if (b >= 0xa0)
							m_ef9341.TA = m_read_exram(a << 12 | b << 4 | slice);
						else
							logerror("ef9341 read slice from internal\n");

						// Increment slice number
						m_ef9340.M = ( m_ef9340.M & 0xf0) | ( ( slice + 1 ) % 10 );
					}
					break;

				default:    /* Illegal or Write command */
					break;
			}
			m_ef9341.busy = false;
		}
		else
		{
			data = m_ef9341.TA;
		}
	}
	return data;
}


void ef9340_1_device::ef9340_scanline(int vpos)
{
	vpos -= m_offset_y;
	if (vpos < 0)
		return;

	int slice = vpos % 10;
	bool dh = false;
	if (vpos == 0)
		m_ef9340.h_parity = false;

	// display automaton active at 40-290, or 32-242
	int max_vpos = (m_ef9340.R & 0x40) ? 250 : 210;

	if (m_ef9340.R & 0x01 && vpos < max_vpos)
	{
		int y_row = 0;
		uint16_t char_data = 0x00;
		uint8_t fg = 0;
		uint8_t bg = 0;
		bool underline = false;
		bool blank = false;
		bool w_parity = false;

		if ( vpos < 10 )
		{
			// Service row
			if (m_ef9340.R & 0x08)
			{
				// Service row is enabled
				y_row = 31;
			}
			else
			{
				// Service row is disabled
				for (int i = 0; i < 40 * 8; i++)
					m_tmp_bitmap.pix16(m_offset_y + vpos, m_offset_x + i) = 8;
				return;
			}
		}
		else
		{
			// Displaying regular row
			y_row = ((m_ef9340.Y0 & 0x1f) + (vpos - 10) / 10) % 24;
		}

		for (int x = 0; x < 40; x++)
		{
			int s = slice;
			uint16_t addr = ef9340_get_c_addr(x, y_row);
			uint8_t a = m_ram_a[addr];
			uint8_t b = m_ram_b[addr];
			bool blink = m_ef9340.R & 0x80 && m_ef9340.blink;
			bool cursor = m_ef9340.R & 0x10 && x == m_ef9340.X && y_row == m_ef9340.Y;
			bool invert = cursor && !blink;
			bool dw = false;
			bool del = false;

			if (a & 0x80)
			{
				// graphics
				if ((b & 0xe0) != 0x80)
				{
					fg = a & 0x07;
					bg = a >> 4 & 0x07;

					if (b & 0x80)
						char_data = m_read_exram(a << 12 | b << 4 | s);
					else
						char_data = m_charset[((b | 0x80) * 10) + s];
				}

				// illegal
				else
					char_data = 0xff;
			}
			else
			{
				// alphanumeric
				if ((b & 0xe0) != 0x80)
				{
					// double height
					if (a & 0x10)
					{
						dh = true;
						if (m_ef9340.h_parity)
							s += 10;
						if (s > 0)
							s = (s - 1) / 2;
					}

					fg = a & 0x07;
					u16 c = 0;

					if (b & 0x80)
						c = m_read_exram(a << 12 | b << 4 | s);
					else if (s == 9 && underline)
						c = 0xff;
					else
						c = m_charset[((b & 0x7f) * 10) + s];

					// double width
					dw = bool(a & 0x20);
					if (dw)
					{
						if (!w_parity)
							char_data = bitswap<16>(c,7,7,6,6,5,5,4,4,3,3,2,2,1,1,0,0);
					}
					else
						char_data = c;

					// inverted
					if (a & 0x40)
					{
						invert = !invert;
						blink = m_ef9340.R & 0x80 && !m_ef9340.blink;
					}
				}

				// deliminator
				else
				{
					fg = a & 0x07;
					bg = a >> 4 & 0x07;
					char_data = 0xff;

					del = true;
				}
			}

			// blink character
			if (blink && !cursor && (b & 0xe0) != 0x80 && ~a & 0x08)
				char_data &= ~0xff;

			if (invert)
				char_data ^= 0xff;

			if (dw)
				w_parity = !w_parity;
			else
				w_parity = false;

			for (int i = 0; i < 8; i++)
			{
				uint16_t d = blank ? 0 : (char_data & 1) ? fg : bg;
				m_tmp_bitmap.pix16(m_offset_y + vpos, m_offset_x + x*8 + i) = d | 8;
				char_data >>= 1;
			}

			if (del)
			{
				blank = m_ef9340.R & 0x04 && b & 0x01;
				underline = bool(b & 0x04);
			}
		}
	}
	else
	{
		for (int i = 0; i < 40 * 8; i++)
			m_tmp_bitmap.pix16(m_offset_y + vpos, m_offset_x + i) = 0;
	}

	// determine next h parity
	if (vpos >= 10 && slice == 9)
	{
		if (dh)
			m_ef9340.h_parity = !m_ef9340.h_parity;
		else
			m_ef9340.h_parity = false;
	}
}


uint32_t ef9340_1_device::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	// note: palette d3 is transparency (datasheet calls it "I"), this handler masks it off
	for (int y = cliprect.min_y; y <= cliprect.max_y; y++)
		for (int x = cliprect.min_x; x <= cliprect.max_x; x++)
			bitmap.pix16(y, x) = m_tmp_bitmap.pix16(y, x) & 7;

	return 0;
}
