// license:BSD-3-Clause
// copyright-holders:Nigel Barnes
/**********************************************************************

    MasterSD BBC Master SD Cartridge

**********************************************************************/

#ifndef MAME_BUS_BBC_CART_MASTERSD_H
#define MAME_BUS_BBC_CART_MASTERSD_H

#include "slot.h"
#include "machine/spi_sdcard.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

class bbc_mastersd_device : public device_t, public device_bbc_cart_interface
{
public:
	// construction/destruction
	bbc_mastersd_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// optional information overrides
	virtual void device_add_mconfig(machine_config &config) override;

	// electron_cart_interface overrides
	virtual uint8_t read(offs_t offset, int infc, int infd, int romqa, int oe, int oe2) override;
	virtual void write(offs_t offset, uint8_t data, int infc, int infd, int romqa, int oe, int oe2) override;

private:
	required_device<spi_sdcard_device> m_sdcard;

	TIMER_CALLBACK_MEMBER(spi_clock);

	emu_timer *m_spi_clock;
	bool m_spi_clock_state;
	bool m_spi_clock_sysclk;
	int m_spi_clock_cycles;
	int m_in_bit;
	uint8_t m_in_latch;
	uint8_t m_out_latch;

	std::unique_ptr<uint8_t[]> m_ram;
};


// device type definition
DECLARE_DEVICE_TYPE(BBC_MASTERSD, bbc_mastersd_device)


#endif // MAME_BUS_BBC_CART_MASTERSD_H
