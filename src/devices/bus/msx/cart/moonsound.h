// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef MAME_BUS_MSX_CART_MOONSOUND_H
#define MAME_BUS_MSX_CART_MOONSOUND_H

#pragma once

#include "cartridge.h"
#include "sound/ymopl.h"


DECLARE_DEVICE_TYPE(MSX_CART_MOONSOUND, msx_cart_moonsound_device)


class msx_cart_moonsound_device : public device_t, public msx_cart_interface
{
public:
	msx_cart_moonsound_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

private:
	DECLARE_WRITE_LINE_MEMBER(irq_w);
	void write_ymf278b_pcm(offs_t offset, u8 data);
	u8 read_ymf278b_pcm(offs_t offset);
	u8 read_c0();
	void ymf278b_map(address_map &map);

	required_device<ymf278b_device> m_ymf278b;
};


#endif // MAME_BUS_MSX_CART_MOONSOUND_H
