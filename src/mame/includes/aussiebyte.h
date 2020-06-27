// license:BSD-3-Clause
// copyright-holders:Robbbert
#ifndef MAME_INCLUDES_AUSSIEBYTE_H
#define MAME_INCLUDES_AUSSIEBYTE_H

#pragma once

/***********************************************************

    Includes

************************************************************/
#include "bus/centronics/ctronics.h"
#include "bus/rs232/rs232.h"

#include "cpu/z80/z80.h"
#include "machine/z80daisy.h"

#include "imagedev/floppy.h"
#include "machine/msm5832.h"
#include "machine/wd_fdc.h"
#include "machine/z80ctc.h"
#include "machine/z80sio.h"
#include "machine/z80dma.h"
#include "machine/z80pio.h"

#include "sound/spkrdev.h"
#include "sound/votrax.h"

#include "video/mc6845.h"

#include "imagedev/snapquik.h"

#include "emupal.h"



/***********************************************************

    Class

************************************************************/
class aussiebyte_state : public driver_device
{
public:
	aussiebyte_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_palette(*this, "palette")
		, m_maincpu(*this, "maincpu")
		, m_p_chargen(*this, "chargen")
		, m_ctc(*this, "ctc")
		, m_dma(*this, "dma")
		, m_pio1(*this, "pio1")
		, m_pio2(*this, "pio2")
		, m_centronics(*this, "centronics")
		, m_rs232(*this, "rs232")
		, m_fdc(*this, "fdc")
		, m_floppy0(*this, "fdc:0")
		, m_floppy1(*this, "fdc:1")
		, m_crtc(*this, "crtc")
		, m_speaker(*this, "speaker")
		, m_votrax(*this, "votrax")
		, m_rtc(*this, "rtc")
	{ }

	void aussiebyte(machine_config &config);

	DECLARE_QUICKLOAD_LOAD_MEMBER(quickload_cb);

private:
	u8 memory_read_byte(offs_t offset);
	void memory_write_byte(offs_t offset, u8 data);
	u8 io_read_byte(offs_t offset);
	void io_write_byte(offs_t offset, u8 data);
	void port15_w(u8 data);
	void port16_w(u8 data);
	void port17_w(u8 data);
	void port18_w(u8 data);
	u8 port19_r();
	void port1a_w(u8 data);
	void port1b_w(u8 data);
	void port1c_w(u8 data);
	void port20_w(u8 data);
	u8 port28_r();
	u8 port33_r();
	void port34_w(u8 data);
	void port35_w(u8 data);
	u8 port36_r();
	u8 port37_r();
	u8 rtc_r(offs_t offset);
	void rtc_w(offs_t offset, u8 data);
	DECLARE_WRITE_LINE_MEMBER(fdc_intrq_w);
	DECLARE_WRITE_LINE_MEMBER(fdc_drq_w);
	DECLARE_WRITE_LINE_MEMBER(busreq_w);
	DECLARE_WRITE_LINE_MEMBER(sio1_rdya_w);
	DECLARE_WRITE_LINE_MEMBER(sio1_rdyb_w);
	DECLARE_WRITE_LINE_MEMBER(sio2_rdya_w);
	DECLARE_WRITE_LINE_MEMBER(sio2_rdyb_w);
	void address_w(u8 data);
	void register_w(u8 data);
	MC6845_UPDATE_ROW(crtc_update_row);
	MC6845_ON_UPDATE_ADDR_CHANGED(crtc_update_addr);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	void io_map(address_map &map);
	void mem_map(address_map &map);

	u8 crt8002(u8 ac_ra, u8 ac_chr, u8 ac_attr, u16 ac_cnt, bool ac_curs);
	bool m_port15; // rom switched in (0), out (1)
	u8 m_port17;
	u8 m_port17_rdy;
	u8 m_port19;
	u8 m_port1a; // bank to switch to when write to port 15 happens
	u8 m_port28;
	u8 m_port34;
	u8 m_port35; // byte to be written to vram or aram
	u8 m_video_index;
	u16 m_cnt;
	u16 m_alpha_address;
	u16 m_graph_address;
	bool m_centronics_busy;
	std::unique_ptr<u8[]> m_vram; // video ram, 64k dynamic
	std::unique_ptr<u8[]> m_aram; // attribute ram, 2k static
	std::unique_ptr<u8[]> m_ram;  // main ram, 256k dynamic
	required_device<palette_device> m_palette;
	required_device<z80_device> m_maincpu;
	required_region_ptr<u8> m_p_chargen;
	required_device<z80ctc_device> m_ctc;
	required_device<z80dma_device> m_dma;
	required_device<z80pio_device> m_pio1;
	required_device<z80pio_device> m_pio2;
	required_device<centronics_device> m_centronics;
	required_device<rs232_port_device> m_rs232;
	required_device<wd2797_device> m_fdc;
	required_device<floppy_connector> m_floppy0;
	optional_device<floppy_connector> m_floppy1;
	required_device<mc6845_device> m_crtc;
	required_device<speaker_sound_device> m_speaker;
	required_device<votrax_sc01_device> m_votrax;
	required_device<msm5832_device> m_rtc;
};

#endif // MAME_INCLUDES_AUSSIEBYTE_H
