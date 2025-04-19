// license:BSD-3-Clause
// copyright-holders: kmg, Fabio Priuli
/***********************************************************************************************************


 NES/Famicom cartridge emulation for Sealie Computing and related PCBs


 Here we emulate the following homebrew PCBs

 * SEALIE RET-CUFROM [mapper 29]
 * SEALIE UNROM 512 [mapper 30]

 ***********************************************************************************************************/


#include "emu.h"
#include "sealie.h"


#ifdef NES_PCB_DEBUG
#define VERBOSE 1
#else
#define VERBOSE 0
#endif

#define LOG_MMC(x) do { if (VERBOSE) logerror x; } while (0)


//-------------------------------------------------
//  constructor
//-------------------------------------------------

DEFINE_DEVICE_TYPE(NES_CUFROM,   nes_cufrom_device,   "nes_cufrom",   "NES Cart Sealie RET-CUFROM PCB")
DEFINE_DEVICE_TYPE(NES_UNROM512, nes_unrom512_device, "nes_unrom512", "NES Cart Sealie UNROM 512 PCB")


nes_cufrom_device::nes_cufrom_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_nrom_device(mconfig, NES_CUFROM, tag, owner, clock)
{
}

nes_unrom512_device::nes_unrom512_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: nes_nrom_device(mconfig, NES_UNROM512, tag, owner, clock)
{
}



void nes_cufrom_device::pcb_reset()
{
	prg16_89ab(0);
	prg16_cdef(m_prg_chunks - 1);
	chr8(0, CHRRAM);
}

void nes_unrom512_device::pcb_reset()
{
	prg16_89ab(0);
	prg16_cdef(m_prg_chunks - 1);
	chr8(0, CHRRAM);
	if (m_pcb_ctrl_mirror)
		set_nt_mirroring(PPU_MIRROR_LOW);
}


/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

/*-------------------------------------------------

 Sealie RET-CUFROM board

 Games: Glider (only?)

 This homebrew mapper supports 8x16k PRG banks at 0x8000,
 8k WRAM at 0x6000, and 4x8k VRAM banks. PRG is stored on
 flash ROM, though unlike mapper 30 it doesn't appear to
 be self-flashable, only through an external tool.

 iNES: mapper 29

 In MAME: Supported.

 -------------------------------------------------*/

void nes_cufrom_device::write_h(offs_t offset, u8 data)
{
	LOG_MMC(("cufrom write_h, offset: %04x, data: %02x\n", offset, data));

	prg16_89ab((data >> 2) & 0x07);
	chr8(data & 0x03, CHRRAM);
}

/*-------------------------------------------------

 Sealie UNROM 512 board

 Games: Battle Kid 1 & 2, E.T., many more

 This board has several variations and jumper configurations.
 Currently we only support the Sealie nonflashable config
 with 32x16k PRG banks at 0x8000, 4x8k VRAM, and three
 mirroring configs (H, V, or PCB selected 1-screen modes).

 iNES: mapper 30

 In MAME: Preliminary partial support.

 -------------------------------------------------*/

void nes_unrom512_device::write_h(offs_t offset, u8 data)
{
	LOG_MMC(("unrom512 write_h, offset: %04x, data: %02x\n", offset, data));

	// this pcb is subject to bus conflict in its nonflashable configuration
	data = account_bus_conflict(offset, data);

	if (m_pcb_ctrl_mirror)
		set_nt_mirroring(BIT(data, 7) ? PPU_MIRROR_HIGH : PPU_MIRROR_LOW);
	prg16_89ab(data & 0x1f);
	chr8((data >> 5) & 0x03, CHRRAM);
}
