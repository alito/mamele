// license:BSD-3-Clause
// copyright-holders:Ernesto Corvi
/***************************************************************************

    Kyugo hardware games

***************************************************************************/
#ifndef MAME_INCLUDES_KYUGO_H
#define MAME_INCLUDES_KYUGO_H

#pragma once

#include "emupal.h"
#include "tilemap.h"

class kyugo_state : public driver_device
{
public:
	kyugo_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_fgvideoram(*this, "fgvideoram"),
		m_bgvideoram(*this, "bgvideoram"),
		m_bgattribram(*this, "bgattribram"),
		m_spriteram(*this, "spriteram_%u", 1U),
		m_shared_ram(*this, "shared_ram"),
		m_maincpu(*this, "maincpu"),
		m_subcpu(*this, "sub"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette")
	{ }

	void kyugo_base(machine_config &config);
	void repulse(machine_config &config);
	void flashgala(machine_config &config);
	void srdmissn(machine_config &config);
	void legend(machine_config &config);
	void gyrodine(machine_config &config);

	void init_srdmissn();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

private:
	void nmi_mask_w(int state);
	void coin_counter_w(offs_t offset, uint8_t data);
	void fgvideoram_w(offs_t offset, uint8_t data);
	void bgvideoram_w(offs_t offset, uint8_t data);
	void bgattribram_w(offs_t offset, uint8_t data);
	uint8_t spriteram_2_r(offs_t offset);
	void scroll_x_lo_w(uint8_t data);
	void gfxctrl_w(uint8_t data);
	void scroll_y_w(uint8_t data);
	void flipscreen_w(int state);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(vblank_irq);

	void flashgala_sub_map(address_map &map);
	void flashgala_sub_portmap(address_map &map);
	void gyrodine_main_map(address_map &map);
	void gyrodine_sub_map(address_map &map);
	void gyrodine_sub_portmap(address_map &map);
	void kyugo_main_map(address_map &map);
	void kyugo_main_portmap(address_map &map);
	void legend_sub_map(address_map &map);
	void repulse_sub_map(address_map &map);
	void repulse_sub_portmap(address_map &map);
	void srdmissn_sub_map(address_map &map);
	void srdmissn_sub_portmap(address_map &map);

	// memory pointers
	required_shared_ptr<uint8_t> m_fgvideoram;
	required_shared_ptr<uint8_t> m_bgvideoram;
	required_shared_ptr<uint8_t> m_bgattribram;
	required_shared_ptr_array<uint8_t, 2> m_spriteram;
	required_shared_ptr<uint8_t> m_shared_ram;

	uint8_t m_nmi_mask;

	// video-related
	tilemap_t     *m_bg_tilemap;
	tilemap_t     *m_fg_tilemap;
	uint8_t       m_scroll_x_lo;
	uint8_t       m_scroll_x_hi;
	uint8_t       m_scroll_y;
	uint8_t       m_bgpalbank;
	uint8_t       m_fgcolor;
	const uint8_t *m_color_codes;

	// devices
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_subcpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;

	TILE_GET_INFO_MEMBER(get_fg_tile_info);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
};

#endif // MAME_INCLUDES_KYUGO_H
