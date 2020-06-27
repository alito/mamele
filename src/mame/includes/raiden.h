// license:BSD-3-Clause
// copyright-holders:Bryan McPhail
/*******************************************************************************

    Seibu Raiden hardware

*******************************************************************************/
#ifndef MAME_INCLUDES_RAIDEN_H
#define MAME_INCLUDES_RAIDEN_H

#pragma once

#include "audio/seibu.h"
#include "video/bufsprite.h"
#include "emupal.h"
#include "tilemap.h"

class raiden_state : public driver_device, public seibu_sound_common
{
public:
	raiden_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_seibu_sound(*this, "seibu_sound"),
		m_subcpu(*this, "sub"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_spriteram(*this, "spriteram"),
		m_shared_ram(*this, "shared_ram"),
		m_videoram(*this, "videoram"),
		m_scroll_ram(*this, "scroll_ram"),
		m_back_data(*this, "back_data"),
		m_fore_data(*this, "fore_data")
	{ }

	void raidene(machine_config &config);
	void raiden(machine_config &config);
	void raidenkb(machine_config &config);
	void raidenu(machine_config &config);

	void init_raiden();

protected:
	required_device<cpu_device> m_maincpu;
	required_device<seibu_sound_device> m_seibu_sound;

	uint8_t m_bg_layer_enabled;
	uint8_t m_fg_layer_enabled;
	uint8_t m_tx_layer_enabled;
	uint8_t m_sp_layer_enabled;
	uint8_t m_flipscreen;

	virtual void video_start() override;

	uint32_t screen_update_common(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, uint16_t *scrollregs);

	void raiden_text_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	void common_video_start();

	required_device<cpu_device> m_subcpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<buffered_spriteram16_device> m_spriteram;

	required_shared_ptr<uint16_t> m_shared_ram;
	required_shared_ptr<uint16_t> m_videoram;
	optional_shared_ptr<uint16_t> m_scroll_ram;
	required_shared_ptr<uint16_t> m_back_data;
	required_shared_ptr<uint16_t> m_fore_data;

	tilemap_t *m_bg_layer;
	tilemap_t *m_fg_layer;
	tilemap_t *m_tx_layer;

	void raiden_background_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void raiden_foreground_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void raiden_control_w(uint8_t data);

	TILE_GET_INFO_MEMBER(get_back_tile_info);
	TILE_GET_INFO_MEMBER(get_fore_tile_info);
	TILE_GET_INFO_MEMBER(get_text_tile_info);

	uint32_t screen_update_raiden(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	DECLARE_WRITE_LINE_MEMBER(vblank_irq);

	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, int pri_mask);
	void common_decrypt();

	void main_map(address_map &map);
	void sub_map(address_map &map);
	void raiden_sound_map(address_map &map);
	void raiden_sound_decrypted_opcodes_map(address_map &map);
	void raidenu_main_map(address_map &map);
	void raidenu_sub_map(address_map &map);
	void sei80bu_encrypted_full_map(address_map &map);
};


class raidenb_state : public raiden_state
{
public:
	using raiden_state::raiden_state;

	void raidenb(machine_config &config);

protected:
	virtual void video_start() override;

private:
	uint16_t m_raidenb_scroll_ram[6];

	uint32_t screen_update_raidenb(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void raidenb_control_w(uint8_t data);
	void raidenb_layer_enable_w(uint16_t data);
	void raidenb_layer_scroll_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	void raidenb_main_map(address_map &map);
};

#endif // MAME_INCLUDES_RAIDEN_H
