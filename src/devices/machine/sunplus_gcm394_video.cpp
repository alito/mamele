// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, David Haywood
/*****************************************************************************

    SunPlus GCM394-series SoC peripheral emulation (Video)

**********************************************************************/

#include "emu.h"
#include "sunplus_gcm394_video.h"

DEFINE_DEVICE_TYPE(GCM394_VIDEO, gcm394_video_device, "gcm394_video", "SunPlus GCM394 System-on-a-Chip (Video)")

#define LOG_GCM394_VIDEO_DMA      (1U << 3)
#define LOG_GCM394_TMAP           (1U << 2)
#define LOG_GCM394_VIDEO          (1U << 1)

#define VERBOSE             (LOG_GCM394_TMAP | LOG_GCM394_VIDEO_DMA | LOG_GCM394_VIDEO)

#include "logmacro.h"


gcm394_base_video_device::gcm394_base_video_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, type, tag, owner, clock)
	//, device_gfx_interface(mconfig, *this, nullptr)
	, device_video_interface(mconfig, *this)
	, m_cpu(*this, finder_base::DUMMY_TAG)
	, m_screen(*this, finder_base::DUMMY_TAG)
//  , m_scrollram(*this, "scrollram")
	, m_video_irq_cb(*this)
	, m_palette(*this, "palette")
	, m_gfxdecode(*this, "gfxdecode")
	, m_space_read_cb(*this)
{
}

gcm394_video_device::gcm394_video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gcm394_base_video_device(mconfig, GCM394_VIDEO, tag, owner, clock)
{
}

