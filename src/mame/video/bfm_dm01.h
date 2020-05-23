// license:BSD-3-Clause
// copyright-holders:David Haywood
/************************************************************************************

    Bellfruit dotmatrix driver, (under heavy construction !!!)

*************************************************************************************/
#ifndef MAME_VIDEO_BFM_DM01
#define MAME_VIDEO_BFM_DM01

#pragma once

#include "emupal.h"
#include "screen.h"

class bfm_dm01_device : public device_t
{
public:
	bfm_dm01_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	~bfm_dm01_device() {}

	auto busy_callback() { return m_busy_cb.bind(); }

	uint8_t control_r();
	void control_w(uint8_t data);
	uint8_t mux_r();
	void mux_w(uint8_t data);
	uint8_t comm_r();
	void comm_w(uint8_t data);
	uint8_t unknown_r();
	void unknown_w(uint8_t data);

	void writedata(uint8_t data);
	int busy(void);

	void bfm_dm01_memmap(address_map &map);
protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	static constexpr unsigned BYTES_PER_ROW = 9;

	required_device<cpu_device> m_matrixcpu;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	// internal state
	int m_data_avail;
	int m_control;
	int m_xcounter;
	int m_segbuffer[65];
	int m_busy;

	uint8_t m_scanline[BYTES_PER_ROW];
	uint8_t m_comdata;

	devcb_write_line m_busy_cb;

	int read_data(void);

	bitmap_ind16 m_tmpbitmap;

	INTERRUPT_GEN_MEMBER(nmi_line_assert);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};

DECLARE_DEVICE_TYPE(BFM_DM01, bfm_dm01_device)

#endif // MAME_VIDEO_BFM_DM01
