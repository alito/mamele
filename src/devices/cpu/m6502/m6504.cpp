// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    m6504.c

    MOS Technology 6502, NMOS variant with reduced address bus

***************************************************************************/

#include "emu.h"
#include "m6504.h"

DEFINE_DEVICE_TYPE(M6504, m6504_device, "m6504", "MOS Technology 6504")

m6504_device::m6504_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	m6502_device(mconfig, M6504, tag, owner, clock)
{
	program_config.m_addr_width = 13;
	sprogram_config.m_addr_width = 13;
}