void gcm394_base_video_device::decodegfx(const char* tag)
{
	if (!memregion(tag))
		return;

	uint8_t* gfxregion = memregion(tag)->base();
	int gfxregionsize = memregion(tag)->bytes();

	if (1)
	{
		gfx_layout obj_layout =
		{
			16,16,
			0,
			4,
			{ STEP4(0,1) },
			{ STEP16(0,4) },
			{ STEP16(0,4 * 16) },
			16 * 16 * 4
		};
		obj_layout.total = gfxregionsize / (16 * 16 * 4 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x10 * 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		gfx_layout obj_layout =
		{
			32,16,
			0,
			4,
			{ STEP4(0,1) },
			{ STEP32(0,4) },
			{ STEP16(0,4 * 32) },
			16 * 32 * 4
		};
		obj_layout.total = gfxregionsize / (16 * 32 * 4 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x10 * 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		gfx_layout obj_layout =
		{
			16,32,
			0,
			4,
			{ STEP4(0,1) },
			{ STEP16(0,4) },
			{ STEP32(0,4 * 16) },
			32 * 16 * 4
		};
		obj_layout.total = gfxregionsize / (32 * 16 * 4 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x10 * 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		gfx_layout obj_layout =
		{
			32,32,
			0,
			4,
			{ STEP4(0,1) },
			{ STEP32(0,4) },
			{ STEP32(0,4 * 32) },
			32 * 32 * 4
		};
		obj_layout.total = gfxregionsize / (32 * 32 * 4 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x10 * 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		gfx_layout obj_layout =
		{
			8,16,
			0,
			2,
			{ 0,1 },
			{ STEP8(0,2) },
			{ STEP16(0,2 * 8) },
			8 * 16 * 2
		};
		obj_layout.total = gfxregionsize / (8 * 16 * 2 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x40 * 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		const uint32_t texlayout_xoffset[64] = { STEP64(0,2) };
		const uint32_t texlayout_yoffset[32] = { STEP32(0,2 * 64) };

		gfx_layout obj_layout =
		{
			64,32,
			0,
			2,
			{ 0,1 },
			EXTENDED_XOFFS,
			EXTENDED_YOFFS,
			32 * 64 * 2,
			texlayout_xoffset,
			texlayout_yoffset
		};
		obj_layout.total = gfxregionsize / (16 * 32 * 2 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x40 * 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		gfx_layout obj_layout =
		{
			32,32,
			0,
			8,
			{ STEP8(0,1) },
			{ STEP32(0,8) },
			{ STEP32(0,8 * 32) },
			32 * 32 * 8
		};
		obj_layout.total = gfxregionsize / (32 * 32 * 8 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x10, 0));
		m_maxgfxelement++;
	}

	if (1)
	{
		gfx_layout obj_layout =
		{
			32,32,
			0,
			6,
			{ 0,1,2,3,4,5 },
			{ STEP32(0,6) },
			{ STEP32(0,6 * 32) },
			32 * 32 * 6
		};
		obj_layout.total = gfxregionsize / (32 * 32 * 6 / 8);
		m_gfxdecode->set_gfx(m_maxgfxelement, std::make_unique<gfx_element>(m_palette, obj_layout, gfxregion, 0, 0x40, 0));
		m_maxgfxelement++;
	}
}

void gcm394_base_video_device::device_start()
{
	for (uint8_t i = 0; i < 32; i++)
	{
		m_rgb5_to_rgb8[i] = (i << 3) | (i >> 2);
	}
	for (uint16_t i = 0; i < 0x8000; i++)
	{
		m_rgb555_to_rgb888[i] = (m_rgb5_to_rgb8[(i >> 10) & 0x1f] << 16) |
								(m_rgb5_to_rgb8[(i >>  5) & 0x1f] <<  8) |
								(m_rgb5_to_rgb8[(i >>  0) & 0x1f] <<  0);
	}

	m_video_irq_cb.resolve();

	m_maxgfxelement = 0;

	decodegfx(":maincpu");

	save_item(NAME(m_spriteextra));
	save_item(NAME(m_spriteram));

	m_space_read_cb.resolve_safe(0);
}

void gcm394_base_video_device::device_reset()
{
	for (int i = 0; i < 6; i++)
	{
		m_tmap0_regs[i] = 0x0000;
		m_tmap1_regs[i] = 0x0000;
	}

	for (int i = 0; i < 0x400; i++)
	{
		m_spriteextra[i] = 0x0000;
		m_spriteram[i] = 0x0000;
	}

	for (int i=0;i<0x100;i++)
		m_paletteram[i] = machine().rand()&0x7fff;


	m_707f = 0x0000;
	m_703a_palettebank = 0x0000;
	m_7062 = 0x0000;
	m_7063 = 0x0000;

	m_702a = 0x0000;
	m_7030_brightness = 0x0000;
	m_703c = 0x0000;

	m_7042_sprite = 0x0000;

	m_7080 = 0x0000;
	m_7081 = 0x0000;
	m_7082 = 0x0000;
	m_7083 = 0x0000;
	m_7084 = 0x0000;
	m_7085 = 0x0000;
	m_7086 = 0x0000;
	m_7087 = 0x0000;
	m_7088 = 0x0000;

	m_707e_videodma_bank = 0x0000;
	m_videodma_size = 0x0000;
	m_videodma_dest = 0x0000;
	m_videodma_source = 0x0000;

	m_video_irq_status = 0x0000;

	m_sprite_7022_gfxbase_lsb = 0;
	m_sprite_702d_gfxbase_msb = 0;
	m_unk_vid1_gfxbase_lsb = 0;
	m_unk_vid1_gfxbase_msb = 0;
	m_unk_vid2_gfxbase_lsb = 0;
	m_unk_vid2_gfxbase_msb = 0;

}

/*************************
*     Video Hardware     *
*************************/

template<gcm394_base_video_device::blend_enable_t Blend, gcm394_base_video_device::rowscroll_enable_t RowScroll, gcm394_base_video_device::flipx_t FlipX>
void gcm394_base_video_device::draw(const rectangle &cliprect, uint32_t line, uint32_t xoff, uint32_t yoff, uint32_t bitmap_addr, uint32_t tile, int32_t h, int32_t w, uint8_t bpp, uint32_t yflipmask, uint32_t palette_offset, int addressing_mode)
{
	uint32_t nc_bpp = ((bpp) + 1) << 1;

	// probably don't do this here as this SoC has extended palette for higher bpp modes
	//palette_offset >>= nc_bpp;
	//palette_offset <<= nc_bpp;

	uint32_t bits_per_row = nc_bpp * w / 16;
	uint32_t words_per_tile;

	if (addressing_mode == 1)
	{
		words_per_tile = bits_per_row * h;
	}
	else
	{
		words_per_tile = 8; // seems to be correct for sprites regardless of size / bpp on smartfp
	}

	uint32_t m = (bitmap_addr) + (words_per_tile * tile + bits_per_row * (line ^ yflipmask));


	uint32_t bits = 0;
	uint32_t nbits = 0;
	uint32_t y = line;

	int yy = (yoff + y) & 0x1ff;
	if (yy >= 0x01c0)
		yy -= 0x0200;

	if (yy > cliprect.max_y || yy < 0)
		return;

	int y_index = yy * m_screen->width();

	for (int32_t x = FlipX ? (w - 1) : 0; FlipX ? x >= 0 : x < w; FlipX ? x-- : x++)
	{
		int xx = xoff + x;

		bits <<= nc_bpp;

		if (nbits < nc_bpp)
		{
			uint16_t b = m_space_read_cb((m++ & 0x3fffff));
			b = (b << 8) | (b >> 8);
			bits |= b << (nc_bpp - nbits);
			nbits += 16;
		}
		nbits -= nc_bpp;

		int pen = bits >> 16;

		int current_palette_offset = palette_offset;

		// for planes above 4bpp palette ends up being pulled from different places?
		if (nc_bpp < 6)
		{
			// 2bpp
			// 4bpp

			current_palette_offset |= 0x0800;

		}
		else if (nc_bpp < 8)
		{
			// 6bpp

		}
		else
		{
			//pen = machine().rand() & 0x1f;
			// 8bpp
		}

		uint32_t pal = current_palette_offset + pen;
		bits &= 0xffff;

		if (RowScroll)
			xx -= 0;// (int16_t)m_scrollram[yy & 0x1ff];

		xx &= 0x01ff;
		if (xx >= 0x01c0)
			xx -= 0x0200;

		if (xx >= 0 && xx <= cliprect.max_x)
		{
			int pix_index = xx + y_index;

			uint16_t rgb = m_paletteram[pal];

			if (!(rgb & 0x8000))
			{
				if (Blend)
				{
					/*
					m_screenbuf[pix_index] = (mix_channel((uint8_t)(m_screenbuf[pix_index] >> 16), m_rgb5_to_rgb8[(rgb >> 10) & 0x1f]) << 16) |
					                         (mix_channel((uint8_t)(m_screenbuf[pix_index] >>  8), m_rgb5_to_rgb8[(rgb >> 5) & 0x1f]) << 8) |
					                         (mix_channel((uint8_t)(m_screenbuf[pix_index] >>  0), m_rgb5_to_rgb8[rgb & 0x1f]));
					*/
					m_screenbuf[pix_index] = m_rgb555_to_rgb888[rgb];
				}
				else
				{
					m_screenbuf[pix_index] = m_rgb555_to_rgb888[rgb];
				}
			}
		}
	}
}

void gcm394_base_video_device::draw_page(const rectangle &cliprect, uint32_t scanline, int priority, uint32_t bitmap_addr, uint16_t *regs)
{
	uint32_t xscroll = regs[0];
	uint32_t yscroll = regs[1];
	uint32_t attr_reg = regs[2];
	uint32_t ctrl_reg = regs[3];
	uint32_t tilemap = regs[4];
	uint32_t palette_map = regs[5];
	address_space &space = m_cpu->space(AS_PROGRAM);

	if (!(ctrl_reg & PAGE_ENABLE_MASK))
	{
		return;
	}

	if (((attr_reg & PAGE_PRIORITY_FLAG_MASK) >> PAGE_PRIORITY_FLAG_SHIFT) != priority)
	{
		return;
	}

	uint32_t tile_h = 8 << ((attr_reg & PAGE_TILE_HEIGHT_MASK) >> PAGE_TILE_HEIGHT_SHIFT);
	uint32_t tile_w = 8 << ((attr_reg & PAGE_TILE_WIDTH_MASK) >> PAGE_TILE_WIDTH_SHIFT);

	uint32_t tile_count_x = 512 / tile_w;

	uint32_t bitmap_y = (scanline + yscroll) & 0xff;
	uint32_t y0 = bitmap_y / tile_h;
	uint32_t tile_scanline = bitmap_y % tile_h;
	uint32_t tile_address = tile_count_x * y0;

	for (uint32_t x0 = 0; x0 < tile_count_x; x0++, tile_address++)
	{
		uint32_t yy = ((tile_h * y0 - yscroll + 0x10) & 0xff) - 0x10;
		uint32_t xx = (tile_w * x0 - xscroll) & 0x1ff;
		uint32_t tile = (ctrl_reg & PAGE_WALLPAPER_MASK) ? space.read_word(tilemap) : space.read_word(tilemap + tile_address);

		uint16_t palette = (ctrl_reg & PAGE_WALLPAPER_MASK) ? space.read_word(palette_map) : space.read_word(palette_map + tile_address / 2);
		if (x0 & 1)
			palette >>= 8;


		if (!tile)
			continue;


		uint32_t tileattr = attr_reg;
		uint32_t tilectrl = ctrl_reg;

#if 0
		if ((ctrl_reg & 2) == 0)
		{   // -(1) bld(1) flip(2) pal(4)
			tileattr &= ~0x000c;
			tileattr |= (palette >> 2) & 0x000c;    // flip

			tileattr &= ~0x0f00;
			tileattr |= (palette << 8) & 0x0f00;    // palette

			tilectrl &= ~0x0100;
			tilectrl |= (palette << 2) & 0x0100;    // blend
		}
#endif
		bool blend;
		bool row_scroll;
		bool flip_x;
		uint32_t yflipmask;
		uint32_t palette_offset;

		blend = (tileattr & 0x4000 || tilectrl & 0x0100);
		row_scroll = (tilectrl & 0x0010);

		int use_alt_drawmode;

		if ((ctrl_reg & 2) == 0)
		{
			flip_x = 0;
			yflipmask = 0;
			palette_offset = (palette & 0x0f) << 4;
			use_alt_drawmode = 1;
		}
		else
		{
			flip_x = (tileattr & TILE_X_FLIP);
			yflipmask = tileattr & TILE_Y_FLIP ? tile_h - 1 : 0;
			palette_offset = (tileattr & 0x0f00) >> 4;
			tile |= (palette & 0x0007) << 16;
			use_alt_drawmode = 0;
		}


		//palette_offset |= 0x0900;
		palette_offset |= 0x0100;

		const uint8_t bpp = tileattr & 0x0003;


		if (blend)
		{
			if (row_scroll)
			{
				if (flip_x)
					draw<BlendOn, RowScrollOn, FlipXOn>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
				else
					draw<BlendOn, RowScrollOn, FlipXOff>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
			}
			else
			{
				if (flip_x)
					draw<BlendOn, RowScrollOff, FlipXOn>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
				else
					draw<BlendOn, RowScrollOff, FlipXOff>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
			}
		}
		else
		{
			if (row_scroll)
			{
				if (flip_x)
					draw<BlendOff, RowScrollOn, FlipXOn>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
				else
					draw<BlendOff, RowScrollOn, FlipXOff>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
			}
			else
			{
				if (flip_x)
					draw<BlendOff, RowScrollOff, FlipXOn>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
				else
					draw<BlendOff, RowScrollOff, FlipXOff>(cliprect, tile_scanline, xx, yy, bitmap_addr, tile, tile_h, tile_w, bpp, yflipmask, palette_offset, use_alt_drawmode);
			}
		}
	}
}


void gcm394_base_video_device::draw_sprite(const rectangle &cliprect, uint32_t scanline, int priority, uint32_t base_addr)
{
	uint32_t bitmap_addr = (m_sprite_702d_gfxbase_msb << 16) | m_sprite_7022_gfxbase_lsb;
	uint32_t tile = m_spriteram[base_addr + 0];
	int16_t x = m_spriteram[base_addr + 1];
	int16_t y = m_spriteram[base_addr + 2];
	uint16_t attr = m_spriteram[base_addr + 3];


	if (!tile) // this check needs to come before the additional attribute bits are added in? (smartfp title)
	{
		return;
	}

	int addressing_mode = 0;

	// m_7042_sprite is f7 on smartfp
	//                  01 on wrlshunt
	// this is not enough to conclude anything

	if (m_7042_sprite == 0x01)
		addressing_mode = 1;

	tile |= m_spriteextra[base_addr / 4] << 16;

	if (((attr & PAGE_PRIORITY_FLAG_MASK) >> PAGE_PRIORITY_FLAG_SHIFT) != priority)
	{
		return;
	}

	const uint32_t h = 8 << ((attr & PAGE_TILE_HEIGHT_MASK) >> PAGE_TILE_HEIGHT_SHIFT);
	const uint32_t w = 8 << ((attr & PAGE_TILE_WIDTH_MASK) >> PAGE_TILE_WIDTH_SHIFT);

	/*
	if (!(m_video_regs[0x42] & SPRITE_COORD_TL_MASK))
	{
	    x = (160 + x) - w / 2;
	    y = (120 - y) - (h / 2) + 8;
	}
	*/

	x &= 0x01ff;
	y &= 0x01ff;

	uint32_t tile_line = ((scanline - y) + 0x200) % h;
	int16_t test_y = (y + tile_line) & 0x1ff;
	if (test_y >= 0x01c0)
		test_y -= 0x0200;

	if (test_y != scanline)
	{
		return;
	}

	bool blend = (attr & 0x4000);

	bool flip_x = false;

	// different attribute use?
	if (addressing_mode == 0)
	{
		flip_x = (attr & TILE_X_FLIP);
	}

	const uint8_t bpp = attr & 0x0003;
	const uint32_t yflipmask = attr & TILE_Y_FLIP ? h - 1 : 0;
	uint32_t palette_offset = (attr & 0x0f00) >> 4;

	//palette_offset |= 0x0d00;
	palette_offset |= 0x0500;

	if (blend)
	{
		if (flip_x)
			draw<BlendOn, RowScrollOff, FlipXOn>(cliprect, tile_line, x, y, bitmap_addr, tile, h, w, bpp, yflipmask, palette_offset, addressing_mode);
		else
			draw<BlendOn, RowScrollOff, FlipXOff>(cliprect, tile_line, x, y, bitmap_addr, tile, h, w, bpp, yflipmask, palette_offset, addressing_mode);
	}
	else
	{
		if (flip_x)
			draw<BlendOff, RowScrollOff, FlipXOn>(cliprect, tile_line, x, y, bitmap_addr, tile, h, w, bpp, yflipmask, palette_offset, addressing_mode);
		else
			draw<BlendOff, RowScrollOff, FlipXOff>(cliprect, tile_line, x, y, bitmap_addr, tile, h, w, bpp, yflipmask, palette_offset, addressing_mode);
	}
}

void gcm394_base_video_device::draw_sprites(const rectangle &cliprect, uint32_t scanline, int priority)
{
	for (uint32_t n = 0; n < 0x100; n++)
	{
		draw_sprite(cliprect, scanline, priority, 4 * n);
	}
}

uint32_t gcm394_base_video_device::screen_update(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect)
{
	memset(&m_screenbuf[m_screen->width() * cliprect.min_y], 0, 4 *  m_screen->width() * ((cliprect.max_y - cliprect.min_y) + 1));

	const uint32_t page0_addr = (m_page0_addr_lsb | (m_page0_addr_msb<<16));
	const uint32_t page1_addr = (m_page1_addr_lsb | (m_page1_addr_msb<<16));
	uint16_t* page0_regs = m_tmap0_regs;
	uint16_t* page1_regs = m_tmap1_regs;

	for (uint32_t scanline = (uint32_t)cliprect.min_y; scanline <= (uint32_t)cliprect.max_y; scanline++)
	{
		for (int i = 0; i < 4; i++)
		{
			if (1)
			{
				draw_page(cliprect, scanline, i, page0_addr, page0_regs);
				draw_page(cliprect, scanline, i, page1_addr, page1_regs);
			}
			draw_sprites(cliprect, scanline, i);
		}
	}

	for (int y = cliprect.min_y; y <= cliprect.max_y; y++)
	{
		uint32_t *dest = &bitmap.pix32(y, cliprect.min_x);
		uint32_t *src = &m_screenbuf[cliprect.min_x + m_screen->width() * y];
		memcpy(dest, src, sizeof(uint32_t) * ((cliprect.max_x - cliprect.min_x) + 1));
	}

	return 0;
}


void gcm394_base_video_device::write_tmap_regs(int tmap, uint16_t* regs, int offset, uint16_t data)
{
	switch (offset)
	{
	case 0x0: // Page X scroll
		LOGMASKED(LOG_GCM394_TMAP, "%s: write_tmap_regs: Page %d X Scroll = %04x\n", machine().describe_context(), tmap, data & 0x01ff);
		regs[offset] = data & 0x01ff;
		break;

	case 0x1: // Page Y scroll
		LOGMASKED(LOG_GCM394_TMAP, "%s: write_tmap_regs: Page %d Y Scroll = %04x\n",  machine().describe_context(), tmap, data & 0x00ff);
		regs[offset] = data & 0x00ff;
		break;

	case 0x2: // Page Attributes
		LOGMASKED(LOG_GCM394_TMAP, "%s: write_tmap_regs: Page %d Attributes = %04x (unk %01x: Depth:%d, Palette:%d, VSize:%d, HSize:%d, FlipY:%d, FlipX:%d, BPP:%d)\n",  machine().describe_context(), tmap, data,
			(data & 0xc000) >> 14, (data >> 12) & 3, (data >> 8) & 15, 8 << ((data >> 6) & 3), 8 << ((data >> 4) & 3), BIT(data, 3), BIT(data, 2), 2 * ((data & 3) + 1));
		regs[offset] = data;
		break;

	case 0x3: // Page Control
		LOGMASKED(LOG_GCM394_TMAP, "%s: write_tmap_regs: Page %d Control = %04x (unk:%02x Blend:%d, HiColor:%d, unk:%d, unk%d, RowScroll:%d, Enable:%d, Wallpaper:%d, RegSet:%d, Bitmap:%d)\n",  machine().describe_context(), tmap, data,
			(data & 0xfe00) >> 9, BIT(data, 8), BIT(data, 7), BIT(data, 6), BIT(data, 5), BIT(data, 4), BIT(data, 3), BIT(data, 2), BIT(data, 1), BIT(data, 0));
		regs[offset] = data;
		break;

	case 0x4: // Page Tile Address
		LOGMASKED(LOG_GCM394_TMAP, "%s: write_tmap_regs: Page %d Tile Address = %04x\n",  machine().describe_context(), tmap, data);
		regs[offset] = data;
		break;

	case 0x5: // Page Attribute write_tmap_regs
		LOGMASKED(LOG_GCM394_TMAP, "%s: write_tmap_regs: Page %d Attribute Address = %04x\n",  machine().describe_context(), tmap, data);
		regs[offset] = data;
		break;
	}
}

// **************************************** TILEMAP 0 *************************************************

READ16_MEMBER(gcm394_base_video_device::tmap0_regs_r) { return m_tmap0_regs[offset]; }

WRITE16_MEMBER(gcm394_base_video_device::tmap0_regs_w)
{
	LOGMASKED(LOG_GCM394_TMAP, "%s:gcm394_base_video_device::tmap0_regs_w %01x %04x\n", machine().describe_context(), offset, data);
	write_tmap_regs(0, m_tmap0_regs, offset, data);
}

WRITE16_MEMBER(gcm394_base_video_device::tmap0_tilebase_lsb_w)
{
	LOGMASKED(LOG_GCM394_TMAP, "%s:gcm394_base_video_device::tmap0_tilebase_lsb_w %04x\n", machine().describe_context(), data);
	m_page0_addr_lsb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(tmap0 tilegfxbase is now %04x%04x)\n", m_page0_addr_msb, m_page0_addr_lsb);
}

WRITE16_MEMBER(gcm394_base_video_device::tmap0_tilebase_msb_w)
{
	LOGMASKED(LOG_GCM394_TMAP, "%s:gcm394_base_video_device::tmap0_tilebase_msb_w %04x\n", machine().describe_context(), data);
	m_page0_addr_msb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(tmap0 tilegfxbase is now %04x%04x)\n", m_page0_addr_msb, m_page0_addr_lsb);
}

// **************************************** TILEMAP 1 *************************************************

READ16_MEMBER(gcm394_base_video_device::tmap1_regs_r) { return m_tmap1_regs[offset]; }

WRITE16_MEMBER(gcm394_base_video_device::tmap1_regs_w)
{
	LOGMASKED(LOG_GCM394_TMAP, "%s:gcm394_base_video_device::tmap1_regs_w %01x %04x\n", machine().describe_context(), offset, data);
	write_tmap_regs(1, m_tmap1_regs, offset, data);
}

WRITE16_MEMBER(gcm394_base_video_device::tmap1_tilebase_lsb_w)
{
	LOGMASKED(LOG_GCM394_TMAP, "%s:gcm394_base_video_device::tmap1_tilebase_lsb_w %04x\n", machine().describe_context(), data);
	m_page1_addr_lsb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(tmap1 tilegfxbase is now %04x%04x)\n", m_page1_addr_msb, m_page1_addr_lsb);
}

WRITE16_MEMBER(gcm394_base_video_device::tmap1_tilebase_msb_w)
{
	LOGMASKED(LOG_GCM394_TMAP, "%s:gcm394_base_video_device::tmap1_tilebase_msb_w %04x\n", machine().describe_context(), data);
	m_page1_addr_msb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(tmap1 tilegfxbase is now %04x%04x)\n", m_page1_addr_msb, m_page1_addr_lsb);
}

// **************************************** unknown video device handler for below  *************************************************

// offsets 0,1,4,5,6,7 used in main IRQ code
// offsets 2,3 only cleared on startup

// Based on code analysis this seems to be the same as the regular tilemap regs, except for the addition of regs 2,3 which shift the remaining ones along.
// As the hardware appears to support ROZ these are probably 2 extra tile layers, with the 2 additional words being the ROZ parameters?
void gcm394_base_video_device::unk_vid_regs_w(int which, int offset, uint16_t data)
{
	switch (offset)
	{
	case 0x0:
		LOGMASKED(LOG_GCM394_VIDEO, "%s: unk_vid_regs_w (unk chip %d) (offset %01x) (data %04x) (X scroll?)\n", machine().describe_context(), which, offset, data); // masked with 0x3ff in code like x-scroll for tilemaps
		break;
	case 0x1:
		LOGMASKED(LOG_GCM394_VIDEO, "%s: unk_vid_regs_w (unk chip %d) (offset %01x) (data %04x) (y scroll?)\n", machine().describe_context(), which, offset, data); // masked with 0x3ff in code like x-scroll for tilemaps
		break;

	case 0x05: // seems to be similar / the same as Page Control for tilemaps (written with same basic code, but for these layers)
		LOGMASKED(LOG_GCM394_VIDEO, "%s: unk_vid_regs_w (unk chip %d) (offset %01x) (data %04x) (Page Control?)\n", machine().describe_context(), which, offset, data);
		break;

	case 0x02: // startup?
	case 0x03: // startup?
	case 0x04:
	case 0x06:
	case 0x07:
		LOGMASKED(LOG_GCM394_VIDEO, "%s: unk_vid_regs_w (unk chip %d) (offset %01x) (data %04x)\n", machine().describe_context(), which, offset, data);
		break;

	}
}

// **************************************** unknown video device 1 (another tilemap? roz? line? zooming sprite layer?) *************************************************

WRITE16_MEMBER(gcm394_base_video_device::unk_vid1_regs_w)
{
	unk_vid_regs_w(0, offset, data);
}

WRITE16_MEMBER(gcm394_base_video_device::unk_vid1_gfxbase_lsb_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::unk_vid1_gfxbase_lsb_w %04x\n", machine().describe_context(), data);
	m_unk_vid1_gfxbase_lsb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(unk_vid1 tilegfxbase is now %04x%04x)\n", m_unk_vid1_gfxbase_msb, m_unk_vid1_gfxbase_lsb);
}

WRITE16_MEMBER(gcm394_base_video_device::unk_vid1_gfxbase_msb_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::unk_vid1_gfxbase_msb_w %04x\n", machine().describe_context(), data);
	m_unk_vid1_gfxbase_msb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(unk_vid1 tilegfxbase is now %04x%04x)\n", m_unk_vid1_gfxbase_msb, m_unk_vid1_gfxbase_lsb);
}

// **************************************** unknown video device 2 (another tilemap? roz? lines? zooming sprite layer?) *************************************************

WRITE16_MEMBER(gcm394_base_video_device::unk_vid2_regs_w)
{
	unk_vid_regs_w(1, offset, data);
}

WRITE16_MEMBER(gcm394_base_video_device::unk_vid2_gfxbase_lsb_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::unk_vid2_gfxbase_lsb_w %04x\n", machine().describe_context(), data);
	m_unk_vid2_gfxbase_lsb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(unk_vid2 tilegfxbase is now %04x%04x)\n", m_unk_vid2_gfxbase_msb, m_unk_vid2_gfxbase_lsb);
}

WRITE16_MEMBER(gcm394_base_video_device::unk_vid2_gfxbase_msb_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::unk_vid2_gfxbase_msb_w %04x\n", machine().describe_context(), data);
	m_unk_vid2_gfxbase_msb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(unk_vid2 tilegfxbase is now %04x%04x)\n", m_unk_vid2_gfxbase_msb, m_unk_vid2_gfxbase_lsb);
}

// **************************************** sprite control registers *************************************************

// set to 001264c0 in wrlshunt, which point at the menu selectors (game names, arrows etc.)

WRITE16_MEMBER(gcm394_base_video_device::sprite_7022_gfxbase_lsb_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::sprite_7022_gfxbase_lsb_w %04x\n", machine().describe_context(), data);
	m_sprite_7022_gfxbase_lsb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(sprite tilebase is now %04x%04x)\n", m_sprite_702d_gfxbase_msb, m_sprite_7022_gfxbase_lsb);
}

WRITE16_MEMBER(gcm394_base_video_device::sprite_702d_gfxbase_msb_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::sprite_702d_gfxbase_msb_w %04x\n", machine().describe_context(), data);
	m_sprite_702d_gfxbase_msb = data;
	LOGMASKED(LOG_GCM394_TMAP, "\t(sprite tilebase tilegfxbase is now %04x%04x)\n", m_sprite_702d_gfxbase_msb, m_sprite_7022_gfxbase_lsb);
}

READ16_MEMBER(gcm394_base_video_device::sprite_7042_extra_r)
{
	uint16_t retdata = m_7042_sprite;
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::sprite_7042_extra_r (returning: %04x)\n", machine().describe_context(), retdata);
	return retdata;
}

WRITE16_MEMBER(gcm394_base_video_device::sprite_7042_extra_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::sprite_7042_extra_w %04x\n", machine().describe_context(), data);
	m_7042_sprite = data;
}


// **************************************** video DMA device *************************************************

WRITE16_MEMBER(gcm394_base_video_device::video_dma_source_w)
{
	LOGMASKED(LOG_GCM394_VIDEO_DMA, "%s:gcm394_base_video_device::video_dma_source_w %04x\n", machine().describe_context(), data);
	m_videodma_source = data;
}

WRITE16_MEMBER(gcm394_base_video_device::video_dma_dest_w)
{
	LOGMASKED(LOG_GCM394_VIDEO_DMA, "%s:gcm394_base_video_device::video_dma_dest_w %04x\n", machine().describe_context(), data);
	m_videodma_dest = data;
}

READ16_MEMBER(gcm394_base_video_device::video_dma_size_busy_r)
{
	LOGMASKED(LOG_GCM394_VIDEO_DMA, "%s:gcm394_base_video_device::video_dma_size_busy_r\n", machine().describe_context());
	return 0x0000;
}

WRITE16_MEMBER(gcm394_base_video_device::video_dma_size_trigger_w)
{
	LOGMASKED(LOG_GCM394_VIDEO_DMA, "%s:gcm394_base_video_device::video_dma_size_trigger_w %04x\n", machine().describe_context(), data);
	m_videodma_size = data;

	LOGMASKED(LOG_GCM394_VIDEO_DMA, "%s: doing sprite / video DMA source %04x dest %04x size %04x value of 707e (bank) %04x value of 707f %04x\n", machine().describe_context(), m_videodma_source, m_videodma_dest, m_videodma_size, m_707e_videodma_bank, m_707f );

	for (int i = 0; i <= m_videodma_size; i++)
	{
		uint16_t dat = space.read_word(m_videodma_source+i);
		space.write_word(m_videodma_dest + i, dat);
	}

	m_videodma_size = 0x0000;
}

WRITE16_MEMBER(gcm394_base_video_device::video_dma_unk_w)
{
	LOGMASKED(LOG_GCM394_VIDEO_DMA, "%s:gcm394_base_video_device::video_dma_unk_w %04x\n", machine().describe_context(), data);
	m_707e_videodma_bank = data;
}

READ16_MEMBER(gcm394_base_video_device::video_707c_r)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_707c_r\n", machine().describe_context());
	return 0x8000;
}

/* 707f is VERY important, lots of rendering codepaths in the code depend on the value it returns.

   all operations in the code based on 707f are bit based, usually read register, set / clear a bit
   and then write register, or read register and test an individual bit.

   our current codeflow means that bits are only ever set, not cleared.

   are the bits triggers? acks? enables? status flags?

   in wrlshunt this ends up being set to  02f9   ---- --x- xxxx x--x
   and in smartfp it ends up being set to 0065   ---- ---- -xx- -x-x

   is this because wrlshunt uses more layers?
*/

/*  wrlshunt BG Tilemap location note

   background tilemap appears to be at 24ad30 - 24af87 (byte address) in RAM  == 125698 - 1257c3 (word address)
   there are pointers to this
   (2879-287a) = 98 56 12 00 (00125698) (main background tilemap data is at this address)
   (287b-287c) = 30 5e 12 00 (00125e30) (address for other layer tilemap)
   where do we get these copied to registers or used as a source to copy from?
   does it depend on 707f behavior?

   if you return rand() on 707f reads sometimes you see
   [:maincpu] pc:053775: r4 = r4 lsr r3  (5698 0009) : [:maincpu] result 002b

   (bg tile addressing is also done by tile #, like the sprites, not fixed step like smartfp)

*/

READ16_MEMBER(gcm394_base_video_device::video_707f_r)
{
	uint16_t retdata = m_707f;
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_707f_r (returning %04x)\n", machine().describe_context(), retdata);
	return retdata;
}
WRITE16_MEMBER(gcm394_base_video_device::video_707f_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_707f_w %04x\n", machine().describe_context(), data);

	for (int i = 0; i < 16; i++)
	{
		uint16_t mask = 1 << i;

		if ((m_707f & mask) != (data & mask))
		{
			if (data & mask)
			{
				LOGMASKED(LOG_GCM394_VIDEO, "\tbit %04x Low -> High\n", mask);
			}
			else
			{
				LOGMASKED(LOG_GCM394_VIDEO, "\tbit %04x High -> Low\n", mask);
			}
		}
	}

	m_707f = data;
}

READ16_MEMBER(gcm394_base_video_device::video_703a_palettebank_r)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_703a_palettebank_r\n", machine().describe_context());
	return m_703a_palettebank;
}

WRITE16_MEMBER(gcm394_base_video_device::video_703a_palettebank_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_703a_palettebank_w %04x\n", machine().describe_context(), data);
	m_703a_palettebank = data;
}

READ16_MEMBER(gcm394_base_video_device::video_7062_r) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7062_r\n", machine().describe_context()); return m_7062; }
WRITE16_MEMBER(gcm394_base_video_device::video_7062_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7062_w %04x\n", machine().describe_context(), data); m_7062 = data; }

READ16_MEMBER(gcm394_base_video_device::video_7063_videoirq_source_r)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7063_videoirq_source_r\n", machine().describe_context());
	return machine().rand();
}


WRITE16_MEMBER(gcm394_base_video_device::video_7063_videoirq_source_ack_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7063_videoirq_source_ack_w %04x\n", machine().describe_context(), data);
	m_7063 = data;

	// ack or enable? happens near start of the IRQ
	if (data & 0x01)
	{
		m_video_irq_status &= ~1;
		check_video_irq();
	}
}

WRITE16_MEMBER(gcm394_base_video_device::video_702a_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_702a_w %04x\n", machine().describe_context(), data); m_702a = data; }

READ16_MEMBER(gcm394_base_video_device::video_curline_r)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s: video_r: Current Line: %04x\n", machine().describe_context(), m_screen->vpos());
	return m_screen->vpos();
}

// read in IRQ
READ16_MEMBER(gcm394_base_video_device::video_7030_brightness_r)
{
	/* wrlshunt ends up doing an explicit jump to 0000 shortly after boot if you just return the value written here, however I think that is correct code flow and something else is wrong
	   as this simply looks like some kind of brightness register - there is code to decrease it from 0xff to 0x00 by 0x5 increments (waiting for it to hit 0x05) and code to do the reverse
	   either way it really looks like the data written should be read back.
	*/
	uint16_t retdat = m_7030_brightness;
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7030_brightness_r (returning %04x)\n", machine().describe_context(), retdat);
	return retdat;
}

WRITE16_MEMBER(gcm394_base_video_device::video_7030_brightness_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7030_brightness_w %04x\n", machine().describe_context(), data);
	m_7030_brightness = data;
}

WRITE16_MEMBER(gcm394_base_video_device::video_703c_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_703c_w %04x\n", machine().describe_context(), data); m_703c = data; }

READ16_MEMBER(gcm394_base_video_device::video_7051_r)
{
	/* related to what ends up crashing wrlshunt? */
	uint16_t retdat = 0x03ff;
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7051_r (returning %04x)\n", machine().describe_context(), retdat);
	return retdat;
}

// this block get set once, in a single function, could be important
WRITE16_MEMBER(gcm394_base_video_device::video_7080_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7080_w %04x\n", machine().describe_context(), data); m_7080 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7081_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7081_w %04x\n", machine().describe_context(), data); m_7081 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7082_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7082_w %04x\n", machine().describe_context(), data); m_7082 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7083_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7083_w %04x\n", machine().describe_context(), data); m_7083 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7084_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7084_w %04x\n", machine().describe_context(), data); m_7084 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7085_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7085_w %04x\n", machine().describe_context(), data); m_7085 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7086_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7086_w %04x\n", machine().describe_context(), data); m_7086 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7087_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7087_w %04x\n", machine().describe_context(), data); m_7087 = data; }
WRITE16_MEMBER(gcm394_base_video_device::video_7088_w) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7088_w %04x\n", machine().describe_context(), data); m_7088 = data; }

