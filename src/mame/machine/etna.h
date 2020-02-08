// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, Ash Wolf
/***************************************************************************

        Psion 5mx (EPOC R5) series ETNA peripheral

        Skeleton driver by Ryan Holtz, ported from work by Ash Wolf

****************************************************************************/

#ifndef MAME_MACHINE_ETNA_H
#define MAME_MACHINE_ETNA_H

#pragma once

#include "emu.h"

class etna_device : public device_t
{
public:
	etna_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock = 0U);

	DECLARE_WRITE8_MEMBER(regs_w);
	DECLARE_READ8_MEMBER(regs_r);

	DECLARE_WRITE_LINE_MEMBER(eeprom_cs_in);
	DECLARE_WRITE_LINE_MEMBER(eeprom_clk_in);
	DECLARE_WRITE_LINE_MEMBER(eeprom_data_in);

	auto eeprom_data_out() { return m_eeprom_data_out.bind(); }

private:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	enum
	{
		REG_UNK0,
		REG_UNK1,
		REG_UART_INT_STATUS,
		REG_UART_INT_MASK,
		REG_UART_BAUD_LO,
		REG_UART_BAUD_HI,
		REG_PCCD_INT_STATUS,
		REG_PCCD_INT_MASK,
		REG_INT_CLEAR,
		REG_SKT_VAR_A0,
		REG_SKT_VAR_A1,
		REG_SKT_CTRL,
		REG_WAKE1,
		REG_SKT_VAR_B0,
		REG_SKT_VAR_B1,
		REG_WAKE2
	};

	devcb_write_line m_eeprom_data_out;

	uint8_t m_prom_addr_count;
	uint16_t m_prom_addr;
	uint16_t m_prom_value;
	bool m_prom_cs;
	bool m_prom_clk;

	uint8_t m_pending_ints;

	uint8_t m_regs[0x10];
	uint8_t m_prom[0x80];
};

DECLARE_DEVICE_TYPE(ETNA, etna_device)

#endif // MAME_MACHINE_ETNA_H
