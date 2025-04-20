// license:BSD-3-Clause
// copyright-holders:Nathan Woods, R. Belmont
/*********************************************************************

    ap_dsk35.h

    Apple 3.5" disk images

*********************************************************************/
#ifndef MAME_FORMATS_AP_DSK35_H
#define MAME_FORMATS_AP_DSK35_H

#pragma once

#include "flopimg.h"

class dc42_format : public floppy_image_format_t
{
public:
	dc42_format();

	virtual int identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const override;
	virtual bool load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const override;
	virtual bool save(util::random_read_write &io, const std::vector<uint32_t> &variants, floppy_image *image) const override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;

private:
	static void update_chk(const uint8_t *data, int size, uint32_t &chk);
};

extern const dc42_format FLOPPY_DC42_FORMAT;

class apple_gcr_format : public floppy_image_format_t
{
public:
	apple_gcr_format();

	virtual int identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const override;
	virtual bool load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const override;
	virtual bool save(util::random_read_write &io, const std::vector<uint32_t> &variants, floppy_image *image) const override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;
};

extern const apple_gcr_format FLOPPY_APPLE_GCR_FORMAT;

class apple_2mg_format : public floppy_image_format_t
{
public:
	apple_2mg_format();

	virtual int identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const override;
	virtual bool load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const override;
	virtual bool save(util::random_read_write &io, const std::vector<uint32_t> &variants, floppy_image *image) const override;

	virtual const char *name() const override;
	virtual const char *description() const override;
	virtual const char *extensions() const override;
	virtual bool supports_save() const override;
};

extern const apple_2mg_format FLOPPY_APPLE_2MG_FORMAT;

#endif // MAME_FORMATS_AP_DSK35_H
