// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/ami_dsk.h

    Amiga disk images

*********************************************************************/
#ifndef MAME_FORMATS_AMI_DSK_H
#define MAME_FORMATS_AMI_DSK_H

#pragma once

#include "flopimg.h"

class adf_format : public floppy_image_format_t
{
public:
	adf_format();

	virtual int identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants) override;
	virtual bool load(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) override;
	virtual bool save(io_generic *io, const std::vector<uint32_t> &variants, floppy_image *image) override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;

private:
	static uint32_t g32(const std::vector<bool> &bitsteam, uint32_t pos);
	static uint32_t checksum(const std::vector<bool> &bitsteam, uint32_t pos, int long_count);
};

extern const floppy_format_type FLOPPY_ADF_FORMAT;

#endif // MAME_FORMATS_AMI_DSK_H
