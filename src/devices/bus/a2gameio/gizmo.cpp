// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    gizmo.cpp - HAL Labs Gizmo digital joystick adapter
    Used by Vindicator and Super Taxman 2.

*********************************************************************/

#include "emu.h"
#include "bus/a2gameio/gizmo.h"

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
DEFINE_DEVICE_TYPE(APPLE2_GIZMO, apple2_gizmo_device, "a2gizmo", "HAL Labs Gizmo")

//**************************************************************************
//  INPUT PORTS
//**************************************************************************

static INPUT_PORTS_START( apple2_gizmo )
	PORT_START("joystick_p1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 )
INPUT_PORTS_END

//**************************************************************************
//  DEVICE IMPLEMENTATION
//**************************************************************************

apple2_gizmo_device::apple2_gizmo_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, APPLE2_GIZMO, tag, owner, clock)
	, device_a2gameio_interface(mconfig, *this)
	, m_player1(*this, "joystick_p1")
{
}

ioport_constructor apple2_gizmo_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(apple2_gizmo);
}

void apple2_gizmo_device::device_start()
{
	save_item(NAME(m_an0));
	save_item(NAME(m_an1));
	save_item(NAME(m_an2));
}

READ_LINE_MEMBER(apple2_gizmo_device::sw0_r)
{
	static const int gizmo_bits[8] = { 1,3,2,0,4,5,5,5 };
	return BIT(m_player1->read(),gizmo_bits[m_an0+(m_an1<<1)+(m_an2<<2)]);
}

WRITE_LINE_MEMBER(apple2_gizmo_device::an0_w)
{
	m_an0 = state;
}

WRITE_LINE_MEMBER(apple2_gizmo_device::an1_w)
{
	m_an1 = state;
}

WRITE_LINE_MEMBER(apple2_gizmo_device::an2_w)
{
	m_an2 = state;
}
