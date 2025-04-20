// license:BSD-3-Clause
// copyright-holders:Angelo Salese

#ifndef MAME_CPU_SH_SH7032_H
#define MAME_CPU_SH_SH7032_H

#pragma once

#include "sh2.h"

class sh1_sh7032_device : public sh2_device
{
public:
	sh1_sh7032_device(const machine_config &mconfig, const char *_tag, device_t *_owner, uint32_t _clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void sh7032_map(address_map &map);

	uint16_t sh7032_r(offs_t offset);
	void sh7032_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	uint16_t m_sh7032_regs[0x200];
};

DECLARE_DEVICE_TYPE(SH1_SH7032, sh1_sh7032_device)

#endif // MAME_CPU_SH_SH7032_H
