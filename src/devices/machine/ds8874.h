// license:BSD-3-Clause
// copyright-holders:hap
/*

  National Semiconductor DS8874 9-Digit Shift Input LED Driver

*/

#ifndef MAME_MACHINE_DS8874_H
#define MAME_MACHINE_DS8874_H

#pragma once

// pinout reference

/*
              _______
       CP  1 |*      | 14 Vcc
    _DATA  2 |       | 13 LOW BATT OUT
    OUT 1  3 |       | 12 OUT 9
    OUT 2  4 |DS8874N| 11 OUT 8
    OUT 3  5 |       | 10 OUT 7
    OUT 4  6 |       | 9  OUT 6
      GND  7 |_______| 8  OUT 5

    CP = CLOCK PULSE

*/


class ds8874_device : public device_t
{
public:
	ds8874_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);

	// configuration helpers
	auto write_output() { return m_write_output.bind(); } // OUT pins

	DECLARE_WRITE_LINE_MEMBER(data_w);
	DECLARE_WRITE_LINE_MEMBER(cp_w);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override { refresh_output(); }

private:
	void refresh_output();

	int m_data = 0;
	int m_cp = 0;
	u16 m_shift = 0xff;

	// callbacks
	devcb_write16 m_write_output;
};


DECLARE_DEVICE_TYPE(DS8874, ds8874_device)

#endif // MAME_MACHINE_DS8874_H
