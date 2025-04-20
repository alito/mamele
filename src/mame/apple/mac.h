// license:BSD-3-Clause
// copyright-holders:Nathan Woods, Raphael Nabet, R. Belmont
/*****************************************************************************
 *
 * mac.h
 *
 * Macintosh II driver declarations
 *
 ****************************************************************************/
#ifndef MAME_INCLUDES_MAC_H
#define MAME_INCLUDES_MAC_H

#pragma once

#include "machine/8530scc.h"
#include "machine/6522via.h"
#include "machine/ncr5380.h"
#include "machine/nscsi_bus.h"
#include "machine/ram.h"
#include "machine/timer.h"
#include "adbmodem.h"
#include "egret.h"
#include "macadb.h"
#include "bus/nubus/nubus.h"
#include "bus/macpds/macpds.h"
#include "machine/applefdintf.h"
#include "macrtc.h"
#include "macscsi.h"
#include "sound/asc.h"
#include "cpu/m68000/m68000.h"
#include "emupal.h"
#include "screen.h"

#define MAC_SCREEN_NAME "screen"

// model helpers
#define ADB_IS_BITBANG_CLASS    ((m_model >= MODEL_MAC_II && m_model <= MODEL_MAC_IICI) || (m_model == MODEL_MAC_SE30))
#define ADB_IS_EGRET            (m_model == MODEL_MAC_IISI)

// video parameters for classic Macs
#define MAC_H_VIS   (512)
#define MAC_V_VIS   (342)
#define MAC_H_TOTAL (704)  // (512+192)
#define MAC_V_TOTAL (370) // (342+28)

#define MACII_USE_ADBMODEM (0)

// Mac driver data

class mac_state:public driver_device
{
public:
	mac_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_via1(*this, "via6522_0"),
		m_via2(*this, "via6522_1"),
		m_asc(*this, "asc"),
		m_egret(*this, EGRET_TAG),
		m_adbmodem(*this, "adbmodem"),
		m_macadb(*this, "macadb"),
		m_ram(*this, RAM_TAG),
		m_scc(*this, "scc"),
		m_ncr5380(*this, "scsi:7:ncr5380"),
		m_scsihelp(*this, "scsihelp"),
		m_fdc(*this, "fdc"),
		m_floppy(*this, "fdc:%d", 0U),
		m_rtc(*this, "rtc"),
		m_montype(*this, "MONTYPE"),
		m_main_buffer(true),
		m_vram(*this,"vram"),
		m_via2_ca1_hack(0),
		m_screen(*this, "screen"),
		m_palette(*this, "palette")
	{
		m_rom_size = 0;
		m_rom_ptr = nullptr;
		m_cur_floppy = nullptr;
	}

	void add_scsi(machine_config &config, bool cdrom = false);
	void add_base_devices(machine_config &config, bool rtc = true, int woz_version = 0);
	void add_asc(machine_config &config, asc_device::asc_type type = asc_device::asc_type::ASC);
	void add_nubus(machine_config &config, bool bank1 = true, bool bank2 = true);
	template <typename T> void add_nubus_pds(machine_config &config, const char *slot_tag, T &&opts);
	void add_via1_adb(machine_config &config, bool macii);
	void add_via2(machine_config &config);
	void add_egret(machine_config &config, int type);

	void maciisi(machine_config &config);
	void maciici(machine_config &config);
	void maciix(machine_config &config, bool nubus_bank1 = true, bool nubus_bank2 = true);
	void maciicx(machine_config &config);
	void macse30(machine_config &config);
	void maciifx(machine_config &config);
	void macii(machine_config &config, bool cpu = true, asc_device::asc_type asc_type = asc_device::asc_type::ASC,
		   bool nubus = true, bool nubus_bank1 = true, bool nubus_bank2 = true, int woz_version = 0);
	void maciihmu(machine_config &config);
	void maciihd(machine_config &config);

	void init_maciifdhd();
	void init_macse30();
	void init_macii();
	void init_maciifx();
	void init_maciici();
	void init_maciix();
	void init_maciisi();
	void init_maciicx();

