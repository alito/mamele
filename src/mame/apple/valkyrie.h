// license:BSD-3-Clause
// copyright-holders:R. Belmont

#ifndef MAME_APPLE_VALKYRIE_H
#define MAME_APPLE_VALKYRIE_H

#pragma once

#include "cpu/m68000/m68040.h"

#include "emupal.h"
#include "screen.h"

class valkyrie_device : public device_t
{
public:
	valkyrie_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);
	virtual ~valkyrie_device() = default;

	void map(address_map &map);

	auto write_irq() { return m_irq.bind(); }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

	void recalc_ints();
	void recalc_mode();

	u32 m_vram_size;
	u32 m_pixel_clock;

	u8 m_pal_address, m_pal_idx, m_mode;

private:
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_ioport m_monitor_config;
	devcb_write_line m_irq;

	std::unique_ptr<u32[]> m_vram;
	emu_timer *m_vbl_timer;
	u32 m_vram_offset;
	u8 m_monitor_id;
	u32 m_base, m_stride, m_video_timing;
	s32 m_int_status;
	u32 m_hres, m_vres, m_htotal, m_vtotal, m_config;
	bool m_enable;

	u32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	u32 regs_r(offs_t offset);
	void regs_w(offs_t offset, u32 data);
	u32 ramdac_r(offs_t offset);
	void ramdac_w(offs_t offset, u32 data);
	u32 vram_r(offs_t offset);
	void vram_w(offs_t offset, u32 data, u32 mem_mask);

	TIMER_CALLBACK_MEMBER(vbl_tick);
};

DECLARE_DEVICE_TYPE(VALKYRIE, valkyrie_device)

#endif  /* MAME_APPLE_VALKYRIE_H */
