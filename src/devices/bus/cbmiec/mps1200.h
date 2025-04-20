// license:BSD-3-Clause
// copyright-holders:AJR
/**********************************************************************

    Commodore MPS-1200 & MPS-1250 printers

**********************************************************************/

#ifndef MAME_BUS_CBMIEC_MPS1200_H
#define MAME_BUS_CBMIEC_MPS1200_H

#pragma once

#include "cbmiec.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> mps1200_device

class mps1200_device : public device_t, public device_cbm_iec_interface
{
public:
	// device type constructor
	mps1200_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	static constexpr feature_type unemulated_features() { return feature::PRINTER; }

protected:
	mps1200_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	// device_cbm_iec_interface overrides
	DECLARE_WRITE_LINE_MEMBER(cbm_iec_atn) override;
	DECLARE_WRITE_LINE_MEMBER(cbm_iec_data) override;
	DECLARE_WRITE_LINE_MEMBER(cbm_iec_reset) override;

private:
	void mem_map(address_map &map);
	void data_map(address_map &map);

	required_device<cpu_device> m_mpscpu;
};

// ======================> mps1250_device

class mps1250_device : public mps1200_device
{
public:
	// device type constructor
	mps1250_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;
};


// device type declarations
DECLARE_DEVICE_TYPE(MPS1200, mps1200_device)
DECLARE_DEVICE_TYPE(MPS1250, mps1250_device)

#endif // MAME_BUS_CBMIEC_MPS1200_H
