// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    Solidisk Twomeg 128K Shadow and Sideways ROM/RAM board

**********************************************************************/


#ifndef MAME_BUS_BBC_INTERNAL_STL2M128_H
#define MAME_BUS_BBC_INTERNAL_STL2M128_H

#include "internal.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class bbc_stl2m128_device :
	public device_t,
	public device_bbc_internal_interface
{
public:
	// construction/destruction
	bbc_stl2m128_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	virtual bool overrides_rom() override { return true; }
	virtual bool overrides_ram() override { return true; }
	virtual uint8_t ram_r(offs_t offset) override;
	virtual void ram_w(offs_t offset, uint8_t data) override;
	virtual void romsel_w(offs_t offset, uint8_t data) override;
	virtual uint8_t paged_r(offs_t offset) override;
	virtual void paged_w(offs_t offset, uint8_t data) override;

	optional_device_array<bbc_romslot_device, 16> m_rom;
	required_ioport m_wp;

	std::unique_ptr<uint8_t[]> m_ram;

	uint8_t m_romsel;
	uint8_t m_shadow;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_STL2M128, bbc_stl2m128_device);


#endif /* MAME_BUS_BBC_INTERNAL_STL2M128_H */
