// license:GPL-2.0+
// copyright-holders:Dirk Best
/***************************************************************************

    VTech Laser/VZ Printer Interface

    VTech PI 20
    Dick Smith Electronics X-7320

***************************************************************************/

#include "emu.h"
#include "printer.h"


//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(VTECH_PRINTER_INTERFACE, vtech_printer_interface_device, "vtech_printer", "Laser/VZ Printer Interface")

//-------------------------------------------------
//  device_add_mconfig - add device configuration
//-------------------------------------------------

void vtech_printer_interface_device::device_add_mconfig(machine_config &config)
{
	CENTRONICS(config, m_centronics, centronics_devices, "printer");
	m_centronics->busy_handler().set(FUNC(vtech_printer_interface_device::busy_w));

	OUTPUT_LATCH(config, m_latch);
	m_centronics->set_output_latch(*m_latch);
}


//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  vtech_printer_interface_device - constructor
//-------------------------------------------------

vtech_printer_interface_device::vtech_printer_interface_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, VTECH_PRINTER_INTERFACE, tag, owner, clock),
	device_vtech_ioexp_interface(mconfig, *this),
	m_centronics(*this, "centronics"),
	m_latch(*this, "latch"),
	m_centronics_busy(0)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void vtech_printer_interface_device::device_start()
{
}

//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void vtech_printer_interface_device::device_reset()
{
	io_space().install_read_handler(0x00, 0x00, read8smo_delegate(*this, FUNC(vtech_printer_interface_device::busy_r)));
	io_space().install_write_handler(0x0d, 0x0d, write8smo_delegate(*this, FUNC(vtech_printer_interface_device::strobe_w)));
	io_space().install_write_handler(0x0e, 0x0e, write8smo_delegate(*m_latch, FUNC(output_latch_device::write)));
}


//**************************************************************************
//  IMPLEMENTATION
//**************************************************************************

WRITE_LINE_MEMBER( vtech_printer_interface_device::busy_w )
{
	m_centronics_busy = state;
}

uint8_t vtech_printer_interface_device::busy_r()
{
	return 0xfe | m_centronics_busy;
}

void vtech_printer_interface_device::strobe_w(uint8_t data)
{
	m_centronics->write_strobe(1);
	m_centronics->write_strobe(0);
}
