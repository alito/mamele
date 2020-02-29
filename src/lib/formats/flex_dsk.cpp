// license:BSD-3-Clause
// copyright-holders:Barry Rodewald, 68bit
/*
 * flex_dsk.c  -  FLEX compatible disk images
 *
 *  Created on: 24/06/2014
 *
 * This FLEX floppy disk image support leverages the wd177x_format support
 * with the few differences handled by format variations.
 *
 * FLEX numbers sectors on the second side of a track continuing from the
 * first side, so there are separate formats for each head on double sided
 * disk formats.
 *
 * FLEX on the 6809 typically encodes the first track, on both heads, in
 * single density for double density disks. This simplifies the ROM boot code
 * which need only handle single density. It might also have been a strategy
 * at the time for reading both single and double density disks. So there are
 * double density disk format variations that have the first track encoded in
 * single density.
 *
 * However FLEX double density disks did not necessarily have the first sector
 * encoded in single density. FLEX floppy disk drivers for the 6800 were
 * typically just patched for double density support, with the controller
 * jumpered for double density, and so did not encode the first sector in
 * single density. Also there are many 'emulator' disk images distributed for
 * which the first track has been padded to give a consistent number of
 * sectors per track. So there are variations for which the first track is
 * also encoded in double density.
 *
 * FLEX generally numbers sectors starting with ID 1, but there are exceptions
 * for 6800 boot sectors where the first one or two sectors of track 0 and
 * head 0 are numbered starting with ID 0 and so there are format variations
 * for these. This strategy simplified the 6800 ROM boot loader, minimising
 * its code size, which simply issues a multi-sector read and relies on a gap
 * in the sector IDs to halt that operation. So if only one sector is to be
 * loaded then the sector IDs are 0, 2, 3, ..., and if two sectors are to be
 * loaded then the sector IDs are 0, 1, 3, 4, .... This strategy was abandoned
 * for FLEX 6809 which numbers the boot sectors starting from ID 1 too.
 *
 * The formats below include interleaved sectors to improve performance. The
 * interleave has been chosen to perform well on slower hardware and software
 * combinations while still offering some performance improvement. Tighter
 * interleaving may well be possible but it depends on the systems and
 * software.
 *
 * Not all of the formats are practical on all hardware and software. E.g. The
 * higher density formats can require data rates that the CPU can not keep up
 * with.
 */

#include "flex_dsk.h"
#include "formats/imageutl.h"

flex_format::flex_format() : wd177x_format(formats)
{
}

const char *flex_format::name() const
{
	return "flex";
}

const char *flex_format::description() const
{
	return "FLEX compatible disk image";
}

const char *flex_format::extensions() const
{
	return "dsk";
}

int flex_format::identify(io_generic *io, uint32_t form_factor)
{
	int type = find_size(io, form_factor);

	if (type != -1)
		return 75;
	return 0;
}

