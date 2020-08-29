// license:BSD-3-Clause
// copyright-holders:hap
/**********************************************************************

    Videopac+ C7420 Home Computer Module emulation

**********************************************************************/

#ifndef MAME_BUS_ODYSSEY2_HOMECOMP_H
#define MAME_BUS_ODYSSEY2_HOMECOMP_H

#pragma once

#include "slot.h"

#include "cpu/z80/z80.h"
#include "imagedev/cassette.h"
#include "machine/gen_latch.h"


// ======================> o2_homecomp_device

class o2_homecomp_device : public device_t,
						public device_o2_cart_interface
{
public:
	// construction/destruction
	o2_homecomp_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual void cart_init() override;

	virtual u8 read_rom04(offs_t offset) override { return m_rom[offset + 0x4000]; }
	virtual u8 read_rom0c(offs_t offset) override { return read_rom04(offset); }

	virtual void write_p1(u8 data) override;
	virtual void io_write(offs_t offset, u8 data) override;
	virtual u8 io_read(offs_t offset) override;
	virtual DECLARE_READ_LINE_MEMBER(t0_read) override { return m_latch[0]->pending_r(); }

private:
	required_device<cpu_device> m_maincpu;
	required_device_array<generic_latch_8_device, 2> m_latch;
	required_device<cassette_image_device> m_cass;

	void internal_io_w(offs_t offset, u8 data);
	u8 internal_io_r(offs_t offset);
	u8 internal_rom_r(offs_t offset) { return m_rom[offset]; }

	void homecomp_io(address_map &map);
	void homecomp_mem(address_map &map);

	u8 m_control = 0;
};

// device type definition
DECLARE_DEVICE_TYPE(O2_ROM_HOMECOMP, o2_homecomp_device)

#endif // MAME_BUS_ODYSSEY2_HOMECOMP_H
