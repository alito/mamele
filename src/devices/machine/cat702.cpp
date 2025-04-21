// license:BSD-3-Clause
// copyright-holders:smf
/*

  CAT702 ZN security chip

  A serial magic latch.

  It's a DIP20 chip with a sticker of the form XXnn, where XX is the
  company and nn a number:
    AC = Acclaim
    AT = Atlus
    CP = Capcom
    ET = Raizing
    KN = Konami
    MG = Tecmo
    TT = Taito
    TW = Atari

  There usually are 2 of them, one on the cpu board and one on the rom
  board.  The cpu board one is usually numbered 01.

  Pinout:             GND -11  10- GND
                        ? -12   9- +5V
                      +5V -13   8- Data in
                 Data out- 14   7- Clock
                      +5V -15   6- Select
                        ? -16   5- Select
                      +5V -17   4- +5V
                      +5V -18   3- +5V
                      +5V -19   2- +5V
                      +5V -20   1- ?

  The chip works with the '?' lines left unconnected.

  The communication protocol is serial, and in practice the standard
  psx controller communication protocol minus the ack.  Drive both
  select to ground to start a communication, send bits and get the
  results on the raising clock.  Put both select back to +5V when
  finished.  The bios seems to use two communication clock speeds,
  ~300KHz (standard psx) and ~2MHz.  Driving it with lower clocks
  works reasonably, at least at 1KHz.

  The data is divided in bytes but there is no signal for end-of-byte.
  In all of the following the data will be considered coming and going
  lower-bit first.

  Internally the chip has a 8-bit state, initialized at communication
  start to 0xfc.  The structure is simple:


                  +---------+         bit number        +--------+
  Clock   ------->| bit     |-----+-------------------->| bit    |---------> Data out
                  | counter |     |                     | select |
                  +---------+     v      +-------+ out  |        |
                      |        +-----+   | 8bit  |=====>|        |
  Data in ------------|------->| TF1 |<=>| state |      +--------+
                      |        +-----+   |       |
                      |                  |       |
                      | start  +-----+   |       |
                      +------->| TF2 |<=>|       |
                               +-----+   +-------+

  The chip starts by tranforming the state with TF2.  Then, for each
  input bit from 0 to 7:
    - the nth bit from the state is sent to the output
    - the state is transformed by TF1 if the input bit is 0

  TF2 is a fixed linear substitution box (* = and, + = xor):
    o = ff*s0 + fe*s1 + fc*s2 + f8*s3 + f0*s4 + e0*s5 + c0*s6 + 7f*s7

  TF1 is a chip-dependent set of 8 linear sboxes, one per bit number.
  In practice, only the sbox for bit 0 is defined for the chip, the 7
  other are derived from it.  Defining the byte transformation Shift
  as:
       Shift(i7..i0) = i6..i0, i7^i6

  and noting the sboxes as:
       Sbox(n, i7..i0) =    Xor(    c[n, bit]*i[bit])
                         0<=bit<=7
  then
       c[n, bit=0..6] = Shift(c[n-1, (bit-1)&7])
       c[n, 7]        = Shift(c[n-1, 6])^c[n, 0]
                      = Shift(c[n-1, 6])^Shift(c[n-1, 7])
*/

#include "emu.h"
#include "cat702.h"

static constexpr uint8_t initial_sbox[8] = { 0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x7f };

DEFINE_DEVICE_TYPE(CAT702, cat702_device, "cat702", "CAT702")
DEFINE_DEVICE_TYPE(CAT702_PIU, cat702_piu_device, "cat702_piu", "CAT702_PIU")

cat702_device_base::cat702_device_base(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, type, tag, owner, clock),
	m_select(1),
	m_clock(1),
	m_datain(1),
	m_state(0),
	m_bit(0),
	m_dataout_handler(*this),
	m_region(*this, DEVICE_SELF)
{
}