	/* tells which model is being emulated (set by macxxx_init) */
	enum model_t
	{
		MODEL_MAC_II,       // Mac II class 68020/030 machines
		MODEL_MAC_II_FDHD,
		MODEL_MAC_IIX,
		MODEL_MAC_IICX,
		MODEL_MAC_IICI,
		MODEL_MAC_IISI,
		MODEL_MAC_IIFX,
		MODEL_MAC_SE30
	};

	model_t m_model;

private:
	required_device<cpu_device> m_maincpu;
	required_device<via6522_device> m_via1;
	optional_device<via6522_device> m_via2;
	optional_device<asc_device> m_asc;
	optional_device<egret_device> m_egret;
	optional_device<adbmodem_device> m_adbmodem;
	optional_device<macadb_device> m_macadb;
	required_device<ram_device> m_ram;
	required_device<scc8530_legacy_device> m_scc;
	required_device<ncr53c80_device> m_ncr5380;
	required_device<mac_scsi_helper_device> m_scsihelp;
	required_device<applefdintf_device> m_fdc;
	required_device_array<floppy_connector, 2> m_floppy;
	optional_device<rtc3430042_device> m_rtc;
	optional_ioport m_montype;

	virtual void machine_start() override;
	virtual void machine_reset() override;

	uint32_t m_overlay = 0;

	uint32_t m_via2_vbl = 0;
	uint32_t m_se30_vbl_enable = 0;
	uint8_t m_nubus_irq_state = 0;

	emu_timer *m_overlay_timeout = nullptr;
	TIMER_CALLBACK_MEMBER(overlay_timeout_func);
	uint32_t rom_switch_r(offs_t offset);

	bool m_main_buffer = false;
	int m_adb_irq_pending = 0;
	int m_screen_buffer = 0;
	int ca1_data = 0;

	// 60.15 Hz timer for RBV/V8/Eagle/VASP/etc.
	emu_timer *m_6015_timer = nullptr;

	// ADB refresh timer, independent of anything else going on
	emu_timer *m_adbupdate_timer = nullptr;

	WRITE_LINE_MEMBER(adb_irq_w) { m_adb_irq_pending = state; }

	// RBV and friends (V8, etc)
	uint8_t m_rbv_regs[256]{}, m_rbv_ier = 0, m_rbv_ifr = 0, m_rbv_montype = 0, m_rbv_vbltime = 0;
	uint32_t m_rbv_colors[3]{}, m_rbv_count = 0, m_rbv_clutoffs = 0, m_rbv_immed10wr = 0;
	uint32_t m_rbv_palette[256]{};
	emu_timer *m_vbl_timer = nullptr, *m_cursor_timer = nullptr;
	uint16_t m_cursor_line = 0;

	// this is shared among all video setups with vram
	optional_shared_ptr<uint32_t> m_vram;

	// interrupts
	int m_scc_interrupt = false, m_via_interrupt = false, m_via2_interrupt = false, m_scsi_interrupt = false, m_last_taken_interrupt = false;

	// defined in machine/mac.c
	void v8_resize();
	void set_memory_overlay(int overlay);
	void scc_mouse_irq( int x, int y );
	void nubus_slot_interrupt(uint8_t slot, uint32_t state);
	DECLARE_WRITE_LINE_MEMBER(set_scc_interrupt);
	void set_via_interrupt(int value);
	void set_via2_interrupt(int value);
	void field_interrupts();
	void vblank_irq();
	void rbv_recalc_irqs();
	void update_volume();

