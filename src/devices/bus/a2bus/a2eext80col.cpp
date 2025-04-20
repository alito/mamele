// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    a2eext80col.cpp

    Apple IIe Extended 80 Column Card (64K of RAM, double-hi-res)

*********************************************************************/

#include "emu.h"
#include "a2eext80col.h"


/***************************************************************************
    PARAMETERS
***************************************************************************/

//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

DEFINE_DEVICE_TYPE(A2EAUX_EXT80COL, a2eaux_ext80col_device, "a2eext80", "Apple IIe Extended 80-Column Card")

//**************************************************************************
//  LIVE DEVICE
//**************************************************************************

a2eaux_ext80col_device::a2eaux_ext80col_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
		a2eaux_ext80col_device(mconfig, A2EAUX_EXT80COL, tag, owner, clock)
{
}

a2eaux_ext80col_device::a2eaux_ext80col_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
		device_t(mconfig, type, tag, owner, clock),
		device_a2eauxslot_card_interface(mconfig, *this)
{
}

//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void a2eaux_ext80col_device::device_start()
{
	memset(m_ram, 0, sizeof(m_ram));
	save_item(NAME(m_ram));
}

void a2eaux_ext80col_device::device_reset()
{
}

u8 a2eaux_ext80col_device::read_auxram(uint16_t offset)
{
	return m_ram[offset];
}

void a2eaux_ext80col_device::write_auxram(uint16_t offset, u8 data)
{
	m_ram[offset] = data;
}

u8 *a2eaux_ext80col_device::get_vram_ptr()
{
	return &m_ram[0];
}

u8 *a2eaux_ext80col_device::get_auxbank_ptr()
{
	return &m_ram[0];
}

u16 a2eaux_ext80col_device::get_auxbank_mask()
{
	return 0xffff;
}
