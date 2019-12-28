// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*
 * cpc_rom.h
 * Amstrad CPC mountable ROM image device
 *
 */

#ifndef MAME_BUS_CPC_CPC_ROM_H
#define MAME_BUS_CPC_CPC_ROM_H

#pragma once

#include "cpcexp.h"

/*** ROM image device ***/

// ======================> cpc_rom_image_device

class cpc_rom_image_device : public device_t, public device_image_interface
{
public:
	// construction/destruction
	cpc_rom_image_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~cpc_rom_image_device();

	// image-level overrides
	virtual image_init_result call_load() override;
	virtual void call_unload() override;

	virtual iodevice_t image_type() const noexcept override { return IO_ROM; }

	virtual bool is_readable()  const noexcept override { return true; }
	virtual bool is_writeable() const noexcept override { return false; }
	virtual bool is_creatable() const noexcept override { return false; }
	virtual bool must_be_loaded() const noexcept override { return false; }
	virtual bool is_reset_on_load() const noexcept override { return true; }
	virtual const char *image_interface() const noexcept override { return "cpc_rom"; }
	virtual const char *file_extensions() const noexcept override { return "rom,bin"; }

	uint8_t* base() { return m_base.get(); }

protected:
	// device-level overrides
	virtual void device_start() override;

private:
	std::unique_ptr<uint8_t[]> m_base;
};


// device type definition
DECLARE_DEVICE_TYPE(CPC_ROMSLOT, cpc_rom_image_device)


/*** ROM box device ***/

class cpc_rom_device  : public device_t,
						public device_cpc_expansion_card_interface
{
public:
	// construction/destruction
	cpc_rom_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	uint8_t* base(uint8_t slot) { if(slot >=1 && slot <= 8) return m_rom[slot]->base(); else return nullptr; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

private:
	required_device_array<cpc_rom_image_device, 8> m_rom;
};

// device type definition
DECLARE_DEVICE_TYPE(CPC_ROM, cpc_rom_device)


#endif // MAME_BUS_CPC_CPC_ROM_H
