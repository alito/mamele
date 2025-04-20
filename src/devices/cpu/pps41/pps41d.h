// license:BSD-3-Clause
// copyright-holders:hap
/*

  Rockwell PPS-4/1 disassembler

*/

#ifndef MAME_CPU_PPS41_PPS41D_H
#define MAME_CPU_PPS41_PPS41D_H

#pragma once


class pps41_common_disassembler : public util::disasm_interface
{
public:
	pps41_common_disassembler();
	virtual ~pps41_common_disassembler() = default;

	virtual u32 opcode_alignment() const override { return 1; }
	virtual u32 interface_flags() const override { return NONLINEAR_PC | PAGED; }
	virtual u32 page_address_bits() const override { return 6; }
	virtual offs_t pc_linear_to_real(offs_t pc) const override { return (pc & ~0x3f) | m_l2r[pc & 0x3f]; }
	virtual offs_t pc_real_to_linear(offs_t pc) const override { return (pc & ~0x3f) | m_r2l[pc & 0x3f]; }

protected:
	// opcode mnemonics
	enum e_mnemonics
	{
		// MM76/shared
		mILL /* 0! */,
		mXAB, mLBA, mLB, mEOB2,
		mSB, mRB, mSKBF,
		mXAS, mLSA,
		mL, mX, mXDSK, mXNSK,
		mA, mAC, mACSK, mASK, mCOM, mRC, mSC, mSKNC, mLAI, mAISK,
		mRT, mRTSK, mT, mNOP, mTL, mTM, mTML, mTR,
		mSKMEA, mSKBEI, mSKAEI,
		mSOS, mROS, mSKISL, mIBM, mOB, mIAM, mOA, mIOS, mI1, mI2C, mINT1H, mDIN1, mINT0L, mDIN0, mSEG1, mSEG2,

		// MM78 differences
		mINT0H, mINT1L, mSAG, mEOB3, mTAB,
		mI1SK, mIX, mOX, mLXA, mXAX, mIOA,
		mTLB, mTMLB
	};

	static const char *const s_name[];
	static const u8 s_bits[];
	static const u32 s_flags[];

	u8 m_l2r[0x40];
	u8 m_r2l[0x40];

	offs_t increment_pc(offs_t pc);
	offs_t common_disasm(const u8 *lut_opmap, std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params);
};

class mm76_disassembler : public pps41_common_disassembler
{
public:
	mm76_disassembler() = default;
	virtual ~mm76_disassembler() = default;

	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) override;

private:
	static const u8 mm76_opmap[0x100];

};

class mm78_disassembler : public pps41_common_disassembler
{
public:
	mm78_disassembler() = default;
	virtual ~mm78_disassembler() = default;

	virtual offs_t disassemble(std::ostream &stream, offs_t pc, const data_buffer &opcodes, const data_buffer &params) override;

private:
	static const u8 mm78_opmap[0x100];

};

#endif // MAME_CPU_PPS41_PPS41D_H
