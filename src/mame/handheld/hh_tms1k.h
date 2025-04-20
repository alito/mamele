// license:BSD-3-Clause
// copyright-holders:hap
/*

  TMS1000 MCU series tabletops/handhelds or other simple devices.

*/

#ifndef MAME_INCLUDES_HH_TMS1K_H
#define MAME_INCLUDES_HH_TMS1K_H

#pragma once

#include "cpu/tms1000/tms1000.h"
#include "cpu/tms1000/tms1000c.h"
#include "cpu/tms1000/tms1100.h"
#include "cpu/tms1000/tms1400.h"
#include "cpu/tms1000/tms2100.h"
#include "cpu/tms1000/tms2400.h"
#include "cpu/tms1000/tms0970.h"
#include "cpu/tms1000/tms0980.h"
#include "cpu/tms1000/tms0270.h"
#include "cpu/tms1000/tp0320.h"
#include "video/pwm.h"
#include "sound/spkrdev.h"


class hh_tms1k_state : public driver_device
{
public:
	hh_tms1k_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_display(*this, "display"),
		m_speaker(*this, "speaker"),
		m_inputs(*this, "IN.%u", 0),
		m_out_power(*this, "power")
	{ }

	virtual DECLARE_INPUT_CHANGED_MEMBER(reset_button);
	virtual DECLARE_INPUT_CHANGED_MEMBER(power_button);

	template<int Sel> DECLARE_INPUT_CHANGED_MEMBER(switch_next) { if (newval) switch_change(Sel, param, true); }
	template<int Sel> DECLARE_INPUT_CHANGED_MEMBER(switch_prev) { if (newval) switch_change(Sel, param, false); }

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	// devices
	required_device<tms1k_base_device> m_maincpu;
	optional_device<pwm_display_device> m_display;
	optional_device<speaker_sound_device> m_speaker;
	optional_ioport_array<18> m_inputs; // max 18
	output_finder<> m_out_power; // power state, eg. led

	// misc common
	u32 m_r = 0U;                        // MCU R-pins data
	u16 m_o = 0U;                        // MCU O-pins data
	u32 m_inp_mux = 0U;                  // multiplexed inputs mask
	bool m_power_on = false;

	u32 m_grid = 0U;                     // VFD/LED current row data
	u32 m_plate = 0U;                    // VFD/LED current column data

	u8 read_inputs(int columns);
	u8 read_rotated_inputs(int columns, u8 rowmask = 0xf);
	virtual DECLARE_WRITE_LINE_MEMBER(auto_power_off);
	virtual void power_off();
	virtual void set_power(bool state);
	void switch_change(int sel, u32 mask, bool next);
};


#endif // MAME_INCLUDES_HH_TMS1K_H
