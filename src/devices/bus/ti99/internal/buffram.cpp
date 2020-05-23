// license:LGPL-2.1+
// copyright-holders:Michael Zapf
/*******************************************************************************
    Buffered SRAM

    Michael Zapf
    March 2020

*******************************************************************************/

#include "emu.h"
#include "buffram.h"

#define VERBOSE ( LOG_GENERAL )
#include "logmacro.h"

#define BUFFRAM_TAG "buffered_ram"

DEFINE_DEVICE_TYPE_NS(BUFF_RAM, bus::ti99::internal, buffered_ram_device, BUFFRAM_TAG, "Buffered SRAM")

namespace bus { namespace ti99 { namespace internal {


// ========== Buffered SRAM ============

buffered_ram_device::buffered_ram_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	:  device_t(mconfig, BUFF_RAM, tag, owner, clock),
	   device_nvram_interface(mconfig, *this),
	   m_buffered(true),
	   m_size(0),
	   m_mem(nullptr)
{
}

void buffered_ram_device::device_start()
{
	m_mem = std::make_unique<u8 []>(m_size);

	// register for state saving
	save_item(NAME(m_size));
	save_pointer(NAME(m_mem), m_size);
}

void buffered_ram_device::nvram_default()
{
	std::fill_n(m_mem.get(), m_size, 0x00);
}

void buffered_ram_device::nvram_read(emu_file &file)
{
	if (m_buffered)
		file.read(m_mem.get(), m_size);
}

void buffered_ram_device::nvram_write(emu_file &file)
{
	if (m_buffered)
		file.write(m_mem.get(), m_size);
}

} } } // end namespace bus::ti99::internal
