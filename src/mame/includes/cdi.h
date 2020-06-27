// license:BSD-3-Clause
// copyright-holders:Ryan Holtz

#ifndef MAME_INCLUDES_CDI_H
#define MAME_INCLUDES_CDI_H

#include "machine/scc68070.h"
#include "machine/cdislavehle.h"
#include "machine/cdicdic.h"
#include "sound/dmadac.h"
#include "video/mcd212.h"
#include "cpu/mcs51/mcs51.h"
#include "cpu/m6805/m68hc05.h"
#include "screen.h"

/*----------- driver state -----------*/

class cdi_state : public driver_device
{
public:
	cdi_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_planea(*this, "mcd212:planea")
		, m_slave_hle(*this, "slave_hle")
		, m_servo(*this, "servo")
		, m_slave(*this, "slave")
		, m_cdic(*this, "cdic")
		, m_cdda(*this, "cdda")
		, m_mcd212(*this, "mcd212")
		, m_lcd(*this, "lcd")
		, m_dmadac(*this, "dac%u", 1U)
	{ }

	void cdimono1_base(machine_config &config);
	void cdimono1(machine_config &config);
	void cdimono2(machine_config &config);
	void cdi910(machine_config &config);

protected:
	virtual void machine_reset() override;

	void cdimono1_mem(address_map &map);

	required_device<scc68070_device> m_maincpu;

private:
	virtual void video_start() override;

	enum servo_portc_bit_t
	{
		INV_JUC_OUT = (1 << 2),
		INV_DIV4_IN = (1 << 5),
		INV_CADDYSWITCH_IN = (1 << 7)
	};

	void draw_lcd(int y);
	uint32_t screen_update_cdimono1_lcd(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);

	void cdi910_mem(address_map &map);
	void cdimono2_mem(address_map &map);
	void cdi070_cpuspace(address_map &map);

	uint16_t dvc_r(offs_t offset, uint16_t mem_mask = ~0);
	void dvc_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);

	required_shared_ptr<uint16_t> m_planea;
	optional_device<cdislave_hle_device> m_slave_hle;
	optional_device<m68hc05c8_device> m_servo;
	optional_device<m68hc05c8_device> m_slave;
	optional_device<cdicdic_device> m_cdic;
	required_device<cdda_device> m_cdda;
	required_device<mcd212_device> m_mcd212;
	optional_device<screen_device> m_lcd;

	required_device_array<dmadac_sound_device, 2> m_dmadac;

	bitmap_rgb32 m_lcdbitmap;
};

class quizard_state : public cdi_state
{
public:
	quizard_state(const machine_config &mconfig, device_type type, const char *tag)
		: cdi_state(mconfig, type, tag)
		, m_mcu(*this, "mcu")
		, m_inputs(*this, "P%u", 0U)
	{ }

	void quizard(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	uint8_t mcu_p0_r();
	uint8_t mcu_p1_r();
	uint8_t mcu_p2_r();
	uint8_t mcu_p3_r();
	void mcu_p0_w(uint8_t data);
	void mcu_p1_w(uint8_t data);
	void mcu_p2_w(uint8_t data);
	void mcu_p3_w(uint8_t data);
	void mcu_tx(uint8_t data);
	uint8_t mcu_rx();

	void mcu_rx_from_cpu(uint8_t data);
	void mcu_rtsn_from_cpu(int state);

	required_device<i8751_device> m_mcu;
	required_ioport_array<3> m_inputs;

	uint8_t m_mcu_rx_from_cpu;
	bool m_mcu_initial_byte;
};

// Quizard 2 language values:
// 0x2b1: Italian
// 0x001: French
// 0x188: German

#endif // MAME_INCLUDES_CDI_H
