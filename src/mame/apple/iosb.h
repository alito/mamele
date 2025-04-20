// license:BSD-3-Clause
// copyright-holders:R. Belmont

#ifndef MAME_APPLE_IOSB_H
#define MAME_APPLE_IOSB_H

#pragma once

#include "macrtc.h"

#include "cpu/m68000/m68040.h"
#include "machine/6522via.h"
#include "machine/applefdintf.h"
#include "machine/ncr53c90.h"
#include "machine/swim2.h"
#include "sound/asc.h"
#include "speaker.h"

// ======================> iosb_device

class iosb_device :  public device_t
{
public:
	// construction/destruction
	iosb_device(const machine_config &mconfig, const char *tag, device_t *owner)
		: iosb_device(mconfig, tag, owner, (uint32_t)0)
	{
	}

	iosb_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// interface routines
	auto write_adb_st() { return m_adb_st.bind(); } // ADB state
	auto write_cb1() { return m_cb1.bind(); }   // ADB clock
	auto write_cb2() { return m_cb2.bind(); }   // ADB data

	auto read_pa1()  { return m_pa1.bind(); }   // ID bits
	auto read_pa2()  { return m_pa2.bind(); }
	auto read_pa4()  { return m_pa4.bind(); }
	auto read_pa6()  { return m_pa6.bind(); }

	void map(address_map &map);

	template <typename... T> void set_maincpu_tag(T &&... args) { m_maincpu.set_tag(std::forward<T>(args)...); }
	template <typename... T> void set_scsi_tag(T &&... args) { m_ncr.set_tag(std::forward<T>(args)...); }

	void pb3_w(int state) { m_adb_interrupt = state; }
	void cb1_w(int state);  // ADB clock
	void cb2_w(int state);  // ADB data
	void scsi_irq_w(int state);
	void scc_irq_w(int state);

	template <u8 mask>
	void via2_irq_w(int state);

	void via_sync();

	void scsi_drq_w(int state);
	u8 turboscsi_r(offs_t offset);
	void turboscsi_w(offs_t offset, u8 data);
	u32 turboscsi_dma_r(offs_t offset, u32 mem_mask);
	void turboscsi_dma_w(offs_t offset, u32 data, u32 mem_mask);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	devcb_write8 m_adb_st;
	devcb_write_line m_cb1, m_cb2;
	devcb_read_line m_pa1, m_pa2, m_pa4, m_pa6;

	required_device<m68000_musashi_device> m_maincpu;
	required_device<ncr53c96_device> m_ncr;
	required_device<via6522_device> m_via1, m_via2;
	required_device<asc_device> m_asc;
	required_device<rtc3430042_device> m_rtc;
	required_device<applefdintf_device> m_fdc;
	required_device_array<floppy_connector, 2> m_floppy;

	emu_timer *m_6015_timer;
	int m_via_interrupt, m_via2_interrupt, m_scc_interrupt, m_last_taken_interrupt;
	floppy_image_device *m_cur_floppy = nullptr;
	int m_hdsel;
	int m_adb_interrupt;
	int m_via2_ca1_hack;
	u8 m_nubus_irqs;

	int m_drq, m_scsi_irq, m_asc_irq;
	int m_scsi_read_cycles, m_scsi_write_cycles, m_scsi_dma_read_cycles, m_scsi_dma_write_cycles;
	u32 m_scsi_dma_result;
	bool m_scsi_second_half;

	u16 m_iosb_regs[0x20];

	u16 iosb_regs_r(offs_t offset);
	void iosb_regs_w(offs_t offset, u16 data, u16 mem_mask);

	uint16_t mac_via_r(offs_t offset);
	void mac_via_w(offs_t offset, uint16_t data, uint16_t mem_mask);
	uint16_t mac_via2_r(offs_t offset);
	void mac_via2_w(offs_t offset, uint16_t data, uint16_t mem_mask);

	uint8_t via_in_a();
	uint8_t via2_in_a();
	uint8_t via_in_b();
	void via_out_a(uint8_t data);
	void via_out_b(uint8_t data);
	void field_interrupts();
	void via_out_cb1(int state);
	void via_out_cb2(int state);
	void via1_irq(int state);
	void via2_irq(int state);
	void asc_irq(int state);
	TIMER_CALLBACK_MEMBER(mac_6015_tick);

	void phases_w(uint8_t phases);
	void devsel_w(uint8_t devsel);
	uint16_t swim_r(offs_t offset, u16 mem_mask);
	void swim_w(offs_t offset, u16 data, u16 mem_mask);
};

// device type definition
DECLARE_DEVICE_TYPE(IOSB, iosb_device)

#endif // MAME_APPLE_IOSB_H
