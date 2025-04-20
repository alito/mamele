// license:BSD-3-Clause
// copyright-holders:Curt Coder, smf
/**********************************************************************

    Z80 Video Pak cartridge emulation

**********************************************************************/

#ifndef MAME_BUS_C64_Z80VIDEOPAK_H
#define MAME_BUS_C64_Z80VIDEOPAK_H

#pragma once


#include "cpm.h"
#include "video/mc6845.h"
#include "emupal.h"



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> c64_z80videopak_device

class c64_z80videopak_device : public c64_cpm_cartridge_device
{
public:
	// construction/destruction
	c64_z80videopak_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	// device_c64_expansion_card_interface overrides
	virtual uint8_t c64_cd_r(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;
	virtual void c64_cd_w(offs_t offset, uint8_t data, int sphi2, int ba, int roml, int romh, int io1, int io2) override;

private:
	MC6845_UPDATE_ROW( crtc_update_row );

	required_device<hd6845s_device> m_crtc;
	required_device<palette_device> m_palette;
	required_memory_region m_char_rom;
	memory_share_creator<uint8_t> m_ram;
	int m_case;
};


// device type definition
DECLARE_DEVICE_TYPE(C64_Z80VIDEOPAK, c64_z80videopak_device)


#endif // MAME_BUS_C64_Z80VIDEOPAK_H
