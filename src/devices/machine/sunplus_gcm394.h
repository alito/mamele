// license:BSD-3-Clause
// copyright-holders:David Haywood
/*****************************************************************************

  SunPlus "GCM394" (based on die pictures)

**********************************************************************/

#ifndef MAME_MACHINE_SUNPLUS_GCM394_H
#define MAME_MACHINE_SUNPLUS_GCM394_H

#pragma once

#include "cpu/unsp/unsp.h"
#include "screen.h"
#include "emupal.h"
#include "sunplus_gcm394_video.h"
#include "spg2xx_audio.h"

typedef device_delegate<void (uint16_t, uint16_t, uint16_t, uint16_t, uint16_t)> sunplus_gcm394_cs_callback_device;

class sunplus_gcm394_base_device : public unsp_20_device, public device_mixer_interface
{
public:
	sunplus_gcm394_base_device(const machine_config& mconfig, device_type type, const char* tag, device_t* owner, uint32_t clock) :
		sunplus_gcm394_base_device(mconfig, type, tag, owner, clock, address_map_constructor(FUNC(sunplus_gcm394_base_device::gcm394_internal_map), this))
	{
	}

	sunplus_gcm394_base_device(const machine_config& mconfig, device_type type, const char* tag, device_t* owner, uint32_t clock, address_map_constructor internal) :
		unsp_20_device(mconfig, type, tag, owner, clock, internal),
		device_mixer_interface(mconfig, *this, 2),
		m_screen(*this, finder_base::DUMMY_TAG),
		m_spg_video(*this, "spgvideo"),
		m_spg_audio(*this, "spgaudio"),
		m_internalrom(*this, "internal"),
		m_porta_in(*this),
		m_portb_in(*this),
		m_portc_in(*this),
		m_porta_out(*this),
		m_nand_read_cb(*this),
		m_csbase(0x20000),
		m_romtype(0),
		m_space_read_cb(*this),
		m_space_write_cb(*this),
		m_boot_mode(0),
		m_cs_callback(*this, DEVICE_SELF, FUNC(sunplus_gcm394_base_device::default_cs_callback))
	{
	}

	uint32_t screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect) { return m_spg_video->screen_update(screen, bitmap, cliprect); }

	auto porta_in() { return m_porta_in.bind(); }
	auto portb_in() { return m_portb_in.bind(); }
	auto portc_in() { return m_portc_in.bind(); }

	auto porta_out() { return m_porta_out.bind(); }

	auto space_read_callback() { return m_space_read_cb.bind(); }
	auto space_write_callback() { return m_space_write_cb.bind(); }

	auto nand_read_callback() { return m_nand_read_cb.bind(); }

	DECLARE_WRITE_LINE_MEMBER(vblank) { m_spg_video->vblank(state); }

	virtual void device_add_mconfig(machine_config& config) override;

	void set_bootmode(int mode) { m_boot_mode = mode; }

	IRQ_CALLBACK_MEMBER(irq_vector_cb);
	template <typename... T> void set_cs_config_callback(T &&... args) { m_cs_callback.set(std::forward<T>(args)...); }
	void default_cs_callback(uint16_t cs0, uint16_t cs1, uint16_t cs2, uint16_t cs3, uint16_t cs4 );

	void set_cs_space(address_space* csspace) { m_cs_space = csspace; }

	void set_paldisplaybank_high_hack(int pal_displaybank_high) { m_spg_video->set_paldisplaybank_high(pal_displaybank_high); }
	void set_alt_tile_addressing_hack(int alt_tile_addressing) { m_spg_video->set_alt_tile_addressing(alt_tile_addressing); }
	void set_romtype(int romtype) { m_romtype = romtype; }

