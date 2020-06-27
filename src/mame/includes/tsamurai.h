// license:BSD-3-Clause
// copyright-holders:Phil Stroffolino
#ifndef MAME_INCLUDES_TSAMURAI_H
#define MAME_INCLUDES_TSAMURAI_H

#pragma once

#include "machine/gen_latch.h"
#include "emupal.h"
#include "tilemap.h"

class tsamurai_state : public driver_device
{
public:
	tsamurai_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_audio2(*this, "audio2"),
		m_audio3(*this, "audio3"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch"),
		m_videoram(*this, "videoram"),
		m_colorram(*this, "colorram"),
		m_bg_videoram(*this, "bg_videoram"),
		m_spriteram(*this, "spriteram")
	{ }

	void tsamurai(machine_config &config);
	void m660(machine_config &config);
	void vsgongf(machine_config &config);

	void init_the26thz();

private:
	// common
	DECLARE_WRITE_LINE_MEMBER(nmi_enable_w);
	DECLARE_WRITE_LINE_MEMBER(coin1_counter_w);
	DECLARE_WRITE_LINE_MEMBER(coin2_counter_w);
	DECLARE_WRITE_LINE_MEMBER(textbank1_w);
	void fg_videoram_w(offs_t offset, uint8_t data);

	// tsamurai and m660 specific
	void bg_videoram_w(offs_t offset, uint8_t data);
	void fg_colorram_w(offs_t offset, uint8_t data);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_w);
	void scrolly_w(uint8_t data);
	void scrollx_w(uint8_t data);
	void bgcolor_w(uint8_t data);
	uint8_t unknown_d806_r();
	uint8_t unknown_d900_r();
	uint8_t unknown_d938_r();
	void sound_command1_w(uint8_t data);
	void sound_command2_w(uint8_t data);
	uint8_t sound_command1_r();
	uint8_t sound_command2_r();

	// tsamurai specific
	uint8_t tsamurai_unknown_d803_r();

	// m660 specific
	DECLARE_WRITE_LINE_MEMBER(textbank2_w);
	uint8_t m660_unknown_d803_r();
	void m660_sound_command3_w(uint8_t data);
	uint8_t m660_sound_command3_r();

	// vsgongf specific
	void vsgongf_color_w(uint8_t data);
	void vsgongf_sound_nmi_enable_w(uint8_t data);
	uint8_t vsgongf_a006_r();
	uint8_t vsgongf_a100_r();
	void vsgongf_sound_command_w(uint8_t data);

	DECLARE_MACHINE_START(m660);
	DECLARE_MACHINE_START(tsamurai);
	DECLARE_MACHINE_START(vsgongf);
	DECLARE_VIDEO_START(m660);
	DECLARE_VIDEO_START(tsamurai);
	DECLARE_VIDEO_START(vsgongf);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_vsgongf(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_WRITE_LINE_MEMBER(vblank_irq);
	INTERRUPT_GEN_MEMBER(vsgongf_sound_interrupt);

	void m660_map(address_map &map);
	void main_map(address_map &map);
	void sound1_m660_map(address_map &map);
	void sound1_map(address_map &map);
	void sound2_m660_map(address_map &map);
	void sound2_map(address_map &map);
	void sound3_m660_io_map(address_map &map);
	void sound3_m660_map(address_map &map);
	void sound_vsgongf_map(address_map &map);
	void vsgongf_audio_io_map(address_map &map);
	void vsgongf_map(address_map &map);
	void z80_io_map(address_map &map);
	void z80_m660_io_map(address_map &map);

	virtual void machine_start() override;
	virtual void video_start() override;

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	optional_device<cpu_device> m_audio2;
	optional_device<cpu_device> m_audio3;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	optional_device<generic_latch_8_device> m_soundlatch; // vsgongf only

	required_shared_ptr<uint8_t> m_videoram;
	optional_shared_ptr<uint8_t> m_colorram;
	optional_shared_ptr<uint8_t> m_bg_videoram;
	required_shared_ptr<uint8_t> m_spriteram;

	tilemap_t *m_background;
	tilemap_t *m_foreground;

	//common
	int m_flicker;
	int m_textbank1;
	int m_nmi_enabled;

	// tsamurai and m660 specific
	int m_bgcolor;
	int m_sound_command1;
	int m_sound_command2;

	//m660 specific
	int m_textbank2;
	int m_sound_command3;

	//vsgongf specific
	int m_vsgongf_sound_nmi_enabled;
	int m_vsgongf_color;
	int m_key_count; //debug only

	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_vsgongf_tile_info);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect );
};

#endif // MAME_INCLUDES_TSAMURAI_H
