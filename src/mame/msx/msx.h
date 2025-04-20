// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef MAME_MSX_MSX_H
#define MAME_MSX_MSX_H

#pragma once

#include "bus/centronics/ctronics.h"
#include "bus/msx/ctrl/ctrl.h"
#include "bus/msx/slot/cartridge.h"
#include "cpu/z80/z80.h"
#include "imagedev/cassette.h"
#include "machine/buffer.h"
#include "machine/i8255.h"
#include "machine/input_merger.h"
#include "machine/rp5c01.h"
#include "msx_switched.h"
#include "sound/ay8910.h"
#include "sound/dac.h"
#include "sound/ymopl.h"
#include "speaker.h"
#include "video/v9938.h"
#include "video/tms9928a.h"


class msx_hw_def
{
public:
	msx_hw_def() {}
	bool has_cassette() const { return m_has_cassette; }
	bool has_printer_port() const { return m_has_printer_port; }
	bool has_cartslot() const { return m_has_cartslot; }
	bool has_fdc() const { return m_has_fdc; }
	msx_hw_def &has_cassette(bool has_cassette) { m_has_cassette = has_cassette; return *this;}
	msx_hw_def &has_printer_port(bool has_printer_port) { m_has_printer_port = has_printer_port; return *this; }
	msx_hw_def &has_cartslot(bool has_cartslot) { m_has_cartslot = has_cartslot; return *this; }
	msx_hw_def &has_fdc(bool has_fdc) { m_has_fdc = has_fdc; return *this; }

private:
	bool m_has_cassette = true;
	bool m_has_printer_port = true;
	bool m_has_cartslot = false;
	bool m_has_fdc = false;
};

class msx_state : public driver_device
{
protected:
	msx_state(const machine_config &mconfig, device_type type, const char *tag);

	enum ay8910_type
	{
		SND_AY8910,
		SND_YM2149
	};

	enum vdp_type
	{
		VDP_TMS9118,
		VDP_TMS9128,
		VDP_TMS9129,
		VDP_TMS9918,
		VDP_TMS9918A,
		VDP_TMS9928A,
		VDP_TMS9929A
	};

	void msx_base(ay8910_type ay8910_type, machine_config &config, XTAL xtal, int cpu_divider);
	void msx1(vdp_type vdp_type, ay8910_type ay8910_type, machine_config &config);
	void msx1_add_softlists(machine_config &config);

