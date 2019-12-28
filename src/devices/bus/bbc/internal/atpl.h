// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    ATPL Sidewise ROM/RAM Expansion board
    https://www.retro-kit.co.uk/page.cfm/content/ATPL-Sidewise-Sideways-ROM-board/

    ATPL Sidewise+
    http://chrisacorns.computinghistory.org.uk/8bit_Upgrades/ATPL_Sidewise+.html

**********************************************************************/


#ifndef MAME_BUS_BBC_INTERNAL_ATPL_H
#define MAME_BUS_BBC_INTERNAL_ATPL_H

#include "internal.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class bbc_atplsw_device :
	public device_t,
	public device_bbc_internal_interface
{
public:
	// construction/destruction
	bbc_atplsw_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual ioport_constructor device_input_ports() const override;

	virtual bool overrides_rom() override { return true; }
	virtual void romsel_w(offs_t offset, uint8_t data) override { m_romsel = data & 0x0f; }
	virtual uint8_t paged_r(offs_t offset) override;
	virtual void paged_w(offs_t offset, uint8_t data) override;

private:
	optional_device_array<bbc_romslot_device, 16> m_rom;
	required_ioport m_wp;

	uint8_t m_romsel;
};


class bbc_atplswp_device :
	public device_t,
	public device_bbc_internal_interface
{
public:
	// construction/destruction
	bbc_atplswp_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

	virtual bool overrides_rom() override { return true; }
	virtual void romsel_w(offs_t offset, uint8_t data) override { m_romsel = data & 0x0f; }
	virtual uint8_t paged_r(offs_t offset) override;
	virtual void paged_w(offs_t offset, uint8_t data) override;

private:
	optional_device_array<bbc_romslot_device, 16> m_rom;

	uint8_t m_romsel;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_ATPLSW, bbc_atplsw_device);
DECLARE_DEVICE_TYPE(BBC_ATPLSWP, bbc_atplswp_device);


#endif /* MAME_BUS_BBC_INTERNAL_ATPL_H */
