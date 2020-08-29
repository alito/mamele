// license:BSD-3-Clause
// copyright-holders:Ramiro Polla, Felipe Sanches
/*
 * Epson LX-810L dot matrix printer emulation
 *
 */
#ifndef MAME_BUS_CENTRONICS_EPSON_LX810L_H
#define MAME_BUS_CENTRONICS_EPSON_LX810L_H

#pragma once

#include "ctronics.h"
#include "cpu/upd7810/upd7810.h"
#include "machine/e05a30.h"
#include "machine/eepromser.h"
#include "machine/steppers.h"
#include "sound/dac.h"
#include "screen.h"

#include <cstdlib>


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> epson_lx810l_device

class epson_lx810l_device : public device_t, public device_centronics_peripheral_interface
{
public:
	// construction/destruction
	epson_lx810l_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	/* Centronics stuff */
	virtual DECLARE_WRITE_LINE_MEMBER( input_strobe ) override { m_e05a30->centronics_input_strobe(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data0 ) override { m_e05a30->centronics_input_data0(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data1 ) override { m_e05a30->centronics_input_data1(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data2 ) override { m_e05a30->centronics_input_data2(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data3 ) override { m_e05a30->centronics_input_data3(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data4 ) override { m_e05a30->centronics_input_data4(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data5 ) override { m_e05a30->centronics_input_data5(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data6 ) override { m_e05a30->centronics_input_data6(state); }
	virtual DECLARE_WRITE_LINE_MEMBER( input_data7 ) override { m_e05a30->centronics_input_data7(state); }

	/* Panel buttons */
	DECLARE_INPUT_CHANGED_MEMBER(online_sw);

protected:
	epson_lx810l_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

private:
	uint8_t porta_r(offs_t offset);
	void porta_w(offs_t offset, uint8_t data);
	uint8_t portb_r(offs_t offset);
	void portb_w(offs_t offset, uint8_t data);
	uint8_t portc_r(offs_t offset);
	void portc_w(offs_t offset, uint8_t data);

	/* fake memory I/O to get past memory reset check */
	uint8_t fakemem_r();
	void fakemem_w(uint8_t data);

	/* Extended Timer Output */
	DECLARE_WRITE_LINE_MEMBER(co0_w);

	/* ADC */
	uint8_t an0_r();
	uint8_t an1_r();
	uint8_t an2_r();
	uint8_t an3_r();
	uint8_t an4_r();
	uint8_t an5_r();
	uint8_t an6_r();
	uint8_t an7_r();


	/* GATE ARRAY */
	void printhead(uint16_t data);
	void pf_stepper(uint8_t data);
	void cr_stepper(uint8_t data);
	DECLARE_WRITE_LINE_MEMBER(e05a30_ready);

	DECLARE_WRITE_LINE_MEMBER(e05a30_centronics_ack) { output_ack(state); }
	DECLARE_WRITE_LINE_MEMBER(e05a30_centronics_busy) { output_busy(state); }
	DECLARE_WRITE_LINE_MEMBER(e05a30_centronics_perror) { output_perror(state); }
	DECLARE_WRITE_LINE_MEMBER(e05a30_centronics_fault) { output_fault(state); }
	DECLARE_WRITE_LINE_MEMBER(e05a30_centronics_select) { output_select(state); }

	void lx810l_mem(address_map &map);

	/* Video hardware (simulates paper) */
	uint32_t screen_update_lx810l(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	unsigned int bitmap_line(int i) { return ((std::abs(m_pf_pos_abs) / 6) + i) % m_bitmap.height(); }

	required_device<cpu_device> m_maincpu;
	required_device<stepper_device> m_pf_stepper;
	required_device<stepper_device> m_cr_stepper;
	required_device<eeprom_serial_93cxx_device> m_eeprom;
	required_device<e05a30_device> m_e05a30;
	required_device<screen_device> m_screen;

	output_finder<> m_online_led;

	int m_93c06_clk;
	int m_93c06_cs;
	uint16_t m_printhead;
	int m_pf_pos_abs;
	int m_cr_pos_abs;
	int m_real_cr_pos;
	int m_real_cr_steps;
	int m_real_cr_dir; /* 1 is going right, -1 is going left */
	uint8_t m_fakemem;
	bitmap_rgb32 m_bitmap;

	enum {
		TIMER_CR
	};

	emu_timer *m_cr_timer;
};

// ======================> epson_ap2000_t

class epson_ap2000_device : public epson_lx810l_device
{
public:
	// construction/destruction
	epson_ap2000_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
};


// device type definition
DECLARE_DEVICE_TYPE(EPSON_LX810L, epson_lx810l_device)
DECLARE_DEVICE_TYPE(EPSON_AP2000, epson_ap2000_device)

#endif // MAME_BUS_CENTRONICS_EPSON_LX810L_H
