// license:BSD-3-Clause
// copyright-holders:Wilbert Pol,Gabriele D'Antona

/*

Research Machines RM 380Z

*/

#ifndef MAME_RM_RM380Z_H
#define MAME_RM_RM380Z_H

#pragma once

#include "cpu/z80/z80.h"
#include "imagedev/cassette.h"
#include "imagedev/floppy.h"
#include "machine/keyboard.h"
#include "machine/ram.h"
#include "machine/wd_fdc.h"

//
//
//

#define RM380Z_MAINCPU_TAG      "maincpu"

//
//
//


class rm380z_state : public driver_device
{
public:
	rm380z_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_chargen(*this, "chargen"),
		m_maincpu(*this, RM380Z_MAINCPU_TAG),
		m_screen(*this, "screen"),
		m_cassette(*this, "cassette"),
		m_messram(*this, RAM_TAG),
		m_fdc(*this, "wd1771"),
		m_floppy0(*this, "wd1771:0"),
		m_floppy1(*this, "wd1771:1")
	{
	}

	void rm480z(machine_config &config);
	void rm380z(machine_config &config);

	void init_rm380z();
	void init_rm380z34d();
	void init_rm380z34e();
	void init_rm480z();

protected:
	virtual void machine_reset() override;
	virtual void machine_start() override;

private:
	template <int ROWS, int COLS>
	class rm380z_vram
	{
	public:
		void set_char(int row, int col, uint8_t data) { m_chars[get_row(row)][col] = data; }
		void set_attrib(int row, int col, uint8_t data) { m_attribs[get_row(row)][col] = data; }
		void set_scroll_register(uint8_t value) { m_scroll_reg = value; }
		void reset() { memset(m_attribs, 0, sizeof(m_attribs)); memset(m_chars, 0x80, sizeof(m_chars)); }

		uint8_t get_char(int row, int col) const { return m_chars[get_row(row)][col]; }
		uint8_t get_attrib(int row, int col) const { return m_attribs[get_row(row)][col]; }

	private:
		int get_row(int row) const { return (row + m_scroll_reg) % ROWS; }

		uint8_t m_chars[ROWS][COLS];
		uint8_t m_attribs[ROWS][COLS];
		uint8_t m_scroll_reg = 0;
	};

	static inline constexpr int RM380Z_VIDEOMODE_40COL = 0x01;
	static inline constexpr int RM380Z_VIDEOMODE_80COL = 0x02;

	static inline constexpr int RM380Z_CHDIMX = 5;
	static inline constexpr int RM380Z_CHDIMY = 9;
	static inline constexpr int RM380Z_NCX = 8;
	static inline constexpr int RM380Z_NCY = 16;
	static inline constexpr int RM380Z_SCREENCOLS = 80;
	static inline constexpr int RM380Z_SCREENROWS = 24;

	bool ports_enabled_high() const { return ( m_port0 & 0x80 ); }
	bool ports_enabled_low() const { return !( m_port0 & 0x80 ); }

	bool get_rowcol_from_offset(int &row, int &col, offs_t offset) const;
	void put_point(int charnum, int x, int y, int col);
	void init_graphic_chars();

	void putChar_vdu80(int charnum, int attribs, int x, int y, bitmap_ind16 &bitmap);
	void putChar_vdu40(int charnum, int x, int y, bitmap_ind16 &bitmap);
	void decode_videoram_char(int row, int col, uint8_t &chr, uint8_t &attrib);
	void config_videomode();

	void port_write(offs_t offset, uint8_t data);
	uint8_t port_read(offs_t offset);
	void port_write_1b00(offs_t offset, uint8_t data);
	uint8_t port_read_1b00(offs_t offset);

	uint8_t videoram_read(offs_t offset);
	void videoram_write(offs_t offset, uint8_t data);

	uint8_t rm380z_portlow_r();
	void rm380z_portlow_w(offs_t offset, uint8_t data);
	uint8_t rm380z_porthi_r();
	void rm380z_porthi_w(offs_t offset, uint8_t data);

	void disk_0_control(uint8_t data);

	void keyboard_put(u8 data);

	DECLARE_MACHINE_RESET(rm480z);

	void config_memory_map();
	void update_screen_vdu80(bitmap_ind16 &bitmap);
	void update_screen_vdu40(bitmap_ind16 &bitmap);
	uint32_t screen_update_rm380z(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_rm480z(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	TIMER_CALLBACK_MEMBER(static_vblank_timer);

	void rm380z_io(address_map &map);
	void rm380z_mem(address_map &map);
	void rm480z_io(address_map &map);
	void rm480z_mem(address_map &map);

	uint8_t m_port0 = 0;
	uint8_t m_port0_mask = 0;
	uint8_t m_port0_kbd = 0;
	uint8_t m_port1 = 0;
	uint8_t m_fbfd_mask = 0;
	uint8_t m_fbfe = 0;

	uint8_t m_character_row = 0;
	uint8_t m_character = 0;

	uint8_t m_graphic_chars[0x80][(RM380Z_CHDIMX+1)*(RM380Z_CHDIMY+1)];
	uint8_t m_user_defined_chars[2048];

	rm380z_vram<RM380Z_SCREENROWS, RM380Z_SCREENCOLS> m_vram;

	int m_rasterlineCtr = 0;
	emu_timer* m_vblankTimer = nullptr;

	int m_videomode = 0;

	emu_timer *m_static_vblank_timer = nullptr;

	required_region_ptr<u8> m_chargen;
	required_device<cpu_device> m_maincpu;
	optional_device<screen_device> m_screen;
	optional_device<cassette_image_device> m_cassette;
	optional_device<ram_device> m_messram;
	optional_device<fd1771_device> m_fdc;
	optional_device<floppy_connector> m_floppy0;
	optional_device<floppy_connector> m_floppy1;
};

#endif // MAME_RM_RM380Z_H
