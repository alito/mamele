// license:BSD-3-Clause
// copyright-holders:Nathan Woods
/*********************************************************************

    formats/pc_dsk.c

    PC disk images

*********************************************************************/

#include <cstring>
#include <cassert>

#include "formats/pc_dsk.h"

pc_format::pc_format() : upd765_format(formats)
{
}

const char *pc_format::name() const
{
	return "pc";
}

const char *pc_format::description() const
{
	return "PC floppy disk image";
}

const char *pc_format::extensions() const
{
	return "dsk,ima,img,ufi,360";
}

int pc_format::identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants)
{
	uint64_t size = io_generic_size(io);

	/* some 360K images have a 512-byte header */
	if (size == 368640 + 0x200) {
		file_header_skip_bytes = 0x200;
	}

	/* Disk Copy 4.2 images have an 84-byte header */
	if (size == 1474560 + 84) {
		file_header_skip_bytes = 84;
	}

	/* some 1.44MB images have a 1024-byte footer */
	if (size == 1474560 + 0x400) {
		file_footer_skip_bytes = 0x400;
	}

	return upd765_format::identify(io, form_factor, variants);
}

const pc_format::format pc_format::formats[] = {
	{   /*  160K 5 1/4 inch double density single sided */
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000,  8, 40, 1, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /*  320K 5 1/4 inch double density */
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000,  8, 40, 2, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /*  180K 5 1/4 inch double density single sided */
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000,  9, 40, 1, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /*  360K 5 1/4 inch double density */
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000,  9, 40, 2, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /*  360K 5 1/4 inch double density, 41 tracks */
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000,  9, 41, 2, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /*  360K 5 1/4 inch double density, 42 tracks */
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000,  9, 42, 2, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /*  400K 5 1/4 inch double density - gaps unverified */
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 10, 40, 2, 512, {}, 1, {}, 80, 50, 22, 36
	},
	{   /*  720K 5 1/4 inch quad density - gaps unverified */
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000,  9, 80, 2, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /* 1200K 5 1/4 inch high density */
		floppy_image::FF_525, floppy_image::DSHD, floppy_image::MFM,
		1200, 15, 80, 2, 512, {}, 1, {}, 80, 50, 22, 84
	},
	{   /*  720K 3 1/2 inch double density */
		floppy_image::FF_35,  floppy_image::DSDD, floppy_image::MFM,
		2000,  9, 80, 2, 512, {}, 1, {}, 80, 50, 22, 80
	},
	{   /* 1200K 3 1/2 inch high density (japanese variant) - gaps unverified */
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 15, 80, 2, 512, {}, 1, {}, 80, 50, 22, 84
	},
	{   /* 1440K 3 1/2 inch high density */
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 18, 80, 2, 512, {}, 1, {}, 80, 50, 22, 108
	},
	{   /* Microsoft DMF 1680K 3 1/2 inch high density - gaps unverified */
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 21, 80, 2, 512, {}, 1, {}, 80, 50, 22, 0xc
	},
	{   /* 2880K 3 1/2 inch extended density - gaps unverified */
		floppy_image::FF_35,  floppy_image::DSED, floppy_image::MFM,
			500, 36, 80, 2, 512, {}, 1, {}, 80, 50, 41, 80
	},
	{}
};

const floppy_format_type FLOPPY_PC_FORMAT = &floppy_image_format_creator<pc_format>;
