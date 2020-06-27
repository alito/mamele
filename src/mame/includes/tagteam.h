// license:BSD-3-Clause
// copyright-holders:Steve Ellenoff, Brad Oliver
#ifndef MAME_INCLUDES_TAGTEAM_H
#define MAME_INCLUDES_TAGTEAM_H

#pragma once

#include "machine/gen_latch.h"
#include "emupal.h"
#include "tilemap.h"

class tagteam_state : public driver_device
{
public:
	tagteam_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch"),
		m_videoram(*this, "videoram"),
		m_colorram(*this, "colorram")
	{ }

	void tagteam(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(coin_inserted);

protected:
	virtual void machine_start() override;
	virtual void video_start() override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;

	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_colorram;

	int m_palettebank;
	tilemap_t *m_bg_tilemap;
	uint8_t m_sound_nmi_mask;

	void irq_clear_w(uint8_t data);
	void sound_nmi_mask_w(uint8_t data);
	void videoram_w(offs_t offset, uint8_t data);
	void colorram_w(offs_t offset, uint8_t data);
	uint8_t mirrorvideoram_r(offs_t offset);
	uint8_t mirrorcolorram_r(offs_t offset);
	void mirrorvideoram_w(offs_t offset, uint8_t data);
	void mirrorcolorram_w(offs_t offset, uint8_t data);
	void control_w(uint8_t data);
	void flipscreen_w(uint8_t data);

	INTERRUPT_GEN_MEMBER(sound_timer_irq);

	TILE_GET_INFO_MEMBER(get_bg_tile_info);

	void tagteam_palette(palette_device &palette) const;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void main_map(address_map &map);
	void sound_map(address_map &map);
};

#endif // MAME_INCLUDES_TAGTEAM_H
