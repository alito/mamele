// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    PEDL Multiform Z80 2nd processor

    http://chrisacorns.computinghistory.org.uk/8bit_Upgrades/Technomatic_MultiformZ80.html

**********************************************************************/


#ifndef MAME_BUS_BBC_1MHZBUS_MULTIFORM_H
#define MAME_BUS_BBC_1MHZBUS_MULTIFORM_H

#include "1mhzbus.h"
#include "cpu/z80/z80.h"
#include "machine/gen_latch.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> bbc_multiform_device

class bbc_multiform_device :
	public device_t,
	public device_bbc_1mhzbus_interface
{
public:
	// construction/destruction
	bbc_multiform_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ8_MEMBER( mem_r );
	DECLARE_WRITE8_MEMBER( mem_w );
	DECLARE_WRITE8_MEMBER( rom_disable_w );

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual uint8_t fred_r(offs_t offset) override;
	virtual void fred_w(offs_t offset, uint8_t data) override;

private:
	IRQ_CALLBACK_MEMBER( irq_callback );

	required_device<cpu_device> m_z80;
	required_device_array<generic_latch_8_device, 2> m_host_latch;
	required_device_array<generic_latch_8_device, 2> m_parasite_latch;
	required_memory_region m_osm;

	void z80_mem(address_map &map);
	void z80_io(address_map &map);

	std::unique_ptr<uint8_t[]> m_ram;
	bool m_rom_enabled;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_MULTIFORM, bbc_multiform_device)


#endif /* MAME_BUS_BBC_1MHZBUS_MULTIFORM_H */
