// license:BSD-3-Clause
// copyright-holders:David Haywood

#ifndef MAME_MACHINE_ELAN_EU3A14SYS_H
#define MAME_MACHINE_ELAN_EU3A14SYS_H

#include "elan_eu3a05commonsys.h"

class elan_eu3a14sys_device : public elan_eu3a05commonsys_device, public device_memory_interface
{
public:
	elan_eu3a14sys_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	DECLARE_READ8_MEMBER(dma_trigger_r);
	DECLARE_WRITE8_MEMBER(dma_trigger_w);

	DECLARE_READ8_MEMBER(dma_param_r);
	DECLARE_WRITE8_MEMBER(dma_param_w);

	virtual void map(address_map& map) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual space_config_vector memory_space_config() const override;

private:
	const address_space_config      m_space_config;
	uint8_t m_dmaparams[9];
};



DECLARE_DEVICE_TYPE(ELAN_EU3A14_SYS, elan_eu3a14sys_device)

#endif // MAME_MACHINE_ELAN_EU3A14SYS_H