	// configuration helpers
	template <typename T, typename U>
	auto &add_internal_slot(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, u8 page, u8 numpages)
	{
		return add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, page, numpages);
	}
	template <typename T, typename U>
	auto &add_internal_slot(machine_config &config, T &&type, U &&tag, u8 prim, u8 page, u8 numpages)
	{
		return add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, false, 0, page, numpages);
	}
	template <typename T, typename U>
	auto &add_internal_slot(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, page, numpages, region, offset);
	}
	template <typename T, typename U>
	auto &add_internal_slot(machine_config &config, T &&type, U &&tag, u8 prim, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, false, 0, page, numpages, region, offset);
	}
	template <typename T, typename U>
	auto &add_internal_disk(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_disk(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, page, numpages, region, offset);
	}
	template <typename T, typename U>
	auto &add_internal_disk(machine_config &config, T &&type, U &&tag, u8 prim, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_disk(config, std::forward<T>(type), std::forward<U>(tag), prim, false, 0, page, numpages, region, offset);
	}
	template <int N, typename T, typename U>
	auto &add_internal_slot_irq(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_slot_irq<N>(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, page, numpages, region, offset);
	}
	template <int N, typename T, typename U>
	auto &add_internal_slot_irq(machine_config &config, T &&type, U &&tag, u8 prim, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_slot_irq<N>(config, std::forward<T>(type), std::forward<U>(tag), prim, false, 0, page, numpages, region, offset);
	}
	template <int N, typename T, typename U>
	auto &add_internal_slot_irq_mirrored(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		auto &device = add_internal_slot_irq<N>(config, std::forward<T>(type), std::forward<U>(tag), prim, sec, page, numpages, region, offset);
		device.set_size(0x4000);
		return device;
	}
	template <typename T, typename U>
	auto &add_internal_disk_mirrored(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_disk_mirrored(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, page, numpages, region, offset);
	}
	template <typename T, typename U>
	auto &add_internal_disk_mirrored(machine_config &config, T &&type, U &&tag, u8 prim, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		return add_internal_disk_mirrored(config, std::forward<T>(type), std::forward<U>(tag), prim, false, 0, page, numpages, region, offset);
	}
	template <int N, typename T, typename U, typename V>
	auto &add_cartridge_slot(machine_config &config, T &&type, U &&tag, u8 prim, u8 sec, V &&intf, const char *deft)
	{
		return add_cartridge_slot<N>(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, intf, deft);
	}
	template <int N, typename T, typename U, typename V>
	auto &add_cartridge_slot(machine_config &config, T &&type, U &&tag, u8 prim, V &&intf, const char *deft)
	{
		return add_cartridge_slot<N>(config, std::forward<T>(type), std::forward<U>(tag), prim, false, 0, intf, deft);
	}
	template <int N>
	auto &add_cartridge_slot(machine_config &config, u8 prim, u8 sec)
	{
		return add_cartridge_slot<N>(config, prim, true, sec);
	}
	template <int N>
	auto &add_cartridge_slot(machine_config &config, u8 prim)
	{
		return add_cartridge_slot<N>(config, prim, false, 0);
	}
	virtual void driver_start() override;
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void expanded_slot_w(u8 data);
	u8 expanded_slot_r();
	u8 kanji_r(offs_t offset);
	void kanji_w(offs_t offset, u8 data);
	void ppi_port_a_w(u8 data);
	void ppi_port_c_w(u8 data);
	u8 ppi_port_b_r();
	u8 psg_port_a_r();
	u8 psg_port_b_r();
	void psg_port_a_w(u8 data);
	void psg_port_b_w(u8 data);

	void msx_base_io_map(address_map &map);
	void msx1_io_map(address_map &map);
	void memory_map(address_map &map);
	void memory_expand_slot(int slot);
	memory_view::memory_view_entry *get_view(int page, int prim, int sec);

	required_device<z80_device> m_maincpu;
	optional_device<cassette_image_device> m_cassette;
	required_device<ay8910_device> m_ay8910;
	required_device<dac_bit_interface> m_dac;
	required_device<i8255_device> m_ppi;
	optional_device<tms9928a_device> m_tms9928a;
	optional_device<input_buffer_device> m_cent_status_in;
	optional_device<output_latch_device> m_cent_ctrl_out;
	optional_device<output_latch_device> m_cent_data_out;
	optional_device<centronics_device> m_centronics;
	required_device<speaker_device> m_speaker;
	required_device<input_merger_any_high_device> m_mainirq;
	required_device<screen_device> m_screen;
	optional_memory_region m_region_kanji;
	required_device<msx_general_purpose_port_device> m_gen_port1;
	required_device<msx_general_purpose_port_device> m_gen_port2;
	required_ioport_array<11> m_io_key;
	output_finder<2> m_leds;
	msx_hw_def m_hw_def;
	// This is here until more direct rom dumps from kanji font roms become available.
	bool m_kanji_fsa1fx = false;
	memory_view m_view_page0;
	memory_view m_view_page1;
	memory_view m_view_page2;
	memory_view m_view_page3;
	memory_view *m_view[4];
	// There must be a better way to do this
	memory_view m_view_slot0_page0;
	memory_view m_view_slot0_page1;
	memory_view m_view_slot0_page2;
	memory_view m_view_slot0_page3;
	memory_view m_view_slot1_page0;
	memory_view m_view_slot1_page1;
	memory_view m_view_slot1_page2;
	memory_view m_view_slot1_page3;
	memory_view m_view_slot2_page0;
	memory_view m_view_slot2_page1;
	memory_view m_view_slot2_page2;
	memory_view m_view_slot2_page3;
	memory_view m_view_slot3_page0;
	memory_view m_view_slot3_page1;
	memory_view m_view_slot3_page2;
	memory_view m_view_slot3_page3;
	memory_view *m_exp_view[4][4];
	struct internal_slot
	{
		int prim;
		bool is_expanded;
		int sec;
		int page;
		int numpages;
		msx_internal_slot_interface *internal_slot;
	};
	std::vector<internal_slot> m_internal_slots;

	// PSG
	u8 m_psg_b;
	// kanji
	u32 m_kanji_latch;
	// memory
	bool m_slot_expanded[4];
	u8 m_primary_slot;
	u8 m_secondary_slot[4];
	u8 m_port_c_old;
	u8 m_keylatch;

