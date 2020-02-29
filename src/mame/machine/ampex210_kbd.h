// license:BSD-3-Clause
// copyright-holders:AJR

#ifndef MAME_MACHINE_AMPEX210_KBD_H
#define MAME_MACHINE_AMPEX210_KBD_H

#include "cpu/mcs48/mcs48.h"

class ampex230_keyboard_device : public device_t
{
public:
	// construction/destruction
	ampex230_keyboard_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_resolve_objects() override;
	virtual void device_start() override;

private:
	// handlers
	u8 bus_r();
	DECLARE_READ_LINE_MEMBER(t0_r);
	DECLARE_READ_LINE_MEMBER(t1_r);
	void p1_w(u8 data);
	void p2_w(u8 data);

	required_device<mcs48_cpu_device> m_mcu;
	required_ioport_array<14> m_key_row;

	u16 m_key_scan;
};

// device type declaration
DECLARE_DEVICE_TYPE(AMPEX230_KEYBOARD, ampex230_keyboard_device)

#endif // MAME_MACHINE_AMPEX210_KBD_H
