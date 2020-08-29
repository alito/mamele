// license:BSD-3-Clause
// copyright-holders:Frank Palazzolo, Ryan Holtz
#ifndef MAME_INCLUDES_STARCRUS_H
#define MAME_INCLUDES_STARCRUS_H

#pragma once

#include "emupal.h"
#include "machine/netlist.h"
#include "netlist/nl_setup.h"
#include "audio/nl_starcrus.h"

class starcrus_state : public driver_device
{
public:
	starcrus_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag) ,
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_led(*this, "led2"),
		m_explode(*this, "sound_nl:explode%u", 1U),
		m_launch(*this, "sound_nl:launch%u", 1U),
		m_engine(*this, "sound_nl:engine%u", 1U)
	{ }

	void starcrus(machine_config &config);

private:
	void s1_x_w(uint8_t data);
	void s1_y_w(uint8_t data);
	void s2_x_w(uint8_t data);
	void s2_y_w(uint8_t data);
	void p1_x_w(uint8_t data);
	void p1_y_w(uint8_t data);
	void p2_x_w(uint8_t data);
	void p2_y_w(uint8_t data);
	void ship_parm_1_w(uint8_t data);
	void ship_parm_2_w(uint8_t data);
	void proj_parm_1_w(uint8_t data);
	void proj_parm_2_w(uint8_t data);
	uint8_t coll_det_r();

	virtual void machine_start() override;
	virtual void video_start() override;

	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	output_finder<> m_led;

	std::unique_ptr<bitmap_ind16> m_ship1_vid;
	std::unique_ptr<bitmap_ind16> m_ship2_vid;
	std::unique_ptr<bitmap_ind16> m_proj1_vid;
	std::unique_ptr<bitmap_ind16> m_proj2_vid;

	int m_s1_x;
	int m_s1_y;
	int m_s2_x;
	int m_s2_y;
	int m_p1_x;
	int m_p1_y;
	int m_p2_x;
	int m_p2_y;

	int m_p1_sprite;
	int m_p2_sprite;
	int m_s1_sprite;
	int m_s2_sprite;

	int m_collision_reg;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	int collision_check_s1s2();
	int collision_check_p1p2();
	int collision_check_s1p1p2();
	int collision_check_s2p1p2();

	void starcrus_io_map(address_map &map);
	void starcrus_map(address_map &map);

	required_device_array<netlist_mame_logic_input_device, 2> m_explode;
	required_device_array<netlist_mame_logic_input_device, 2> m_launch;
	required_device_array<netlist_mame_logic_input_device, 2> m_engine;
};

#endif // MAME_INCLUDES_STARCRUS_H