private:
	// configuration helpers
	template <typename T, typename U>
	auto &add_base_slot(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, u8 page, u8 numpages)
	{
		auto &device(std::forward<T>(type)(config, std::forward<U>(tag), 0U));
		device.set_memory_space(m_maincpu, AS_PROGRAM);
		device.set_io_space(m_maincpu, AS_IO);
		device.set_maincpu(m_maincpu);
		m_internal_slots.push_back({prim, expanded, sec, page, numpages, &device});
		return device;
	}
	template <typename T, typename U>
	auto &add_internal_slot(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, u8 page, u8 numpages)
	{
		auto &device = add_base_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, expanded, sec, page, numpages);
		device.set_start_address(page * 0x4000);
		device.set_size(numpages * 0x4000);
		return device;
	}
	template <typename T, typename U>
	auto &add_internal_slot(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		auto &device = add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, expanded, sec, page, numpages);
		device.set_rom_start(region, offset);
		return device;
	}
	template <typename T, typename U>
	auto &add_internal_disk(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		auto &device = add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, true, sec, page, numpages, region, offset);
		m_hw_def.has_fdc(true);
		return device;
	}
	template <int N, typename T, typename U>
	auto &add_internal_slot_irq(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		auto &device = add_internal_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, expanded, sec, page, numpages, region, offset);
		device.irq_handler().set(m_mainirq, FUNC(input_merger_device::in_w<N>));
		return device;
	}
	template <typename T, typename U>
	auto &add_internal_disk_mirrored(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, u8 page, u8 numpages, const char *region, u32 offset = 0)
	{
		// Memory mapped FDC registers are also accessible through page 2
		auto &device = add_internal_disk(config, std::forward<T>(type), std::forward<U>(tag), prim, expanded, sec, page, numpages, region, offset);
		device.set_size(0x4000);
		return device;
	}
	template <int N, typename T, typename U, typename V>
	auto &add_cartridge_slot(machine_config &config, T &&type, U &&tag, u8 prim, bool expanded, u8 sec, V &&intf, const char *deft)
	{
		auto &device = add_base_slot(config, std::forward<T>(type), std::forward<U>(tag), prim, expanded, sec, 0, 4);
		device.option_reset();
		intf(device);
		device.set_default_option(deft);
		device.set_fixed(false);
		device.irq_handler().set(m_mainirq, FUNC(input_merger_device::in_w<N>));
		return device;
	}
	template <int N>
	auto &add_cartridge_slot(machine_config &config, u8 prim, bool expanded, u8 sec)
	{
		static const char *tags[4] = {
			"cartslot1", "cartslot2", "cartslot3", "cartslot4"
		};
		static_assert(N >= 1 && N <= 4, "Invalid cartridge slot number");
		m_hw_def.has_cartslot(true);
		return add_cartridge_slot<N>(config, MSX_SLOT_CARTRIDGE, tags[N-1], prim, expanded, sec, msx_cart, nullptr);
	}
};


class msx2_base_state : public msx_state
{
protected:
	msx2_base_state(const machine_config &mconfig, device_type type, const char *tag)
		: msx_state(mconfig, type, tag)
		, m_v9938(*this, "v9938")
		, m_v9958(*this, "v9958")
		, m_rtc(*this, "rtc")
		, m_rtc_latch(0)
	{
	}

	virtual void machine_start() override;

	void msx2_base(ay8910_type ay8910_type, machine_config &config);
	void msx2(ay8910_type ay8910_type, machine_config &config);
	void msx2_pal(ay8910_type ay8910_type, machine_config &config);
	void msx2plus_base(ay8910_type ay8910_type, machine_config &config);
	void msx2plus(ay8910_type ay8910_type, machine_config &config);
	void msx2plus_pal(ay8910_type ay8910_type, machine_config &config);
	void turbor(ay8910_type ay8910_type, machine_config &config);
	void msx2_add_softlists(machine_config &config);
	void msx2plus_add_softlists(machine_config &config);
	void turbor_add_softlists(machine_config &config);
	void msx_ym2413(machine_config &config);
	void msx2_64kb_vram(machine_config &config);
	u8 rtc_reg_r();
	void rtc_reg_w(u8 data);
	void rtc_latch_w(u8 data);
	u8 switched_r(offs_t offset);
	void switched_w(offs_t offset, u8 data);
	DECLARE_WRITE_LINE_MEMBER(turbo_w);
	void msx2_base_io_map(address_map &map);
	void msx2_io_map(address_map &map);
	void msx2plus_io_map(address_map &map);

	std::vector<msx_switched_interface *> m_switched;

	optional_device<v9938_device> m_v9938;
	optional_device<v9958_device> m_v9958;
	required_device<rp5c01_device> m_rtc;

	// rtc
	u8 m_rtc_latch = 0;
};

#endif // MAME_MSX_MSX_H
