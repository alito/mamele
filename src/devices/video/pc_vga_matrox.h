// license:BSD-3-Clause
// copyright-holders:Angelo Salese

#ifndef MAME_VIDEO_PC_VGA_MATROX_H
#define MAME_VIDEO_PC_VGA_MATROX_H

#pragma once

#include "video/pc_vga.h"

#include "screen.h"

class matrox_vga_device :  public svga_device
{
public:
	matrox_vga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void ramdac_ext_map(address_map &map);

	virtual uint8_t mem_r(offs_t offset) override;
	virtual void mem_w(offs_t offset, uint8_t data) override;

	// TODO: polarity select
	bool vsync_status() { return vga_vblank(); }
	u32 vcount_r() { return screen().vpos() & 0xfff; }

protected:
	virtual void io_3bx_3dx_map(address_map &map) override;

	virtual void device_start() override;
	virtual void device_reset() override;

	virtual uint16_t offset() override;
	virtual uint32_t start_addr() override;
	virtual void recompute_params() override;

	void crtcext_map(address_map &map);
	void ramdac_indexed_map(address_map &map);
	void flush_true_color_mode();
private:
	virtual space_config_vector memory_space_config() const override;
	virtual u16 line_compare_mask() override;

	address_space_config m_crtcext_space_config;
	address_space_config m_ramdac_indexed_space_config;

	// CRTC
	u8 crtcext0_address_gen_r();
	void crtcext0_address_gen_w(offs_t offset, u8 data);
	u8 crtcext1_horizontal_counter_r();
	void crtcext1_horizontal_counter_w(offs_t offset, u8 data);
	u8 crtcext2_vertical_counter_r();
	void crtcext2_vertical_counter_w(offs_t offset, u8 data);
	u8 crtcext3_misc_r();
	void crtcext3_misc_w(offs_t offset, u8 data);
	u8 m_crtcext_index = 0;

	u8 m_crtcext_misc = 0;
	u8 m_crtcext_horz_counter = 0;
	u8 m_crtcext_vert_counter = 0;
	u8 m_crtcext_horz_half_count = 0;
	bool m_mgamode = false;
	bool m_interlace_mode = false;

	// RAMDAC
	u8 ramdac_ext_indexed_r();
	void ramdac_ext_indexed_w(offs_t offset, u8 data);

	u8 cursor_write_index_r();
	void cursor_write_index_w(offs_t offset, u8 data);
	u8 cursor_data_r();
	void cursor_data_w(offs_t offset, u8 data);
	u8 cursor_read_index_r();
	void cursor_read_index_w(offs_t offset, u8 data);

	u8 m_cursor_write_index = 0;
	u8 m_cursor_read_index = 0;
	u8 m_cursor_index_state = 0;
	u8 m_cursor_color[12]{};

	u8 truecolor_ctrl_r();
	void truecolor_ctrl_w(offs_t offset, u8 data);
	u8 multiplex_ctrl_r();
	void multiplex_ctrl_w(offs_t offset, u8 data);

	u8 m_multiplex_ctrl = 0;
	u8 m_truecolor_ctrl = 0;
};

DECLARE_DEVICE_TYPE(MATROX_VGA, matrox_vga_device)

#endif // MAME_VIDEO_PC_VGA_MATROX_H
