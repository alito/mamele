// license:BSD-3-Clause
// copyright-holders:Robbbert
#ifndef MAME_INCLUDES_KAYPRO_H
#define MAME_INCLUDES_KAYPRO_H

#pragma once

#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"
#include "machine/z80pio.h"
#include "bus/centronics/ctronics.h"
#include "imagedev/floppy.h"
#include "imagedev/snapquik.h"
#include "sound/beep.h"
#include "video/mc6845.h"
#include "machine/wd_fdc.h"
#include "emupal.h"
#include "screen.h"

class kaypro_state : public driver_device
{
public:
	enum
	{
		TIMER_FLOPPY
	};

	kaypro_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_palette(*this, "palette")
		, m_screen(*this, "screen")
		, m_maincpu(*this, "maincpu")
		, m_p_chargen(*this, "chargen")
		, m_pio_g(*this, "z80pio_g")
		, m_pio_s(*this, "z80pio_s")
		, m_centronics(*this, "centronics")
		, m_fdc(*this, "fdc")
		, m_floppy0(*this, "fdc:0")
		, m_floppy1(*this, "fdc:1")
		, m_crtc(*this, "crtc")
		, m_beep(*this, "beeper")
		, m_bankr(*this, "bankr")
		, m_bankw(*this, "bankw")
		, m_bank3(*this, "bank3")
		{}

	void omni2(machine_config &config);
	void kayproiv(machine_config &config);
	void kayproii(machine_config &config);
	void kaypronew2(machine_config &config);
	void kaypro484(machine_config &config);
	void kaypro10(machine_config &config);
	void kaypro284(machine_config &config);

	void init_kaypro();

private:
	void kaypro484_io(address_map &map);
	void kaypro_map(address_map &map);
	void kayproii_io(address_map &map);

	DECLARE_WRITE_LINE_MEMBER(write_centronics_busy);
	u8 kaypro484_87_r();
	u8 kaypro484_system_port_r();
	u8 kaypro484_status_r();
	u8 kaypro484_videoram_r();
	void kaypro484_system_port_w(u8 data);
	void kaypro484_index_w(u8 data);
	void kaypro484_register_w(u8 data);
	void kaypro484_videoram_w(u8 data);
	u8 pio_system_r();
	void kayproii_pio_system_w(u8 data);
	void kayproiv_pio_system_w(u8 data);
	DECLARE_WRITE_LINE_MEMBER(fdc_intrq_w);
	DECLARE_WRITE_LINE_MEMBER(fdc_drq_w);
	u8 kaypro_videoram_r(offs_t offset);
	void kaypro_videoram_w(offs_t offset, u8 data);
	void machine_start() override;
	void machine_reset() override;
	void kaypro_palette(palette_device &palette) const;
	uint32_t screen_update_kayproii(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_kaypro484(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_omni2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	MC6845_UPDATE_ROW(kaypro484_update_row);
	DECLARE_QUICKLOAD_LOAD_MEMBER(quickload_cb);

	void mc6845_screen_configure();
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	u8 m_mc6845_reg[32];
	u8 m_mc6845_ind;
	u8 m_framecnt;
	std::unique_ptr<u8[]> m_ram;
	std::unique_ptr<u8[]> m_vram; // video ram
	std::unique_ptr<u8[]> m_dummy;  // black hole for write to rom
	int m_centronics_busy;
	bool m_is_motor_off;
	u8 m_fdc_rq;
	u8 m_system_port;
	uint16_t m_mc6845_video_address;
	floppy_image_device *m_floppy;

	required_device<palette_device> m_palette;
	required_device<screen_device> m_screen;
	required_device<z80_device> m_maincpu;
	required_region_ptr<u8> m_p_chargen;
	optional_device<z80pio_device> m_pio_g;
	optional_device<z80pio_device> m_pio_s;
	required_device<centronics_device> m_centronics;
	required_device<fd1793_device> m_fdc;
	required_device<floppy_connector> m_floppy0;
	optional_device<floppy_connector> m_floppy1;
	optional_device<mc6845_device> m_crtc;
	required_device<beep_device> m_beep;
	required_memory_bank m_bankr;
	required_memory_bank m_bankw;
	required_memory_bank m_bank3;
};

#endif // MAME_INCLUDES_KAYPRO_H
