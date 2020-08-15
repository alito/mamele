// license: GPL-2.0+
// copyright-holders: Dirk Best
/***************************************************************************

    SAM Mouse Interface for SAM Coupe

***************************************************************************/

#ifndef MAME_BUS_SAMCOUPE_MOUSE_MOUSE_H
#define MAME_BUS_SAMCOUPE_MOUSE_MOUSE_H

#pragma once

#include "mouseport.h"


//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> sam_mouse_device

class sam_mouse_device : public device_t, public device_samcoupe_mouse_interface
{
public:
	// construction/destruction
	sam_mouse_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// from host
	virtual uint8_t read() override;

protected:
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

private:
	required_ioport m_io_buttons;
	required_ioport m_io_x;
	required_ioport m_io_y;

	emu_timer *m_reset;

	int m_mouse_index;
	uint8_t m_mouse_data[9];
	int m_mouse_x, m_mouse_y;
};

// device type definition
DECLARE_DEVICE_TYPE(SAM_MOUSE, sam_mouse_device)

#endif // MAME_BUS_SAMCOUPE_MOUSE_MOUSE_H