int flex_format::find_size(io_generic *io, uint32_t form_factor)
{
	uint64_t size = io_generic_size(io);
	uint8_t boot0[256], boot1[256];

	// Look at the boot sector.
	// Density, sides, link??
	io_generic_read(io, &boot0, 256 * 0, sizeof(boot0));
	io_generic_read(io, &boot1, 256 * 1, sizeof(boot1));
	// Look at the system information sector.
	io_generic_read(io, &info, 256 * 2, sizeof(struct sysinfo_sector));

	LOG_FORMATS("FLEX floppy dsk: size %d bytes, %d total sectors, %d remaining bytes, expected form factor %x\n", (uint32_t)size, (uint32_t)size / 256, (uint32_t)size % 256, form_factor);

	// Consistency checks.
	if (info.fc_start_trk > info.last_trk || info.fc_end_trk > info.last_trk)
		return -1;
	if (info.fc_start_sec > info.last_sec || info.fc_end_sec > info.last_sec)
		return -1;
	if (info.month < 1 || info.month > 12 || info.day < 1 || info.day > 31)
		return -1;

	boot0_sector_id = 1;
	boot1_sector_id = 2;

	// This floppy format uses a strategy of looking for 6800 boot code to
	// set the numbering of the first two sectors. If this is shown to not
	// be practical in some common cases then a separate format variant
	// might be needed.
	if (boot0[0] == 0x8e && boot0[3] == 0x20)
	{
		// Found a 6800 stack load and branch, looks like a 6800 boot sector.
		boot0_sector_id = 0;

		// Look for a link to the next sector, normal usage.
		if (boot1[0] != 0 || boot1[1] != 3)
		{
			// If not then assume it is a boot sector.
			boot1_sector_id = 1;
		}
	}

	for (int i=0; formats[i].form_factor; i++) {
		const format &f = formats[i];
		if (form_factor != floppy_image::FF_UNKNOWN && form_factor != f.form_factor)
			continue;

		// Check consistency with the sysinfo record sector.
		if (f.track_count != info.last_trk + 1)
			continue;

		if (f.sector_count * f.head_count != info.last_sec)
			continue;

		unsigned int format_size = 0;
		for (int track=0; track < f.track_count; track++) {
			for (int head=0; head < f.head_count; head++) {
				const format &tf = get_track_format(f, head, track);
				format_size += compute_track_size(tf);
			}
		}

		if (format_size != size)
			continue;

		// Check that the boot sector ID matches.
		const format &ft0 = formats_track0[i];
		if (ft0.form_factor) {
			// There is a specialized track 0 format.
			if (ft0.sector_base_id == -1) {
				if (ft0.per_sector_id[0] != boot0_sector_id)
					continue;
			} else {
				if (ft0.sector_base_id != boot0_sector_id)
					continue;
			}
		} else {
			// Otherwise check the default track format.
			if (f.sector_base_id == -1) {
				if (f.per_sector_id[0] != boot0_sector_id)
					continue;
			} else {
				if (f.sector_base_id != boot0_sector_id)
					continue;
			}
		}

		LOG_FORMATS("FLEX matching format index %d\n", i);
		return i;
	}
	return -1;
}

const wd177x_format::format &flex_format::get_track_format(const format &f, int head, int track)
{
	int n = -1;

	for (int i = 0; formats[i].form_factor; i++) {
		if (&formats[i] == &f) {
			n = i;
			break;
		}
	}

	if (n < 0) {
		LOG_FORMATS("Error format not found\n");
		return f;
	}

	if (head >= f.head_count) {
		LOG_FORMATS("Error invalid head %d\n", head);
		return f;
	}

	if (track >= f.track_count) {
		LOG_FORMATS("Error invalid track %d\n", track);
		return f;
	}

	if (track > 0) {
		if (head == 1) {
			const format &fh1 = formats_head1[n];
			if (!fh1.form_factor) {
				LOG_FORMATS("Error expected a head 1 format\n");
				return f;
			}
			return fh1;
		}
		return f;
	}

	// Track 0

	if (head == 1) {
		const format &fh1t0 = formats_head1_track0[n];
		if (fh1t0.form_factor) {
			return fh1t0;
		}
		const format &fh1 = formats_head1[n];
		if (fh1.form_factor) {
			return fh1;
		}
		LOG_FORMATS("Error expected a head 1 format\n");
		return f;
	}

	// Head 0

	const format &ft0 = formats_track0[n];
	if (ft0.form_factor) {
		return ft0;
	}

	return f;
}


