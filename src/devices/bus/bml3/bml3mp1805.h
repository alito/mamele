// license:GPL-2.0+
// copyright-holders:Jonathan Edwards
/*********************************************************************

    bml3mp1805.h

    Hitachi MP-1805 floppy disk controller card for the MB-6890
    Floppy drive is attached

*********************************************************************/

#ifndef MAME_BUS_BML3_BML3MP1805_H
#define MAME_BUS_BML3_BML3MP1805_H

#pragma once

#include "bml3bus.h"
#include "imagedev/floppy.h"
#include "machine/mc6843.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class bml3bus_mp1805_device:
	public device_t,
	public device_bml3bus_card_interface
{
public:
	// construction/destruction
	bml3bus_mp1805_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t bml3_mp1805_r();
	void bml3_mp1805_w(uint8_t data);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	required_device_array<floppy_connector, 4> m_floppy;
	required_device<mc6843_device> m_mc6843;

private:
	uint8_t *m_rom;

	uint8_t m_control;

	static void floppy_drives(device_slot_interface &device);
};

// device type definition
DECLARE_DEVICE_TYPE(BML3BUS_MP1805, bml3bus_mp1805_device)

#endif // MAME_BUS_BML3_BML3MP1805_H
