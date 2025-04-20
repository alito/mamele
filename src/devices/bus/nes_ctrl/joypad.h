// license:BSD-3-Clause
// copyright-holders:Fabio Priuli
/**********************************************************************

    Nintendo Family Computer & Entertainment System Joypads

**********************************************************************/

#ifndef MAME_BUS_NES_CTRL_JOYPAD_H
#define MAME_BUS_NES_CTRL_JOYPAD_H

#pragma once

#include "ctrl.h"

INPUT_PORTS_EXTERN( nes_joypad );

//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> nes_joypad_device

class nes_joypad_device : public device_t,
							public device_nes_control_port_interface
{
public:
	// construction/destruction
	nes_joypad_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	virtual ioport_constructor device_input_ports() const override;

protected:
	nes_joypad_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	// device-level overrides
	virtual void device_start() override;

	virtual u8 read_bit0() override;
	virtual void write(u8 data) override;

	required_ioport m_joypad;
	u32 m_latch;  // wider than standard joypad's 8-bit latch to accomodate subclass devices
};


// ======================> nes_fcpadexp_device

class nes_fcpadexp_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_fcpadexp_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	nes_fcpadexp_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	virtual u8 read_bit0() override { return 0; }
	virtual u8 read_exp(offs_t offset) override;
};


// ======================> nes_fcpad2_device

class nes_fcpad2_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_fcpad2_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	virtual ioport_constructor device_input_ports() const override;

protected:
	virtual u8 read_bit2() override;

private:
	required_ioport m_mic;
};


// ======================> nes_ccpadl_device

class nes_ccpadl_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_ccpadl_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	virtual ioport_constructor device_input_ports() const override;
};


// ======================> nes_ccpadr_device

class nes_ccpadr_device : public nes_joypad_device
{
public:
	// construction/destruction
	nes_ccpadr_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

	virtual ioport_constructor device_input_ports() const override;
};


// ======================> nes_arcstick_device

class nes_arcstick_device : public nes_fcpadexp_device
{
public:
	// construction/destruction
	nes_arcstick_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual u8 read_exp(offs_t offset) override;
	virtual void write(u8 data) override;

	required_device<nes_control_port_device> m_daisychain;
	required_ioport m_cfg;
};


// device type definition
DECLARE_DEVICE_TYPE(NES_JOYPAD,         nes_joypad_device)
DECLARE_DEVICE_TYPE(NES_FCPAD_EXP,      nes_fcpadexp_device)
DECLARE_DEVICE_TYPE(NES_FCPAD_P2,       nes_fcpad2_device)
DECLARE_DEVICE_TYPE(NES_CCPAD_LEFT,     nes_ccpadl_device)
DECLARE_DEVICE_TYPE(NES_CCPAD_RIGHT,    nes_ccpadr_device)
DECLARE_DEVICE_TYPE(NES_ARCSTICK,       nes_arcstick_device)

#endif // MAME_BUS_NES_CTRL_JOYPAD_H
