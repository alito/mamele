// license:BSD-3-Clause
// copyright-holders:Fabio Priuli, Acho A. Tang, R. Belmont
#ifndef MAME_VIDEO_K037122_H
#define MAME_VIDEO_K037122_H
#pragma once

#include "tilemap.h"

class k037122_device : public device_t,
						public device_video_interface,
						public device_gfx_interface
{
public:
	static constexpr feature_type imperfect_features() { return feature::GRAPHICS; } // unimplemented tilemap ROZ, scroll registers

	k037122_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// configuration
	void set_gfx_index(int index) { m_gfx_index = index; }

	void tile_draw( screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect );
	uint32_t sram_r(offs_t offset);
	void sram_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	uint32_t char_r(offs_t offset);
	void char_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);
	uint32_t reg_r(offs_t offset);
	void reg_w(offs_t offset, uint32_t data, uint32_t mem_mask = ~0);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	// internal state
	tilemap_t     *m_layer[2];

	std::unique_ptr<uint32_t[]>       m_tile_ram;
	std::unique_ptr<uint32_t[]>       m_char_ram;
	std::unique_ptr<uint32_t[]>       m_reg;

	int            m_gfx_index;

	TILE_GET_INFO_MEMBER(tile_info_layer0);
	TILE_GET_INFO_MEMBER(tile_info_layer1);
	void update_palette_color( uint32_t palette_base, int color );
};

DECLARE_DEVICE_TYPE(K037122, k037122_device)

#endif // MAME_VIDEO_K037122_H
