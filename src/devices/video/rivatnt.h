// license:BSD-3-Clause
// copyright-holders: Angelo Salese

#ifndef MAME_VIDEO_RIVATNT_H
#define MAME_VIDEO_RIVATNT_H

#pragma once

#include "machine/pci.h"
#include "video/pc_vga_nvidia.h"
#include "riva128.h"

class rivatnt_device : public riva128_device
{
public:
	rivatnt_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	rivatnt_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_start() override;
};

class rivatnt2_device : public rivatnt_device
{
public:
	rivatnt2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	rivatnt2_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_start() override;
};

class rivatnt2_ultra_device : public rivatnt2_device
{
public:
	rivatnt2_ultra_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};

class vanta_device : public rivatnt2_device
{
public:
	vanta_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};

class rivatnt2_model64_device : public rivatnt2_device
{
public:
	rivatnt2_model64_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	virtual const tiny_rom_entry *device_rom_region() const override;
};

DECLARE_DEVICE_TYPE(RIVATNT, rivatnt_device)
DECLARE_DEVICE_TYPE(RIVATNT2, rivatnt2_device)
DECLARE_DEVICE_TYPE(RIVATNT2_ULTRA, rivatnt2_ultra_device)
DECLARE_DEVICE_TYPE(VANTA, vanta_device)
DECLARE_DEVICE_TYPE(RIVATNT2_M64, rivatnt2_model64_device)


#endif