READ16_MEMBER(gcm394_base_video_device::video_7083_r) { LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::video_7083_r\n", machine().describe_context()); return m_7083; }

WRITE16_MEMBER(gcm394_base_video_device::spriteram_w)
{
	// transfers an additional word for each sprite with this bit set (smartfp) or an entire extra bank (wrlshunt)
	// wrlshunt instead seems to base if it writes the extra data based on 707f so maybe this is more complex than banking

	// however for 707e only 0/1 is written, and it also gets written before system DMA, so despite being in the video DMA
	// region seems to operate separate from that.

	if (m_707e_videodma_bank == 0x0000)
	{
		m_spriteram[offset] = data;
	}
	else if (m_707e_videodma_bank == 0x0001)
	{
		m_spriteextra[offset] = data;
	}
	else
	{
		LOGMASKED(LOG_GCM394_VIDEO, "%s: spriteram_w %04x %04x unknown bank %04x\n", machine().describe_context(), offset, data, m_707e_videodma_bank);
	}
}

READ16_MEMBER(gcm394_base_video_device::spriteram_r)
{
	if (m_707e_videodma_bank == 0x0000)
	{
		return m_spriteram[offset];
	}
	else if (m_707e_videodma_bank == 0x0001)
	{
		return m_spriteextra[offset];
	}
	else
	{
		LOGMASKED(LOG_GCM394_VIDEO, "%s: spriteram_r %04x unknown bank %04x\n", machine().describe_context(), offset,  m_707e_videodma_bank);
		return 0x0000;
	}
}

WRITE16_MEMBER(gcm394_base_video_device::palette_w)
{
	LOGMASKED(LOG_GCM394_VIDEO, "%s:gcm394_base_video_device::palette_w %04x : %04x (value of 0x703a is %04x)\n", machine().describe_context(), offset, data, m_703a_palettebank);

	if (m_703a_palettebank & 0xfff0)
	{
		fatalerror("palette writes with m_703a_palettebank %04x\n", m_703a_palettebank);
	}
	else
	{
		offset |= (m_703a_palettebank & 0x000f) << 8;

		m_paletteram[offset] = data;

		uint32_t pal = m_rgb555_to_rgb888[data & 0x7fff];
		int r = (pal >> 16) & 0xff;
		int g = (pal >> 8) & 0xff;
		int b = (pal >> 0) & 0xff;

		m_palette->set_pen_color(offset, rgb_t(r, g, b));
	}
}

READ16_MEMBER(gcm394_base_video_device::palette_r)
{
	if (m_703a_palettebank & 0xfff0)
	{
		fatalerror("palette read with m_703a_palettebank %04x\n", m_703a_palettebank);
	}
	else
	{
		offset |= (m_703a_palettebank & 0x000f) << 8;
		return m_paletteram[offset];
	}
}


void gcm394_base_video_device::check_video_irq()
{
	m_video_irq_cb((m_video_irq_status & 1) ? ASSERT_LINE : CLEAR_LINE);
}

WRITE_LINE_MEMBER(gcm394_base_video_device::vblank)
{
	int i = 0x0001;

	if (!state)
	{
		m_video_irq_status &= ~i;
		check_video_irq();
		return;
	}

	//if (m_video_irq_enable & 1)
	{
		m_video_irq_status |= i;
		check_video_irq();
	}
}

static GFXDECODE_START( gfx )
GFXDECODE_END

void gcm394_base_video_device::device_add_mconfig(machine_config &config)
{
	PALETTE(config, m_palette).set_format(palette_device::xRGB_555, 256*0x10);
	GFXDECODE(config, m_gfxdecode, m_palette, gfx);

}


