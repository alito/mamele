// license:BSD-3-Clause
// copyright-holders:Zsolt Vasvari
/*************************************************************************

    Hana Awase

*************************************************************************/
#ifndef MAME_INCLUDES_HANAAWAS_H
#define MAME_INCLUDES_HANAAWAS_H

#pragma once

#include "emupal.h"
#include "tilemap.h"

class hanaawas_state : public driver_device
{
public:
	hanaawas_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_videoram(*this, "videoram"),
		m_colorram(*this, "colorram"),
		m_maincpu(*this, "maincpu"),
		m_gfxdecode(*this, "gfxdecode")
	{ }

	void hanaawas(machine_config &config);

private:
	/* memory pointers */
	required_shared_ptr<uint8_t> m_videoram;
	required_shared_ptr<uint8_t> m_colorram;

	/* video-related */
	tilemap_t    *m_bg_tilemap;

	/* misc */
	int        m_mux;
	uint8_t    m_coin_settings;
	uint8_t    m_coin_impulse;
	uint8_t hanaawas_input_port_0_r();
	void hanaawas_inputs_mux_w(uint8_t data);
	void hanaawas_videoram_w(offs_t offset, uint8_t data);
	void hanaawas_colorram_w(offs_t offset, uint8_t data);
	void key_matrix_status_w(uint8_t data);
	void irq_ack_w(uint8_t data);
	TILE_GET_INFO_MEMBER(get_bg_tile_info);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	void hanaawas_palette(palette_device &palette) const;
	uint32_t screen_update_hanaawas(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void hanaawas_portB_w(uint8_t data);
	required_device<cpu_device> m_maincpu;
	required_device<gfxdecode_device> m_gfxdecode;
	void hanaawas_map(address_map &map);
	void io_map(address_map &map);
};

#endif // MAME_INCLUDES_HANAAWAS_H
