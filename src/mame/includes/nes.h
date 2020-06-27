// license:BSD-3-Clause
// copyright-holders:Brad Oliver,Fabio Priuli
/*****************************************************************************

    nes.h

    Nintendo Entertainment System / Famicom

 ****************************************************************************/

#ifndef MAME_INCLUDES_NES_H
#define MAME_INCLUDES_NES_H

#pragma once


#include "bus/nes/disksys.h"
#include "bus/nes/nes_slot.h"
#include "bus/nes/nes_carts.h"
#include "bus/nes_ctrl/ctrl.h"
#include "video/ppu2c0x.h"
#include "screen.h"

/***************************************************************************
    CONSTANTS
***************************************************************************/

#define NES_BATTERY_SIZE 0x2000

/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

/*PPU fast banking constants and structures */

#define CHRROM 0
#define CHRRAM 1


/*PPU nametable fast banking constants and structures */

#define CIRAM 0
#define ROM 1
#define EXRAM 2
#define MMC5FILL 3
#define CART_NTRAM 4

#define NES_BATTERY 0
#define NES_WRAM 1

class nes_base_state : public driver_device
{
public:
	nes_base_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_ctrl1(*this, "ctrl1"),
		m_ctrl2(*this, "ctrl2")
	{ }

	required_device<cpu_device> m_maincpu;
	optional_device<nes_control_port_device> m_ctrl1;
	optional_device<nes_control_port_device> m_ctrl2;

	uint8_t nes_in0_r();
	uint8_t nes_in1_r();
	void nes_in0_w(uint8_t data);
};

class nes_state : public nes_base_state
{
public:
	nes_state(const machine_config &mconfig, device_type type, const char *tag) :
		nes_base_state(mconfig, type, tag),
		m_ppu(*this, "ppu"),
		m_screen(*this, "screen"),
		m_exp(*this, "exp"),
		m_cartslot(*this, "nes_slot"),
		m_disk(*this, "disk")
	{ }


	int nes_ppu_vidaccess(int address, int data);


	uint8_t fc_in0_r();
	uint8_t fc_in1_r();
	void fc_in0_w(uint8_t data);
	void nes_vh_sprite_dma_w(address_space &space, uint8_t data);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	virtual void video_reset() override;
	uint32_t screen_update_nes(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
	void screen_vblank_nes(int state);

	void init_famicom();

	// these are needed until we modernize the FDS controller
	DECLARE_MACHINE_START(fds);
	DECLARE_MACHINE_START(famitwin);
	DECLARE_MACHINE_RESET(fds);
	DECLARE_MACHINE_RESET(famitwin);
	void setup_disk(nes_disksys_device *slot);

	void suborkbd(machine_config &config);
	void famipalc(machine_config &config);
	void famicom(machine_config &config);
	void famitwin(machine_config &config);
	void nespal(machine_config &config);
	void nespalc(machine_config &config);
	void nes(machine_config &config);
	void fds(machine_config &config);
	void nes_map(address_map &map);
private:
	memory_bank       *m_prg_bank_mem[5];

	/* video-related */
	int m_last_frame_flip;

	/* misc */
	ioport_port       *m_io_disksel;

	uint8_t      *m_vram;
	std::unique_ptr<uint8_t[]>    m_ciram; //PPU nametable RAM - external to PPU!


	required_device<ppu2c0x_device> m_ppu;
	required_device<screen_device> m_screen;
	optional_device<nes_control_port_device> m_exp;
	optional_device<nes_cart_slot_device> m_cartslot;
	optional_device<nes_disksys_device> m_disk;
};

#endif // MAME_INCLUDES_NES_H
