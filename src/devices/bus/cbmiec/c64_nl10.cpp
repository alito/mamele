// license:BSD-3-Clause
// copyright-holders:Curt Coder
/**********************************************************************

    Star NL-10 Printer Interface Cartridge emulation

**********************************************************************/

#include "emu.h"
#include "c64_nl10.h"

#include "cpu/m6800/m6801.h"



//**************************************************************************
//  DEVICE DEFINITIONS
//**************************************************************************

DEFINE_DEVICE_TYPE(C64_NL10_INTERFACE, c64_nl10_interface_device, "c64_nl10", "Star NL-10 C64 Interface Cartridge")


//-------------------------------------------------
//  ROM( c64_nl10_interface )
//-------------------------------------------------

ROM_START( c64_nl10_interface )
	ROM_REGION( 0x8000, "rom", 0 )
	ROM_LOAD( "nlc 1.5.ic2", 0x0000, 0x8000, CRC(748840b6) SHA1(5b3b9e8a93d5d77a49160b3d0c2489ba7be99c9a) )
ROM_END


//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

const tiny_rom_entry *c64_nl10_interface_device::device_rom_region() const
{
	return ROM_NAME( c64_nl10_interface );
}



//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

//-------------------------------------------------
//  c64_nl10_interface_device - constructor
//-------------------------------------------------

c64_nl10_interface_device::c64_nl10_interface_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, C64_NL10_INTERFACE, tag, owner, clock)
	, device_cbm_iec_interface(mconfig, *this)
{
}


//-------------------------------------------------
//  mem_map - CPU memory map
//-------------------------------------------------

void c64_nl10_interface_device::mem_map(address_map &map)
{
	map(0x0000, 0x0027).m("bufcpu", FUNC(hd6303y_cpu_device::hd6301y_io)); // TODO: internalize this
	map(0x0040, 0x013f).ram(); // TODO: internalize this
	map(0x6000, 0x7fff).ram();
	map(0x8000, 0xffff).rom().region("rom", 0);
}


//-------------------------------------------------
//  device_add_mconfig - machine configuration
//-------------------------------------------------

void c64_nl10_interface_device::device_add_mconfig(machine_config &config)
{
	hd6303y_cpu_device &bufcpu(HD6303Y(config, "bufcpu", 8'000'000));
	bufcpu.set_addrmap(AS_PROGRAM, &c64_nl10_interface_device::mem_map);
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void c64_nl10_interface_device::device_start()
{
}


//-------------------------------------------------
//  device_reset - device-specific reset
//-------------------------------------------------

void c64_nl10_interface_device::device_reset()
{
}


//-------------------------------------------------
//  cbm_iec_atn -
//-------------------------------------------------

void c64_nl10_interface_device::cbm_iec_atn(int state)
{
}


//-------------------------------------------------
//  cbm_iec_data -
//-------------------------------------------------

void c64_nl10_interface_device::cbm_iec_data(int state)
{
}


//-------------------------------------------------
//  cbm_iec_reset -
//-------------------------------------------------

void c64_nl10_interface_device::cbm_iec_reset(int state)
{
	if (!state)
	{
		device_reset();
	}
}
