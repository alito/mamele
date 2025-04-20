// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Nintendo Family Computer Coconuts Japan CJPC-102 Pachinko Controller

**********************************************************************/

#include "emu.h"
#include "pachinko.h"

//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(NES_PACHINKO, nes_pachinko_device, "nes_pachinko", "Coconuts Japan Pachinko Controller")


static INPUT_PORTS_START( nes_pachinko )
	PORT_INCLUDE( nes_joypad )

	PORT_START("TRIGGER")
	PORT_BIT( 0xff, 0, IPT_PEDAL ) PORT_MINMAX(0, 0x63) PORT_SENSITIVITY(25) PORT_KEYDELTA(20)
INPUT_PORTS_END

//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor nes_pachinko_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( nes_pachinko );
}

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  nes_pachinko_device - constructor
//-------------------------------------------------

nes_pachinko_device::nes_pachinko_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_fcpadexp_device(mconfig, NES_PACHINKO, tag, owner, clock)
	, m_trigger(*this, "TRIGGER")
{
}


//-------------------------------------------------
//  read
//-------------------------------------------------

u8 nes_pachinko_device::read_exp(offs_t offset)
{
	u8 ret = 0;
	// this controller behaves like a standard P3 joypad, with longer stream of inputs
	if (offset == 0)    //$4016
	{
		if (m_strobe)
			set_latch();
		ret = (m_latch & 1) << 1;
		m_latch >>= 1;
	}
	return ret;
}

//-------------------------------------------------
//  write
//-------------------------------------------------

void nes_pachinko_device::write(u8 data)
{
	if (write_strobe(data))
		set_latch();
}

void nes_pachinko_device::set_latch()
{
	m_latch = m_joypad->read();
	m_latch |= ~bitswap<8>(m_trigger->read(), 0, 1, 2, 3, 4, 5, 6, 7) << 8;
}
