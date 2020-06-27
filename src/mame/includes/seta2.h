// license:BSD-3-Clause
// copyright-holders:Luca Elia, David Haywood
#ifndef MAME_INCLUDES_SETA2_H
#define MAME_INCLUDES_SETA2_H

#pragma once


#include "machine/tmp68301.h"
#include "machine/eepromser.h"
#include "machine/intelfsh.h"
#include "machine/ticket.h"
#include "machine/timer.h"
#include "machine/upd4992.h"
#include "sound/okim9810.h"
#include "sound/x1_010.h"
#include "emupal.h"
#include "screen.h"

class seta2_state : public driver_device
{
public:
	seta2_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_sub(*this, "sub"),
		m_gfxdecode(*this, "gfxdecode"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),

		m_oki(*this, "oki"),
		m_eeprom(*this, "eeprom"),
		m_flash(*this, "flash"),
		m_dispenser(*this, "dispenser"),

		m_x1_bank(*this, "x1_bank_%u", 1U),
		m_nvram(*this, "nvram"),
		m_spriteram(*this, "spriteram", 0),
		m_tileram(*this, "tileram", 0),
		m_vregs(*this, "vregs", 0),
		m_leds(*this, "led%u", 0U),
		m_lamps(*this, "lamp%u", 0U)
	{ }

	void seta2(machine_config &config);
	void seta2_32m(machine_config &config);
	void grdians(machine_config &config);
	void grdiansa(machine_config &config);
	void myangel(machine_config &config);
	void penbros(machine_config &config);
	void pzlbowl(machine_config &config);
	void myangel2(machine_config &config);
	void reelquak(machine_config &config);
	void ablastb(machine_config &config);
	void gundamex(machine_config &config);
	void telpacfl(machine_config &config);
	void samshoot(machine_config &config);
	void namcostr(machine_config &config);

	void init_namcostr();

protected:
	virtual void machine_start() override;
	virtual void video_start() override;

	void grdians_lockout_w(uint8_t data);

	uint16_t mj4simai_p1_r();
	uint16_t mj4simai_p2_r();

	uint16_t pzlbowl_protection_r(address_space &space);
	uint8_t pzlbowl_coins_r();
	void pzlbowl_coin_counter_w(uint8_t data);

	void reelquak_leds_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void reelquak_coin_w(uint8_t data);

	void samshoot_coin_w(uint8_t data);

	void telpacfl_lamp1_w(uint8_t data);
	void telpacfl_lamp2_w(uint8_t data);
	void telpacfl_lockout_w(uint8_t data);

	uint16_t gundamex_eeprom_r();
	void gundamex_eeprom_w(uint16_t data);

	void vregs_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t spriteram_r(offs_t offset);
	void spriteram_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	int calculate_global_xoffset(int nozoom_fixedpalette_fixedposition);
	int calculate_global_yoffset(int nozoom_fixedpalette_fixedposition);
	void draw_sprites_line(bitmap_ind16 &bitmap, const rectangle &cliprect, int scanline, int realscanline, int xoffset, uint32_t xzoom, bool xzoominverted);
	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(screen_vblank);

	void sound_bank_w(offs_t offset, uint8_t data);

	INTERRUPT_GEN_MEMBER(seta2_interrupt);
	INTERRUPT_GEN_MEMBER(samshoot_interrupt);

	void ablastb_map(address_map &map);
	void grdians_map(address_map &map);
	void gundamex_map(address_map &map);
	void mj4simai_map(address_map &map);
	void myangel2_map(address_map &map);
	void myangel_map(address_map &map);
	void namcostr_map(address_map &map);
	void penbros_base_map(address_map &map);
	void penbros_map(address_map &map);
	void pzlbowl_map(address_map &map);
	void reelquak_map(address_map &map);
	void samshoot_map(address_map &map);
	void telpacfl_map(address_map &map);
	void x1_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_sub;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	optional_device<okim9810_device> m_oki;
	optional_device<eeprom_serial_93cxx_device> m_eeprom;
	optional_device<intelfsh16_device> m_flash;
	optional_device<ticket_dispenser_device> m_dispenser;

