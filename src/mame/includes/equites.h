// license:BSD-3-Clause
// copyright-holders:Acho A. Tang, Nicola Salmoria
/*************************************************************************

    Equites, Splendor Blast driver

*************************************************************************/
#ifndef MAME_INCLUDES_EQUITES_H
#define MAME_INCLUDES_EQUITES_H

#pragma once

#include "machine/74259.h"
#include "machine/alpha8201.h"
#include "machine/timer.h"
#include "emupal.h"
#include "screen.h"
#include "tilemap.h"


class equites_state : public driver_device
{
public:
	equites_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_bg_videoram(*this, "bg_videoram"),
		m_spriteram(*this, "spriteram"),
		m_spriteram_2(*this, "spriteram_2"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_screen(*this, "screen"),
		m_alpha_8201(*this, "alpha_8201"),
		m_mainlatch(*this, "mainlatch")
	{ }

	/* memory pointers */
	required_shared_ptr<uint16_t> m_bg_videoram;
	std::unique_ptr<uint8_t[]> m_fg_videoram;    // 8bits
	required_shared_ptr<uint16_t> m_spriteram;
	optional_shared_ptr<uint16_t> m_spriteram_2;

	/* video-related */
	tilemap_t *m_fg_tilemap;
	tilemap_t *m_bg_tilemap;
	uint8_t     m_bgcolor;

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<screen_device> m_screen;
	required_device<alpha_8201_device> m_alpha_8201;
	required_device<ls259_device> m_mainlatch;

	DECLARE_READ16_MEMBER(equites_spriteram_kludge_r);
	DECLARE_WRITE8_MEMBER(mainlatch_w);
	DECLARE_READ8_MEMBER(equites_fg_videoram_r);
	DECLARE_WRITE8_MEMBER(equites_fg_videoram_w);
	DECLARE_WRITE16_MEMBER(equites_bg_videoram_w);
	DECLARE_WRITE8_MEMBER(equites_bgcolor_w);
	DECLARE_WRITE16_MEMBER(equites_scrollreg_w);
	DECLARE_WRITE_LINE_MEMBER(flip_screen_w);
	void init_equites();
	TILE_GET_INFO_MEMBER(equites_fg_info);
	TILE_GET_INFO_MEMBER(equites_bg_info);
	DECLARE_VIDEO_START(equites);
	void equites_palette(palette_device &palette) const;
	uint32_t screen_update_equites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_DEVICE_CALLBACK_MEMBER(equites_scanline);
	void equites_draw_sprites_block(bitmap_ind16 &bitmap, const rectangle &cliprect, int start, int end);
	void equites_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void unpack_block(const char *region, int offset, int size);
	void unpack_region(const char *region);
	void equites(machine_config &config);
	void bngotime(machine_config &config);

protected:
	virtual void machine_start() override;
	void bngotime_map(address_map &map);
	void equites_map(address_map &map);
	void equites_common_map(address_map &map);
};

class gekisou_state : public equites_state
{
public:
	using equites_state::equites_state;
	DECLARE_READ_LINE_MEMBER(gekisou_unknown_bit_r);
	void gekisou(machine_config &config);

protected:
	virtual void machine_start() override;
	void gekisou_map(address_map &map);
	DECLARE_WRITE16_MEMBER(gekisou_unknown_bit_w);

private:
	int m_gekisou_unknown_bit;
};


class splndrbt_state : public equites_state
{
public:
	using equites_state::equites_state;
	void init_splndrbt();
	void splndrbt(machine_config &config);

protected:
	virtual void machine_start() override;
	void splndrbt_map(address_map &map);
	DECLARE_WRITE_LINE_MEMBER(splndrbt_selchar_w);
	DECLARE_WRITE16_MEMBER(splndrbt_bg_scrollx_w);
	DECLARE_WRITE16_MEMBER(splndrbt_bg_scrolly_w);
	TILE_GET_INFO_MEMBER(splndrbt_fg_info);
	TILE_GET_INFO_MEMBER(splndrbt_bg_info);
	DECLARE_VIDEO_START(splndrbt);
	void splndrbt_palette(palette_device &palette) const;
	uint32_t screen_update_splndrbt(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_DEVICE_CALLBACK_MEMBER(splndrbt_scanline);
	void splndrbt_draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void splndrbt_copy_bg(bitmap_ind16 &dst_bitmap, const rectangle &cliprect);

private:
	int       m_fg_char_bank;
	uint16_t  m_splndrbt_bg_scrollx;
	uint16_t  m_splndrbt_bg_scrolly;
};

#endif // MAME_INCLUDES_EQUITES_H
