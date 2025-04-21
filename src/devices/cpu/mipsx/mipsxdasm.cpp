// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "mipsxdasm.h"

static const bool SHOW_R0_AS_0 = false;

u32 mipsx_disassembler::opcode_alignment() const
{
	return 4;
}

int mipsx_disassembler::get_ty(u32 opcode)
{
	return BIT(opcode, 30, 2);
}

int mipsx_disassembler::get_op(u32 opcode)
{
	return BIT(opcode, 27, 3);
}

int mipsx_disassembler::get_src1(u32 opcode)
{
	return BIT(opcode, 22, 5);
}

int mipsx_disassembler::get_src2_dest(u32 opcode)
{
	return BIT(opcode, 17, 5);
}

int mipsx_disassembler::get_compute_dest(u32 opcode)
{
	return BIT(opcode, 12, 5);
}

int mipsx_disassembler::get_compute_compfunc(u32 opcode)
{
	return BIT(opcode, 0, 12);
}

int mipsx_disassembler::get_offset(u32 opcode)
{
	return BIT(opcode, 0, 17);
}

int mipsx_disassembler::get_sq(u32 opcode)
{
	return BIT(opcode, 16, 1);
}

int mipsx_disassembler::get_asr_amount(int shift)
{
	// TODO
	return shift;
}

int mipsx_disassembler::get_sh_amount(int shift)
{
	// TODO
	return shift;
}

static const char *regnames[32] = { "r0",  "r1",  "r2",  "r3",  "r4",  "r5",  "r6",  "r7",  "r8",  "r9",  "r10", "r11", "r12", "r13", "r14", "r15"
									"r16", "r17", "r18", "r19", "r20", "r21", "r22", "r23", "r24", "r25", "r26", "r27", "r28", "r29", "r30", "r31" };

const char *mipsx_disassembler::get_regname(u8 reg)
{
	// general purpose register 0 just acts as a constant 0, it can't be changed, if desired simply show it as a non-canonical 0 for readability
	if ((reg == 0) && (SHOW_R0_AS_0))
		return "#0";

	return regnames[reg];
}

