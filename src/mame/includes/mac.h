// license:BSD-3-Clause
// copyright-holders:Nathan Woods, Raphael Nabet, R. Belmont
/*****************************************************************************
 *
 * includes/mac.h
 *
 * Macintosh driver declarations
 *
 ****************************************************************************/
#ifndef MAME_INCLUDES_MAC_H
#define MAME_INCLUDES_MAC_H

#pragma once

#include "machine/8530scc.h"
#include "machine/6522via.h"
#include "machine/ram.h"
#include "machine/timer.h"
#include "machine/egret.h"
#include "machine/cuda.h"
#include "machine/macadb.h"
#include "bus/nubus/nubus.h"
#include "bus/macpds/macpds.h"
#include "machine/applefdc.h"
#include "machine/applefdintf.h"
#include "machine/ncr539x.h"
#include "machine/ncr5380.h"
#include "machine/macrtc.h"
#include "sound/asc.h"
#include "sound/awacs.h"
#include "sound/dac.h"
#include "cpu/m68000/m68000.h"
#include "emupal.h"
#include "screen.h"

#define NEW_SWIM 0

#define MAC_SCREEN_NAME "screen"
#define MAC_539X_1_TAG "539x_1"
#define MAC_539X_2_TAG "539x_2"

// model helpers
#define ADB_IS_BITBANG_CLASS    ((m_model == MODEL_MAC_SE || m_model == MODEL_MAC_CLASSIC) || (m_model >= MODEL_MAC_II && m_model <= MODEL_MAC_IICI) || (m_model == MODEL_MAC_SE30) || (m_model == MODEL_MAC_QUADRA_700))
#define ADB_IS_EGRET    (m_model >= MODEL_MAC_LC && m_model <= MODEL_MAC_CLASSIC_II) || ((m_model >= MODEL_MAC_IISI) && (m_model <= MODEL_MAC_IIVI))
#define ADB_IS_CUDA     ((m_model >= MODEL_MAC_COLOR_CLASSIC && m_model <= MODEL_MAC_LC_580) || ((m_model >= MODEL_MAC_QUADRA_660AV) && (m_model <= MODEL_MAC_QUADRA_630)) || (m_model >= MODEL_MAC_POWERMAC_6100))

// video parameters for classic Macs
#define MAC_H_VIS   (512)
#define MAC_V_VIS   (342)
#define MAC_H_TOTAL (704)       // (512+192)
#define MAC_V_TOTAL (370)       // (342+28)

/*----------- defined in machine/mac.c -----------*/

void mac_fdc_set_enable_lines(device_t *device, int enable_mask);

/* Mac driver data */

class mac_state : public driver_device
{
public:
	mac_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_via1(*this, "via6522_0"),
		m_via2(*this, "via6522_1"),
		m_asc(*this, "asc"),
		m_awacs(*this, "awacs"),
		m_egret(*this, EGRET_TAG),
		m_cuda(*this, CUDA_TAG),
		m_macadb(*this, "macadb"),
		m_ram(*this, RAM_TAG),
		m_scc(*this, "scc"),
		m_539x_1(*this, MAC_539X_1_TAG),
		m_539x_2(*this, MAC_539X_2_TAG),
		m_ncr5380(*this, "ncr5380"),
		m_fdc(*this, "fdc"),
		m_floppy(*this, "fdc:%d", 0U),
		m_rtc(*this, "rtc"),
		m_montype(*this, "MONTYPE"),
		m_main_buffer(true),
		m_vram(*this,"vram"),
		m_vram16(*this,"vram16"),
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
	void add_cuda(machine_config &config, int type);

	void maclc(machine_config &config, bool cpu = true, bool egret = true, asc_device::asc_type asc_type = asc_device::asc_type::V8, int woz_version = 1);
	void maciisi(machine_config &config);
	void maclc2(machine_config &config, bool egret = true, int woz_version = 1);
	void maclc3(machine_config &config, bool egret = true);
	void macpd210(machine_config &config);
	void maciici(machine_config &config);
	void maciix(machine_config &config, bool nubus_bank1 = true, bool nubus_bank2 = true);
	void maclc520(machine_config &config);
	void pwrmac(machine_config &config);
	void maciivx(machine_config &config);
	void maccclas(machine_config &config);
	void maciivi(machine_config &config);
	void maciicx(machine_config &config);
	void macse30(machine_config &config);
	void maciifx(machine_config &config);
	void macclas2(machine_config &config);
	void macii(machine_config &config, bool cpu = true, asc_device::asc_type asc_type = asc_device::asc_type::ASC,
		   bool nubus = true, bool nubus_bank1 = true, bool nubus_bank2 = true, int woz_version = 0);
	void maciihmu(machine_config &config);

	void init_maclc2();
	void init_maciifdhd();
	void init_macse30();
	void init_maciivx();
	void init_maciivi();
	void init_macii();
	void init_macclassic();
	void init_macclassic2();
	void init_maciifx();
	void init_maclc();
	void init_macse();
	void init_macpm6100();
	void init_maclc520();
	void init_maciici();
	void init_maciix();
	void init_maclrcclassic();
	void init_maciisi();
	void init_maciicx();
	void init_maclc3();
	void init_maclc3plus();
	void init_macpm7100();
	void init_macpm8100();

