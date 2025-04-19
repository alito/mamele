// license:BSD-3-Clause
// copyright-holders:David Shah, David Haywood
/***************************************************************************

    m6502_swap_op_d5_d6.h

    6502 / N2A03 with instruction scrambling

***************************************************************************/

#ifndef MAME_M6502_SWAP_OP_D5_D6_H
#define MAME_M6502_SWAP_OP_D5_D6_H

#pragma once

#include "cpu/m6502/n2a03.h"
#include "cpu/m6502/n2a03d.h"

class m6502_swap_op_d5_d6 : public m6502_device {
public:
	m6502_swap_op_d5_d6(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	class mi_decrypt : public mi_default {
	public:

		virtual ~mi_decrypt() {}
		virtual uint8_t read_sync(uint16_t adr) override;

		uint8_t descramble(uint8_t op);
	};

	class disassembler : public m6502_disassembler {
	public:
		mi_decrypt *mintf;

		disassembler(mi_decrypt *m);
		virtual ~disassembler() = default;
		virtual u32 interface_flags() const override;
		virtual u8 decrypt8(u8 value, offs_t pc, bool opcode) const override;
	};

	virtual void device_reset() override;
	virtual void device_start() override;
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;
};

class n2a03_core_swap_op_d5_d6 : public n2a03_core_device {
public:
	n2a03_core_swap_op_d5_d6(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	class mi_decrypt : public mi_default {
	public:

		virtual ~mi_decrypt() {}
		virtual uint8_t read_sync(uint16_t adr) override;

		uint8_t descramble(uint8_t op);
	};

	class disassembler : public n2a03_disassembler {
	public:
		mi_decrypt *mintf;

		disassembler(mi_decrypt *m);
		virtual ~disassembler() = default;
		virtual u32 interface_flags() const override;
		virtual u8 decrypt8(u8 value, offs_t pc, bool opcode) const override;
	};

	virtual void device_reset() override;
	virtual void device_start() override;
	virtual std::unique_ptr<util::disasm_interface> create_disassembler() override;
};


DECLARE_DEVICE_TYPE(M6502_SWAP_OP_D5_D6, m6502_swap_op_d5_d6)
DECLARE_DEVICE_TYPE(N2A03_CORE_SWAP_OP_D5_D6, n2a03_core_swap_op_d5_d6)

#endif // MAME_M6502_SWAP_OP_D5_D6_H