	optional_memory_bank_array<8> m_x1_bank;
	optional_shared_ptr<uint16_t> m_nvram;
	optional_shared_ptr<uint16_t> m_spriteram;
	optional_shared_ptr<uint16_t> m_tileram;
	optional_shared_ptr<uint16_t> m_vregs;
	output_finder<7> m_leds;
	output_finder<11> m_lamps;

	int m_keyboard_row;
	std::unique_ptr<uint16_t[]> m_private_spriteram;

private:
	void drawgfx_line(bitmap_ind16 &bitmap, const rectangle &cliprect, int gfx, const uint8_t* const addr, const uint32_t realcolor, int flipx, int flipy, int base_sx, uint32_t xzoom, int shadow, int screenline, int line, int opaque);
	inline void get_tile(uint16_t* spriteram, int is_16x16, int x, int y, int page, int& code, int& attr, int& flipx, int& flipy, int& color);

	std::unique_ptr<uint32_t[]> m_realtilenumber;
	gfx_element *m_spritegfx;

	uint16_t m_rasterposition;
	uint16_t m_rasterenabled;
	TIMER_CALLBACK_MEMBER(raster_timer_done);
	emu_timer *m_raster_timer;
};


class mj4simai_state : public seta2_state
{
public:
	mj4simai_state(const machine_config &mconfig, device_type type, const char *tag)
		: seta2_state(mconfig, type, tag)
	{ }

protected:
	virtual void machine_start() override;
};


class funcube_state : public seta2_state
{
public:
	funcube_state(const machine_config &mconfig, device_type type, const char *tag)
		: seta2_state(mconfig, type, tag)
		, m_outputs(*this, "outputs")
		, m_funcube_leds(*this, "funcube_leds")
	{ }

	void funcube(machine_config &config);
	void funcube3(machine_config &config);
	void funcube2(machine_config &config);

	void init_funcube3();
	void init_funcube();
	void init_funcube2();

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	uint32_t nvram_r(offs_t offset);
	void nvram_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	uint32_t debug_r();
	uint16_t coins_r();
	void leds_w(uint16_t data);
	uint16_t outputs_r();
	void outputs_w(uint16_t data);
	uint16_t battery_r();

	TIMER_DEVICE_CALLBACK_MEMBER(funcube_interrupt);

	void funcube2_map(address_map &map);
	void funcube2_sub_io(address_map &map);
	void funcube_map(address_map &map);
	void funcube_sub_io(address_map &map);
	void funcube_sub_map(address_map &map);

	void funcube_debug_outputs();

	required_shared_ptr<uint16_t> m_outputs;
	required_shared_ptr<uint16_t> m_funcube_leds;
	uint64_t m_coin_start_cycles;
	uint8_t m_hopper_motor;
};


class staraudi_state : public seta2_state
{
public:
	staraudi_state(const machine_config &mconfig, device_type type, const char *tag) :
		seta2_state(mconfig, type, tag),
		m_rtc(*this, "rtc"),
		m_rgbram(*this, "rgbram", 0)
	{
	}
	static constexpr feature_type unemulated_features() { return feature::CAMERA | feature::PRINTER; }

	void staraudi(machine_config &config);

private:
	void camera_w(offs_t offset, uint8_t data, uint8_t mem_mask = ~0);
	void lamps1_w(offs_t offset, uint8_t data, uint8_t mem_mask = ~0);
	void lamps2_w(offs_t offset, uint8_t data, uint8_t mem_mask = ~0);
	uint16_t tileram_r(offs_t offset);
	void tileram_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	uint32_t staraudi_screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void staraudi_map(address_map &map);

	virtual void driver_start() override;

	void staraudi_debug_outputs();

	void draw_rgbram(bitmap_ind16 &bitmap);

	required_device<upd4992_device> m_rtc;
	required_shared_ptr<uint16_t> m_rgbram;

	uint8_t m_lamps1 = 0, m_lamps2 = 0, m_cam = 0;
};

#endif // MAME_INCLUDES_SETA2_H
