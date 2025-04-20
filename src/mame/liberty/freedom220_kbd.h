// license: BSD-3-Clause
// copyright-holders: Dirk Best
/***************************************************************************

    Liberty Freedom 220 keyboard

***************************************************************************/

#ifndef MAME_LIBERTY_FREEDOM220_KBD_H
#define MAME_LIBERTY_FREEDOM220_KBD_H

#pragma once

#include "cpu/mcs48/mcs48.h"
#include "sound/spkrdev.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> freedom220_kbd_device

class freedom220_kbd_device :  public device_t
{
public:
	// construction/destruction
	freedom220_kbd_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0);

	// callbacks
	auto tx_handler() { return m_tx_handler.bind(); }

	void rx_w(int state);

protected:
	// device_t overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void mem_map(address_map &map);
	void io_map(address_map &map);

	uint8_t select_r(offs_t offset);
	void speaker_w(offs_t offset, uint8_t data);
	uint8_t p1_r();
	void p2_w(uint8_t data);

	required_device<i8039_device> m_mcu;
	required_device<speaker_sound_device> m_speaker;
	required_ioport_array<16> m_keys;

	devcb_write_line m_tx_handler;

	uint8_t m_select;
};

// device type definition
DECLARE_DEVICE_TYPE(FREEDOM220_KBD, freedom220_kbd_device)

#endif // MAME_LIBERTY_FREEDOM220_KBD_H
