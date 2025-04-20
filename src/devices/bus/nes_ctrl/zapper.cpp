// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Nintendo Family Computer & Entertainment System Zapper Lightgun
    Nintendo Family Computer Bandai Hyper Shot Lightgun

**********************************************************************/

#include "emu.h"
#include "zapper.h"

//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(NES_ZAPPER, nes_zapper_device, "nes_zapper", "Nintendo Zapper Lightgun")
DEFINE_DEVICE_TYPE(NES_BANDAIHS, nes_bandaihs_device, "nes_bandaihs", "Bandai Hyper Shot Lightgun")


static INPUT_PORTS_START( nes_zapper )
	PORT_START("ZAPPER_X")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_X ) PORT_CROSSHAIR(X, 1.0, 0.0, 0) PORT_SENSITIVITY(70) PORT_KEYDELTA(30) PORT_MINMAX(0, 255)
	PORT_START("ZAPPER_Y")
	PORT_BIT( 0xff, 0x80, IPT_LIGHTGUN_Y ) PORT_CROSSHAIR(Y, 1.0, 0.0, 0) PORT_SENSITIVITY(50) PORT_KEYDELTA(30) PORT_MINMAX(0, 239)
	PORT_START("ZAPPER_T")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("Lightgun Trigger")
INPUT_PORTS_END


static INPUT_PORTS_START( nes_bandaihs )
	PORT_INCLUDE( nes_zapper )

	PORT_START("JOYPAD")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED ) // has complete joypad inputs except button A
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("%p B")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_SELECT )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_START )
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_8WAY
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_8WAY
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_8WAY
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_8WAY
INPUT_PORTS_END


//-------------------------------------------------
//  input_ports - device-specific input ports
//-------------------------------------------------

ioport_constructor nes_zapper_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( nes_zapper );
}

ioport_constructor nes_bandaihs_device::device_input_ports() const
{
	return INPUT_PORTS_NAME( nes_bandaihs );
}

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void nes_zapper_device::device_add_mconfig(machine_config &config)
{
	NES_ZAPPER_SENSOR(config, m_sensor, 0);
	if (m_port != nullptr)
		m_sensor->set_screen_tag(m_port->m_screen);
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  constructor
//-------------------------------------------------

nes_zapper_device::nes_zapper_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, type, tag, owner, clock)
	, device_nes_control_port_interface(mconfig, *this)
	, m_sensor(*this, "sensor")
	, m_lightx(*this, "ZAPPER_X")
	, m_lighty(*this, "ZAPPER_Y")
	, m_trigger(*this, "ZAPPER_T")
{
}

nes_zapper_device::nes_zapper_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_zapper_device(mconfig, NES_ZAPPER, tag, owner, clock)
{
}

nes_bandaihs_device::nes_bandaihs_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_zapper_device(mconfig, NES_BANDAIHS, tag, owner, clock)
	, m_joypad(*this, "JOYPAD")
	, m_latch(0)
{
}


//-------------------------------------------------
//  device_start
//-------------------------------------------------

void nes_zapper_device::device_start()
{
}

void nes_bandaihs_device::device_start()
{
	nes_zapper_device::device_start();
	save_item(NAME(m_latch));
	save_item(NAME(m_strobe));
}


//-------------------------------------------------
//  read
//-------------------------------------------------

u8 nes_zapper_device::read_bit34()
{
	u8 ret = m_trigger->read();

	if (!m_sensor->detect_light(m_lightx->read(), m_lighty->read()))
		ret |= 0x08;

	return ret;
}

u8 nes_zapper_device::read_exp(offs_t offset)
{
	u8 ret = 0;
	if (offset == 1)    // $4017
		ret = nes_zapper_device::read_bit34();
	return ret;
}

u8 nes_bandaihs_device::read_exp(offs_t offset)
{
	u8 ret = 0;

	if (offset == 0)    // $4016
	{
		if (m_strobe)
			m_latch = m_joypad->read();
		ret = (m_latch & 1) << 1;
		m_latch = (m_latch >> 1) | 0x80;
	}
	else                // $4017
		ret = nes_zapper_device::read_exp(offset);

	return ret;
}

// In addition to bit 0 used here the real Bandai Hyper Shot also responds to
// bits 1 and 2, neither emulated here. Bit 1 turns on and off a motor, which
// causes the machine gun to shake as if to simulate recoil. Bit 2 turns on and
// off an internal speaker in the gun. Videos demostrate that the speaker plays
// the same audio as the Famicom itself. The analog audio signal of the FC is
// carried on expansion port pin 2, so there's likely nothing more going on.
void nes_bandaihs_device::write(u8 data)
{
	if (write_strobe(data))
		m_latch = m_joypad->read();
}
