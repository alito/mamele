// license:BSD-3-Clause
// copyright-holders:Nicola Salmoria
#include "emu.h"
#include "includes/ohmygod.h"

/***************************************************************************

  Callbacks for the TileMap code

***************************************************************************/

TILE_GET_INFO_MEMBER(ohmygod_state::get_tile_info)
{
	uint16_t code = m_videoram[2 * tile_index + 1];
	uint16_t attr = m_videoram[2 * tile_index];
	tileinfo.set(0,
			code,
			(attr & 0x0f00) >> 8,
			0);
}



/***************************************************************************

  Start the video hardware emulation.

***************************************************************************/

void ohmygod_state::video_start()
{
	m_bg_tilemap = &machine().tilemap().create(*m_gfxdecode, tilemap_get_info_delegate(*this, FUNC(ohmygod_state::get_tile_info)), TILEMAP_SCAN_ROWS, 8, 8, 64, 64);
}



/***************************************************************************

  Memory handlers

***************************************************************************/

void ohmygod_state::ohmygod_videoram_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_videoram[offset]);
	m_bg_tilemap->mark_tile_dirty(offset / 2);
}

void ohmygod_state::ohmygod_spritebank_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	if (ACCESSING_BITS_8_15)
		m_spritebank = data & 0x8000;
}

void ohmygod_state::ohmygod_scrollx_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_scrollx);
	m_bg_tilemap->set_scrollx(0, m_scrollx - 0x81ec);
}

void ohmygod_state::ohmygod_scrolly_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	COMBINE_DATA(&m_scrolly);
	m_bg_tilemap->set_scrolly(0, m_scrolly - 0x81ef);
}


/***************************************************************************

  Display refresh

***************************************************************************/

void ohmygod_state::draw_sprites( bitmap_ind16 &bitmap, const rectangle &cliprect )
{
	uint16_t *spriteram = m_spriteram;
	int offs;

	for (offs = 0; offs < m_spriteram.bytes() / 4; offs += 4)
	{
		int sx, sy, code, color, flipx;
		uint16_t *sr;

		sr = m_spritebank ? (spriteram + m_spriteram.bytes() / 4) : spriteram;

		code = sr[offs + 3] & 0x0fff;
		color = sr[offs + 2] & 0x000f;
		sx = sr[offs + 0] - 29;
		sy = sr[offs + 1];
		if (sy >= 32768)
			sy -= 65536;
		flipx = sr[offs + 3] & 0x8000;

		m_gfxdecode->gfx(1)->transpen(bitmap,cliprect,
				code,
				color,
				flipx,0,
				sx,sy,0);
	}
}

uint32_t ohmygod_state::screen_update_ohmygod(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	m_bg_tilemap->draw(screen, bitmap, cliprect, 0, 0);
	draw_sprites(bitmap, cliprect);
	return 0;
}
