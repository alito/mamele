// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef MAME_BUS_MSX_CART_SUPER_SWANGI_H
#define MAME_BUS_MSX_CART_SUPER_SWANGI_H

#pragma once

#include "cartridge.h"


DECLARE_DEVICE_TYPE(MSX_CART_SUPER_SWANGI, msx_cart_super_swangi_device)


class msx_cart_super_swangi_device : public device_t, public msx_cart_interface
{
public:
	msx_cart_super_swangi_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	virtual void initialize_cartridge() override;

protected:
	// device-level overrides
	virtual void device_start() override { }

private:
	void bank_w(u8 data);

	memory_bank_creator m_rombank;
};


#endif // MAME_BUS_MSX_CART_SUPER_SWANGI_H
