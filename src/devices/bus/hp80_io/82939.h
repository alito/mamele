// license:BSD-3-Clause
// copyright-holders: F. Ulivi
/*********************************************************************

    82939.h

    82939 module (RS232 interface)

*********************************************************************/

#ifndef MAME_BUS_HP80_IO_82939_H
#define MAME_BUS_HP80_IO_82939_H

#pragma once

#include "hp80_io.h"
#include "cpu/mcs48/mcs48.h"
#include "machine/1mb5.h"
#include "machine/ins8250.h"
#include "bus/rs232/rs232.h"

class hp82939_io_card_device : public device_t, public device_hp80_io_interface
{
public:
	// construction/destruction
	hp82939_io_card_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~hp82939_io_card_device();

protected:
	virtual void device_start() override;

	// device-level overrides
	virtual ioport_constructor device_input_ports() const override;
	virtual const tiny_rom_entry *device_rom_region() const override;
	virtual void device_add_mconfig(machine_config &config) override;

	virtual void install_read_write_handlers(address_space& space , uint16_t base_addr) override;

	virtual void inten() override;
	virtual void clear_service() override;

private:
	required_device<i8049_device> m_cpu;
	required_device<hp_1mb5_device> m_translator;
	required_device<rs232_port_device> m_rs232;
	required_device<ins8250_device> m_uart;
	required_ioport m_sw12;

	DECLARE_READ8_MEMBER(p1_r);
	DECLARE_WRITE8_MEMBER(p1_w);
	DECLARE_READ8_MEMBER(p2_r);
	DECLARE_READ8_MEMBER(cpu_r);
	DECLARE_WRITE8_MEMBER(cpu_w);
	void cpu_io_map(address_map &map);
};

// device type definition
DECLARE_DEVICE_TYPE(HP82939_IO_CARD, hp82939_io_card_device)

#endif // MAME_BUS_HP80_IO_82939_H
