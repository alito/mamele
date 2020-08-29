// license:BSD-3-Clause
// copyright-holders:Aaron Giles,Ernesto Corvi
/*************************************************************************

    Driver for Midway Wolf-unit games.

**************************************************************************/
#ifndef MAME_INCLUDES_MIDWUNIT_H
#define MAME_INCLUDES_MIDWUNIT_H

#pragma once

#include "audio/dcs.h"
#include "cpu/tms34010/tms34010.h"
#include "machine/midwayic.h"
#include "video/midtunit.h"
#include "emupal.h"

class midwunit_state : public driver_device
{
public:
	midwunit_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_video(*this, "video")
		, m_dcs(*this, "dcs")
		, m_palette(*this, "palette")
		, m_gfxrom(*this, "gfxrom")
		, m_midway_serial_pic(*this, "serial_security_sim")
		, m_midway_serial_pic_emu(*this, "serial_security")
		, m_nvram(*this, "nvram")
		, m_mainram(*this, "mainram")
		, m_ports(*this, { { "IN0", "IN1", "DSW", "IN2" } })
	{ }

	void wunit(machine_config &config);
	void wunit_picemu(machine_config &config);
	void wunit_picsim(machine_config &config);

	void init_mk3r10();
	void init_nbahangt();
	void init_wwfmania();
	void init_umk3();
	void init_mk3();
	void init_openice();
	void init_rmpgwt();
	void init_umk3r11();
	void init_mk3r20();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	void midwunit_cmos_enable_w(uint16_t data);
	void midwunit_cmos_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t midwunit_cmos_r(offs_t offset);
	void midwunit_io_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t midwunit_io_r(offs_t offset);
	uint16_t midwunit_security_r();
	void midwunit_security_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t midwunit_sound_r();
	uint16_t midwunit_sound_state_r();
	void midwunit_sound_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void umk3_palette_hack_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void wwfmania_io_0_w(uint16_t data);

	void init_mk3_common();
	void main_map(address_map &map);

	required_device<tms340x0_device> m_maincpu;
	required_device<midtunit_video_device> m_video;
	required_device<dcs_audio_device> m_dcs;
	required_device<palette_device> m_palette;
	required_memory_region m_gfxrom;

	optional_device<midway_serial_pic_device> m_midway_serial_pic;
	optional_device<midway_serial_pic_emu_device> m_midway_serial_pic_emu;
	required_shared_ptr<uint16_t> m_nvram;
	required_shared_ptr<uint16_t> m_mainram;
	required_ioport_array<4> m_ports;

	uint8_t m_cmos_write_enable;
	uint16_t m_iodata[8];
	uint8_t m_ioshuffle[16];
	uint8_t m_uart[8];
	uint8_t m_security_bits;
	uint16_t *m_umk3_palette;
};

#endif // MAME_INCLUDES_MIDWUNIT_H
