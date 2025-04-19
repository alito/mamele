// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

#ifndef MAME_CPU_NS32000_NS32000_H
#define MAME_CPU_NS32000_NS32000_H

#pragma once

#include "slave.h"

template <int Width>
class ns32000_device : public cpu_device
{
public:
	template <typename T> void set_fpu(T &&tag) { m_fpu.set_tag(std::forward<T>(tag)); }

	// construction/destruction
	ns32000_device(machine_config const &mconfig, device_type type, char const *tag, device_t *owner, u32 clock);

protected:
	ns32000_device(machine_config const &mconfig, device_type type, char const *tag, device_t *owner, u32 clock, int databits, int addrbits);

	// device_t overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_execute_interface overrides
	virtual u32 execute_min_cycles() const noexcept override { return 1; }
	virtual u32 execute_max_cycles() const noexcept override { return 6; }
	virtual u32 execute_input_lines() const noexcept override { return 2; }
	virtual void execute_run() override;
	virtual void execute_set_input(int inputnum, int state) override;
	virtual bool execute_input_edge_triggered(int inputnum) const noexcept override { return inputnum == INPUT_LINE_NMI; }

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;
	virtual bool memory_translate(int spacenum, int intention, offs_t &address) override;

	// device_state_interface overrides
	virtual void state_string_export(device_state_entry const &entry, std::string &str) const override;

	// device_disasm_interface overrides
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;

	enum addr_mode_type : unsigned
	{
		IMM,
		REG,
		MEM,
		IND,
		EXT,
		TOS,
	};
	enum access_class : unsigned
	{
		NONE,
		READ,
		WRITE,
		RMW,
		ADDR,
		REGADDR,
	};
	enum size_code : unsigned
	{
		SIZE_B = 0,
		SIZE_W = 1,
		SIZE_D = 3,
		SIZE_Q = 7,
	};

	// time needed to read or write a memory operand
	unsigned top(size_code const size, u32 const address = 0) const
	{
		// TODO: mmu cycles
		static constexpr unsigned tmmu = 0;

		switch (size)
		{
		case SIZE_B: return 3 + tmmu;
		case SIZE_W: return (address & 1) ? 7 + 2 * tmmu : 3 + tmmu;
		case SIZE_D: return (address & 1) ? 11 + 3 * tmmu : 7 + 2 * tmmu;
		case SIZE_Q: return (address & 1) ? 19 + 5 * tmmu : 15 + 4 * tmmu;
		}

		// can't happen
		return 0;
	}

	struct addr_mode
	{
		addr_mode(unsigned gen)
			: gen(gen)
			, access(NONE)
			, slave(false)
			, base(0)
			, disp(0)
			, imm(0)
			, tea(0)
		{};

		void read_i(size_code code)  { access = READ;    size = code; }
		void read_f(size_code code)  { access = READ;    size = code; slave = true; }
		void write_i(size_code code) { access = WRITE;   size = code; }
		void write_f(size_code code) { access = WRITE;   size = code; slave = true; }
		void rmw_i(size_code code)   { access = RMW;     size = code; }
		void rmw_f(size_code code)   { access = RMW;     size = code; slave = true; }
		void addr()                  { access = ADDR;    size = SIZE_D; }
		void regaddr()               { access = REGADDR; size = SIZE_D; }

		unsigned gen;
		addr_mode_type type;
		access_class access;
		size_code size;
		bool slave;
		u32 base;
		u32 disp;
		u64 imm;
		unsigned tea;
	};

	// instruction decoding helpers
	s32 displacement(unsigned &bytes);
	void decode(addr_mode *mode, unsigned &bytes);

	// operand read/write helpers
	u32 ea(addr_mode const mode);
	u64 gen_read(addr_mode mode);
	s64 gen_read_sx(addr_mode mode);
	void gen_write(addr_mode mode, u64 data);

	// other execution helpers
	bool condition(unsigned const cc);
	void flags(u32 const src1, u32 const src2, u32 const dest, unsigned const size, bool const subtraction);
	void interrupt(unsigned const vector, u32 const return_address, bool const trap = true);
	u16 slave(addr_mode op1, addr_mode op2);

private:
	// configuration
	address_space_config m_program_config;
	address_space_config m_interrupt_config;

	optional_device<ns32000_slave_interface> m_fpu;

	// emulation state
	int m_icount;

	typename memory_access<24, Width, 0, ENDIANNESS_LITTLE>::specific m_bus[16];

	u32 m_pc;      // program counter
	u32 m_sb;      // static base
	u32 m_fp;      // frame pointer
	u32 m_sp1;     // user stack pointer
	u32 m_sp0;     // interrupt stack pointer
	u32 m_intbase; // interrupt base
	u16 m_psr;     // processor status
	u16 m_mod;     // module
	u8 m_cfg;      // configuration

	u32 m_r[8];
	u32 m_f[8];

	bool m_nmi_line;
	bool m_int_line;
	bool m_wait;
	bool m_sequential;
};

class ns32008_device : public ns32000_device<0>
{
public:
	ns32008_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);
};

class ns32016_device : public ns32000_device<1>
{
public:
	ns32016_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);
};

class ns32032_device : public ns32000_device<2>
{
public:
	ns32032_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);
};

DECLARE_DEVICE_TYPE(NS32008, ns32008_device)
DECLARE_DEVICE_TYPE(NS32016, ns32016_device)
DECLARE_DEVICE_TYPE(NS32032, ns32032_device)

#endif // MAME_CPU_NS32000_NS32000_H
