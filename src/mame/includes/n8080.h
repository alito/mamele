// license:BSD-3-Clause
// copyright-holders:Pierpaolo Prazzoli
#ifndef MAME_INCLUDES_N8080_H
#define MAME_INCLUDES_N8080_H

#pragma once

#include "cpu/i8085/i8085.h"
#include "cpu/mcs48/mcs48.h"
#include "machine/timer.h"
#include "sound/dac.h"
#include "sound/sn76477.h"
#include "emupal.h"
#include "screen.h"

class n8080_state : public driver_device
{
public:
	n8080_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_prom(*this, "proms"),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_n8080_dac(*this, "n8080_dac"),
		m_sn(*this, "snsnd"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette")
	{ }

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	virtual void sound_pins_changed();
	virtual void update_SN76477_status();
	virtual void delayed_sound_1(int data);
	virtual void delayed_sound_2(int data);

	/* memory pointers */
	required_shared_ptr<uint8_t> m_videoram;
	optional_memory_region m_prom;

	/* video-related */
	int m_sheriff_color_mode;
	int m_sheriff_color_data;

	/* sound-related */
	emu_timer* m_sound_timer[3];
	uint16_t m_prev_sound_pins;
	uint16_t m_curr_sound_pins;
	int m_mono_flop[3];
	uint8_t m_prev_snd_data;

	/* other */
	unsigned m_shift_data;
	unsigned m_shift_bits;
	int m_inte;

	/* devices */
	required_device<i8080a_cpu_device> m_maincpu;
	required_device<i8035_device> m_audiocpu;
	optional_device<dac_bit_interface> m_n8080_dac;
	optional_device<sn76477_device> m_sn;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;

	void n8080_shift_bits_w(uint8_t data);
	void n8080_shift_data_w(uint8_t data);
	uint8_t n8080_shift_r();
	void n8080_video_control_w(uint8_t data);
	void n8080_sound_1_w(uint8_t data);
	void n8080_sound_2_w(uint8_t data);
	uint8_t n8080_8035_p1_r();
	DECLARE_READ_LINE_MEMBER(n8080_8035_t0_r);
	DECLARE_READ_LINE_MEMBER(n8080_8035_t1_r);
	void n8080_dac_w(uint8_t data);
	DECLARE_WRITE_LINE_MEMBER(n8080_inte_callback);
	void n8080_status_callback(uint8_t data);
	void n8080_palette(palette_device &palette) const;
	TIMER_DEVICE_CALLBACK_MEMBER(rst1_tick);
	TIMER_DEVICE_CALLBACK_MEMBER(rst2_tick);
	void start_mono_flop( int n, const attotime &expire );
	void stop_mono_flop( int n );
	TIMER_CALLBACK_MEMBER( stop_mono_flop_callback );
	TIMER_CALLBACK_MEMBER( delayed_sound_1_callback );
	TIMER_CALLBACK_MEMBER( delayed_sound_2_callback );

	void main_cpu_map(address_map &map);
	void main_io_map(address_map &map);
	void n8080_sound_cpu_map(address_map &map);
};

class spacefev_state : public n8080_state
{
public:
	spacefev_state(const machine_config &mconfig, device_type type, const char *tag) :
		n8080_state(mconfig, type, tag),
		m_video_conf(*this, "VIDEO")
	{ }

	void spacefev(machine_config &config);

protected:
	virtual void machine_reset() override;
	virtual void sound_start() override;
	virtual void sound_reset() override;
	virtual void video_start() override;

	virtual void sound_pins_changed() override;
	virtual void update_SN76477_status() override;
	virtual void delayed_sound_1(int data) override;
	virtual void delayed_sound_2(int data) override;

private:
	required_ioport m_video_conf;

	void spacefev_sound(machine_config &config);

	TIMER_DEVICE_CALLBACK_MEMBER(vco_voltage_timer);

	TIMER_CALLBACK_MEMBER(stop_red_cannon);
	void start_red_cannon();
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	emu_timer* m_cannon_timer;
	int m_red_screen;
	int m_red_cannon;
};

class sheriff_state : public n8080_state
{
public:
	sheriff_state(const machine_config &mconfig, device_type type, const char *tag) :
		n8080_state(mconfig, type, tag)
	{ }

	void sheriff(machine_config &config);
	void westgun2(machine_config &config);

protected:
	virtual void machine_reset() override;
	virtual void sound_start() override;
	virtual void sound_reset() override;
	virtual void video_start() override;

	virtual void sound_pins_changed() override;
	virtual void update_SN76477_status() override;

private:
	void sheriff_sound(machine_config &config);

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
};

class helifire_state : public n8080_state
{
public:
	helifire_state(const machine_config &mconfig, device_type type, const char *tag) :
		n8080_state(mconfig, type, tag),
		m_dac(*this, "helifire_dac"),
		m_colorram(*this, "colorram"),
		m_pot(*this, "POT%u", 0)
	{ }

	void helifire(machine_config &config);

protected:
	virtual void machine_reset() override;
	virtual void sound_start() override;
	virtual void sound_reset() override;
	virtual void video_start() override;

	virtual void sound_pins_changed() override;
	virtual void delayed_sound_2(int data) override;

private:
	void helifire_sound(machine_config &config);

	TIMER_DEVICE_CALLBACK_MEMBER(dac_volume_timer);
	DECLARE_READ_LINE_MEMBER(helifire_8035_t0_r);
	DECLARE_READ_LINE_MEMBER(helifire_8035_t1_r);
	uint8_t helifire_8035_external_ram_r();
	uint8_t helifire_8035_p2_r();
	void sound_ctrl_w(uint8_t data);
	void sound_io_map(address_map &map);

	void helifire_palette(palette_device &palette) const;
	void next_line();
	DECLARE_WRITE_LINE_MEMBER(screen_vblank);
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void main_cpu_map(address_map &map);

	required_device<dac_8bit_r2r_device> m_dac;
	required_shared_ptr<uint8_t> m_colorram;
	required_ioport_array<2> m_pot;

	int m_dac_phase = 0;
	double m_dac_volume = 0;
	double m_dac_timing = 0;

	int m_flash = 0;
	uint8_t m_LSFR[63];
	unsigned m_mv = 0;
	unsigned m_sc = 0; // IC56
};


#endif // MAME_INCLUDES_N8080_H
