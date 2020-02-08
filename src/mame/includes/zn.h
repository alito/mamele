// license:BSD-3-Clause
// copyright-holders:smf, R. Belmont

#ifndef MAME_INCLUDES_ZN_H
#define MAME_INCLUDES_ZN_H

#pragma once

#include "audio/rax.h"
#include "audio/taito_zm.h"
#include "audio/taitosnd.h"

#include "bus/ata/atadev.h"
#include "bus/ata/ataintf.h"
#include "cpu/m68000/m68000.h"
#include "cpu/mcs51/mcs51.h"
#include "cpu/psx/psx.h"
#include "cpu/z80/z80.h"
#include "machine/7200fifo.h"
#include "machine/at28c16.h"
#include "machine/bankdev.h"
#include "machine/cat702.h"
#include "machine/gen_latch.h"
#include "machine/mb3773.h"
#include "machine/nvram.h"
#include "machine/ram.h"
#include "machine/vt83c461.h"
#include "machine/watchdog.h"
#include "machine/znmcu.h"
#include "sound/2610intf.h"
#include "sound/okim6295.h"
#include "sound/qsound.h"
#include "sound/spu.h"
#include "sound/ymf271.h"
#include "sound/ymz280b.h"
#include "video/psx.h"

#include "screen.h"
#include "speaker.h"

#include <algorithm>

class zn_state : public driver_device
{
public:
	zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_gpu(*this, "gpu"),
		m_gpu_screen(*this, "screen"),
		m_sio0(*this, "maincpu:sio0"),
		m_cat702(*this, "cat702_%u", 1),
		m_znmcu(*this, "znmcu"),
		m_maincpu(*this, "maincpu"),
		m_ram(*this, "maincpu:ram"),
		m_znmcu_dataout(1)
	{
		std::fill(std::begin(m_cat702_dataout), std::end(m_cat702_dataout), 1);
	}

	void zn_1mb_vram(machine_config &config);
	void zn_2mb_vram(machine_config &config);
	void zn2(machine_config &config);
	void gameboard_cat702(machine_config &config);

protected:
	template<int Chip> DECLARE_WRITE_LINE_MEMBER(cat702_dataout) { m_cat702_dataout[Chip] = state; update_sio0_rxd(); }
	DECLARE_WRITE_LINE_MEMBER(znmcu_dataout) { m_znmcu_dataout = state; update_sio0_rxd(); }
	void update_sio0_rxd() { m_sio0->write_rxd(m_cat702_dataout[0] && m_cat702_dataout[1] && m_znmcu_dataout); }
	DECLARE_READ8_MEMBER(znsecsel_r);
	DECLARE_WRITE8_MEMBER(znsecsel_w);
	DECLARE_READ8_MEMBER(boardconfig_r);
	DECLARE_READ16_MEMBER(unknown_r);
	DECLARE_WRITE8_MEMBER(coin_w);

	void zn_base_map(address_map &map);
	void zn_rom_base_map(address_map &map);

	virtual void machine_start() override;

	inline void ATTR_PRINTF(3,4) verboselog( int n_level, const char *s_fmt, ... );
	inline void psxwriteword( uint32_t *p_n_psxram, uint32_t n_address, uint16_t n_data );

	uint8_t m_n_znsecsel;

	required_device<psxgpu_device> m_gpu;
	required_device<screen_device> m_gpu_screen;
	required_device<psxsio0_device> m_sio0;
	required_device_array<cat702_device, 2> m_cat702;
	required_device<znmcu_device> m_znmcu;
	required_device<cpu_device> m_maincpu;
	required_device<ram_device> m_ram;

	int m_cat702_dataout[2];
	int m_znmcu_dataout;
};

//
// Capcom ZN1/2 hardware
//

class capcom_zn_state : public zn_state
{
public:
	capcom_zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank"),
		m_audiocpu(*this, "audiocpu"),
		m_soundlatch(*this, "soundlatch"),
		m_soundbank(*this, "soundbank")
	{
	}

	void zn_base(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
	required_device<cpu_device> m_audiocpu;
	required_device<generic_latch_8_device> m_soundlatch;
	required_memory_bank m_soundbank;

private:
	void main_map(address_map &map);
	void qsound_map(address_map &map);
	void qsound_portmap(address_map &map);

	DECLARE_READ16_MEMBER(kickharness_r);
	DECLARE_WRITE8_MEMBER(bank_w);
	DECLARE_WRITE8_MEMBER(qsound_bankswitch_w);
	INTERRUPT_GEN_MEMBER(qsound_interrupt);
};

class zn1_state : public capcom_zn_state
{
public:
	zn1_state(const machine_config &mconfig, device_type type, const char *tag) :
		capcom_zn_state(mconfig, type, tag)
	{
	}

