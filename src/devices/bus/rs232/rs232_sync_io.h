// license:BSD-3-Clause
// copyright-holders: F. Ulivi
/*********************************************************************

    rs232_sync_io.h

    Synchronous I/O on RS232 port

*********************************************************************/

#ifndef MAME_BUS_RS232_RS232_SYNC_IO_H
#define MAME_BUS_RS232_RS232_SYNC_IO_H

#pragma once

#include "rs232.h"
#include "imagedev/bitbngr.h"

class rs232_sync_io_device : public device_t, public device_rs232_port_interface
{
public:
	// construction/destruction
	rs232_sync_io_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	virtual ~rs232_sync_io_device();

	virtual DECLARE_WRITE_LINE_MEMBER(input_txd) override;
	virtual DECLARE_WRITE_LINE_MEMBER(input_rts) override;

	DECLARE_WRITE_LINE_MEMBER(update_serial);

protected:
	// device-level overrides
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	required_device<bitbanger_device> m_stream;
	required_ioport m_rs232_baud;
	required_ioport m_rts_duplex;
	required_ioport m_txc_setting;
	required_ioport m_rxc_setting;

	emu_timer *m_clk_timer;
	bool m_clk;
	bool m_txd;
	bool m_rts;
	bool m_tx_enabled;
	bool m_rx_enabled;
	uint8_t m_tx_byte;
	uint8_t m_rx_byte;
	uint8_t m_tx_counter;
	uint8_t m_rx_counter;
};

// device type definitions
DECLARE_DEVICE_TYPE(RS232_SYNC_IO, rs232_sync_io_device)

#endif // MAME_BUS_RS232_RS232_SYNC_IO_H
