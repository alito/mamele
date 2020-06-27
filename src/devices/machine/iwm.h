// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    Apple IWM floppy disk controller

*********************************************************************/

#pragma once

#ifndef MAME_MACHINE_IWM_H
#define MAME_MACHINE_IWM_H

#include "applefdintf.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class iwm_device: public applefdintf_device
{
public:
	// construction/destruction
	iwm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, uint32_t q3_clock = 0);
	iwm_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, XTAL q3_clock) :
		iwm_device(mconfig, tag, owner, clock, q3_clock.value()) {}

	virtual u8 read(offs_t offset) override;
	virtual void write(offs_t offset, u8 data) override;

	virtual void set_floppy(floppy_image_device *floppy) override;
	virtual floppy_image_device *get_floppy() const override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	enum {
		MODE_IDLE,
		MODE_ACTIVE, MODE_DELAY, // m_active modes
		MODE_READ, MODE_WRITE    // m_rw modes
	};

	// state machine states
	enum {
		S_IDLE,
		SR_WINDOW_EDGE_0,
		SR_WINDOW_EDGE_1,
		SW_WINDOW_MIDDLE,
		SW_WINDOW_END
	};

	floppy_image_device *m_floppy;
	emu_timer *m_timer;
	double m_q3_fclk_ratio, m_fclk_q3_ratio;
	u64 m_last_sync, m_next_state_change, m_sync_update, m_async_update;
	u64 m_flux_write_start;
	std::array<u64, 16> m_flux_write;
	u32 m_flux_write_count;
	u32 m_q3_clock;
	int m_active, m_rw, m_rw_state;
	u8 m_data, m_whd, m_mode, m_status, m_control;
	u8 m_rsh, m_wsh;

	void sync();
	u8 control(int offset, u8 data);
	u64 time_to_cycles(const attotime &tm) const;
	attotime cycles_to_time(u64 cycles) const;
	u64 fclk_to_q3(u64 cycles) const;
	u64 q3_to_fclk(u64 cycles) const;

	void mode_w(u8 data);
	void data_w(u8 data);

	u64 window_size() const;
	u64 half_window_size() const;
	u64 read_register_update_delay() const;
	u64 write_sync_half_window_size() const;
	inline bool is_sync() const;
	void flush_write();
};

DECLARE_DEVICE_TYPE(IWM, iwm_device)

#endif  /* MAME_MACHINE_IWM_H */