	void coh1000c(machine_config &config);
	void coh1002c(machine_config &config);
};

class glpracr_state : public zn1_state
{
public:
	glpracr_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn1_state(mconfig, type, tag)
	{
	}

private:
	virtual void machine_reset() override;
};

class zn2_state : public capcom_zn_state
{
public:
	zn2_state(const machine_config &mconfig, device_type type, const char *tag) :
		capcom_zn_state(mconfig, type, tag)
	{
	}

	void coh3002c(machine_config &config);
};

//
// Acclaim ZN-derived hardware
//

class acclaim_zn_state : public zn_state
{
public:
	acclaim_zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag)
	{
	}

	void coh1000a(machine_config &config);

protected:
	void coh1000a_map(address_map &map);

private:
	DECLARE_WRITE16_MEMBER(acpsx_00_w);
	DECLARE_WRITE16_MEMBER(acpsx_10_w);
};

class nbajamex_state : public acclaim_zn_state
{
public:
	nbajamex_state(const machine_config &mconfig, device_type type, const char *tag) :
		acclaim_zn_state(mconfig, type, tag),
		m_rax(*this, "rax"),
		m_bankmap(*this, "nbajamex_bankmap"),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank%u", 1)
	{
	}

	void nbajamex(machine_config &config);

private:
	virtual void driver_init() override;
	virtual void machine_start() override;
	virtual void machine_reset() override;

	DECLARE_WRITE16_MEMBER(bank_w);
	DECLARE_WRITE16_MEMBER(sound_80_w);
	DECLARE_WRITE8_MEMBER(backup_w);
	DECLARE_READ16_MEMBER(sound_08_r);
	DECLARE_READ16_MEMBER(sound_80_r);

	void main_map(address_map &map);
	void bank_map(address_map &map);

	std::unique_ptr<uint8_t[]> m_sram;
	uint32_t m_curr_rombank[2];

	required_device<acclaim_rax_device> m_rax;
	required_device<address_map_bank_device> m_bankmap;
	required_memory_region m_bankedroms;
	required_memory_bank_array<2> m_rombank;
};

class jdredd_state : public acclaim_zn_state
{
public:
	jdredd_state(const machine_config &mconfig, device_type type, const char *tag) :
		acclaim_zn_state(mconfig, type, tag),
		m_gun_x(*this, "GUN%uX", 1U),
		m_gun_y(*this, "GUN%uY", 1U)
	{
	}

	void jdredd(machine_config &config);

	DECLARE_READ_LINE_MEMBER(gun_mux_r);

private:
	virtual void machine_start() override;

	void main_map(address_map &map);

	DECLARE_WRITE_LINE_MEMBER(vblank);

	required_ioport_array<2> m_gun_x;
	required_ioport_array<2> m_gun_y;
	bool m_gun_mux;
};

//
// Atlus ZN-derived hardware
//

class atlus_zn_state : public zn_state
{
public:
	atlus_zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank"),
		m_audiocpu(*this, "audiocpu"),
		m_soundlatch16(*this, "soundlatch16")
	{
	}

	void coh1001l(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	DECLARE_WRITE8_MEMBER(bank_w);
	DECLARE_WRITE16_MEMBER(sound_unk_w);

	void main_map(address_map &map);
	void sound_map(address_map &map);

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
	required_device<cpu_device> m_audiocpu;
	required_device<generic_latch_16_device> m_soundlatch16;
};

//
// Video System Co. ZN-derived hardware
//

class visco_zn_state : public zn_state
{
public:
	visco_zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank")
	{
	}

	void coh1002v(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	DECLARE_WRITE8_MEMBER(bank_w);

	void main_map(address_map &map);

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
};

//
// Eighting / Raizing ZN-derived hardware
//

class raizing_zn_state : public zn_state
{
public:
	raizing_zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank"),
		m_audiocpu(*this, "audiocpu"),
		m_soundlatch(*this, "soundlatch")
	{
	}

	void coh1002e(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void main_map(address_map &map);
	void psarc_sound_map(address_map &map);

	DECLARE_WRITE8_MEMBER(bank_w);
	DECLARE_WRITE8_MEMBER(sound_irq_w);

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
	required_device<cpu_device> m_audiocpu;
	required_device<generic_latch_8_device> m_soundlatch;
};

class beastrzrb_state : public raizing_zn_state
{
public:
	beastrzrb_state(const machine_config &mconfig, device_type type, const char *tag) :
		raizing_zn_state(mconfig, type, tag),
		m_okibank(*this, "okibank")
	{
	}

