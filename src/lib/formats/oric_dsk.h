// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/oric_dsk.h

    Oric disk images

*********************************************************************/
#ifndef MAME_FORMATS_ORIC_DSK_H
#define MAME_FORMATS_ORIC_DSK_H

#pragma once

#include "flopimg.h"

class oric_dsk_format : public floppy_image_format_t
{
public:
	oric_dsk_format() = default;
	virtual int identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants) override;
	virtual bool load(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) override;
	virtual bool save(io_generic *io, const std::vector<uint32_t> &variants, floppy_image *image) override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;
};

extern const floppy_format_type FLOPPY_ORIC_DSK_FORMAT;

class oric_jasmin_format : public floppy_image_format_t
{
public:
	oric_jasmin_format() = default;

	virtual int identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants) override;
	virtual bool load(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) override;
	virtual bool save(io_generic *io, const std::vector<uint32_t> &variants, floppy_image *image) override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;
};

extern const floppy_format_type FLOPPY_ORIC_JASMIN_FORMAT;

#endif // MAME_FORMATS_ORIC_DSK_H
