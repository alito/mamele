// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
 * s3virge.h
 *
 * S3 ViRGE 2D/3D video card
 *
 */

#ifndef MAME_BUS_ISA_S3VIRGE_H
#define MAME_BUS_ISA_S3VIRGE_H

#pragma once

#include "video/pc_vga.h"

// ======================> s3virge_vga_device

class s3virge_vga_device :  public s3_vga_device
{
public:
	// construction/destruction
	s3virge_vga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto linear_config_changed() { return m_linear_config_changed_cb.bind(); }

	virtual uint8_t port_03b0_r(offs_t offset) override;
	virtual void port_03b0_w(offs_t offset, uint8_t data) override;
	virtual uint8_t port_03c0_r(offs_t offset) override;
	virtual void port_03c0_w(offs_t offset, uint8_t data) override;
	virtual uint8_t port_03d0_r(offs_t offset) override;
	virtual void port_03d0_w(offs_t offset, uint8_t data) override;
	virtual uint8_t mem_r(offs_t offset) override;
	virtual void mem_w(offs_t offset, uint8_t data) override;

	uint8_t fb_r(offs_t offset);
	void fb_w(offs_t offset, uint8_t data);
	uint32_t s3d_sub_status_r();
	void s3d_sub_control_w(uint32_t data);
	uint32_t s3d_func_ctrl_r();

	uint32_t s3d_register_r(offs_t offset);
	void s3d_register_w(offs_t offset, uint32_t data);

	void image_xfer(uint32_t data)
	{
//      if(s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_COMMAND] & 0x00000080)
		{
//      logerror("IMG Xfer:(%u):%08x  X:%u(%u) Y:%u(%u)\n",s3virge.s3d.bitblt_step_count,data,s3virge.s3d.bitblt_x_current,s3virge.s3d.bitblt_width,s3virge.s3d.bitblt_y_current,s3virge.s3d.bitblt_height);
		s3virge.s3d.image_xfer = data;
		bitblt_step();
		}
	}

	uint16_t get_crtc_port() { return (vga.miscellaneous_output&1)?0x3d0:0x3b0; }
	uint32_t get_linear_address() { return s3virge.linear_address; }
	void set_linear_address(uint32_t addr) { s3virge.linear_address = addr; }
	uint8_t get_linear_address_size() { return s3virge.linear_address_size; }
	uint32_t get_linear_address_size_full() { return s3virge.linear_address_size_full; }
	bool is_linear_address_active() { return s3virge.linear_address_enable; }
	bool is_new_mmio_active() { return s3.cr53 & 0x08; }
	uint16_t dest_stride()
	{
//      if((s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_COMMAND] & 0x0000001c) == 0x08)
//      {
//          popmessage("Stride=%08x",(((s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_DEST_SRC_STR] >> 16) & 0xfff8) / 3)
//              + ((s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_DEST_SRC_STR] >> 16) & 0xfff8));
//          return (((s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_DEST_SRC_STR] >> 16) & 0xfff8) / 3)
//              + ((s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_DEST_SRC_STR] >> 16) & 0xfff8);
//      }
//      else
			return (s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_DEST_SRC_STR] >> 16) & 0xfff8;
	}

	ibm8514a_device* get_8514() { fatalerror("s3virge requested non-existent 8514/A device\n"); return nullptr; }

	enum
	{
		TIMER_DRAW_STEP = 10
	};


