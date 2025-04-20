// license:BSD-3-Clause
// copyright-holders:R. Belmont
/**********************************************************************

    macrtc.h - Apple 343-0042 real time clock and battery RAM
    by R. Belmont

**********************************************************************/

#ifndef MAME_MACHINE_MACRTC_H
#define MAME_MACHINE_MACRTC_H

#pragma once

#include "dirtc.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> rtc3430042_device

class rtc3430042_device :  public device_t,
						public device_rtc_interface,
						public device_nvram_interface
{
public:
	// construction/destruction
	rtc3430042_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_WRITE_LINE_MEMBER( ce_w );
	DECLARE_WRITE_LINE_MEMBER( clk_w );
	DECLARE_READ_LINE_MEMBER( data_r );
	DECLARE_WRITE_LINE_MEMBER( data_w );

	// 1 second square wave output
	auto cko_cb() { return m_cko_cb.bind(); }

protected:
	// device-level overrides
	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_rtc_interface overrides
	virtual void rtc_clock_updated(int year, int month, int day, int day_of_week, int hour, int minute, int second) override;
	virtual bool rtc_feature_leap_year() const override { return true; }

	// device_nvram_interface overrides
	virtual void nvram_default() override;
	virtual bool nvram_read(util::read_stream &file) override;
	virtual bool nvram_write(util::write_stream &file) override;

	TIMER_CALLBACK_MEMBER(half_seconds_tick);

private:
	devcb_write_line m_cko_cb;

	/* state of rTCEnb and rTCClk lines */
	uint8_t m_rtc_rTCEnb = 0;
	uint8_t m_rtc_rTCClk = 0;

	/* serial transmit/receive register : bits are shifted in/out of this byte */
	uint8_t m_rtc_data_byte = 0;
	/* serial transmitted/received bit count */
	uint8_t m_rtc_bit_count = 0;
	/* direction of the current transfer (0 : VIA->RTC, 1 : RTC->VIA) */
	uint8_t m_rtc_data_dir = 0;
	/* when rtc_data_dir == 1, state of rTCData as set by RTC (-> data bit seen by VIA) */
	uint8_t m_rtc_data_out = 0;

	/* set to 1 when command in progress */
	uint8_t m_rtc_cmd = 0;

	/* write protect flag */
	uint8_t m_rtc_write_protect = 0;

	/* internal seconds register */
	uint8_t m_rtc_seconds[/*8*/4]{};
	/* 20-byte long PRAM, or 256-byte long XPRAM */
	uint8_t m_pram[256]{};
	/* current extended address and RTC state */
	uint8_t m_rtc_xpaddr = 0;
	uint8_t m_rtc_state = 0;
	uint8_t m_data_latch = 0;
	bool m_cko = false;

	// timers
	emu_timer *m_clock_timer = nullptr;

	void rtc_shift_data(int data);
	void rtc_execute_cmd(int data);
};


// device type definition
DECLARE_DEVICE_TYPE(RTC3430042, rtc3430042_device)

#endif // MAME_MACHINE_MACRTC_H
