// license:BSD-3-Clause
// copyright-holders:Ryan Holtz
//============================================================
//
//  bgfxutil.cpp - BGFX renderer helper utils
//
//============================================================

// MAMEOS headers
#include "emu.h"
#include "bgfxutil.h"
#include "copyutil.h"

#include "render.h"


const bgfx::Memory* bgfx_util::mame_texture_data_to_bgfx_texture_data(bgfx::TextureFormat::Enum &dst_format, uint32_t src_format, int width, int height, int rowpixels, const rgb_t *palette, void *base, uint16_t *out_pitch)
{
	bgfx::TextureInfo info;
	switch (src_format)
	{
		case PRIMFLAG_TEXFORMAT(TEXFORMAT_PALETTE16):
		case PRIMFLAG_TEXFORMAT(TEXFORMAT_YUY16):
			dst_format = bgfx::TextureFormat::RG8;
			if (out_pitch)
				*out_pitch = rowpixels * 2;
			break;
		case PRIMFLAG_TEXFORMAT(TEXFORMAT_ARGB32):
		case PRIMFLAG_TEXFORMAT(TEXFORMAT_RGB32):
			dst_format = bgfx::TextureFormat::BGRA8;
			if (out_pitch)
				*out_pitch = rowpixels * 4;
			break;
	}
	bgfx::calcTextureSize(info, rowpixels, height, 1, false, false, 1, dst_format);
	return bgfx::copy(base, info.storageSize);
	/*const bgfx::Memory* mem = bgfx::alloc(width * height * 4);
	uint32_t* dst = reinterpret_cast<uint32_t*>(mem->data);
	uint16_t* src16 = reinterpret_cast<uint16_t*>(base);
	uint32_t* src32 = reinterpret_cast<uint32_t*>(base);

	for (int y = 0; y < height; y++)
	{
	    switch (format)
	    {
	    case PRIMFLAG_TEXFORMAT(TEXFORMAT_PALETTE16):
	        copy_util::copyline_palette16(dst, src16, width, palette);
	        src16 += rowpixels;
	        break;
	    case PRIMFLAG_TEXFORMAT(TEXFORMAT_YUY16):
	        copy_util::copyline_yuy16_to_argb(dst, src16, width, palette, 1);
	        src16 += rowpixels;
	        break;
	    case PRIMFLAG_TEXFORMAT(TEXFORMAT_ARGB32):
	        copy_util::copyline_argb32(dst, src32, width, palette);
	        src32 += rowpixels;
	        break;
	    case PRIMFLAG_TEXFORMAT(TEXFORMAT_RGB32):
	        copy_util::copyline_rgb32(dst, src32, width, palette);
	        src32 += rowpixels;
	        break;
	    default:
	        break;
	    }
	    dst += width;
	}
	return mem;*/
}

uint64_t bgfx_util::get_blend_state(uint32_t blend)
{
	switch (blend)
	{
		case BLENDMODE_ALPHA:
			return BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA);
		case BLENDMODE_RGB_MULTIPLY:
			return BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_DST_COLOR, BGFX_STATE_BLEND_ZERO);
		case BLENDMODE_ADD:
			return BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_ONE);
		default:
			return 0L;
	}
	return 0L;
}
