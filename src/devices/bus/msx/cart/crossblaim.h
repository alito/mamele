// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef MAME_BUS_MSX_CART_CROSSBLAIM_H
#define MAME_BUS_MSX_CART_CROSSBLAIM_H

#pragma once

#include "cartridge.h"


DECLARE_DEVICE_TYPE(MSX_CART_CROSSBLAIM, msx_cart_crossblaim_device)


class msx_cart_crossblaim_device : public device_t, public msx_cart_interface
{
public:
	msx_cart_crossblaim_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void initialize_cartridge() override;

protected:
	// device-level overrides
	virtual void device_start() override { }

private:
	void mapper_write(u8 data);

	memory_bank_creator m_rombank;
};


#endif // MAME_BUS_MSX_CART_CROSSBLAIM_H