	/* tells which model is being emulated (set by macxxx_init) */
	enum model_t
	{
		MODEL_MAC_128K512K, // 68000 machines
		MODEL_MAC_512KE,
		MODEL_MAC_PLUS,
		MODEL_MAC_SE,
		MODEL_MAC_CLASSIC,

		MODEL_MAC_PORTABLE, // Portable/PB100 are sort of hybrid classic and Mac IIs
		MODEL_MAC_PB100,

		MODEL_MAC_II,       // Mac II class 68020/030 machines
		MODEL_MAC_II_FDHD,
		MODEL_MAC_IIX,
		MODEL_MAC_IICX,
		MODEL_MAC_IICI,
		MODEL_MAC_IISI,
		MODEL_MAC_IIVX,
		MODEL_MAC_IIVI,
		MODEL_MAC_IIFX,
		MODEL_MAC_SE30,

		MODEL_MAC_LC,       // LC class 68030 machines, generally using a V8 or compatible gate array
		MODEL_MAC_LC_II,
		MODEL_MAC_LC_III,
		MODEL_MAC_LC_III_PLUS,
		MODEL_MAC_CLASSIC_II,
		MODEL_MAC_COLOR_CLASSIC,

		MODEL_MAC_LC_475,   // LC III clones with Cuda instead of Egret and 68LC040 on most models
		MODEL_MAC_LC_520,
		MODEL_MAC_LC_550,
		MODEL_MAC_TV,
		MODEL_MAC_LC_575,
		MODEL_MAC_LC_580,

		MODEL_MAC_PB140,    // 68030 PowerBooks.  140/145/145B/170 all have the same machine ID
		MODEL_MAC_PB160,    // 160/180/165 all have the same machine ID too
		MODEL_MAC_PB165c,
		MODEL_MAC_PB180c,
		MODEL_MAC_PB150,    // 150 is fairly radically different from the other 1x0s

		MODEL_MAC_PBDUO_210,    // 68030 PowerBook Duos
		MODEL_MAC_PBDUO_230,
		MODEL_MAC_PBDUO_250,
		MODEL_MAC_PBDUO_270c,

		MODEL_MAC_QUADRA_700,   // 68(LC)040 desktops
		MODEL_MAC_QUADRA_610,
		MODEL_MAC_QUADRA_650,
		MODEL_MAC_QUADRA_800,
		MODEL_MAC_QUADRA_900,
		MODEL_MAC_QUADRA_950,
		MODEL_MAC_QUADRA_660AV,
		MODEL_MAC_QUADRA_840AV,
		MODEL_MAC_QUADRA_605,
		MODEL_MAC_QUADRA_630,

		MODEL_MAC_PB550c,   // 68(LC)040 PowerBooks
		MODEL_MAC_PB520,
		MODEL_MAC_PB520c,
		MODEL_MAC_PB540,
		MODEL_MAC_PB540c,
		MODEL_MAC_PB190,
		MODEL_MAC_PB190cs,

		MODEL_MAC_POWERMAC_6100,    // NuBus PowerMacs
		MODEL_MAC_POWERMAC_7100,
		MODEL_MAC_POWERMAC_8100
	};

	model_t m_model;

private:
	required_device<cpu_device> m_maincpu;
	required_device<via6522_device> m_via1;
	optional_device<via6522_device> m_via2;
	optional_device<asc_device> m_asc;
	optional_device<awacs_device> m_awacs;
	optional_device<egret_device> m_egret;
	optional_device<cuda_device> m_cuda;
	optional_device<macadb_device> m_macadb;
	required_device<ram_device> m_ram;
	required_device<scc8530_legacy_device> m_scc;
	optional_device<ncr539x_device> m_539x_1;
	optional_device<ncr539x_device> m_539x_2;
	optional_device<ncr5380_device> m_ncr5380;
#if NEW_SWIM
	required_device<applefdintf_device> m_fdc;
	required_device_array<floppy_connector, 2> m_floppy;
#else
	required_device<applefdc_base_device> m_fdc;
	optional_device_array<floppy_connector, 2> m_floppy;
#endif
	optional_device<rtc3430042_device> m_rtc;

	//required_ioport m_mouse0, m_mouse1, m_mouse2;
	//optional_ioport_array<7> m_keys;
	optional_ioport m_montype;

	virtual void machine_start() override;
	virtual void machine_reset() override;

	enum
	{
		RBV_TYPE_RBV = 0,
		RBV_TYPE_V8,
		RBV_TYPE_SONORA,
		RBV_TYPE_DAFB
	};


	uint32_t m_overlay;

	uint32_t m_via2_vbl;
	uint32_t m_se30_vbl_enable;
	uint8_t m_nubus_irq_state;

	emu_timer *m_overlay_timeout;
	TIMER_CALLBACK_MEMBER(overlay_timeout_func);
	uint32_t rom_switch_r(offs_t offset);

