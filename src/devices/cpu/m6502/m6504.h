// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    m6504.h

    MOS Technology 6502, NMOS variant with reduced address bus

***************************************************************************/

#ifndef MAME_CPU_M6502_M6504_H
#define MAME_CPU_M6502_M6504_H

#include "m6502.h"

class m6504_device : public m6502_device {
public:
	m6504_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


enum {
	M6504_IRQ_LINE = m6502_device::IRQ_LINE,
	M6504_NMI_LINE = m6502_device::NMI_LINE,
	M6504_SET_OVERFLOW = m6502_device::V_LINE
};

DECLARE_DEVICE_TYPE(M6504, m6504_device);

#endif // MAME_CPU_M6502_M6504_H
