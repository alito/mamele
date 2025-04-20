// license:BSD-3-Clause
// copyright-holders: Angelo Salese

#ifndef MAME_BUS_ISA_SVGA_PARADISE_H
#define MAME_BUS_ISA_SVGA_PARADISE_H

#pragma once

#include "isa.h"
#include "video/pc_vga_paradise.h"

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class isa16_pvga1a_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_pvga1a_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<pvga1a_vga_device> m_vga;
};

class isa16_pvga1a_jk_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_pvga1a_jk_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<pvga1a_vga_device> m_vga;
};

class isa8_wd90c90_jk_device :
		public device_t,
		public device_isa8_card_interface
{
public:
	// construction/destruction
	isa8_wd90c90_jk_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<pvga1a_vga_device> m_vga;
};

class isa16_wd90c00_jk_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c00_jk_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c00_vga_device> m_vga;
};

class isa16_wd90c11_lr_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c11_lr_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c11a_vga_device> m_vga;
};

class isa16_wd90c30_lr_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c30_lr_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c30_vga_device> m_vga;
};

class isa16_wd90c31_lr_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c31_lr_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c31_vga_device> m_vga;
};

class isa16_wd90c31a_lr_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c31a_lr_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c31_vga_device> m_vga;
};

class isa16_wd90c31a_zs_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c31a_zs_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c31_vga_device> m_vga;
};

class isa16_wd90c33_zz_device :
		public device_t,
		public device_isa16_card_interface
{
public:
	// construction/destruction
	isa16_wd90c33_zz_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual const tiny_rom_entry *device_rom_region() const override;

	void io_isa_map(address_map &map);

private:
	required_device<wd90c33_vga_device> m_vga;
};

// device type definition
DECLARE_DEVICE_TYPE(ISA16_PVGA1A,        isa16_pvga1a_device)
DECLARE_DEVICE_TYPE(ISA16_PVGA1A_JK,     isa16_pvga1a_jk_device)
DECLARE_DEVICE_TYPE(ISA8_WD90C90_JK,     isa8_wd90c90_jk_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C00_JK,    isa16_wd90c00_jk_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C11_LR,    isa16_wd90c11_lr_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C30_LR,    isa16_wd90c30_lr_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C31_LR,    isa16_wd90c31_lr_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C31A_LR,   isa16_wd90c31a_lr_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C31A_ZS,   isa16_wd90c31a_zs_device)
DECLARE_DEVICE_TYPE(ISA16_WD90C33_ZZ,    isa16_wd90c33_zz_device)


#endif // MAME_BUS_ISA_SVGA_PARADISE_H