const flex_format::format flex_format::formats[] = {
	{ // 0 87.5K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 1 87.5K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 2 87.5K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 3 100K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 4 100K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 5 100K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 6 200K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 7 200K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 8 200K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 9 175K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 10 175K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 11 175K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 12 200K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 13 200K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 14 200K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 15 400K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 16 400K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 17 400K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 18 155.5K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 19 155.5K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 20 155.5K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 21 157.5K 5 1/4 inch double density
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 22 157.5K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 23 157.5K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 24 311K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 25 311K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 26 311K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 27 315K 5 1/4 inch double density
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 28 315K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 29 315K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 30 178K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 31 178K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 32 178K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 33 180K 5 1/4 inch double density
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 34 180K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 35 180K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 36 356K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 37 356K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 38 356K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 39 360K 5 1/4 inch double density
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 40 360K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 41 360K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 42 358K 5 1/4 inch quad density (single density track 0)
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 43 358K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 44 358K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 45 360K 5 1/4 inch quad density
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 46 360K 5 1/4 inch quad density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 47 360K 5 1/4 inch quad density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 48 716K 5 1/4 inch quad density (single density track 0)
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 49 716K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 50 716K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 51 720K 5 1/4 inch quad density
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 52 720K 5 1/4 inch quad density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 53 720K 5 1/4 inch quad density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 54 288.75K 8 inch single density
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::FM,
		2000, 15, 77, 1, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 55 288.75K 8 inch single density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::FM,
		2000, 15, 77, 1, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 56 288.75K 8 inch single density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::FM,
		2000, 15, 77, 1, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 57 577.5K 8 inch single density
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 58 577.5K 8 inch single density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 59 577.5K 8 inch single density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 60 497.75K 8 inch double density (single density track 0)
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 61 497.75K 8 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 62 497.75K 8 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 63 500.5K 8 inch double density
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 64 500.5K 8 inch double density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 65 500.5K 8 inch double density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 66 995.5K 8 inch double density (single density track 0)
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 67 995.5K 8 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 68 995.5K 8 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 69 1001K 8 inch double density
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 70 1001K 8 inch double density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 71 1001K 8 inch double density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {1, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 72 1440K 3 1/2 inch high density (single density track 0)
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {1, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{ // 73 1440K 3 1/2 inch high density (single density track 0), 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {1, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{ // 74 1440K 3 1/2 inch high density (single density track 0), 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {1, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{ // 75 1440K 3 1/2 inch high density.
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {1, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{ // 76 1440K 3 1/2 inch high density, 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {1, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{ // 77 1440K 3 1/2 inch high density, 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {1, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{}
};

const flex_format::format flex_format::formats_head1[] = {
	{ // 0 87.5K 5 1/4 inch single density
	},
	{ // 1 87.5K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 2 87.5K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 3 100K 5 1/4 inch single density
	},
	{ // 4 100K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 5 100K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 6 200K 5 1/4 inch single density
	},
	{ // 7 200K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 8 200K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 9 175K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 10 175K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 11 175K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 12 200K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 13 200K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 14 200K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 15 400K 5 1/4 inch single density
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 16 400K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 17 400K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 18 155.5K 5 1/4 inch double density (single density track 0)
	},
	{ // 19 155.5K 5 1/4 inch double density (single density track 0), 6800 one boot sector
	},
	{ // 20 155.5K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
	},
	{ // 21 157.5K 5 1/4 inch double density
	},
	{ // 22 157.5K 5 1/4 inch double density, 6800 one boot sector
	},
	{ // 23 157.5K 5 1/4 inch double density, 6800 two boot sectors
	},
	{ // 24 311K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 25 311K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 26 311K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 27 315K 5 1/4 inch double density
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 28 315K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 29 315K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 30 178K 5 1/4 inch double density (single density track 0)
	},
	{ // 31 178K 5 1/4 inch double density (single density track 0), 6800 one boot sector
	},
	{ // 32 178K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
	},
	{ // 33 180K 5 1/4 inch double density
	},
	{ // 34 180K 5 1/4 inch double density, 6800 one boot sector
	},
	{ // 35 180K 5 1/4 inch double density, 6800 two boot sectors
	},
	{ // 36 356K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 37 356K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 38 356K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 39 360K 5 1/4 inch double density
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 40 360K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 41 360K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 42 358K 5 1/4 inch quad density (single density track 0)
	},
	{ // 43 358K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
	},
	{ // 44 358K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
	},
	{ // 45 360K 5 1/4 inch quad density
	},
	{ // 46 360K 5 1/4 inch quad density, 6800 one boot sector
	},
	{ // 47 360K 5 1/4 inch quad density, 6800 two boot sectors
	},
	{ // 48 716K 5 1/4 inch quad density (single density track 0)
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 49 716K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 50 716K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 51 720K 5 1/4 inch quad density
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 52 720K 5 1/4 inch quad density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 53 720K 5 1/4 inch quad density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 54 288.75K 8 inch single density
	},
	{ // 55 288.75K 8 inch single density, 6800 one boot sector
	},
	{ // 56 288.75K 8 inch single density, 6800 two boot sectors
	},
	{ // 57 577.5K 8 inch single density
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {16, 18, 20, 22, 24, 26, 28, 30, 17, 19, 21, 23, 25, 27, 29}, 40, 12, 12
	},
	{ // 58 577.5K 8 inch single density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {16, 18, 20, 22, 24, 26, 28, 30, 17, 19, 21, 23, 25, 27, 29}, 40, 12, 12
	},
	{ // 59 577.5K 8 inch single density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {16, 18, 20, 22, 24, 26, 28, 30, 17, 19, 21, 23, 25, 27, 29}, 40, 12, 12
	},
	{ // 60 497.75K 8 inch single density (single density track 0)
	},
	{ // 61 497.75K 8 inch single density (single density track 0), 6800 one boot sector
	},
	{ // 62 497.75K 8 inch single density (single density track 0), 6800 two boot sectors
	},
	{ // 63 500.5K 8 inch double density
	},
	{ // 64 500.5K 8 inch double density, 6800 one boot sector
	},
	{ // 65 500.5K 8 inch double density, 6800 two boot sectors
	},
	{ // 66 995.5K 8 inch double density (single density track 0)
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {27, 46, 39, 32, 51, 44, 37, 30, 49, 42, 35, 28, 47, 40, 33, 52, 45, 38, 31, 50, 43, 36, 29, 48, 41, 34}, 80, 22, 24
	},
	{ // 67 995.5K 8 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {27, 46, 39, 32, 51, 44, 37, 30, 49, 42, 35, 28, 47, 40, 33, 52, 45, 38, 31, 50, 43, 36, 29, 48, 41, 34}, 80, 22, 24
	},
	{ // 68 995.5K 8 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {27, 46, 39, 32, 51, 44, 37, 30, 49, 42, 35, 28, 47, 40, 33, 52, 45, 38, 31, 50, 43, 36, 29, 48, 41, 34}, 80, 22, 24
	},
	{ // 69 1001K 8 inch double density
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {27, 46, 39, 32, 51, 44, 37, 30, 49, 42, 35, 28, 47, 40, 33, 52, 45, 38, 31, 50, 43, 36, 29, 48, 41, 34}, 80, 22, 24
	},
	{ // 70 1001K 8 inch double density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {27, 46, 39, 32, 51, 44, 37, 30, 49, 42, 35, 28, 47, 40, 33, 52, 45, 38, 31, 50, 43, 36, 29, 48, 41, 34}, 80, 22, 24
	},
	{ // 71 1001K 8 inch double density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {27, 46, 39, 32, 51, 44, 37, 30, 49, 42, 35, 28, 47, 40, 33, 52, 45, 38, 31, 50, 43, 36, 29, 48, 41, 34}, 80, 22, 24
	},
	{ // 72 1440K 3 1/2 inch high density (single density track 0)
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {37, 62, 51, 40, 65, 54, 43, 68, 57, 46, 71, 60, 49, 38, 63, 52, 41, 66, 55, 44, 69, 58, 47, 72, 61, 50, 39, 64, 53, 42, 67, 56, 45, 70, 59, 48}, 80, 22, 24
	},
	{ // 73 1440K 3 1/2 inch high density (single density track 0), 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {37, 62, 51, 40, 65, 54, 43, 68, 57, 46, 71, 60, 49, 38, 63, 52, 41, 66, 55, 44, 69, 58, 47, 72, 61, 50, 39, 64, 53, 42, 67, 56, 45, 70, 59, 48}, 80, 22, 24
	},
	{ // 74 1440K 3 1/2 inch high density (single density track 0), 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {37, 62, 51, 40, 65, 54, 43, 68, 57, 46, 71, 60, 49, 38, 63, 52, 41, 66, 55, 44, 69, 58, 47, 72, 61, 50, 39, 64, 53, 42, 67, 56, 45, 70, 59, 48}, 80, 22, 24
	},
	{ // 75 1440K 3 1/2 inch high density
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {37, 62, 51, 40, 65, 54, 43, 68, 57, 46, 71, 60, 49, 38, 63, 52, 41, 66, 55, 44, 69, 58, 47, 72, 61, 50, 39, 64, 53, 42, 67, 56, 45, 70, 59, 48}, 80, 22, 24
	},
	{ // 76 1440K 3 1/2 inch high density, 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {37, 62, 51, 40, 65, 54, 43, 68, 57, 46, 71, 60, 49, 38, 63, 52, 41, 66, 55, 44, 69, 58, 47, 72, 61, 50, 39, 64, 53, 42, 67, 56, 45, 70, 59, 48}, 80, 22, 24
	},
	{ // 77 1440K 3 1/2 inch high density, 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {37, 62, 51, 40, 65, 54, 43, 68, 57, 46, 71, 60, 49, 38, 63, 52, 41, 66, 55, 44, 69, 58, 47, 72, 61, 50, 39, 64, 53, 42, 67, 56, 45, 70, 59, 48}, 80, 22, 24
	},
	{}
};

const flex_format::format flex_format::formats_track0[] = {
	{ // 0 87.5K 5 1/4 inch single density
	},
	{ // 1 87.5K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 2 87.5K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 3 100K 5 1/4 inch single density
	},
	{ // 4 100K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 5 100K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 6 200K 5 1/4 inch single density
	},
	{ // 7 200K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 8 200K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 9 175K 5 1/4 inch single density
	},
	{ // 10 175K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 11 175K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 12 200K 5 1/4 inch single density
	},
	{ // 13 200K 5 1/4 inch single densityo, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 14 200K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 15 400K 5 1/4 inch single density
	},
	{ // 16 400K 5 1/4 inch single density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 17 400K 5 1/4 inch single density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 18 155.5K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 19 155.5K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 20 155.5K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 35, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 21 157.5K 5 1/4 inch double density
	},
	{ // 22 157.5K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 23 157.5K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 35, 1, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 24 311K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 25 311K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 26 311K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 27 315K 5 1/4 inch double density
	},
	{ // 28 315K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 29 315K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 35, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 30 178K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 31 178K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 32 178K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 40, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 33 180K 5 1/4 inch double density
	},
	{ // 34 180K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 35 180K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSDD, floppy_image::MFM,
		2000, 18, 40, 1, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 36 356K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 37 356K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 38 356K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 39 360K 5 1/4 inch double density
	},
	{ // 40 360K 5 1/4 inch double density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 41 360K 5 1/4 inch double density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSDD, floppy_image::MFM,
		2000, 18, 40, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 42 358K 5 1/4 inch quad density (single density track 0)
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 43 358K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 44 358K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSSD, floppy_image::FM,
		4000, 10, 80, 1, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 45 360K 5 1/4 inch quad density
	},
	{ // 46 360K 5 1/4 inch quad density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 47 360K 5 1/4 inch quad density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::SSQD, floppy_image::MFM,
		2000, 18, 80, 1, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 48 716K 5 1/4 inch quad density (single density track 0)
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {1, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 49 716K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 2, 5, 8}, 40, 16, 11
	},
	{ // 50 716K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {0, 4, 7, 10, 3, 6, 9, 1, 5, 8}, 40, 16, 11
	},
	{ // 51 720K 5 1/4 inch quad density
	},
	{ // 52 720K 5 1/4 inch quad density, 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 53 720K 5 1/4 inch quad density, 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSQD, floppy_image::MFM,
		2000, 18, 80, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 80, 22, 24
	},
	{ // 54 288.75K 8 inch single density
	},
	{ // 55 288.75K 8 inch single density, 6800 one boot sector
	},
	{ // 56 288.75K 8 inch single density, 6800 two boot sectors
	},
	{ // 57 577.5K 8 inch single density
	},
	{ // 58 577.5K 8 inch single density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {0, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 59 577.5K 8 inch single density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {0, 3, 5, 7, 9, 11, 13, 15, 1, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 60 497.75K 8 inch double density (single density track 0)
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::FM,
		2000, 15, 77, 1, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 61 497.75K 8 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::FM,
		2000, 15, 77, 1, 256, {}, -1, {0, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 62 497.75K 8 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_8, floppy_image::SSSD, floppy_image::FM,
		2000, 15, 77, 1, 256, {}, -1, {0, 3, 5, 7, 9, 11, 13, 15, 1, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 63 500.5K 8 inch double density
	},
	{ // 64 500.5K 8 inch double density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {0, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 65 500.5K 8 inch double density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::SSDD, floppy_image::MFM,
		1000, 26, 77, 1, 256, {}, -1, {0, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 1, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 66 995.5K 8 inch double density (single density track 0)
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {1, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 67 995.5K 8 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {0, 3, 5, 7, 9, 11, 13, 15, 2, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 68 995.5K 8 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {0, 3, 5, 7, 9, 11, 13, 15, 1, 4, 6, 8, 10, 12, 14}, 40, 12, 12
	},
	{ // 69 1001K 8 inch double density
	},
	{ // 70 1001K 8 inch double density, 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {0, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 2, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 71 1001K 8 inch double density, 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSDD, floppy_image::MFM,
		1000, 26, 77, 2, 256, {}, -1, {0, 20, 13, 6, 25, 18, 11, 4, 23, 16, 9, 1, 21, 14, 7, 26, 19, 12, 5, 24, 17, 10, 3, 22, 15, 8}, 80, 22, 24
	},
	{ // 72 1440K 3 1/2 inch high density (single density track 0)
		floppy_image::FF_35,  floppy_image::DSSD, floppy_image::FM,
		2000, 18, 80, 2, 256, {}, -1, {1, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 40, 12, 12
	},
	{ // 73 1440K 3 1/2 inch high density (single density track 0), 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSSD, floppy_image::FM,
		2000, 18, 80, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 2, 7, 12, 17, 4, 9, 14}, 40, 12, 12
	},
	{ // 74 1440K 3 1/2 inch high density (single density track 0), 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSSD, floppy_image::FM,
		2000, 18, 80, 2, 256, {}, -1, {0, 6, 11, 16, 3, 8, 13, 18, 5, 10, 15, 1, 7, 12, 17, 4, 9, 14}, 40, 12, 12
	},
	{ // 75 1440K 3 1/2 inch high density
	},
	{ // 76 1440K 3 1/2 inch high density, 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {0, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 2, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{ // 77 1440K 3 1/2 inch high density, 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSHD, floppy_image::MFM,
		1000, 36, 80, 2, 256, {}, -1, {0, 26, 15, 4, 29, 18, 7, 32, 21, 10, 35, 24, 13, 1, 27, 16, 5, 30, 19, 8, 33, 22, 11, 36, 25, 14, 3, 28, 17, 6, 31, 20, 9, 34, 23, 12}, 80, 22, 24
	},
	{}
};

const flex_format::format flex_format::formats_head1_track0[] = {
	{ // 0 87.5K 5 1/4 inch single density
	},
	{ // 1 87.5K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 2 87.5K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 3 100K 5 1/4 inch single density
	},
	{ // 4 100K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 5 100K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 6 200K 5 1/4 inch single density
	},
	{ // 7 200K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 8 200K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 9 175K 5 1/4 inch single density
	},
	{ // 10 175K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 11 175K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 12 200K 5 1/4 inch single density
	},
	{ // 13 200K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 14 200K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 15 400K 5 1/4 inch single density
	},
	{ // 16 400K 5 1/4 inch single density, 6800 one boot sector
	},
	{ // 17 400K 5 1/4 inch single density, 6800 two boot sectors
	},
	{ // 18 155.5K 5 1/4 inch double density (single density track 0)
	},
	{ // 19 155.5K 5 1/4 inch double density (single density track 0), 6800 one boot sector
	},
	{ // 20 155.5K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
	},
	{ // 21 157.5K 5 1/4 inch double density
	},
	{ // 22 157.5K 5 1/4 inch double density
	},
	{ // 23 157.5K 5 1/4 inch double density
	},
	{ // 24 311K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 25 311K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 26 311K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 35, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 27 315K 5 1/4 inch double density
	},
	{ // 28 315K 5 1/4 inch double density, 6800 one boot sector
	},
	{ // 29 315K 5 1/4 inch double density, 6800 two boot sectors
	},
	{ // 30 178K 5 1/4 inch double density (single density track 0)
	},
	{ // 31 178K 5 1/4 inch double density (single density track 0), 6800 one boot sector
	},
	{ // 32 178K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
	},
	{ // 33 180K 5 1/4 inch double density
	},
	{ // 34 180K 5 1/4 inch double density, 6800 one boot sector
	},
	{ // 35 180K 5 1/4 inch double density, 6800 two boot sectors
	},
	{ // 36 356K 5 1/4 inch double density (single density track 0)
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 37 356K 5 1/4 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 38 356K 5 1/4 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 40, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 39 360K 5 1/4 inch double density
	},
	{ // 40 360K 5 1/4 inch double density, 6800 one boot sector
	},
	{ // 41 360K 5 1/4 inch double density, 6800 two boot sectors
	},
	{ // 42 358K 5 1/4 inch quad density (single density track 0)
	},
	{ // 43 358K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
	},
	{ // 44 358K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
	},
	{ // 45 360K 5 1/4 inch quad density
	},
	{ // 46 360K 5 1/4 inch quad density, 6800 one boot sector
	},
	{ // 47 360K 5 1/4 inch quad density, 6800 two boot sectors
	},
	{ // 48 716K 5 1/4 inch quad density (single density track 0)
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 49 716K 5 1/4 inch quad density (single density track 0), 6800 one boot sector
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 50 716K 5 1/4 inch quad density (single density track 0), 6800 two boot sectors
		floppy_image::FF_525, floppy_image::DSSD, floppy_image::FM,
		4000, 10, 80, 2, 256, {}, -1, {11, 14, 17, 20, 13, 16, 19, 12, 15, 18}, 40, 16, 11
	},
	{ // 51 720K 5 1/4 inch quad density
	},
	{ // 52 720K 5 1/4 inch quad density, 6800 one boot sector
	},
	{ // 53 720K 5 1/4 inch quad density, 6800 two boot sectors
	},
	{ // 54 288.75K 8 inch single density
	},
	{ // 55 288.75K 8 inch single density, 6800 one boot sector
	},
	{ // 56 288.75K 8 inch single density, 6800 two boot sectors
	},
	{ // 57 577.5K 8 inch single density
	},
	{ // 58 577.5K 8 inch single density, 6800 one boot sector
	},
	{ // 59 577.5K 8 inch single density, 6800 two boot sectors
	},
	{ // 60 497.75K 8 inch double density (single density track 0)
	},
	{ // 61 497.75K 8 inch double density (single density track 0), 6800 one boot sector
	},
	{ // 62 497.75K 8 inch double density (single density track 0), 6800 two boot sectors
	},
	{ // 63 500.5K 8 inch double density
	},
	{ // 64 500.5K 8 inch double density, 6800 one boot sector
	},
	{ // 65 500.5K 8 inch double density, 6800 two boot sectors
	},
	{ // 66 995.5K 8 inch double density (single density track 0)
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {16, 18, 20, 22, 24, 26, 28, 30, 17, 19, 21, 23, 25, 27, 29}, 40, 12, 12
	},
	{ // 67 995.5K 8 inch double density (single density track 0), 6800 one boot sector
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {16, 18, 20, 22, 24, 26, 28, 30, 17, 19, 21, 23, 25, 27, 29}, 40, 12, 12
	},
	{ // 68 995.5K 8 inch double density (single density track 0), 6800 two boot sectors
		floppy_image::FF_8, floppy_image::DSSD, floppy_image::FM,
		2000, 15, 77, 2, 256, {}, -1, {16, 18, 20, 22, 24, 26, 28, 30, 17, 19, 21, 23, 25, 27, 29}, 40, 12, 12
	},
	{ // 69 1001K 8 inch double density
	},
	{ // 70 1001K 8 inch double density, 6800 one boot sector
	},
	{ // 71 1001K 8 inch double density, 6800 two boot sectors
	},
	{ // 72 1440K 3 1/2 inch high density (single density track 0)
		floppy_image::FF_35,  floppy_image::DSSD, floppy_image::FM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 73 1440K 3 1/2 inch high density (single density track 0), 6800 one boot sector
		floppy_image::FF_35,  floppy_image::DSSD, floppy_image::FM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 74 1440K 3 1/2 inch high density (single density track 0), 6800 two boot sectors
		floppy_image::FF_35,  floppy_image::DSSD, floppy_image::FM,
		2000, 18, 80, 2, 256, {}, -1, {19, 24, 29, 34, 21, 26, 31, 36, 23, 28, 33, 20, 25, 30, 35, 22, 27, 32}, 80, 22, 24
	},
	{ // 75 1440K 3 1/2 inch high density
	},
	{ // 76 1440K 3 1/2 inch high density, 6800 one boot sector
	},
	{ // 77 1440K 3 1/2 inch high density, 6800 two boot sectors
	},
	{}
};

const floppy_format_type FLOPPY_FLEX_FORMAT = &floppy_image_format_creator<flex_format>;
