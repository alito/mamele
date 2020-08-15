// license:BSD-3-Clause
// copyright-holders:usernameak
/**********************************************************************

    GRiD 2102 Portable Floppy emulation

**********************************************************************/

#ifndef MAME_BUS_IEEE488_GRID2102_H
#define MAME_BUS_IEEE488_GRID2102_H

#pragma once

#include "ieee488.h"

#include <queue>
#include <vector>


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> grid210x_device
class grid210x_device :  public device_t,
						public device_ieee488_interface,
						public device_image_interface
{
public:
	// construction/destruction
	grid210x_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, int bus_addr, uint8_t *identify_response, attotime read_delay = attotime::from_msec(5));

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// device_ieee488_interface overrides
	virtual void ieee488_eoi(int state) override;
	virtual void ieee488_dav(int state) override;
	virtual void ieee488_nrfd(int state) override;
	virtual void ieee488_ndac(int state) override;
	virtual void ieee488_ifc(int state) override;
	virtual void ieee488_srq(int state) override;
	virtual void ieee488_atn(int state) override;
	virtual void ieee488_ren(int state) override;

	// image-level overrides
	virtual iodevice_t image_type() const noexcept override { return IO_FLOPPY; }

	virtual bool is_readable()  const noexcept override { return true; }
	virtual bool is_writeable() const noexcept override { return true; }
	virtual bool is_creatable() const noexcept override { return false; }
	virtual bool must_be_loaded() const noexcept override { return false; }
	virtual bool is_reset_on_load() const noexcept override { return false; }
	virtual const char *file_extensions() const noexcept override { return "img"; }

	void accept_transfer();
	void update_ndac(int atn);
private:
	int m_gpib_loop_state;
	int m_floppy_loop_state;
	uint8_t m_last_recv_byte;
	int m_last_recv_eoi;
	int m_last_recv_atn;
	uint8_t m_byte_to_send;
	int m_send_eoi;
	bool listening, talking, serial_polling;
	bool has_srq;
	uint8_t serial_poll_byte;
	uint32_t floppy_sector_number;
	int bus_addr;
	uint8_t *identify_response_ptr;
	std::vector<uint8_t> m_data_buffer;
	std::queue<uint8_t> m_output_data_buffer;
	uint16_t io_size;
	emu_timer *m_delay_timer;
protected:
	attotime read_delay;
};

class grid2102_device : public grid210x_device {
public:
	// construction/destruction
	grid2102_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
private:
	static uint8_t identify_response[];
};

class grid2101_floppy_device : public grid210x_device {
public:
	// construction/destruction
	grid2101_floppy_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
private:
	static uint8_t identify_response[];
};

class grid2101_hdd_device : public grid210x_device {
public:
	// construction/destruction
	grid2101_hdd_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// image-level overrides
	virtual iodevice_t image_type() const noexcept override { return IO_HARDDISK; }

private:
	static uint8_t identify_response[];
};

// device type definition
DECLARE_DEVICE_TYPE(GRID2102, grid2102_device)
DECLARE_DEVICE_TYPE(GRID2101_FLOPPY, grid2101_floppy_device)
DECLARE_DEVICE_TYPE(GRID2101_HDD, grid2101_hdd_device)

#endif // MAME_BUS_IEEE488_GRID2102_H
