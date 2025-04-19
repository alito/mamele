// license:BSD-3-Clause
// copyright-holders:hap
/*

  National Semiconductor COPS(MM57 MCU family) disassembler

*/

#ifndef MAME_CPU_COPS1_COPS1D_H
#define MAME_CPU_COPS1_COPS1D_H

#pragma once


class cops1_common_disassembler : public util::disasm_interface
{
public:
	cops1_common_disassembler();
	virtual ~cops1_common_disassembler() = default;

	virtual u32 opcode_alignment() const override { return 1; }
	virtual u32 interface_flags() const override { return NONLINEAR_PC | PAGED; }
	virtual u32 page_address_bits() const override { return 6; }
	virtual offs_t pc_linear_to_real(offs_t pc) const override { return (pc & ~0x3f) | m_l2r[pc & 0x3f]; }
	virtual offs_t pc_real_to_linear(offs_t pc) const override { return (pc & ~0x3f) | m_r2l[pc & 0x3f]; }

protected:
	// opcode mnemonics
	enum e_mnemonics
	{
		em_ILL,
		em_AD, em_ADD, em_SUB, em_COMP, em_0TA, em_ADX, em_HXA, em_TAM, em_SC, em_RSC, em_TC,
		em_TIN, em_TF, em_TKB, em_TIR,
		em_BTD, em_DSPA, em_DSPS, em_AXO, em_LDF, em_READ,
		em_GO, em_CALL, em_RET, em_RETS, em_LG, em_LGCALL, em_NOP,
		em_EXC, em_EXCM, em_EXCP, em_MTA, em_LM,
		em_SM1, em_SM2, em_SM4, em_SM8, em_RSM1, em_RSM2, em_RSM4, em_RSM8, em_TM,
		em_LB, em_LBL, em_ATB, em_BTA, em_HXBR
	};

	static const char *const s_name[];
	static const uint8_t s_bits[];
	static const uint32_t s_flags[];

	u8 m_l2r[0x40];
	u8 m_r2l[0x40];

	offs_t increment_pc(offs_t pc);
	offs_t common_disasm(const u8 *lut_opmap, std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params);
};

class mm5799_disassembler : public cops1_common_disassembler
{
public:
	mm5799_disassembler() = default;
	virtual ~mm5799_disassembler() = default;

	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) override;

private:
	static const u8 mm5799_opmap[0x100];

};

#endif // MAME_CPU_COPS1_COPS1D_H
