// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
/*************************************************************************

    Tail to Nose / Super Formula

*************************************************************************/
#ifndef MAME_INCLUDES_TAIL2NOS_H
#define MAME_INCLUDES_TAIL2NOS_H

#pragma once

#include "machine/6850acia.h"
#include "machine/gen_latch.h"
#include "video/k051316.h"
#include "emupal.h"
#include "tilemap.h"

class tail2nos_state : public driver_device
{
public:
	tail2nos_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_txvideoram(*this, "txvideoram"),
		m_spriteram(*this, "spriteram"),
		m_zoomram(*this, "k051316"),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_k051316(*this, "k051316"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_soundlatch(*this, "soundlatch"),
		m_acia(*this, "acia"),
		m_analog(*this, "AN%u", 0U)
	{ }

	void tail2nos(machine_config &config);

	template <int N> DECLARE_CUSTOM_INPUT_MEMBER(analog_in_r);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;

private:
	/* memory pointers */
	required_shared_ptr<uint16_t> m_txvideoram;
	required_shared_ptr<uint16_t> m_spriteram;
	required_shared_ptr<uint16_t> m_zoomram;

	/* video-related */
	tilemap_t   *m_tx_tilemap;
	int         m_txbank;
	int         m_txpalette;
	bool        m_video_enable;
	bool        m_flip_screen;

	/* devices */
	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_audiocpu;
	required_device<k051316_device> m_k051316;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<palette_device> m_palette;
	required_device<generic_latch_8_device> m_soundlatch;
	required_device<acia6850_device> m_acia;
	required_ioport_array<2> m_analog;

	void tail2nos_txvideoram_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void tail2nos_zoomdata_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0);
	void tail2nos_gfxbank_w(uint8_t data);
	void sound_bankswitch_w(uint8_t data);
	uint8_t sound_semaphore_r();
	TILE_GET_INFO_MEMBER(get_tile_info);
	uint32_t screen_update_tail2nos(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void tail2nos_postload();
	void draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect );
	K051316_CB_MEMBER(zoom_callback);
	void main_map(address_map &map);
	void sound_map(address_map &map);
	void sound_port_map(address_map &map);
};

#endif // MAME_INCLUDES_TAIL2NOS_H