offs_t mipsx_disassembler::disassemble(std::ostream& stream, offs_t pc, const data_buffer& opcodes, const data_buffer& params)
{
	const u32 opcode = opcodes.r32(pc);
	const u8 ty = get_ty(opcode);

	switch (ty)
	{
	case 0:
	{
		const u8 op = get_op(opcode);
		const int disp = util::sext(opcode, 16) * 4;
		const u32 basepc = pc + 8;
		const int src1 = get_src1(opcode);
		const int src2 = get_src2_dest(opcode);
		const std::string squash = get_sq(opcode) ? "sq" : "";

		switch (op)
		{
		case 1:
		{
			if ((src1 == 0) && (src2 == 0))
			{
				// beq #0, #0, offset is used as an alias for unconditional branch
				util::stream_format(stream, "bra%s 0x%08x", squash, basepc + disp);
			}
			else
			{
				util::stream_format(stream, "beq%s %s,%s,0x%08x", squash, get_regname(src1), get_regname(src2), basepc + disp);
			}
			break;
		}
		case 2:
		{
			util::stream_format(stream, "bhs%s %s,%s,0x%08x", squash, get_regname(src1), get_regname(src2), basepc + disp);
			break;
		}
		case 3:
		{
			util::stream_format(stream, "blt%s %s,%s,0x%08x", squash, get_regname(src1), get_regname(src2), basepc + disp);
			break;
		}
		case 5:
		{
			util::stream_format(stream, "bne%s %s,%s,0x%08x", squash, get_regname(src1), get_regname(src2), basepc + disp);
			break;
		}
		case 6:
		{
			util::stream_format(stream, "blo%s %s,%s,0x%08x", squash, get_regname(src1), get_regname(src2), basepc + disp);
			break;
		}
		case 7:
		{
			util::stream_format(stream, "bge%s %s,%s,0x%08x", squash, get_regname(src1), get_regname(src2), basepc + disp);
			break;
		}

		default:
		{
			util::stream_format(stream, "Unhandled TY0 (%08x)", opcode);
			break;
		}
		}
		break;
	}

	case 1:
	{
		u8 op = get_op(opcode);

		switch (op)
		{
		case 0:
		{
			const u16 comp = get_compute_compfunc(opcode);
			if (comp == 0x0e6)
			{
				u8 src1 = get_src1(opcode);

				if (src1 == 0)
				{
					util::stream_format(stream, "mstart %s,%s", get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
				}
				else
				{
					util::stream_format(stream, "illegal mstart form");
				}
			}
			else if (comp == 0x099)
			{
				util::stream_format(stream, "mstep %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x166)
			{
				util::stream_format(stream, "dstep %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else
			{
				util::stream_format(stream, "unknown TY1 subcase 0 (%02x, %02x, %02x %04x)", get_src1(opcode), get_src2_dest(opcode), get_compute_dest(opcode), get_compute_compfunc(opcode));
			}
			break;
		}

		case 1:
		{
			const u16 comp = get_compute_compfunc(opcode);

			if (comp == 0x080)
			{
				util::stream_format(stream, "rotlcb %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x0c0)
			{
				util::stream_format(stream, "rotlb %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if ((comp & 0xf80) == 0x200)
			{
				int shift = get_sh_amount(get_compute_compfunc(opcode) & 0x7f);
				util::stream_format(stream, "sh %s,%s,%s,#%d", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)), shift);
			}
			else if ((comp & 0xf80) == 0x100)
			{
				const u8 src2 = get_src2_dest(opcode);

				if (src2 == 0x0)
				{
					int shift = get_asr_amount((get_compute_compfunc(opcode) & 0x7f));
					util::stream_format(stream, "asr %s,%s,#%d", get_regname(get_src1(opcode)), get_regname(get_compute_dest(opcode)), shift);
				}
				else
				{
					util::stream_format(stream, "illegal asr form");
				}
			}
			else
			{
				util::stream_format(stream, "unknown TY1 subcase 1 (%02x, %02x, %02x %04x)", get_src1(opcode), get_src2_dest(opcode), get_compute_dest(opcode), get_compute_compfunc(opcode));
			}
			break;
		}

		case 4:
		{
			const u16 comp = get_compute_compfunc(opcode);

			if (comp == 0x00b)
			{
				util::stream_format(stream, "blc %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x00f)
			{
				if (get_src2_dest(opcode) == 0x00)
				{
					util::stream_format(stream, "not %s,%s", get_regname(get_src1(opcode)), get_regname(get_compute_dest(opcode)));
				}
				else
				{
					util::stream_format(stream, "illegal not form");
				}
			}
			else if (comp == 0x019)
			{
				u8 src2 = get_src2_dest(opcode);
				if (src2 == 0)
				{
					const u8 src1 = get_src1(opcode);
					const u8 dest = get_compute_dest(opcode);

					if ((src1 == 0) && (dest == 0))
					{
						util::stream_format(stream, "nop");
					}
					else
					{
						util::stream_format(stream, "mov %s,%s", get_regname(get_src1(opcode)), get_regname(get_compute_dest(opcode)));
					}
				}
				else
				{
					util::stream_format(stream, "add %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
				}
			}
			else if (comp == 0x01b)
			{
				util::stream_format(stream, "xor %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x023)
			{
				util::stream_format(stream, "and %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x026)
			{
				util::stream_format(stream, "subnc %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x03b)
			{
				util::stream_format(stream, "or %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else if (comp == 0x066)
			{
				util::stream_format(stream, "sub %s,%s,%s", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), get_regname(get_compute_dest(opcode)));
			}
			else
			{
				util::stream_format(stream, "unknown compute (%02x, %02x, %02x %04x)", get_src1(opcode), get_src2_dest(opcode), get_compute_dest(opcode), get_compute_compfunc(opcode));
			}
			break;
		}

		default:
		{
			util::stream_format(stream, "Unhandled TY1 (%08x)", opcode);
			break;
		}
		}
		break;

	}

	case 2:
	{
		const u8 op = get_op(opcode);
		const int imm17 = util::sext(opcode, 17);

		switch (op)
		{
		case 0:
		{
			// ld - Load
			// ld Offset[rSrc1], rDest
			util::stream_format(stream, "ld 0x%08x[%s],%s", imm17, get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)));
			break;
		}
		case 1:
		{
			// ldt - Load Through
			// ldt Offset[rSrc1], rDest
			util::stream_format(stream, "ldt 0x%08x[%s],%s", imm17, get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)));
			break;
		}
		case 2:
		{
			// st - Store
			// st Offset[rSrc1], rDest
			util::stream_format(stream, "st 0x%08x[%s],%s", imm17, get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)));
			break;
		}
		case 3:
		{
			// stt - Store Through
			// stt Offset[rSrc1], rDest
			util::stream_format(stream, "stt 0x%08x[%s],%s", imm17, get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)));
			break;
		}
		case 4:
		{
			// ldf - Load Floating Point
			// ldf Offset[rSrc1], rDest
			util::stream_format(stream, "ldf 0x%08x[%s],%s", imm17, get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)));
			break;

		}
		case 5: // movfrc or aluc
		{
			// this is just a suggested form
			const u8 c2 =   BIT(opcode, 0, 4);
			const u8 c1 =   BIT(opcode, 4, 4);
			const u8 func = BIT(opcode, 8, 6);
			const u8 op =   BIT(opcode, 14, 3);
			const u8 dest = get_src2_dest(opcode);
			const u8 src =  get_src1(opcode);

			if (src == 0)
			{
				util::stream_format(stream, "movfrc %s,(%02x,%02x,%02x,%02x)", get_regname(dest), op, func, c1, c2);
			}
			else
			{
				util::stream_format(stream, "illegal movfrc form");
			}
			break;

		}
		case 6:
		{
			// stf - Store Floating Point
			// stf Offset[rSrc1], rDest
			util::stream_format(stream, "stf 0x%08x[%s],%s", imm17, get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)));
			break;
		}
		case 7: // movtoc
		{
			// this is just a suggested form
			const u8 c2 =   BIT(opcode, 0, 4);
			const u8 c1 =   BIT(opcode, 4, 4);
			const u8 func = BIT(opcode, 8, 6);
			const u8 op =   BIT(opcode, 14, 3);
			const u8 dest = get_src2_dest(opcode);
			const u8 src =  get_src1(opcode);

			if (src == 0)
			{
				util::stream_format(stream, "movtoc %s,(%02x,%02x,%02x,%02x)", get_regname(dest), op, func, c1, c2);
			}
			else
			{
				//util::stream_format(stream, "illegal movtoc form");
				// this form appears to be used
				util::stream_format(stream, "movtoc %s,(%02x,%02x,%02x,%02x) (src1 == %s)", get_regname(dest), op, func, c1, c2, get_regname(src));
			}
			break;

		}
		default:
		{
			util::stream_format(stream, "Unhandled TY2 (%08x)", opcode);
			break;
		}
		}
		break;
	}

	case 3:
	{
		const u8 op = get_op(opcode);

		switch (op)
		{
		case 0:
		{
			// jspci - Jump Indexed and Store PC
			// jspci rSrc1,#Immed,Rdest
			const int imm17 = util::sext(opcode, 17) * 2;
			util::stream_format(stream, "jspci %s,#%02x,%s", get_regname(get_src1(opcode)), imm17, get_regname(get_src2_dest(opcode)));
			break;
		}

		case 1:
		{
			// Halt and Spontaneously Combust
			if ((opcode & 0x07ffffff) == 0x07c00000)
			{
				util::stream_format(stream, "hsc");
			}
			else
			{
				util::stream_format(stream, "illegal hsc form");
			}
			break;
		}

		case 2:
		{
			// movtos - Move to Special Register
			// movtos rSrc1, SpecialReg

			// psw  001
			// md   010
			// pcm1 100
			const u16 comp = get_compute_compfunc(opcode);
			if ((comp & 0xffe) == 0x000)
			{
				u8 dest = get_src2_dest(opcode);

				if (dest == 0x00)
				{
					u8 src = get_src1(opcode);
					// TODO: name register?
					util::stream_format(stream, "movtos %s,%01x", get_regname(src), comp & 0x7);
				}
				else
				{
					util::stream_format(stream, "illegal movtos form");
				}
			}
			else
			{
				util::stream_format(stream, "illegal movtos form");
			}
			break;

		}

		case 3:
		{
			// movfrs - Move from Special Register
			// movfrs SpecialReg, rDest

			// psw  001
			// md   010
			// pcm4 100
			const u16 comp = get_compute_compfunc(opcode);
			if ((comp & 0xffe) == 0x000)
			{
				u8 src = get_src1(opcode);
				if (src == 0x00)
				{
					const u8 dest = get_src2_dest(opcode);
					// TODO: name register?
					util::stream_format(stream, "movfrs %01x,%s", comp & 0x7, get_regname(dest));
				}
				else
				{
					util::stream_format(stream, "illegal movfrs form");
				}
			}
			else
			{
				util::stream_format(stream, "illegal movfrs form");
			}
			break;

		}

		case 4:
		{
			const int imm17 = util::sext(opcode, 17);
			util::stream_format(stream, "addi %s,%s,0x%08x", get_regname(get_src1(opcode)), get_regname(get_src2_dest(opcode)), imm17);
			break;
		}

		case 5:
		{
			const u16 comp = get_compute_compfunc(opcode);
			if (comp == 0x03)
			{
				const u8 dest = get_src2_dest(opcode);
				const u8 src = get_src1(opcode);

				if ((src == 0x00) && (dest == 0x00))
				{
					util::stream_format(stream, "jpc");
				}
				else
				{
					util::stream_format(stream, "illegal jpc form");
				}
			}
			else
			{
				util::stream_format(stream, "illegal jpc form");
			}
			break;
		}

		case 6:
		{
			if ((opcode & 0x07fff807) == 0x00000003)
			{
				const int vector = (opcode & 0x000007f8) >> 3;
				util::stream_format(stream, "trap %02x", vector);
			}
			else
			{
				util::stream_format(stream, "illegal trap form");
			}
			break;
		}

		case 7:
		{
			const u16 comp = get_compute_compfunc(opcode);
			if (comp == 0x03)
			{
				const u8 dest = get_src2_dest(opcode);
				const u8 src = get_src1(opcode);

				if ((src == 0x00) && (dest == 0x00))
				{
					util::stream_format(stream, "jpcrs");
				}
				else
				{
					util::stream_format(stream, "illegal jpcrs form");
				}
			}
			else
			{
				util::stream_format(stream, "illegal jpcrs form");
			}
			break;
		}

		default:
		{
			util::stream_format(stream, "Unhandled TY3 (%08x)", opcode);
			break;
		}
		}
		break;
	}

	default:
	{
		util::stream_format(stream, "Unhandled (%08x)", opcode);
		break;
	}
	}

	return 4;
}
