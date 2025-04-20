// license:BSD-3-Clause
// copyright-holders:R. Belmont
#ifndef MAME_MACHINE_ADBMODEM_H
#define MAME_MACHINE_ADBMODEM_H

#pragma once

#define USE_BUS_ADB (0)

#if USE_BUS_ADB
#include "bus/adb/adb.h"
#endif

#include "cpu/pic16c5x/pic16c5x.h"

// ======================> adbmodem_device

class adbmodem_device :  public device_t
{
public:
	// construction/destruction
	adbmodem_device(const machine_config &mconfig, const char *tag, device_t *owner, int type)
		: adbmodem_device(mconfig, tag, owner, (uint32_t)0)
	{
		set_type(type);
	}

	adbmodem_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// inline configuration helpers
	void set_type(int type) { rom_offset = type; }


#if USE_BUS_ADB
	void adb_w(int id, int state);
	void adb_poweron_w(int id, int state);
	void adb_change();
#endif

	// interface routines
	u8 get_via_data() { return via_data; }
	void set_via_data(uint8_t dat) { via_data = dat; }
	u8 get_via_clock() { return via_clock; }
	void set_adb_line(int linestate) { adb_in = (linestate == ASSERT_LINE) ? true : false; }
	void set_via_state(u8 state) { m_via_state = state; }

	int rom_offset;

	auto reset_callback() { return write_reset.bind(); }
	auto linechange_callback() { return write_linechange.bind(); }
	auto via_clock_callback() { return write_via_clock.bind(); }
	auto via_data_callback() { return write_via_data.bind(); }
	auto irq_callback() { return write_irq.bind(); }

	devcb_write_line write_reset, write_linechange;
	devcb_write_line write_via_clock, write_via_data, write_irq;

	void adbmodem_map(address_map &map);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	required_device<pic16c5x_device> m_maincpu;

private:
	u8 porta_r();
	void porta_w(u8 data);
	u8 portb_r();
	void portb_w(u8 data);

	u8 via_data = 0;
	u8 via_clock = 0;
	u8 last_adb = 0;
	u8 m_via_state = 0;
	u64 last_adb_time = 0;
	bool adb_in = false;
	int reset_line = 0;
	int m_adb_dtime = 0;

	#if USE_BUS_ADB
	optional_device <adb_connector> m_adb_connector[2];
	adb_device *m_adb_device[2]{};
	bool m_adb_device_out[2]{};
	bool m_adb_device_poweron[2]{};
	bool m_adb_out = false;
	#endif

	void send_port(uint8_t offset, uint8_t data);
};

// device type definition
DECLARE_DEVICE_TYPE(ADBMODEM, adbmodem_device)

#endif // MAME_MACHINE_ADBMODEM_H
