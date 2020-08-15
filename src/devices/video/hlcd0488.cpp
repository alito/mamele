// license:BSD-3-Clause
// copyright-holders:Wilbert Pol, hap
/*

Hughes HLCD 0488 LCD Driver
16*16 segment outputs, direct drive

TODO:
- should all control be edge-triggered?
- output polarity flip

*/

#include "emu.h"
#include "video/hlcd0488.h"


DEFINE_DEVICE_TYPE(HLCD0488, hlcd0488_device, "hlcd0488", "Hughes HLCD 0488 LCD Driver")

//-------------------------------------------------
//  constructor
//-------------------------------------------------

hlcd0488_device::hlcd0488_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock) :
	device_t(mconfig, HLCD0488, tag, owner, clock),
	m_write_cols(*this)
{
	memset(m_latch, 0, 8);
	memset(m_hold, 0, 8);
}


//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void hlcd0488_device::device_start()
{
	m_write_cols.resolve_safe();
	m_sync_timer = timer_alloc();

	// register for savestates
	save_item(NAME(m_latch_pulse));
	save_item(NAME(m_latch_pulse_prev));
	save_item(NAME(m_data_clk));
	save_item(NAME(m_data_clk_prev));
	save_item(NAME(m_data));
	save_item(NAME(m_count));
	save_item(NAME(m_latch));
	save_item(NAME(m_hold));
}


//-------------------------------------------------
//  handlers
//-------------------------------------------------

void hlcd0488_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	// Latch pulse, when high, resets the %8 latch address counter
	if (m_latch_pulse)
		m_count = 0;

	// The addressed latches load when -Data Clk is low
	if (!m_data_clk)
		m_latch[m_count] = m_data;

	// The latch address counter is incremented on rising edges of -Data Clk
	if (!m_data_clk_prev && m_data_clk && !m_latch_pulse)
		m_count = (m_count + 1) & 7;

	// A parallel transfer of data from the addressed latches to the holding latches occurs
	// whenever Latch Pulse is high and -Data Clk is high
	if (m_latch_pulse && m_data_clk)
	{
		for (int i = 0; i < 8; i++)
			m_hold[i] = m_latch[i];

		u16 row = (m_hold[0] << 12) | (m_hold[1] << 8) | (m_hold[2] << 4) | m_hold[3];
		u16 col = (m_hold[4] << 12) | (m_hold[5] << 8) | (m_hold[6] << 4) | m_hold[7];
		m_write_cols(row, col);
	}

	m_latch_pulse_prev = m_latch_pulse;
	m_data_clk_prev = m_data_clk;
}

WRITE_LINE_MEMBER(hlcd0488_device::latch_pulse_w)
{
	m_latch_pulse = state ? 1 : 0;
	m_sync_timer->adjust(attotime::zero);
}

WRITE_LINE_MEMBER(hlcd0488_device::data_clk_w)
{
	m_data_clk = state ? 1 : 0;
	m_sync_timer->adjust(attotime::zero);
}

void hlcd0488_device::data_w(u8 data)
{
	m_data = data & 0xf;
	m_sync_timer->adjust(attotime::zero);
}
