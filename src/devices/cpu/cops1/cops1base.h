// license:BSD-3-Clause
// copyright-holders:hap
/*

  National Semiconductor COPS(MM57 MCU family) cores

*/

#ifndef MAME_CPU_COPS1_COPS1BASE_H
#define MAME_CPU_COPS1_COPS1BASE_H

#pragma once

#include "machine/pla.h"


class cops1_base_device : public cpu_device
{
public:
	// configuration helpers
	// I/O ports:

	// 4-bit K inputs
	auto read_k() { return m_read_k.bind(); }

	// INB input pin
	auto read_inb() { return m_read_inb.bind(); }

	// 4-bit F(flags) I/O
	auto read_f() { return m_read_f.bind(); }
	auto write_f() { return m_write_f.bind(); }

	// 4-bit DO(digit output) I/O
	auto read_do3() { return m_read_do3.bind(); } // only DO3 can be an input
	auto write_do() { return m_write_do.bind(); }

	// 8-bit Sa-Sg + Sp segment outputs
	auto write_s() { return m_write_s.bind(); }

	// BLK(display blanking) output pin
	auto write_blk() { return m_write_blk.bind(); }

	// 1-bit serial I/O
	auto read_si() { return m_read_si.bind(); }
	auto write_so() { return m_write_so.bind(); }

	// set MCU mask options:

	// RAM configuration 12 or 16 digits (default false)
	auto &set_option_ram_d12(bool b) { m_option_ram_d12 = b; return *this; }

	// LB 10 real value (default 0)
	auto &set_option_lb_10(u8 lb) { m_option_lb_10 = lb & 0xf; return *this; }

	// skip on Bd 13 with EXP+ (default true)
	auto &set_option_excp_skip(bool b) { m_option_excp_skip = b; return *this; }

	// read SI directly with AXO (default false)
	auto &set_option_axo_si(bool b) { m_option_axo_si = b; return *this; }

	// K and INB pins can be active high or active low (default true)
	auto &set_option_k_active_high(bool b) { m_option_k_active_high = b; return *this; }
	auto &set_option_inb_active_high(bool b) { m_option_inb_active_high = b; return *this; }

protected:
	// construction/destruction
	cops1_base_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

	// device_execute_interface overrides
	virtual uint64_t execute_clocks_to_cycles(uint64_t clocks) const noexcept override { return (clocks + 4 - 1) / 4; }
	virtual uint64_t execute_cycles_to_clocks(uint64_t cycles) const noexcept override { return (cycles * 4); }
	virtual uint32_t execute_min_cycles() const noexcept override { return 1; }
	virtual uint32_t execute_max_cycles() const noexcept override { return 2; }
	virtual void execute_run() override;
	virtual void execute_one() = 0;
	virtual bool op_argument() = 0;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	address_space_config m_program_config;
	address_space_config m_data_config;
	address_space *m_program;
	address_space *m_data;

	int m_icount;

	// fixed settings or mask options
	int m_prgwidth; // ROM/RAM address size
	int m_datawidth; // "
	u16 m_prgmask; // "
	u16 m_datamask; // "

	bool m_option_ram_d12 = false;
	u8 m_option_lb_10 = 0;
	bool m_option_excp_skip = true;
	bool m_option_axo_si = false;
	bool m_option_k_active_high = true;
	bool m_option_inb_active_high = true;

	optional_device<pla_device> m_opla; // segment output PLA

	// i/o handlers
	devcb_read8 m_read_k;
	devcb_read_line m_read_inb;
	devcb_read8 m_read_f;
	devcb_write8 m_write_f;
	devcb_read_line m_read_do3;
	devcb_write8 m_write_do;
	devcb_write8 m_write_s;
	devcb_write_line m_write_blk;
	devcb_read_line m_read_si;
	devcb_write_line m_write_so;

	// internal state, regs
	u16 m_pc;
	u16 m_prev_pc;
	u8 m_op;
	u8 m_prev_op;
	u8 m_arg;

	u8 m_a;
	u8 m_h;
	u8 m_b;
	int m_c;
	bool m_skip;
	u16 m_sa;
	u16 m_sb;
	u8 m_serial;
	u8 m_f;
	u8 m_do;

	// misc handlers
	void cycle();
	void increment_pc();

	u8 ram_r();
	void ram_w(u8 data);
	void pop_pc();
	void push_pc();

	// opcode handlers
	void op_ad();
	void op_add();
	void op_sub();
	void op_comp();
	void op_0ta();
	void op_adx();
	void op_hxa();
	void op_tam();
	void op_sc();
	void op_rsc();
	void op_tc();

	void op_tin();
	void op_tf();
	void op_tkb();
	void op_tir();

	void op_btd();
	void op_dspa();
	void op_dsps();
	void op_axo();
	void op_ldf();
	void op_read();

	void op_go();
	void op_call();
	void op_ret();
	void op_rets();
	void op_lg();
	void op_nop();

	void op_exc();
	void op_excm();
	void op_excp();
	void op_mta();
	void op_lm();

	void op_sm();
	void op_rsm();
	void op_tm();

	void op_lb();
	void op_lbl();
	void op_atb();
	void op_bta();
	void op_hxbr();
};


#endif // MAME_CPU_COPS1_COPS1BASE_H
