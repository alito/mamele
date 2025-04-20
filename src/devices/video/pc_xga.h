// license:BSD-3-Clause
// copyright-holders: Carl

#ifndef MAME_VIDEO_PC_XGA_H
#define MAME_VIDEO_PC_XGA_H

#include "pc_vga.h"

#include "screen.h"


class xga_copro_device : public device_t
{
public:
	enum class TYPE {
		XGA,
		OTI111
	};

	xga_copro_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	u8 xga_read(offs_t offset);
	void xga_write(offs_t offset, u8 data);

	auto mem_read_callback() { return m_mem_read_cb.bind(); }
	auto mem_write_callback() { return m_mem_write_cb.bind(); }
	auto set_type(TYPE var) { m_var = var; }
protected:
	virtual void device_start() override;
	virtual void device_reset() override;
private:
	void start_command();
	void do_pxblt();
	u32 read_map_pixel(int x, int y, int map);
	void write_map_pixel(int x, int y, int map, u32 pixel);
	u32 rop(u32 src, u32 dst, u8 rop);

	u8 m_pelmap;
	u32 m_pelmap_base[4];
	u16 m_pelmap_width[4];
	u16 m_pelmap_height[4];
	u8 m_pelmap_format[4];
	s16 m_bresh_err;
	s16 m_bresh_k1;
	s16 m_bresh_k2;
	u32 m_dir;
	u8 m_fmix;
	u8 m_bmix;
	u8 m_destccc;
	u32 m_destccv;
	u32 m_pelbmask;
	u32 m_carrychain;
	u32 m_fcolor;
	u32 m_bcolor;
	u16 m_opdim1;
	u16 m_opdim2;
	u16 m_maskorigx;
	u16 m_maskorigy;
	u16 m_srcxaddr;
	u16 m_srcyaddr;
	u16 m_patxaddr;
	u16 m_patyaddr;
	u16 m_dstxaddr;
	u16 m_dstyaddr;
	u32 m_pelop;
	TYPE m_var;
	devcb_read8 m_mem_read_cb;
	devcb_write8 m_mem_write_cb;
};

DECLARE_DEVICE_TYPE(XGA_COPRO, xga_copro_device)

class oak_oti111_vga_device : public svga_device
{
public:
	oak_oti111_vga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	u8 xga_read(offs_t offset);
	void xga_write(offs_t offset, u8 data);

	void ramdac_mmio_map(address_map &map);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual uint16_t offset() override;

	virtual void io_3bx_3dx_map(address_map &map) override;
private:
	u8 m_oak_regs[0x3b];
	u8 m_oak_idx;
	required_device<xga_copro_device> m_xga;
};

DECLARE_DEVICE_TYPE(OTI111, oak_oti111_vga_device)

#endif // MAME_VIDEO_PC_XGA_H
