// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/hxcmfm_dsk.h

    HxC Floppy Emulator disk images

*********************************************************************/
#ifndef MAME_FORMATS_HXCMFM_DSK_H
#define MAME_FORMATS_HXCMFM_DSK_H

#pragma once

#include "flopimg.h"

class mfm_format : public floppy_image_format_t
{
public:
	mfm_format();

	virtual int identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const override;
	virtual bool load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const override;
	virtual bool save(util::random_read_write &io, const std::vector<uint32_t> &variants, floppy_image *image) const override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;
};

extern const mfm_format FLOPPY_MFM_FORMAT;

#endif // MAME_FORMATS_HXCMFM_DSK_H
