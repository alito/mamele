// license:GPL-2.0+
// copyright-holders:Jarek Burczynski
#ifndef MAME_INCLUDES_TUBEP_H
#define MAME_INCLUDES_TUBEP_H

#pragma once

#include "cpu/m6800/m6800.h"
#include "machine/gen_latch.h"
#include "sound/msm5205.h"
#include "emupal.h"
#include "screen.h"

class tubep_state : public driver_device
{
public:
	tubep_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_soundcpu(*this, "soundcpu"),
		m_slave(*this, "slave"),
		m_mcu(*this, "mcu"),
		m_soundlatch(*this, "soundlatch"),
		m_msm(*this, "msm"),
		m_screen(*this, "screen"),
		m_textram(*this, "textram"),
		m_backgroundram(*this, "backgroundram"),
		m_sprite_colorsharedram(*this, "sprite_color"),
		m_rjammer_backgroundram(*this, "rjammer_bgram")
	{ }

	void tubepb(machine_config &config);
	void tubep(machine_config &config);
	void rjammer(machine_config &config);

private:
	enum
	{
		TIMER_TUBEP_SCANLINE,
		TIMER_RJAMMER_SCANLINE,
		TIMER_SPRITE
	};

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_soundcpu;
	required_device<cpu_device> m_slave;
	required_device<m6802_cpu_device> m_mcu;
	required_device<generic_latch_8_device> m_soundlatch;
	optional_device<msm5205_device> m_msm;
	required_device<screen_device> m_screen;
	required_shared_ptr<uint8_t> m_textram;
	optional_shared_ptr<uint8_t> m_backgroundram;
	required_shared_ptr<uint8_t> m_sprite_colorsharedram;
	optional_shared_ptr<uint8_t> m_rjammer_backgroundram;

	uint8_t m_ls74;
	uint8_t m_ls377;
	emu_timer *m_interrupt_timer;
	emu_timer *m_sprite_timer;
	int m_curr_scanline;
	std::unique_ptr<uint8_t[]> m_spritemap;
	uint8_t m_prom2[32];
	uint32_t m_romD_addr;
	uint32_t m_romEF_addr;
	uint32_t m_E16_add_b;
	uint32_t m_HINV;
	uint32_t m_VINV;
	uint32_t m_XSize;
	uint32_t m_YSize;
	uint32_t m_mark_1;
	uint32_t m_mark_2;
	uint32_t m_colorram_addr_hi;
	uint32_t m_ls273_g6;
	uint32_t m_ls273_j6;
	uint32_t m_romHI_addr_mid;
	uint32_t m_romHI_addr_msb;
	uint8_t m_DISP;
	uint8_t m_background_romsel;
	uint8_t m_color_A4;
	uint8_t m_ls175_b7;
	uint8_t m_ls175_e8;
	uint8_t m_ls377_data;
	uint32_t m_page;
	DECLARE_WRITE_LINE_MEMBER(coin1_counter_w);
	DECLARE_WRITE_LINE_MEMBER(coin2_counter_w);
	void main_cpu_irq_line_clear_w(uint8_t data);
	void second_cpu_irq_line_clear_w(uint8_t data);
	uint8_t tubep_soundlatch_r();
	uint8_t tubep_sound_irq_ack();
	void rjammer_voice_input_w(uint8_t data);
	void rjammer_voice_intensity_control_w(uint8_t data);
	void tubep_textram_w(offs_t offset, uint8_t data);
	DECLARE_WRITE_LINE_MEMBER(screen_flip_w);
	DECLARE_WRITE_LINE_MEMBER(background_romselect_w);
	DECLARE_WRITE_LINE_MEMBER(colorproms_A4_line_w);
	void tubep_background_a000_w(uint8_t data);
	void tubep_background_c000_w(uint8_t data);
	void tubep_sprite_control_w(offs_t offset, uint8_t data);
	void rjammer_background_LS377_w(uint8_t data);
	void rjammer_background_page_w(uint8_t data);
	void rjammer_voice_startstop_w(uint8_t data);
	void rjammer_voice_frequency_select_w(uint8_t data);
	void ay8910_portA_0_w(uint8_t data);
	void ay8910_portB_0_w(uint8_t data);
	void ay8910_portA_1_w(uint8_t data);
	void ay8910_portB_1_w(uint8_t data);
	void ay8910_portA_2_w(uint8_t data);
	void ay8910_portB_2_w(uint8_t data);
	DECLARE_MACHINE_START(tubep);
	DECLARE_MACHINE_RESET(tubep);
	virtual void video_start() override;
	virtual void video_reset() override;
	void tubep_palette(palette_device &palette);
	DECLARE_MACHINE_START(rjammer);
	DECLARE_MACHINE_RESET(rjammer);
	void rjammer_palette(palette_device &palette) const;
	uint32_t screen_update_tubep(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_rjammer(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(tubep_scanline_callback);
	TIMER_CALLBACK_MEMBER(rjammer_scanline_callback);
	void draw_sprite();
	void tubep_vblank_end();
	void tubep_setup_save_state();
	DECLARE_WRITE_LINE_MEMBER(rjammer_adpcm_vck);

	void nsc_map(address_map &map);
	void rjammer_main_map(address_map &map);
	void rjammer_main_portmap(address_map &map);
	void rjammer_second_map(address_map &map);
	void rjammer_second_portmap(address_map &map);
	void rjammer_sound_map(address_map &map);
	void rjammer_sound_portmap(address_map &map);
	void tubep_main_map(address_map &map);
	void tubep_main_portmap(address_map &map);
	void tubep_second_map(address_map &map);
	void tubep_second_portmap(address_map &map);
	void tubep_sound_map(address_map &map);
	void tubep_sound_portmap(address_map &map);

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;
};

#endif // MAME_INCLUDES_TUBEP_H
