// license:BSD-3-Clause
// copyright-holders:Luca Elia,Olivier Galibert,Paul Priest
/*************************************************************************

    Psikyo Games

*************************************************************************/
#ifndef MAME_INCLUDES_PSIKYO_H
#define MAME_INCLUDES_PSIKYO_H

#pragma once

#include "machine/gen_latch.h"
#include "video/bufsprite.h"
#include "sound/okim6295.h"
#include "emupal.h"
#include "screen.h"
#include "tilemap.h"

#include <algorithm>

class psikyo_state : public driver_device
{
public:
	psikyo_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_vram(*this, "vram_%u", 0U, (u8)32)
		, m_vregs(*this, "vregs")
		, m_bootleg_spritebuffer(*this, "boot_spritebuf")
		, m_spritelut(*this, "spritelut")
		, m_audiobank(*this, "audiobank")
		, m_okibank(*this, "okibank")
		, m_in_dsw(*this, "DSW")
		, m_in_p1_p2(*this, "P1_P2")
		, m_in_coin(*this, "COIN")
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_oki(*this, "oki")
		, m_soundlatch(*this, "soundlatch")
		, m_gfxdecode(*this, "gfxdecode")
		, m_screen(*this, "screen")
		, m_palette(*this, "palette")
		, m_spriteram(*this, "spriteram")
	{
		std::fill(std::begin(m_old_linescroll), std::end(m_old_linescroll), ~u32(0));
		std::fill(std::begin(m_old_tmapsize), std::end(m_old_tmapsize), ~u32(0));
		std::fill(std::begin(m_tmapsize), std::end(m_tmapsize), ~u32(0));
	}

	void sngkace(machine_config &config);
	void s1945n(machine_config &config);
	void gunbird(machine_config &config);
	void s1945(machine_config &config);
	void s1945bl(machine_config &config);

	void init_s1945a();
	void init_s1945j();
	void init_sngkace();
	void init_s1945();
	void init_s1945bl();
	void init_tengai();
	void init_gunbird();

	DECLARE_READ_LINE_MEMBER(z80_nmi_r);
	DECLARE_READ_LINE_MEMBER(mcu_status_r);

private:
	/* memory pointers */
	required_shared_ptr_array<u16, 2> m_vram;
	required_shared_ptr<u32> m_vregs;
	optional_shared_ptr<u32> m_bootleg_spritebuffer;

	required_region_ptr<u16> m_spritelut;
	optional_memory_bank m_audiobank;
	optional_memory_bank m_okibank;
	optional_ioport m_in_dsw;
	optional_ioport m_in_p1_p2;
	optional_ioport m_in_coin;

	/* video-related */
	struct sprite_t
	{
		u8 gfx;
		u32 code,color;
		bool flipx,flipy;
		s32 x,y;
		u32 zoomx,zoomy;
		u32 primask;
	};

	tilemap_t   *m_tilemap[2];
	u8          m_tilemap_bank[2];
	bool        m_ka302c_banking;
	u32         m_old_linescroll[2];
	u32         m_old_tmapsize[2];
	u32         m_tmapsize[2];
	std::unique_ptr<sprite_t[]> m_spritelist;
	struct sprite_t *m_sprite_ptr_pre;
	u16         m_sprite_ctrl;

	/* game-specific */
	// 1945 MCU
	int         m_mcu_status;
	u8          m_s1945_mcu_direction;
	u8          m_s1945_mcu_latch1;
	u8          m_s1945_mcu_latch2;
	u8          m_s1945_mcu_inlatch;
	u8          m_s1945_mcu_index;
	u8          m_s1945_mcu_latching;
	u8          m_s1945_mcu_mode;
	u8          m_s1945_mcu_control;
	u8          m_s1945_mcu_bctrl;
	const u8    *m_s1945_mcu_table;

	DECLARE_READ32_MEMBER(sngkace_input_r);
	DECLARE_READ32_MEMBER(gunbird_input_r);
	void s1945_mcu_data_w(uint8_t data);
	void s1945_mcu_control_w(uint8_t data);
	void s1945_mcu_direction_w(uint8_t data);
	void s1945_mcu_bctrl_w(uint8_t data);
	void s1945_mcu_command_w(uint8_t data);
	uint32_t s1945_mcu_data_r();
	uint8_t s1945_mcu_control_r();
	DECLARE_READ32_MEMBER(s1945_input_r);
	DECLARE_WRITE8_MEMBER(s1945bl_okibank_w);
	template<int Shift> DECLARE_WRITE8_MEMBER(sound_bankswitch_w);
	template<int Layer> u16 vram_r(offs_t offset);
	template<int Layer> void vram_w(offs_t offset, u16 data, u16 mem_mask);

	template<int Layer> TILE_GET_INFO_MEMBER(get_tile_info);
	template<int Layer> TILEMAP_MAPPER_MEMBER(tile_scan);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	DECLARE_VIDEO_START(sngkace);
	DECLARE_VIDEO_START(psikyo);
	u32 screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	u32 screen_update_bootleg(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank_bootleg);
	void switch_bgbanks(u8 tmap, u8 bank);
	void draw_sprites(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void get_sprites();
	void get_sprites_bootleg();
	void s1945_mcu_init();

	/* devices */
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_audiocpu;
	optional_device<okim6295_device> m_oki;
	optional_device<generic_latch_8_device> m_soundlatch;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<buffered_spriteram32_device> m_spriteram;

	void gunbird_map(address_map &map);
	void gunbird_sound_io_map(address_map &map);
	void gunbird_sound_map(address_map &map);
	void psikyo_bootleg_map(address_map &map);
	void psikyo_map(address_map &map);
	void s1945_map(address_map &map);
	void s1945_sound_io_map(address_map &map);
	void s1945bl_oki_map(address_map &map);
	void s1945n_map(address_map &map);
	void sngkace_map(address_map &map);
	void sngkace_sound_io_map(address_map &map);
	void sngkace_sound_map(address_map &map);
};

#endif // MAME_INCLUDES_PSIKYO_H
