// license:BSD-3-Clause
// copyright-holders:AJR
/***************************************************************************

    Motorola M88000 RISC microprocessors

***************************************************************************/

#ifndef MAME_CPU_M88000_M88000_H
#define MAME_CPU_M88000_M88000_H

#pragma once

#include "softfloat3/source/include/softfloat.h"

class mc88100_device : public cpu_device
{
public:
	// construction/destruction
	mc88100_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual void execute_run() override;
	virtual void execute_set_input(int inputnum, int state) override;

	// device_disasm_interface overrides
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;

	void execute(u32 const inst);
	void exception(unsigned vector, bool const trap = false);

	// integer helpers
	void set_cr(unsigned const cr, u32 const data);
	bool condition(unsigned const m5, u32 const src) const;
	u32 cmp(u32 const src1, u32 const src2) const;
	bool carry(u32 const src1, u32 const src2, u32 const dest) const;
	bool overflow(u32 const src1, u32 const src2, u32 const dest) const;

	// floating-point helpers
	void set_fcr(unsigned const fcr, u32 const data);
	u32 fcmp(float64_t const src1, float64_t const src2);
	void fset(unsigned const td, unsigned const d, float64_t const data);

private:
	// address spaces
	address_space_config m_code_config;
	address_space_config m_data_config;
	memory_access<32, 2, 0, ENDIANNESS_BIG>::cache m_inst_cache;
	memory_access<32, 2, 0, ENDIANNESS_BIG>::specific m_data_space;

	// register storage
	u32 m_xip; // execute instruction pointer
	u32 m_nip; // next instruction pointer
	u32 m_fip; // fetch instruction pointer
	u32 m_sb; // scoreboard

	u32 m_r[32];
	u32 m_cr[64];
	u32 m_fcr[64];

	u32 m_xop;
	u32 m_nop;
	u32 m_fop;

	bool m_int_state;

	s32 m_icount;
};

// device type declaration
DECLARE_DEVICE_TYPE(MC88100, mc88100_device)

#endif // MAME_CPU_M88000_M88000_H