	bool m_main_buffer;
	int m_adb_irq_pending;
	int m_screen_buffer;
	int irq_count, ca1_data, ca2_data;

	// 60.15 Hz timer for RBV/V8/Sonora/Eagle/VASP/etc.
	emu_timer *m_6015_timer;

	// ADB refresh timer, independent of anything else going on
	emu_timer *m_adbupdate_timer;

	WRITE_LINE_MEMBER(adb_irq_w) { m_adb_irq_pending = state; }

	// RBV and friends (V8, etc)
	uint8_t m_rbv_regs[256], m_rbv_ier, m_rbv_ifr, m_rbv_type, m_rbv_montype, m_rbv_vbltime;
	uint32_t m_rbv_colors[3], m_rbv_count, m_rbv_clutoffs, m_rbv_immed10wr;
	uint32_t m_rbv_palette[256];
	uint8_t m_sonora_vctl[8];
	emu_timer *m_vbl_timer, *m_cursor_timer;
	uint16_t m_cursor_line;

	// this is shared among all video setups with vram
	optional_shared_ptr<uint32_t> m_vram;
	optional_shared_ptr<uint16_t> m_vram16;

	// interrupts
	int m_scc_interrupt, m_via_interrupt, m_via2_interrupt, m_scsi_interrupt, m_asc_interrupt, m_last_taken_interrupt;

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

	uint32_t rbv_ramdac_r();
	void rbv_ramdac_w(offs_t offset, uint32_t data);
	void ariel_ramdac_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	uint8_t mac_sonora_vctl_r(offs_t offset);
	void mac_sonora_vctl_w(offs_t offset, uint8_t data);
	uint8_t mac_rbv_r(offs_t offset);
	void mac_rbv_w(offs_t offset, uint8_t data);

	uint32_t mac_read_id();

	uint16_t mac_config_r();

	uint32_t biu_r(offs_t offset, uint32_t mem_mask = ~0);
	void biu_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	uint8_t oss_r(offs_t offset);
	void oss_w(offs_t offset, uint8_t data);
	uint32_t buserror_r();
	uint8_t swimiop_r(offs_t offset);
	void swimiop_w(offs_t offset, uint8_t data);
	uint8_t scciop_r(offs_t offset);
	void scciop_w(offs_t offset, uint8_t data);

	uint8_t hmc_r();
	void hmc_w(offs_t offset, uint8_t data);
	uint8_t amic_dma_r();
	void amic_dma_w(offs_t offset, uint8_t data);
	uint8_t pmac_diag_r(offs_t offset);

	DECLARE_WRITE_LINE_MEMBER(nubus_irq_9_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_a_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_b_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_c_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_d_w);
	DECLARE_WRITE_LINE_MEMBER(nubus_irq_e_w);

	DECLARE_WRITE_LINE_MEMBER(irq_539x_1_w);
	DECLARE_WRITE_LINE_MEMBER(drq_539x_1_w);

	DECLARE_WRITE_LINE_MEMBER(cuda_reset_w);

	DECLARE_WRITE_LINE_MEMBER(mac_scsi_irq);
	DECLARE_WRITE_LINE_MEMBER(mac_asc_irq);

	void macii_map(address_map &map);
	void maciici_map(address_map &map);
	void maciifx_map(address_map &map);
	void maclc3_map(address_map &map);
	void maclc_map(address_map &map);
	void macse30_map(address_map &map);
	void pwrmac_map(address_map &map);

	inline bool has_adb() { return m_model >= MODEL_MAC_SE; }

	// wait states for accessing the VIA
	int m_via_cycles;

	uint8_t m_oss_regs[0x400];

	// AMIC for x100 PowerMacs
	uint8_t m_amic_regs[0x200];

	// HMC for x100 PowerMacs
	uint64_t m_hmc_reg, m_hmc_shiftout;

	int m_via2_ca1_hack;
	optional_device<screen_device> m_screen;
	optional_device<palette_device> m_palette;

	uint32_t m_rom_size;
	uint32_t *m_rom_ptr;

	emu_timer *m_scanline_timer;

	floppy_image_device *m_cur_floppy;

	uint8_t m_pm_req, m_pm_state, m_pm_dptr, m_pm_cmd;

	void phases_w(uint8_t phases);
	void sel35_w(int sel35);
	void devsel_w(uint8_t devsel);
	void hdsel_w(int hdsel);

	DECLARE_VIDEO_START(mac);
	DECLARE_VIDEO_START(macsonora);
	DECLARE_VIDEO_RESET(macrbv);
	DECLARE_VIDEO_START(macv8);
	DECLARE_VIDEO_RESET(macsonora);
	DECLARE_VIDEO_RESET(maceagle);
	DECLARE_VIDEO_START(macrbv);
	uint32_t screen_update_mac(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macse30(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macrbv(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macrbvvram(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macv8(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macsonora(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_macpbwd(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
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
	void mac_via_out_b_cdadb(uint8_t data);
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
	offs_t mac_dasm_override(std::ostream &stream, offs_t pc, const util::disasm_interface::data_buffer &opcodes, const util::disasm_interface::data_buffer &params);
};

#endif // MAME_INCLUDES_MAC_H
