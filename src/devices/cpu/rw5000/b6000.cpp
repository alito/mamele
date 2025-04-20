// license:BSD-3-Clause
// copyright-holders:hap
/*

  Rockwell B6000 MCU (based on B5900)

MCU designed for Mattel's electronic games, I/O is a bit more versatile,
and a speaker output was added. It was succeeded by B6100 with a larger ROM.

TODO:
- any other changed opcodes?
- confirm digit segment decoder (10, 11, 15 are unused by the games)

*/

#include "emu.h"
#include "b6000.h"

#include "rw5000d.h"


DEFINE_DEVICE_TYPE(B6000, b6000_cpu_device, "b6000", "Rockwell B6000")


// constructor
b6000_cpu_device::b6000_cpu_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock, int prgwidth, address_map_constructor program, int datawidth, address_map_constructor data) :
	b5000_cpu_device(mconfig, type, tag, owner, clock, prgwidth, program, datawidth, data)
{ }

b6000_cpu_device::b6000_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	b6000_cpu_device(mconfig, B6000, tag, owner, clock, 9, address_map_constructor(FUNC(b6000_cpu_device::program_512x8), this), 6, address_map_constructor(FUNC(b6000_cpu_device::data_45x4), this))
{ }


// internal memory maps
void b6000_cpu_device::program_512x8(address_map &map)
{
	map(0x000, 0x1ff).rom();
}


// disasm
std::unique_ptr<util::disasm_interface> b6000_cpu_device::create_disassembler()
{
	return std::make_unique<b6000_disassembler>();
}


// initialize
void b6000_cpu_device::device_reset()
{
	b5000_cpu_device::device_reset();

	// clear all registers
	m_bl = 0;
	m_bu = 0;
	m_ram_addr = 0;
	m_a = 0;
	m_c = 0;

	// clear outputs
	seg_w(0);
	m_write_str(0);
	m_write_spk(0);
}


// digit segment decoder
u16 b6000_cpu_device::decode_digit(u8 data)
{
	static const u8 lut_segs[0x10] =
	{
		// 0-9 same as B5000
		0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,

		// ?, ?, none, F, G, ?
		0, 0, 0x00, 0x20, 0x40, 0
	};
	return lut_segs[data & 0xf];
}


//-------------------------------------------------
//  execute
//-------------------------------------------------

void b6000_cpu_device::execute_one()
{
	switch (m_op)
	{
		case 0x76: op_atbz(); break;
		case 0x77: op_atb(); break;

		// rest is same as B5000
		default: b5000_cpu_device::execute_one(); break;
	}
}


//-------------------------------------------------
//  changed opcodes (no need for separate file)
//-------------------------------------------------

void b6000_cpu_device::op_tkbs()
{
	// TKBS: load segments and speaker (no TKB step)
	seg_w(m_seg | decode_digit(ram_r()));
	m_write_spk(m_c);
}

void b6000_cpu_device::op_atbz()
{
	// ATBZ: KSEG + load strobe (no ATB step)
	op_kseg();
	m_write_str(1 << m_a);
}
