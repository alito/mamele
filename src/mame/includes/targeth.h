// license:BSD-3-Clause
// copyright-holders:Manuel Abadia
#ifndef MAME_INCLUDES_TARGETH_H
#define MAME_INCLUDES_TARGETH_H

#pragma once

#include "machine/74259.h"
#include "emupal.h"
#include "screen.h"
#include "tilemap.h"

class targeth_state : public driver_device
{
public:
	targeth_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this,"maincpu"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_outlatch(*this, "outlatch"),
		m_videoram(*this, "videoram"),
		m_vregs(*this, "vregs"),
		m_spriteram(*this, "spriteram"),
		m_shareram(*this, "shareram"),
		m_okibank(*this, "okibank")
	{ }

	void targeth(machine_config &config);

private:
	void oki_bankswitch_w(uint8_t data);
	void output_latch_w(offs_t offset, uint16_t data);
	DECLARE_WRITE_LINE_MEMBER(coin1_counter_w);
	DECLARE_WRITE_LINE_MEMBER(coin2_counter_w);
	void shareram_w(offs_t offset, uint8_t data);
	uint8_t shareram_r(offs_t offset);

	void vram_w(offs_t offset, uint16_t data);

	template<int Layer> TILE_GET_INFO_MEMBER(get_tile_info);

	TIMER_CALLBACK_MEMBER(gun1_irq);
	TIMER_CALLBACK_MEMBER(gun2_irq);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void main_map(address_map &map);
	void mcu_hostmem_map(address_map &map);
	void oki_map(address_map &map);

	virtual void video_start() override;
	virtual void machine_start() override;

	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);

	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	required_device<ls259_device> m_outlatch;

	required_shared_ptr<uint16_t> m_videoram;
	required_shared_ptr<uint16_t> m_vregs;
	required_shared_ptr<uint16_t> m_spriteram;
	required_shared_ptr<uint16_t> m_shareram;

	required_memory_bank m_okibank;

	emu_timer       *m_gun_irq_timer[2];

	tilemap_t *m_pant[2];
};

#endif // MAME_INCLUDES_TARGETH_H