void cat702_device_base::device_start()
{
	memset(m_transform, 0xff, sizeof(m_transform));

	if (!m_region.found())
	{
		logerror("cat702(%s):region not found\n", tag());
	}
	else if (m_region->bytes() != sizeof(m_transform))
	{
		logerror("cat702(%s):region length 0x%x expected 0x%x\n", tag(), m_region->bytes(), sizeof(m_transform));
	}
	else
	{
		memcpy(m_transform, m_region->base(), sizeof(m_transform));
	}

	save_item(NAME(m_select));
	save_item(NAME(m_clock));
	save_item(NAME(m_datain));
	save_item(NAME(m_state));
	save_item(NAME(m_bit));

	m_dataout_handler(1);
}

// Given the value for x7..x0 and linear transform coefficients a7..a0
// compute the value of the transform
#if 0
static int c_linear(uint8_t x, uint8_t a)
{
	x &= a;
	uint8_t r = 0;
	for (int i = 0; i < 8; i++)
	{
		if (BIT(x, i))
			r = !r;
	}
	return r;
}
#endif

// Derive the sbox xor mask for a given input and select bit
uint8_t cat702_device_base::compute_sbox_coef(int sel, int bit)
{
	if (!sel)
		return m_transform[bit];

	uint8_t r = compute_sbox_coef((sel - 1) & 7, (bit - 1) & 7);
	r = (r << 1) | (BIT(r, 7) ^ BIT(r, 6));
	if (bit != 7)
		return r;

	return r ^ compute_sbox_coef(sel, 0);
}

// Apply the sbox for a input 0 bit
void cat702_device_base::apply_bit_sbox(int sel)
{
	uint8_t r = 0;
	for (int i = 0; i < 8; i++)
	{
		if (BIT(m_state, i))
			r ^= compute_sbox_coef(sel, i);
	}
	m_state = r;
}

// Apply a sbox
void cat702_device_base::apply_sbox(const uint8_t *sbox)
{
	uint8_t r = 0;
	for (int i = 0; i < 8; i++)
	{
		if (BIT(m_state, i))
			r ^= sbox[i];
	}
	m_state = r;
}

void cat702_device_base::write_datain(int state)
{
	m_datain = state;
}


///////////////


cat702_device::cat702_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	cat702_device_base(mconfig, CAT702, tag, owner, clock)
{
}


void cat702_device::write_select(int state)
{
	if (m_select != state)
	{
		if (!state)
		{
			m_state = 0xfc;
			m_bit = 0;
		}
		else
		{
			m_dataout_handler(1);
		}

		m_select = state;
	}
}

void cat702_device::write_clock(int state)
{
	if (!state && m_clock && !m_select)
	{
		if (m_bit==0)
		{
			// Apply the initial sbox
			apply_sbox(initial_sbox);
		}

		// Compute the output and change the state
		m_dataout_handler(BIT(m_state, m_bit));
	}

	if (state && !m_clock && !m_select)
	{
		if (!m_datain)
			apply_bit_sbox(m_bit);

		m_bit = (m_bit + 1) & 7;
	}

	m_clock = state;
}


///////////////

cat702_piu_device::cat702_piu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	cat702_device_base(mconfig, CAT702_PIU, tag, owner, clock)
{
}

void cat702_piu_device::write_select(int state)
{
	if (m_select != state)
	{
		if (!state)
		{
			m_state = 0xfc;
			m_bit = 0;

			apply_sbox(initial_sbox);
		}
		else
		{
			m_dataout_handler(1);
		}

		m_select = state;
	}
}

void cat702_piu_device::write_clock(int state)
{
	/*
	Pump It Up uses a CAT702 but accesses it directly and in a way that
	seems conflicting with how the ZN uses it through the PS1's SIO.

	This is the sequence performed with clock and data lines write and read data:
	write unkbit 0
	write unkbit 1
	write select 0
	loop for all bits that need to be transferred:
	    write clk = 0, data = x
	    write clk = x, data = 0/1
	    write clk = 1, data = x
	    read data
	write select 1

	Modifying the old code to work with PIU breaks ZN games.
	*/
	if (state && !m_clock && !m_select)
	{
		// Compute the output and change the state
		if (!m_datain)
			apply_bit_sbox(m_bit);

		m_bit = (m_bit + 1) & 7;

		if (m_bit == 0)
		{
			// Apply the initial sbox
			apply_sbox(initial_sbox);
		}

		m_dataout_handler(BIT(m_state, m_bit));
	}

	m_clock = state;
}
