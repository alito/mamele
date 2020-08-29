// license:BSD-3-Clause
// copyright-holders:R. Belmont
/*********************************************************************

    cmsscsi.h

    Implementation of the CMS SCSI II Card

*********************************************************************/

#ifndef MAME_BUS_A2BUS_CMSSCSI_H
#define MAME_BUS_A2BUS_CMSSCSI_H

#pragma once

#include "a2bus.h"
#include "machine/ncr5380n.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class a2bus_cmsscsi_device:
	public device_t,
	public device_a2bus_card_interface
{
public:
	// construction/destruction
	a2bus_cmsscsi_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_WRITE_LINE_MEMBER( drq_w );

protected:
	a2bus_cmsscsi_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	// overrides of standard a2bus slot functions
	virtual uint8_t read_c0nx(uint8_t offset) override;
	virtual void write_c0nx(uint8_t offset, uint8_t data) override;
	virtual uint8_t read_cnxx(uint8_t offset) override;
	virtual void write_cnxx(uint8_t offset, uint8_t data) override;
	virtual uint8_t read_c800(uint16_t offset) override;
	virtual void write_c800(uint16_t offset, uint8_t data) override;

	required_device<ncr5380n_device> m_ncr5380;
	required_device<nscsi_bus_device> m_scsibus;
	required_region_ptr<u8> m_rom;

private:
	uint8_t m_ram[2048];    // 2K SRAM chip on the card, only 128 bytes are used?
	int m_rombank;
};

// device type definition
DECLARE_DEVICE_TYPE(A2BUS_CMSSCSI, a2bus_cmsscsi_device)

#endif // MAME_BUS_A2BUS_CMSSCSI_H