protected:

	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void device_post_load() override;

	void gcm394_internal_map(address_map &map);
	void base_internal_map(address_map &map);

	required_device<screen_device> m_screen;
	required_device<gcm394_video_device> m_spg_video;
	required_device<sunplus_gcm394_audio_device> m_spg_audio;
	optional_memory_region m_internalrom;

	devcb_read16 m_porta_in;
	devcb_read16 m_portb_in;
	devcb_read16 m_portc_in;

	devcb_write16 m_porta_out;

	uint16_t m_dma_params[8][4];

	// unk 78xx
	uint16_t m_7803;

	uint16_t m_7807;

	uint16_t m_membankswitch_7810;

	uint16_t m_7816;
	uint16_t m_7817;


	uint16_t m_7819;

	uint16_t m_782x[5];

	uint16_t m_782d;

	uint16_t m_7835;

	uint16_t m_7860;

	uint16_t m_7861;

	uint16_t m_7862_porta_direction;
	uint16_t m_7863_porta_attribute;

	uint16_t m_786a_portb_direction;
	uint16_t m_786b_portb_attribute;

	uint16_t m_7870;

	//uint16_t m_7871;

	uint16_t m_7872_portc_direction;
	uint16_t m_7873_portc_attribute;

	uint16_t m_7882;
	uint16_t m_7883;

	uint16_t m_78a0;

	uint16_t m_78a4;
	uint16_t m_78a5;
	uint16_t m_78a6;

	uint16_t m_78a8;

	uint16_t m_78b0;
	uint16_t m_78b1;
	uint16_t m_78b2;

	uint16_t m_78b8;

	uint16_t m_78f0;

	uint16_t m_78fb;

	// unk 79xx
	uint16_t m_7934;
	uint16_t m_7935;
	uint16_t m_7936;

	uint16_t m_7960;
	uint16_t m_7961;

	uint16_t m_system_dma_memtype;

	devcb_read16 m_nand_read_cb;
	int m_csbase;

	DECLARE_READ16_MEMBER(internalrom_lower32_r);

	address_space* m_cs_space;

	DECLARE_READ16_MEMBER(cs_space_r);
	DECLARE_WRITE16_MEMBER(cs_space_w);
	DECLARE_READ16_MEMBER(cs_bank_space_r);
	DECLARE_WRITE16_MEMBER(cs_bank_space_w);
	int m_romtype;