	void mac_via_sync();
	uint16_t mac_via_r(offs_t offset);
	void mac_via_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t mac_via2_r(offs_t offset);
	void mac_via2_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t mac_autovector_r(offs_t offset);
	void mac_autovector_w(offs_t offset, uint16_t data);
	uint16_t mac_iwm_r(offs_t offset, uint16_t mem_mask = ~0);
	void mac_iwm_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint16_t mac_scc_r(offs_t offset);
	void mac_scc_w(offs_t offset, uint16_t data);
	void mac_scc_2_w(offs_t offset, uint16_t data);
	uint16_t macplus_scsi_r(offs_t offset, uint16_t mem_mask = ~0);
	void macplus_scsi_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void macii_scsi_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	uint32_t macii_scsi_drq_r(offs_t offset, uint32_t mem_mask = ~0);
	void macii_scsi_drq_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	void scsi_berr_w(uint8_t data);

	uint32_t rbv_ramdac_r();
	void rbv_ramdac_w(offs_t offset, uint32_t data);
	uint8_t mac_rbv_r(offs_t offset);
	void mac_rbv_w(offs_t offset, uint8_t data);

	uint16_t mac_config_r();

	uint32_t biu_r(offs_t offset, uint32_t mem_mask = ~0);
	void biu_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	template <int N> DECLARE_WRITE_LINE_MEMBER(oss_interrupt);
	TIMER_CALLBACK_MEMBER(oss_6015_tick);
	uint8_t oss_r(offs_t offset);
	void oss_w(offs_t offset, uint8_t data);
	uint32_t buserror_r();
	uint8_t maciifx_8010_r();
	uint8_t maciifx_8040_r();

	DECLARE_WRITE_LINE_MEMBER(nubus_irq_9_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_a_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_b_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_c_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_d_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_e_w);

	DECLARE_WRITE_LINE_MEMBER(egret_reset_w);

	DECLARE_WRITE_LINE_MEMBER(mac_scsi_irq);
	DECLARE_WRITE_LINE_MEMBER(mac_asc_irq);

	void macii_map(address_map &map);
	void maciici_map(address_map &map);
	void maciifx_map(address_map &map);
	void macse30_map(address_map &map);

	uint8_t m_oss_regs[0x400]{};

	int m_via2_ca1_hack = 0;
	optional_device<screen_device> m_screen;
	optional_device<palette_device> m_palette;

	uint32_t m_rom_size = 0;
	uint32_t *m_rom_ptr = nullptr;

	emu_timer *m_scanline_timer = nullptr;

	floppy_image_device *m_cur_floppy = nullptr;

	uint8_t m_pm_req = 0, m_pm_state = 0, m_pm_dptr = 0, m_pm_cmd = 0;

	void phases_w(uint8_t phases);
	void sel35_w(int sel35);
	void devsel_w(uint8_t devsel);
	void hdsel_w(int hdsel);

	void macrbv_reset();
	uint32_t screen_update_macse30(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macrbv(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macrbvvram(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	DECLARE_WRITE_LINE_MEMBER(mac_rbv_vbl);
	TIMER_CALLBACK_MEMBER(mac_6015_tick);
	TIMER_CALLBACK_MEMBER(mac_adbrefresh_tick);
	TIMER_CALLBACK_MEMBER(mac_scanline_tick);
	DECLARE_WRITE_LINE_MEMBER(mac_adb_via_out_cb2);
	uint8_t mac_via_in_a();
	uint8_t mac_via_in_b();
	uint8_t mac_via_in_b_ii();
	void mac_via_out_a(uint8_t data);
	void mac_via_out_b(uint8_t data);
	void mac_via_out_b_bbadb(uint8_t data);
	void mac_via_out_b_egadb(uint8_t data);
	uint8_t mac_via2_in_a();
	uint8_t mac_via2_in_b();
	void mac_via2_out_a(uint8_t data);
	void mac_via2_out_b(uint8_t data);
	void mac_state_load();
	DECLARE_WRITE_LINE_MEMBER(mac_via_irq);
	DECLARE_WRITE_LINE_MEMBER(mac_via2_irq);
	void set_scc_waitrequest(int waitrequest);
	void mac_driver_init(model_t model);
	void mac_install_memory(offs_t memory_begin, offs_t memory_end, offs_t memory_size, void *memory_data, int is_rom);
};

#endif // MAME_INCLUDES_MAC_H