	void beastrzrb(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void sound_map(address_map &map);
	void oki_map(address_map &map);

	required_memory_bank m_okibank;
};

class bam2_state : public zn_state
{
public:
	bam2_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank")
	{
	}

	void bam2(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void main_map(address_map &map);

	uint16_t m_mcu_command;

	DECLARE_WRITE16_MEMBER(mcu_w);
	DECLARE_READ16_MEMBER(mcu_r);
	DECLARE_READ16_MEMBER(unk_r);

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
};

//
// Taito FX1a/FX1b
//

class taito_fx_state : public zn_state
{
public:
	taito_fx_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank"),
		m_mb3773(*this, "mb3773")
	{
	}

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	DECLARE_WRITE8_MEMBER(bank_w);

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
	required_device<mb3773_device> m_mb3773;
};

class taito_fx1a_state : public taito_fx_state
{
public:
	taito_fx1a_state(const machine_config &mconfig, device_type type, const char *tag) :
		taito_fx_state(mconfig, type, tag),
		m_audiocpu(*this, "audiocpu"),
		m_soundbank(*this, "soundbank")
	{
	}

	void coh1000ta(machine_config &config);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void main_map(address_map &map);
	void sound_map(address_map &map);

	DECLARE_WRITE8_MEMBER(sound_bankswitch_w);

	required_device<cpu_device> m_audiocpu;
	required_memory_bank m_soundbank;
};

class taito_fx1b_state : public taito_fx_state
{
public:
	taito_fx1b_state(const machine_config &mconfig, device_type type, const char *tag) :
		taito_fx_state(mconfig, type, tag),
		m_zoom(*this, "taito_zoom"),
		m_fm1208s(*this, "fm1208s")
	{
	}

	void base_config(machine_config &config);
	void coh1000tb(machine_config &config);

private:
	virtual void machine_start() override;

	void main_map(address_map &map);

	DECLARE_WRITE8_MEMBER(fram_w);
	DECLARE_READ8_MEMBER(fram_r);

	required_device<taito_zoom_device> m_zoom;
	required_device<nvram_device> m_fm1208s;

	std::unique_ptr<uint8_t[]> m_fram;
};

class gdarius_state : public taito_fx1b_state
{
public:
	gdarius_state(const machine_config &mconfig, device_type type, const char *tag) :
		taito_fx1b_state(mconfig, type, tag)
	{
	}

	void coh1002tb(machine_config &config);
};

//
// Atari ZN-derived hardware (Primal Rage 2)
//

class primrag2_state : public zn_state
{
public:
	primrag2_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_vt83c461(*this, "ide")
	{
	}

	void coh1000w(machine_config &config);

private:
	virtual void machine_start() override;

	void main_map(address_map &map);

	void dma_read(uint32_t *p_n_psxram, uint32_t n_address, int32_t n_size);
	void dma_write(uint32_t *p_n_psxram, uint32_t n_address, int32_t n_size);

	DECLARE_READ16_MEMBER(vt83c461_16_r);
	DECLARE_WRITE16_MEMBER(vt83c461_16_w);
	DECLARE_READ16_MEMBER(vt83c461_32_r);
	DECLARE_WRITE16_MEMBER(vt83c461_32_w);

	required_device<vt83c461_device> m_vt83c461;

	uint16_t m_vt83c461_latch;
};

//
// Tecmo ZN-derived hardware
//

class tecmo_zn_state : public zn_state
{
public:
	tecmo_zn_state(const machine_config &mconfig, device_type type, const char *tag) :
		zn_state(mconfig, type, tag),
		m_bankedroms(*this, "bankedroms"),
		m_rombank(*this, "rombank")
	{
	}

	void coh1002m(machine_config &config);
	void coh1002ml(machine_config &config);

protected:
	void base_map(address_map &map);

private:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	void link_map(address_map &map);
	void link_port_map(address_map &map);

	DECLARE_WRITE8_MEMBER(bank_w);

	required_memory_region m_bankedroms;
	required_memory_bank m_rombank;
};

class cbaj_state : public tecmo_zn_state
{
public:
	cbaj_state(const machine_config &mconfig, device_type type, const char *tag) :
		tecmo_zn_state(mconfig, type, tag),
		m_audiocpu(*this, "audiocpu"),
		m_fifo(*this, "cbaj_fifo%u", 1)
	{
	}

	void cbaj(machine_config &config);

private:
	void main_map(address_map &map);

	void z80_map(address_map &map);
	void z80_port_map(address_map &map);

	DECLARE_READ8_MEMBER(sound_main_status_r);
	DECLARE_READ8_MEMBER(sound_z80_status_r);

	required_device<cpu_device> m_audiocpu;
	required_device_array<fifo7200_device, 2> m_fifo;
};

#endif // MAME_INCLUDES_ZN_H