private:
	devcb_read16 m_space_read_cb;
	devcb_write16 m_space_write_cb;

	DECLARE_READ16_MEMBER(unk_r);
	DECLARE_WRITE16_MEMBER(unk_w);

	void write_dma_params(int channel, int offset, uint16_t data, address_space& space);
	uint16_t read_dma_params(int channel, int offset);
	void trigger_systemm_dma(address_space &space, int channel);

	DECLARE_READ16_MEMBER(system_dma_params_channel0_r);
	DECLARE_WRITE16_MEMBER(system_dma_params_channel0_w);
	DECLARE_READ16_MEMBER(system_dma_params_channel1_r);
	DECLARE_WRITE16_MEMBER(system_dma_params_channel1_w);
	DECLARE_READ16_MEMBER(system_dma_params_channel2_r);
	DECLARE_WRITE16_MEMBER(system_dma_params_channel2_w);
	DECLARE_READ16_MEMBER(system_dma_params_channel3_r);
	DECLARE_WRITE16_MEMBER(system_dma_params_channel3_w);
	DECLARE_READ16_MEMBER(system_dma_status_r);
	DECLARE_WRITE16_MEMBER(system_dma_7abf_unk_w);
	DECLARE_READ16_MEMBER(system_dma_memtype_r);
	DECLARE_WRITE16_MEMBER(system_dma_memtype_w);

	DECLARE_READ16_MEMBER(unkarea_780f_status_r);
	DECLARE_READ16_MEMBER(unkarea_78fb_status_r);

	DECLARE_READ16_MEMBER(unkarea_7803_r);
	DECLARE_WRITE16_MEMBER(unkarea_7803_w);

	DECLARE_WRITE16_MEMBER(unkarea_7807_w);

	DECLARE_WRITE16_MEMBER(waitmode_enter_780c_w);

	DECLARE_READ16_MEMBER(membankswitch_7810_r);
	DECLARE_WRITE16_MEMBER(membankswitch_7810_w);

	DECLARE_WRITE16_MEMBER(unkarea_7816_w);
	DECLARE_WRITE16_MEMBER(unkarea_7817_w);

	DECLARE_READ16_MEMBER(unkarea_7819_r);
	DECLARE_WRITE16_MEMBER(unkarea_7819_w);

	DECLARE_WRITE16_MEMBER(chipselect_csx_memory_device_control_w);

	DECLARE_WRITE16_MEMBER(unkarea_7835_w);

	// Port A
	DECLARE_READ16_MEMBER(ioarea_7860_porta_r);
	DECLARE_WRITE16_MEMBER(ioarea_7860_porta_w);
	DECLARE_READ16_MEMBER(ioarea_7861_porta_buffer_r);
	DECLARE_READ16_MEMBER(ioarea_7862_porta_direction_r);
	DECLARE_WRITE16_MEMBER(ioarea_7862_porta_direction_w);
	DECLARE_READ16_MEMBER(ioarea_7863_porta_attribute_r);
	DECLARE_WRITE16_MEMBER(ioarea_7863_porta_attribute_w);

	// Port B
	DECLARE_READ16_MEMBER(ioarea_7868_portb_r);
	DECLARE_WRITE16_MEMBER(ioarea_7868_portb_w);
	DECLARE_READ16_MEMBER(ioarea_7869_portb_buffer_r);
	DECLARE_READ16_MEMBER(ioarea_786a_portb_direction_r);
	DECLARE_WRITE16_MEMBER(ioarea_786a_portb_direction_w);
	DECLARE_READ16_MEMBER(ioarea_786b_portb_attribute_r);
	DECLARE_WRITE16_MEMBER(ioarea_786b_portb_attribute_w);

	DECLARE_READ16_MEMBER(unkarea_782d_r);
	DECLARE_WRITE16_MEMBER(unkarea_782d_w);



	DECLARE_READ16_MEMBER(ioarea_7870_portc_r);
	DECLARE_WRITE16_MEMBER(ioarea_7870_portc_w);

	DECLARE_READ16_MEMBER(ioarea_7871_portc_buffer_r);

	DECLARE_READ16_MEMBER(ioarea_7872_portc_direction_r);
	DECLARE_WRITE16_MEMBER(ioarea_7872_portc_direction_w);
	DECLARE_READ16_MEMBER(ioarea_7873_portc_attribute_r);
	DECLARE_WRITE16_MEMBER(ioarea_7873_portc_attribute_w);

	DECLARE_READ16_MEMBER(unkarea_7882_r);
	DECLARE_WRITE16_MEMBER(unkarea_7882_w);
	DECLARE_READ16_MEMBER(unkarea_7883_r);
	DECLARE_WRITE16_MEMBER(unkarea_7883_w);

	DECLARE_WRITE16_MEMBER(unkarea_78a0_w);

	DECLARE_READ16_MEMBER(unkarea_78a0_r);
	DECLARE_READ16_MEMBER(unkarea_78a1_r);

	DECLARE_WRITE16_MEMBER(unkarea_78a4_w);
	DECLARE_WRITE16_MEMBER(unkarea_78a5_w);
	DECLARE_WRITE16_MEMBER(unkarea_78a6_w);

	DECLARE_WRITE16_MEMBER(unkarea_78a8_w);

	DECLARE_WRITE16_MEMBER(unkarea_78b0_w);
	DECLARE_WRITE16_MEMBER(unkarea_78b1_w);

	DECLARE_READ16_MEMBER(unkarea_78b2_r);
	DECLARE_WRITE16_MEMBER(unkarea_78b2_w);

	DECLARE_WRITE16_MEMBER(unkarea_78b8_w);

	DECLARE_READ16_MEMBER(unkarea_78d0_r);

	DECLARE_WRITE16_MEMBER(unkarea_78f0_w);

	DECLARE_READ16_MEMBER(unkarea_7934_r);
	DECLARE_WRITE16_MEMBER(unkarea_7934_w);

	DECLARE_READ16_MEMBER(unkarea_7935_r);
	DECLARE_WRITE16_MEMBER(unkarea_7935_w);

	DECLARE_READ16_MEMBER(unkarea_7936_r);
	DECLARE_WRITE16_MEMBER(unkarea_7936_w);

	DECLARE_READ16_MEMBER(spi_7944_rxdata_r);
	DECLARE_READ16_MEMBER(spi_7945_misc_control_reg_r);

	DECLARE_WRITE16_MEMBER(unkarea_7960_w);
	DECLARE_READ16_MEMBER(unkarea_7961_r);
	DECLARE_WRITE16_MEMBER(unkarea_7961_w);


	DECLARE_WRITE_LINE_MEMBER(videoirq_w);
	DECLARE_WRITE_LINE_MEMBER(audioirq_w);

	DECLARE_READ16_MEMBER(system_7a3a_r);

	void checkirq6();

	emu_timer *m_unk_timer;
	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	inline uint16_t read_space(uint32_t offset);
	inline void write_space(uint32_t offset, uint16_t data);

	// config registers (external pins)
	int m_boot_mode; // 2 pins determine boot mode, likely only read at power-on
	sunplus_gcm394_cs_callback_device m_cs_callback;
};



