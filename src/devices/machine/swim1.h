// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    Apple SWIM1 floppy disk controller

*********************************************************************/
#ifndef MAME_MACHINE_SWIM1_H
#define MAME_MACHINE_SWIM1_H

#pragma once

#include "applefdintf.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************


class swim1_device : public applefdintf_device
{
public:
	// construction/destruction
	swim1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, uint32_t q3_clock);
	swim1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual u8 read(offs_t offset) override;
	virtual void write(offs_t offset, u8 data) override;

	virtual void set_floppy(floppy_image_device *floppy) override;
	virtual floppy_image_device *get_floppy() const override;

	virtual void sync() override;

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	enum {
		MODE_IDLE,
		MODE_ACTIVE, MODE_DELAY, // m_iwm_active modes
		MODE_READ, MODE_WRITE    // m_iwm_rw modes
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

	u64 m_flux_write_start;
	std::array<u64, 32> m_flux_write;
	u32 m_flux_write_count;
	u64 m_last_sync;
	double m_iwm_q3_fclk_ratio, m_iwm_fclk_q3_ratio;
	[[maybe_unused]] u32 m_q3_clock;

	u8 m_ism_param[16];
	u8 m_ism_mode, m_ism_setup;
	[[maybe_unused]] u8 m_ism_error;
	u8 m_ism_param_idx, m_ism_fifo_pos;
	[[maybe_unused]] u8 m_ism_tss_sr, m_ism_tss_output, m_ism_current_bit;
	u16 m_ism_fifo[2];
	[[maybe_unused]] u16 m_ism_sr;
	u16 m_ism_crc;
	[[maybe_unused]] u16 m_ism_mfm_sync_counter;
	[[maybe_unused]] u32 m_ism_half_cycles_before_change;

	u64 m_iwm_next_state_change, m_iwm_sync_update, m_iwm_async_update;
	int m_iwm_active, m_iwm_rw, m_iwm_rw_state;
	u8 m_iwm_data, m_iwm_whd, m_iwm_mode, m_iwm_status, m_iwm_control;
	u8 m_iwm_rsh, m_iwm_wsh;
	u8 m_iwm_to_ism_counter;

	u64 time_to_cycles(const attotime &tm) const;
	attotime cycles_to_time(u64 cycles) const;
	void flush_write(u64 when = 0);

	u64 iwm_fclk_to_q3(u64 cycles) const;
	u64 iwm_q3_to_fclk(u64 cycles) const;
	u64 iwm_window_size() const;
	u64 iwm_half_window_size() const;
	u64 iwm_read_register_update_delay() const;
	u64 iwm_write_sync_half_window_size() const;
	inline bool iwm_is_sync() const;
	void iwm_mode_w(u8 data);
	void iwm_data_w(u8 data);
	u8 iwm_control(int offset, u8 data);
	void iwm_sync();

	void ism_fifo_clear();
	bool ism_fifo_push(u16 data);
	u16 ism_fifo_pop();
	void ism_show_mode() const;
	void ism_crc_update(int bit);
	void ism_crc_clear();
	u8 ism_read(offs_t offset);
	void ism_write(offs_t offset, u8 data);
	void ism_sync();
};

DECLARE_DEVICE_TYPE(SWIM1, swim1_device)

#endif  /* MAME_MACHINE_SWIM1_H */
