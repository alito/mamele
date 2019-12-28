// license:BSD-3-Clause
// copyright-holders:Sergey Svishchev

#ifndef MAME_MACHINE_IE15_H
#define MAME_MACHINE_IE15_H

#pragma once

#include "cpu/ie15/ie15.h"
#include "machine/ie15_kbd.h"
#include "sound/beep.h"

#include "screen.h"
#include "speaker.h"
#include "diserial.h"


#define SCREEN_PAGE     (80*48)

#define IE_TRUE         0x80
#define IE_FALSE        0

#define IE15_TOTAL_HORZ 1000
#define IE15_DISP_HORZ  800
#define IE15_HORZ_START 200

#define IE15_TOTAL_VERT (28*11)
#define IE15_DISP_VERT  (25*11)
#define IE15_VERT_START (2*11)
#define IE15_STATUSLINE 11


INPUT_PORTS_EXTERN(ie15);


class ie15_device : public device_t, public device_serial_interface
{
public:
	ie15_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// Interface to a RS232 connection.
	auto rs232_conn_txd_handler() { return m_rs232_conn_txd_handler.bind(); }
	auto rs232_conn_dtr_handler() { return m_rs232_conn_dtr_handler.bind(); }
	auto rs232_conn_rts_handler() { return m_rs232_conn_rts_handler.bind(); }
	DECLARE_WRITE_LINE_MEMBER(rs232_conn_dcd_w);
	DECLARE_WRITE_LINE_MEMBER(rs232_conn_dsr_w);
	DECLARE_WRITE_LINE_MEMBER(rs232_conn_ri_w);
	DECLARE_WRITE_LINE_MEMBER(rs232_conn_cts_w);
	DECLARE_WRITE_LINE_MEMBER(rs232_conn_rxd_w);

	DECLARE_WRITE_LINE_MEMBER(update_serial);

protected:
	ie15_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	virtual void device_resolve_objects() override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual void rcv_complete() override;
	virtual void tra_callback() override;
	virtual void tra_complete() override;

private:
	static const device_timer_id TIMER_HBLANK = 0;
	void scanline_callback();
	void update_leds();
	void draw_scanline(uint32_t *p, uint16_t offset, uint8_t scanline);
	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	void ie15core(machine_config &config);

	DECLARE_WRITE16_MEMBER(kbd_put);
	DECLARE_WRITE8_MEMBER(mem_w);
	DECLARE_READ8_MEMBER(mem_r);
	DECLARE_WRITE8_MEMBER(mem_addr_lo_w);
	DECLARE_WRITE8_MEMBER(mem_addr_hi_w);
	DECLARE_WRITE8_MEMBER(mem_addr_inc_w);
	DECLARE_WRITE8_MEMBER(mem_addr_dec_w);
	DECLARE_READ8_MEMBER(flag_r);
	DECLARE_WRITE8_MEMBER(flag_w);
	DECLARE_WRITE8_MEMBER(beep_w);
	DECLARE_READ8_MEMBER(kb_r);
	DECLARE_READ8_MEMBER(kb_ready_r);
	DECLARE_READ8_MEMBER(kb_s_red_r);
	DECLARE_READ8_MEMBER(kb_s_sdv_r);
	DECLARE_READ8_MEMBER(kb_s_dk_r);
	DECLARE_READ8_MEMBER(kb_s_dupl_r);
	DECLARE_READ8_MEMBER(kb_s_lin_r);
	DECLARE_WRITE8_MEMBER(kb_ready_w);
	DECLARE_READ8_MEMBER(serial_tx_ready_r);
	DECLARE_WRITE8_MEMBER(serial_w);
	DECLARE_READ8_MEMBER(serial_rx_ready_r);
	DECLARE_READ8_MEMBER(serial_r);
	DECLARE_WRITE8_MEMBER(serial_speed_w);
	TIMER_CALLBACK_MEMBER(ie15_beepoff);

	void ie15_io(address_map &map);
	void ie15_mem(address_map &map);

	std::unique_ptr<uint32_t[]> m_tmpbmp;

	emu_timer *m_hblank_timer;

	uint8_t m_long_beep;
	uint8_t m_kb_control;
	uint8_t m_kb_data;
	uint8_t m_kb_flag0;
	uint8_t m_kb_flag;
	uint8_t m_kb_ruslat;
	uint8_t m_latch;

	struct
	{
		uint8_t cursor;
		uint8_t enable;
		uint8_t line25;
		uint32_t ptr1;
		uint32_t ptr2;
	} m_video;

	uint8_t m_serial_rx_ready;
	uint8_t m_serial_rx_char;
	uint8_t m_serial_tx_ready;
	int m_hblank;
	int m_vpos;
	int m_marker_scanline;

	required_device<cpu_device> m_maincpu;
	required_region_ptr<u8> m_p_videoram;
	required_region_ptr<u8> m_p_chargen;
	required_device<beep_device> m_beeper;
	required_device<screen_device> m_screen;
	required_device<ie15_keyboard_device> m_keyboard;
	required_ioport m_io_keyboard;

	output_finder<> m_lat_led;
	output_finder<> m_nr_led;
	output_finder<> m_pch_led;
	output_finder<> m_dup_led;
	output_finder<> m_lin_led;
	output_finder<> m_red_led;
	output_finder<> m_sdv_led;
	output_finder<> m_prd_led;

	devcb_write_line m_rs232_conn_txd_handler;
	devcb_write_line m_rs232_conn_dtr_handler;
	devcb_write_line m_rs232_conn_rts_handler;

	// Until the UART is implemented
	required_ioport m_rs232_txbaud;
	required_ioport m_rs232_rxbaud;
	required_ioport m_rs232_databits;
	required_ioport m_rs232_parity;
	required_ioport m_rs232_stopbits;
};

DECLARE_DEVICE_TYPE(IE15, ie15_device)

#endif // MAME_MACHINE_IE15_H
