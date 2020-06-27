// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Exidy 440 hardware

*************************************************************************/
#ifndef MAME_INCLUDES_EXIDY440_H
#define MAME_INCLUDES_EXIDY440_H

#pragma once

#include "audio/exidy440.h"
#include "emupal.h"
#include "screen.h"

#define EXIDY440_MASTER_CLOCK       (XTAL(12'979'200))


class exidy440_state : public driver_device
{
public:
	exidy440_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_imageram(*this, "imageram"),
		m_spriteram(*this, "spriteram"),
		m_scanline(*this, "scanline"),
		m_maincpu(*this, "maincpu"),
		m_custom(*this, "440audio"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette")
	{ }

	DECLARE_READ_LINE_MEMBER(firq_beam_r);
	DECLARE_READ_LINE_MEMBER(firq_vblank_r);
	DECLARE_CUSTOM_INPUT_MEMBER(hitnmiss_button1_r);
	DECLARE_INPUT_CHANGED_MEMBER(coin_inserted);
	void init_showdown();
	void init_yukon();
	void init_exidy440();
	void init_claypign();
	void exidy440(machine_config &config);

protected:
	void bankram_w(offs_t offset, uint8_t data);
	uint8_t exidy440_input_port_3_r();
	uint8_t sound_command_ack_r();
	void sound_command_w(uint8_t data);
	void exidy440_input_port_3_w(uint8_t data);
	void exidy440_coin_counter_w(uint8_t data);
	uint8_t showdown_bank0_r(offs_t offset);
	uint8_t claypign_protection_r();
	uint8_t exidy440_videoram_r(offs_t offset);
	void exidy440_videoram_w(offs_t offset, uint8_t data);
	uint8_t exidy440_paletteram_r(offs_t offset);
	void exidy440_paletteram_w(offs_t offset, uint8_t data);
	uint8_t exidy440_horizontal_pos_r();
	uint8_t exidy440_vertical_pos_r();
	void exidy440_spriteram_w(offs_t offset, uint8_t data);
	void exidy440_control_w(offs_t offset, uint8_t data);
	void exidy440_interrupt_clear_w(uint8_t data);
	void draw_sprites(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect, int scroll_offset, int check_collision);
	void update_screen(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect,  int scroll_offset, int check_collision);
	uint32_t screen_update_exidy440(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(vblank_interrupt_w);
	TIMER_CALLBACK_MEMBER(delayed_sound_command_w);
	TIMER_CALLBACK_MEMBER(beam_firq_callback);
	TIMER_CALLBACK_MEMBER(collide_firq_callback);
	void exidy440_update_firq();
	void exidy440_bank_select(uint8_t bank);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	void exidy440_video(machine_config &config);
	void exidy440_map(address_map &map);

	required_shared_ptr<uint8_t> m_imageram;
	required_shared_ptr<uint8_t> m_spriteram;
	required_shared_ptr<uint8_t> m_scanline;

	required_device<cpu_device> m_maincpu;
	required_device<exidy440_sound_device> m_custom;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

private:
	uint8_t m_bank;
	const uint8_t *m_showdown_bank_data[2];
	int8_t m_showdown_bank_select;
	uint8_t m_showdown_bank_offset;
	uint8_t m_firq_vblank;
	uint8_t m_firq_beam;
	uint8_t m_latched_x;
	std::unique_ptr<uint8_t[]> m_local_videoram;
	std::unique_ptr<uint8_t[]> m_local_paletteram;
	uint8_t m_firq_enable;
	uint8_t m_firq_select;
	uint8_t m_palettebank_io;
	uint8_t m_palettebank_vis;
	emu_timer *m_beam_firq_timer;
	emu_timer *m_collide_firq_timer;
	uint8_t m_beam_firq_count;
};


class topsecex_state : public exidy440_state
{
public:
	using exidy440_state::exidy440_state;
	void init_topsecex();
	void topsecex(machine_config &config);

protected:
	void topsecex_video(machine_config &config);
	uint8_t topsecex_input_port_5_r();
	void topsecex_yscroll_w(uint8_t data);
	uint32_t screen_update_topsecex(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	virtual void video_start() override;

private:
	uint8_t m_topsecex_yscroll;
};

#endif // MAME_INCLUDES_EXIDY440_H
