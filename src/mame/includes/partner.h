// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/*****************************************************************************
 *
 * includes/partner.h
 *
 ****************************************************************************/

#ifndef MAME_INCLUDES_PARTNER_H
#define MAME_INCLUDES_PARTNER_H

#pragma once

#include "includes/radio86.h"

#include "imagedev/floppy.h"
#include "machine/i8255.h"
#include "machine/wd_fdc.h"
#include "machine/ram.h"

class partner_state : public radio86_state
{
public:
	partner_state(const machine_config &mconfig, device_type type, const char *tag)
		: radio86_state(mconfig, type, tag)
		, m_ram(*this, RAM_TAG)
		, m_fdc(*this, "fdc")
		, m_bank(*this, "bank%u", 1U)
	{ }

	void init_partner();
	void partner(machine_config &config);

private:
	u8 floppy_r(offs_t offset);
	void floppy_w(offs_t offset, u8 data);
	void win_memory_page_w(u8 data);
	void mem_page_w(u8 data);

	I8275_DRAW_CHARACTER_MEMBER(display_pixels);

	DECLARE_FLOPPY_FORMATS( floppy_formats );

	void mem_map(address_map &map);

	void window_1(uint8_t bank_num, uint16_t offset,uint8_t *rom);
	void window_2(uint8_t bank_num, uint16_t offset,uint8_t *rom);
	void iomap_bank(uint8_t *rom);
	void bank_switch();

	u8 m_mem_page;
	u8 m_win_mem_page;

	required_device<ram_device> m_ram;
	required_device<fd1793_device> m_fdc;
	required_memory_bank_array<13> m_bank;

	void machine_reset() override;
	void machine_start() override;
};


#endif // MAME_INCLUDES_PARTNER_H
