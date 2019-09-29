// license:BSD-3-Clause
// copyright-holders:F. Ulivi
/*********************************************************************

    1ma6.h

    HP-85 tape controller (1MA6-0001)

*********************************************************************/

#ifndef MAME_MACHINE_1MA6_H
#define MAME_MACHINE_1MA6_H

#pragma once

#include "formats/hti_tape.h"
#include "machine/hp_dc100_tape.h"

class hp_1ma6_device : public device_t
{
public:
	// construction/destruction
	hp_1ma6_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// Register read/write
	DECLARE_WRITE8_MEMBER(reg_w);
	DECLARE_READ8_MEMBER(reg_r);

	DECLARE_WRITE_LINE_MEMBER(cart_out_w);
	DECLARE_WRITE_LINE_MEMBER(hole_w);
	DECLARE_WRITE_LINE_MEMBER(tacho_tick_w);
	DECLARE_WRITE_LINE_MEMBER(motion_w);
	DECLARE_WRITE_LINE_MEMBER(rd_bit_w);
	DECLARE_READ_LINE_MEMBER(wr_bit_r);

protected:
	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	required_device<hp_dc100_tape_device> m_tape;

	// Registers
	uint8_t m_data_reg;
	uint8_t m_status_reg;
	uint8_t m_control_reg;

	// Reading & writing
	unsigned m_bit_idx;
	uint8_t m_data_sr;

	// Command FSM state
	typedef enum {
		CMD_IDLE,
		CMD_STOPPING,
		CMD_STARTING,
		CMD_RD_WAIT_SYNC,
		CMD_RD,
		CMD_WR_WAIT_SYNC,
		CMD_WR_PREAMBLE,
		CMD_WR,
		CMD_WR_GAP,
		CMD_FAST_FWD_REV
	} cmd_state_t;
	cmd_state_t m_cmd_state;

	void clear_state();
	void load_wr_word();
	void start_cmd_exec(uint8_t new_ctl_reg);
};

// device type definition
DECLARE_DEVICE_TYPE(HP_1MA6, hp_1ma6_device)

#endif /* MAME_MACHINE_1MA6_H */
