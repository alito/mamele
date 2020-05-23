// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Atari Gauntlet hardware

*************************************************************************/
#ifndef MAME_INCLUDES_GAUNTLET_H
#define MAME_INCLUDES_GAUNTLET_H

#pragma once

#include "machine/atarigen.h"
#include "machine/74259.h"
#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "video/atarimo.h"
#include "sound/ym2151.h"
#include "sound/pokey.h"
#include "sound/tms5220.h"
#include "tilemap.h"

class gauntlet_state : public atarigen_state
{
public:
	gauntlet_state(const machine_config &mconfig, device_type type, const char *tag) :
		atarigen_state(mconfig, type, tag),
		m_audiocpu(*this, "audiocpu"),
		m_soundlatch(*this, "soundlatch"),
		m_mainlatch(*this, "mainlatch"),
		m_ym2151(*this, "ymsnd"),
		m_pokey(*this, "pokey"),
		m_tms5220(*this, "tms"),
		m_soundctl(*this, "soundctl"),
		m_playfield_tilemap(*this, "playfield"),
		m_alpha_tilemap(*this, "alpha"),
		m_xscroll(*this, "xscroll"),
		m_yscroll(*this, "yscroll"),
		m_mob(*this, "mob")
	{ }

	void init_gauntlet();
	void init_vindctr2();
	void vindctr2(machine_config &config);
	void gauntlet(machine_config &config);
	void gaunt2p(machine_config &config);
	void gauntlet2(machine_config &config);

protected:
	virtual void video_start() override;

private:
	void video_int_ack_w(uint16_t data = 0);
	TIMER_DEVICE_CALLBACK_MEMBER(scanline_update);
	uint8_t sound_irq_ack_r();
	void sound_irq_ack_w(uint8_t data);
	DECLARE_WRITE_LINE_MEMBER(sound_reset_w);
	uint8_t switch_6502_r();
	DECLARE_WRITE_LINE_MEMBER(speech_squeak_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_left_w);
	DECLARE_WRITE_LINE_MEMBER(coin_counter_right_w);
	void mixer_w(uint8_t data);
	void swap_memory(void *ptr1, void *ptr2, int bytes);
	void common_init(int vindctr2);
	TILE_GET_INFO_MEMBER(get_alpha_tile_info);
	TILE_GET_INFO_MEMBER(get_playfield_tile_info);
	uint32_t screen_update_gauntlet(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE16_MEMBER( gauntlet_xscroll_w );
	DECLARE_WRITE16_MEMBER( gauntlet_yscroll_w );

	void gauntlet_base(machine_config &config);
	void main_map(address_map &map);
	void sound_map(address_map &map);

	required_device<cpu_device> m_audiocpu;
	required_device<generic_latch_8_device> m_soundlatch;
	required_device<generic_latch_8_device> m_mainlatch;
	required_device<ym2151_device> m_ym2151;
	required_device<pokey_device> m_pokey;
	required_device<tms5220_device> m_tms5220;
	required_device<ls259_device> m_soundctl;

	required_device<tilemap_device> m_playfield_tilemap;
	required_device<tilemap_device> m_alpha_tilemap;
	required_shared_ptr<uint16_t> m_xscroll;
	required_shared_ptr<uint16_t> m_yscroll;
	required_device<atari_motion_objects_device> m_mob;

	uint16_t          m_sound_reset_val;
	uint8_t           m_vindctr2_screen_refresh;
	uint8_t           m_playfield_tile_bank;
	uint8_t           m_playfield_color_bank;

	static const atari_motion_objects_config s_mob_config;
};

#endif // MAME_INCLUDES_GAUNTLET_H
