// license:BSD-3-Clause
// copyright-holders:David Haywood, Phil Stroffolino, Carlos A. Lozano
#ifndef MAME_INCLUDES_ARMEDF_H
#define MAME_INCLUDES_ARMEDF_H

#pragma once

#include "machine/nb1414m4.h"
#include "machine/gen_latch.h"
#include "video/bufsprite.h"
#include "screen.h"
#include "emupal.h"
#include "tilemap.h"

class armedf_state : public driver_device
{
public:
	armedf_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_extra(*this, "extra")
		, m_nb1414m4(*this, "nb1414m4")
		, m_screen(*this, "screen")
		, m_gfxdecode(*this, "gfxdecode")
		, m_palette(*this, "palette")
		, m_spriteram(*this, "spriteram")
		, m_soundlatch(*this, "soundlatch")
		, m_text_videoram(*this, "text_videoram", 0)
		, m_spr_pal_clut(*this, "spr_pal_clut")
		, m_fg_videoram(*this, "fg_videoram")
		, m_bg_videoram(*this, "bg_videoram")
	{ }

	void init_cclimbr2();
	void init_armedf();
	void init_legion();
	void init_terraf();
	void init_legionjb();
	void init_kozure();

	void terraf(machine_config &config);
	void terrafb(machine_config &config);
	void legion_common(machine_config &config);
	void legion(machine_config &config);
	void legionjb(machine_config &config);
	void legionjb2(machine_config &config);
	void cclimbr2(machine_config &config);
	void terrafjb(machine_config &config);
	void armedf(machine_config &config);
	void kozure(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	void video_config(machine_config &config, int hchar_start, int vstart, int vend);
	void sound_config(machine_config &config);

	// devices
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_extra;
	optional_device<nb1414m4_device> m_nb1414m4;
	required_device<screen_device> m_screen;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<buffered_spriteram16_device> m_spriteram;
	required_device<generic_latch_8_device> m_soundlatch;

	// memory pointers
	required_shared_ptr<u8> m_text_videoram;
	required_shared_ptr<u16> m_spr_pal_clut;
	required_shared_ptr<u16> m_fg_videoram;
	required_shared_ptr<u16> m_bg_videoram;
	u16 m_legion_cmd[4]; // legionjb only!

	// video-related
	tilemap_t  *m_bg_tilemap;
	tilemap_t  *m_fg_tilemap;
	tilemap_t  *m_tx_tilemap;
	u16   m_scroll_msb;
	u16   m_vreg;
	u16   m_fg_scrollx;
	u16   m_fg_scrolly;
	u16   m_bg_scrollx;
	u16   m_bg_scrolly;
	int   m_scroll_type;
	int   m_sprite_offy;
	int   m_old_mcu_mode;
	int   m_waiting_msb;

	// read/write handlers
	void terraf_io_w(offs_t offset, u16 data, u16 mem_mask);
	void terrafjb_io_w(offs_t offset, u16 data, u16 mem_mask);
	void armedf_io_w(offs_t offset, u16 data, u16 mem_mask);
	void sound_command_w(u8 data);
	u8 soundlatch_clear_r();
	void irq_lv1_ack_w(u16 data);
	void irq_lv2_ack_w(u16 data);

	// video handlers
	void legionjb_fg_scroll_w(offs_t offset, u8 data);
	void blitter_txram_w(offs_t offset, u8 data);
	void terrafjb_fg_scrollx_w(u8 data);
	void terrafjb_fg_scrolly_w(u8 data);
	void terrafjb_fg_scroll_msb_w(u8 data);
	u8 text_videoram_r(offs_t offset);
	void text_videoram_w(offs_t offset, u8 data);
	void fg_videoram_w(offs_t offset, u16 data, u16 mem_mask);
	void bg_videoram_w(offs_t offset, u16 data, u16 mem_mask);
	void terrafb_fg_scrolly_w(u8 data);
	void terrafb_fg_scrollx_w(u8 data);
	void terrafb_fg_scroll_msb_arm_w(u8 data);
	void fg_scrollx_w(offs_t offset, u16 data, u16 mem_mask);
	void fg_scrolly_w(offs_t offset, u16 data, u16 mem_mask);
	void bg_scrollx_w(offs_t offset, u16 data, u16 mem_mask);
	void bg_scrolly_w(offs_t offset, u16 data, u16 mem_mask);

	TILEMAP_MAPPER_MEMBER(armedf_scan_type1);
	TILEMAP_MAPPER_MEMBER(armedf_scan_type2);
	TILEMAP_MAPPER_MEMBER(armedf_scan_type3);

	TILE_GET_INFO_MEMBER(get_nb1414m4_tx_tile_info);
	TILE_GET_INFO_MEMBER(get_armedf_tx_tile_info);
	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);

	DECLARE_VIDEO_START(terraf);
	DECLARE_VIDEO_START(armedf);

	u32 screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect, bitmap_ind8 &primap);
	void armedf_drawgfx(bitmap_ind16 &dest_bmp, const rectangle &clip, gfx_element *gfx,
						u32 code, u32 color, u32 clut, int flipx, int flipy, int offsx, int offsy,
						bitmap_ind8 &primap, u32 pmask, int transparent_color);
	void common_map(address_map &map);
	void armedf_map(address_map &map);
	void cclimbr2_map(address_map &map);
	void cclimbr2_soundmap(address_map &map);
	void kozure_map(address_map &map);
	void legion_common_map(address_map &map);
	void legion_map(address_map &map);
	void legionjb_map(address_map &map);
	void legionjb2_map(address_map &map);
	void sound_3526_portmap(address_map &map);
	void sound_map(address_map &map);
	void sound_portmap(address_map &map);
	void terraf_common_map(address_map &map);
	void terraf_map(address_map &map);
	void terrafb_map(address_map &map);
	void terrafjb_map(address_map &map);
	void terrafjb_extraz80_map(address_map &map);
	void terrafjb_extraz80_portmap(address_map &map);
};

class bigfghtr_state : public armedf_state
{
public:
	bigfghtr_state(const machine_config &mconfig, device_type type, const char *tag)
		: armedf_state(mconfig, type, tag)
		, m_mcu(*this, "mcu")
		, m_sharedram(*this, "sharedram")
	{ }

	void bigfghtr(machine_config &config);

private:
	required_device<cpu_device> m_mcu;
	required_shared_ptr<u8> m_sharedram;

	// mcu interfaces
	u16 latch_r();
	void main_sharedram_w(offs_t offset, u8 data);
	u8 main_sharedram_r(offs_t offset);
	void mcu_spritelist_w(offs_t offset, u8 data);

	void bigfghtr_map(address_map &map);
	void bigfghtr_mcu_io_map(address_map &map);
	void bigfghtr_mcu_map(address_map &map);
};

#endif // MAME_INCLUDES_ARMEDF_H