protected:
	s3virge_vga_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual uint16_t offset() override;

	enum
	{
		LAW_64K = 0,
		LAW_1MB,
		LAW_2MB,
		LAW_4MB
	};

	// register groups
	enum
	{
		OP_BITBLT = 0,
		OP_2DLINE,
		OP_2DPOLY,
		OP_3DLINE,
		OP_3DTRI
	};

	enum
	{
		S3D_STATE_IDLE = 0,
		S3D_STATE_BITBLT,
		S3D_STATE_2DLINE,
		S3D_STATE_2DPOLY,
		S3D_STATE_3DLINE,
		S3D_STATE_3DPOLY
	};

	enum
	{
		S3D_REG_SRC_BASE = 0xd4/4,
		S3D_REG_DEST_BASE = 0xd8/4,
		S3D_REG_CLIP_L_R = 0xdc/4,
		S3D_REG_CLIP_T_B = 0xe0/4,
		S3D_REG_DEST_SRC_STR = 0xe4/4,
		S3D_REG_MONO_PAT_0 = 0xe8/4,
		S3D_REG_MONO_PAT_1 = 0xec/4,
		S3D_REG_PAT_BG_CLR = 0xf0/4,
		S3D_REG_PAT_FG_CLR = 0xf4/4,
		S3D_REG_SRC_BG_CLR = 0xf8/4,
		S3D_REG_SRC_FG_CLR = 0xfc/4,
		S3D_REG_COMMAND = 0x100/4,
		S3D_REG_RWIDTH_HEIGHT = 0x104/4,
		S3D_REG_RSRC_XY = 0x108/4,
		S3D_REG_RDEST_XY = 0x10c/4
	};

	struct
	{
		uint32_t linear_address;
		uint8_t linear_address_size;
		uint32_t linear_address_size_full;
		bool linear_address_enable;
		uint32_t interrupt_enable;

		struct
		{
			int state;
			bool busy;
			struct
			{
				uint32_t reg[256];
				int op_type;
			} cmd_fifo[16];
			int cmd_fifo_next_ptr;  // command added here in FIFO
			int cmd_fifo_current_ptr;  // command currently being processed in FIFO
			int cmd_fifo_slots_free;

			uint8_t pattern[0xc0];
			uint32_t reg[5][256];

			// BitBLT command state
			uint16_t bitblt_x_src;
			uint16_t bitblt_y_src;
			uint16_t bitblt_x_dst;
			uint16_t bitblt_y_dst;
			int16_t bitblt_x_current;
			int16_t bitblt_y_current;
			int16_t bitblt_x_src_current;
			int16_t bitblt_y_src_current;
			int8_t bitblt_pat_x;
			int8_t bitblt_pat_y;
			uint16_t bitblt_height;
			uint16_t bitblt_width;
			uint32_t bitblt_step_count;
			uint64_t bitblt_mono_pattern;
			uint32_t bitblt_current_pixel;
			uint32_t bitblt_pixel_pos;  // current position in a pixel (for packed 24bpp colour image transfers)
			uint32_t image_xfer;  // source data via image transfer ports
			uint16_t clip_l;
			uint16_t clip_r;
			uint16_t clip_t;
			uint16_t clip_b;
		} s3d;
	} s3virge;

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
	void write_pixel32(uint32_t base, uint16_t x, uint16_t y, uint32_t val)
	{
		if(s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_COMMAND] & 0x00000002)
			if(x < s3virge.s3d.clip_l || x > s3virge.s3d.clip_r || y < s3virge.s3d.clip_t || y > s3virge.s3d.clip_b)
				return;
		vga.memory[(base + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] = val & 0xff;
		vga.memory[(base + 1 + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] = (val >> 8) & 0xff;
		vga.memory[(base + 2 + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] = (val >> 16) & 0xff;
		vga.memory[(base + 3 + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] = (val >> 24) & 0xff;
	}
	void write_pixel24(uint32_t base, uint16_t x, uint16_t y, uint32_t val)
	{
		if(s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_COMMAND] & 0x00000002)
			if(x < s3virge.s3d.clip_l || x > s3virge.s3d.clip_r || y < s3virge.s3d.clip_t || y > s3virge.s3d.clip_b)
				return;
		vga.memory[(base + (x*3) + (y*dest_stride())) % vga.svga_intf.vram_size] = val & 0xff;
		vga.memory[(base + 1 + (x*3) + (y*dest_stride())) % vga.svga_intf.vram_size] = (val >> 8) & 0xff;
		vga.memory[(base + 2 + (x*3) + (y*dest_stride())) % vga.svga_intf.vram_size] = (val >> 16) & 0xff;
	}
	void write_pixel16(uint32_t base, uint16_t x, uint16_t y, uint16_t val)
	{
		if(s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_COMMAND] & 0x00000002)
			if(x < s3virge.s3d.clip_l || x > s3virge.s3d.clip_r || y < s3virge.s3d.clip_t || y > s3virge.s3d.clip_b)
				return;
		vga.memory[(base + (x*2) + (y*dest_stride())) % vga.svga_intf.vram_size] = val & 0xff;
		vga.memory[(base + 1 + (x*2) + (y*dest_stride())) % vga.svga_intf.vram_size] = (val >> 8) & 0xff;
	}
	void write_pixel8(uint32_t base, uint16_t x, uint16_t y, uint8_t val)
	{
		if(s3virge.s3d.cmd_fifo[s3virge.s3d.cmd_fifo_current_ptr].reg[S3D_REG_COMMAND] & 0x00000002)
			if(x < s3virge.s3d.clip_l || x > s3virge.s3d.clip_r || y < s3virge.s3d.clip_t || y > s3virge.s3d.clip_b)
				return;
		vga.memory[(base + x + (y*dest_stride())) % vga.svga_intf.vram_size] = val;
	}
	uint32_t read_pixel32(uint32_t base, uint16_t x, uint16_t y)
	{
		return (vga.memory[(base + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] << 24) | (vga.memory[(base + 1 + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] << 16)
			| (vga.memory[(base + 2 + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size] << 8) | vga.memory[(base + 3 + (x*4) + (y*dest_stride())) % vga.svga_intf.vram_size];
	}
	uint32_t read_pixel24(uint32_t base, uint16_t x, uint16_t y)
	{
		return (vga.memory[(base + (x*3) + (y*dest_stride())) % vga.svga_intf.vram_size]) | (vga.memory[(base + 1 + (x*3) + (y*dest_stride())) % vga.svga_intf.vram_size] << 8)
			| (vga.memory[(base + 2 + (x*3) + (y*dest_stride())) % vga.svga_intf.vram_size] << 16);
	}
	uint16_t read_pixel16(uint32_t base, uint16_t x, uint16_t y)
	{
		return (vga.memory[(base + (x*2) + (y*dest_stride()) % vga.svga_intf.vram_size)]) | (vga.memory[(base + 1 + (x*2) + (y*dest_stride())) % vga.svga_intf.vram_size] << 8);
	}
	uint8_t read_pixel8(uint32_t base, uint16_t x, uint16_t y)
	{
		return vga.memory[(base + x + (y*dest_stride())) % vga.svga_intf.vram_size];
	}
	uint32_t GetROP(uint8_t rop, uint32_t src, uint32_t dst, uint32_t pat);
	bool advance_pixel();

	devcb_write_line m_linear_config_changed_cb;

private:
	emu_timer* m_draw_timer;
	void bitblt_step();
	void bitblt_colour_step();
	void bitblt_monosrc_step();
	void line2d_step();
	void poly2d_step();
	void line3d_step();
	void poly3d_step();
	void add_command(int cmd_type);
	void command_start();
	void command_finish();

	virtual uint8_t s3_crtc_reg_read(uint8_t index);
	virtual void s3_define_video_mode(void);
	virtual void s3_crtc_reg_write(uint8_t index, uint8_t data);
	// has no 8514/A device
};


// ======================> s3virgedx_vga_device

class s3virgedx_vga_device :  public s3virge_vga_device
{
public:
	// construction/destruction
	s3virgedx_vga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	s3virgedx_vga_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
};

// ======================> s3virgedx_vga_device

class s3virgedx_rev1_vga_device :  public s3virgedx_vga_device
{
public:
	// construction/destruction
	s3virgedx_rev1_vga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
};

// device type definition
DECLARE_DEVICE_TYPE(S3VIRGE,    s3virge_vga_device)
DECLARE_DEVICE_TYPE(S3VIRGEDX,  s3virgedx_vga_device)
DECLARE_DEVICE_TYPE(S3VIRGEDX1, s3virgedx_rev1_vga_device)

#endif // MAME_BUS_ISA_S3VIRGE_H
