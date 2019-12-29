// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    Apple II paddles

*********************************************************************/

#include "emu.h"
#include "bus/a2gameio/paddles.h"

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
DEFINE_DEVICE_TYPE(APPLE2_PADDLES, apple2_paddles_device, "a2pdls", "Apple II paddles")

//**************************************************************************
//  INPUT PORTS
//**************************************************************************

static INPUT_PORTS_START( apple2_paddles )
	PORT_START("paddle_1")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE) PORT_PLAYER(1) PORT_SENSITIVITY(30) PORT_KEYDELTA(20) PORT_MINMAX(0, 255)

	PORT_START("paddle_2")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE) PORT_PLAYER(2) PORT_SENSITIVITY(30) PORT_KEYDELTA(20) PORT_MINMAX(0, 255)

	PORT_START("paddle_3")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE) PORT_PLAYER(3) PORT_SENSITIVITY(30) PORT_KEYDELTA(20) PORT_MINMAX(0, 255)

	PORT_START("paddle_4")
	PORT_BIT( 0xff, 0x80, IPT_PADDLE) PORT_PLAYER(4) PORT_SENSITIVITY(30) PORT_KEYDELTA(20) PORT_MINMAX(0, 255)

	PORT_START("paddle_buttons")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1)   PORT_PLAYER(1)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_BUTTON1)   PORT_PLAYER(2)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_BUTTON1)   PORT_PLAYER(3)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_BUTTON1)   PORT_PLAYER(4)
INPUT_PORTS_END

//**************************************************************************
//  DEVICE IMPLEMENTATION
//**************************************************************************

apple2_paddles_device::apple2_paddles_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, APPLE2_PADDLES, tag, owner, clock)
	, device_a2gameio_interface(mconfig, *this)
	, m_pdl(*this, "paddle_%u", 1U)
	, m_buttons(*this, "paddle_buttons")
{
}

ioport_constructor apple2_paddles_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(apple2_paddles);
}

void apple2_paddles_device::device_start()
{
}

u8 apple2_paddles_device::pdl0_r()
{
	return m_pdl[0]->read();
}

u8 apple2_paddles_device::pdl1_r()
{
	return m_pdl[1]->read();
}

u8 apple2_paddles_device::pdl2_r()
{
	return m_pdl[2]->read();
}

u8 apple2_paddles_device::pdl3_r()
{
	return m_pdl[3]->read();
}

READ_LINE_MEMBER(apple2_paddles_device::sw0_r)
{
	return BIT(m_buttons->read(), 4);
}

READ_LINE_MEMBER(apple2_paddles_device::sw1_r)
{
	return BIT(m_buttons->read(), 5);
}

READ_LINE_MEMBER(apple2_paddles_device::sw2_r)
{
	return BIT(m_buttons->read(), 6);
}

READ_LINE_MEMBER(apple2_paddles_device::sw3_r)
{
	return BIT(m_buttons->read(), 7);
}
