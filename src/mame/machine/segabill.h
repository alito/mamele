// license: BSD-3-Clause
// copyright-holders: Dirk Best
/***************************************************************************

    Sega Billboard

***************************************************************************/

#ifndef MAME_MACHINE_SEGABILL_H
#define MAME_MACHINE_SEGABILL_H

#pragma once

#include "machine/315_5338a.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class sega_billboard_device : public device_t
{
public:
	// construction/destruction
	sega_billboard_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void mem_map(address_map &map);
	void io_map(address_map &map);

	void write(uint8_t data);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	required_device<cpu_device> m_billcpu;
	required_device<sega_315_5338a_device> m_io;
	output_finder<4> m_digits;
	output_finder<2> m_leds;

	void irq0_line_hold(device_t &device);

	uint8_t cmd_r();
	template<int N> void digit_w(uint8_t data);
	void led_w(uint8_t data);

	uint8_t m_cmd;
};

// device type definition
DECLARE_DEVICE_TYPE(SEGA_BILLBOARD, sega_billboard_device)

#endif // MAME_MACHINE_SEGABILL_H