class sunplus_gcm394_device : public sunplus_gcm394_base_device
{
public:
	template <typename T>
	sunplus_gcm394_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&screen_tag) :
		sunplus_gcm394_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
	}

	sunplus_gcm394_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
};


class generalplus_gpac800_device : public sunplus_gcm394_base_device
{
public:
	template <typename T>
	generalplus_gpac800_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&screen_tag) :
		generalplus_gpac800_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
		m_csbase = 0x30000;
	}

	generalplus_gpac800_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	void gpac800_internal_map(address_map &map);

	//virtual void device_start() override;
	virtual void device_reset() override;

private:
	void recalculate_calculate_effective_nand_address();

	DECLARE_READ16_MEMBER(nand_7850_status_r);
	DECLARE_READ16_MEMBER(nand_7854_r);
	DECLARE_WRITE16_MEMBER(nand_dma_ctrl_w);
	DECLARE_WRITE16_MEMBER(nand_7850_w);
	DECLARE_WRITE16_MEMBER(nand_command_w);
	DECLARE_WRITE16_MEMBER(nand_addr_low_w);
	DECLARE_WRITE16_MEMBER(nand_addr_high_w);
	DECLARE_READ16_MEMBER(nand_ecc_low_byte_error_flag_1_r);
	DECLARE_WRITE16_MEMBER(nand_7856_type_w);
	DECLARE_WRITE16_MEMBER(nand_7857_w);
	DECLARE_WRITE16_MEMBER(nand_785b_w);
	DECLARE_WRITE16_MEMBER(nand_785c_w);
	DECLARE_WRITE16_MEMBER(nand_785d_w);
	DECLARE_READ16_MEMBER(nand_785e_r);

	uint16_t m_nandcommand;

	uint16_t m_nand_addr_low;
	uint16_t m_nand_addr_high;

	uint16_t m_nand_dma_ctrl;
	uint16_t m_nand_7850;
	uint16_t m_nand_785d;
	uint16_t m_nand_785c;
	uint16_t m_nand_785b;
	uint16_t m_nand_7856;
	uint16_t m_nand_7857;

	int m_curblockaddr;
	uint32_t m_effectiveaddress;
};


class generalplus_gpspispi_device : public sunplus_gcm394_base_device
{
public:
	template <typename T>
	generalplus_gpspispi_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&screen_tag) :
		generalplus_gpspispi_device(mconfig, tag, owner, clock)
	{
		m_screen.set_tag(std::forward<T>(screen_tag));
		m_csbase = 0x30000;
	}

	generalplus_gpspispi_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

protected:
	void gpspispi_internal_map(address_map &map);

	//virtual void device_start() override;
	//virtual void device_reset() override;

private:
	DECLARE_READ16_MEMBER(spi_unk_7943_r);
};



DECLARE_DEVICE_TYPE(GCM394, sunplus_gcm394_device)
DECLARE_DEVICE_TYPE(GPAC800, generalplus_gpac800_device)
DECLARE_DEVICE_TYPE(GP_SPISPI, generalplus_gpspispi_device)

#endif // MAME_MACHINE_SUNPLUS_GCM394_H
