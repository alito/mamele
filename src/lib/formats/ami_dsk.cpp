// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/ami_dsk.c

    Amiga disk images

*********************************************************************/

#include <cassert>

#include "formats/ami_dsk.h"

adf_format::adf_format() : floppy_image_format_t()
{
}

const char *adf_format::name() const
{
	return "adf";
}

const char *adf_format::description() const
{
	return "Amiga ADF floppy disk image";
}

const char *adf_format::extensions() const
{
	return "adf";
}

bool adf_format::supports_save() const
{
	return true;
}

int adf_format::identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants)
{
	uint64_t size = io_generic_size(io);
	if ((size == 901120) || (size == 912384) || (size == 1802240))
	{
		return 50;
	}
	return 0;
}

bool adf_format::load(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image)
{
	desc_s sectors[22];
	uint8_t sectdata[512*22];
	bool is_hd = false;
	int tracks = 80;

	for(int i=0; i<22; i++) {
		sectors[i].data = sectdata + 512*i;
		sectors[i].size = 512;
		sectors[i].sector_id = i;
	}

	uint64_t size = io_generic_size(io);
	if(size == 901120)
	{
		is_hd = false;
		tracks = 80;
	}
	else if (size == 912384)
	{
		is_hd = false;
		tracks = 81;
	}
	else
	{
		is_hd = true;
		tracks = 80;
	}

	if (!is_hd) {
		image->set_variant(floppy_image::DSDD);
		for(int track=0; track < tracks; track++) {
			for(int side=0; side < 2; side++) {
				io_generic_read(io, sectdata, (track*2 + side)*512*11, 512*11);
				generate_track(amiga_11, track, side, sectors, 11, 100000, image);
			}
		}
	} else {
		image->set_variant(floppy_image::DSHD);
		for(int track=0; track < tracks; track++) {
			for(int side=0; side < 2; side++) {
				io_generic_read(io, sectdata, (track*2 + side)*512*22, 512*22);
				generate_track(amiga_22, track, side, sectors, 22, 200000, image);
			}
		}
	}

	return true;
}

uint32_t adf_format::g32(const std::vector<bool> &trackbuf, uint32_t pos)
{
	uint32_t res = 0;
	for(int i=0; i != 32; i++) {
		if(trackbuf[pos])
			res |= 0x80000000 >> i;
		pos ++;
		if(pos == trackbuf.size())
			pos = 0;
	}
	return res;
}

uint32_t adf_format::checksum(const std::vector<bool> &trackbuf, uint32_t pos, int long_count)
{
	uint32_t check = 0;
	for(int i=0; i<long_count; i++)
		check ^= g32(trackbuf, pos+32*i);
	return check & 0x55555555;
}

bool adf_format::save(io_generic *io, const std::vector<uint32_t> &variants, floppy_image *image)
{
	uint8_t sectdata[512*22];

	bool hd = image->get_variant() == floppy_image::DSHD;

	int data_track_size = hd ? 512*22 : 512*11;

	for(int track=0; track < 80; track++) {
		for(int side=0; side < 2; side++) {
			auto trackbuf = generate_bitstream_from_track(track, side, hd ? 1000 : 2000, image);

			for(uint32_t i=0; i<trackbuf.size(); i++)
				if(g32(trackbuf, i) == 0x44894489 &&
					(g32(trackbuf, i+384) & 0x55555555) == checksum(trackbuf, i+32, 10) &&
					(g32(trackbuf, i+448) & 0x55555555) == checksum(trackbuf, i+480, 256)) {
					uint32_t head = ((g32(trackbuf, i+32) & 0x55555555) << 1) | (g32(trackbuf, i+64) & 0x55555555);
					int sect = (head >> 8) & 0xff;
					if(sect > (hd ? 22 : 11))
						continue;

					uint8_t *dest = sectdata + 512*sect;
					for(int j=0; j<128; j++) {
						uint32_t val = ((g32(trackbuf, i+480+32*j) & 0x55555555) << 1) | (g32(trackbuf, i+4576+32*j) & 0x55555555);
						*dest++ = val >> 24;
						*dest++ = val >> 16;
						*dest++ = val >> 8;
						*dest++ = val;
					}

					io_generic_write(io, sectdata, (track*2 + side)*data_track_size, data_track_size);
				}
		}
	}
	return true;
}

const floppy_format_type FLOPPY_ADF_FORMAT = &floppy_image_format_creator<adf_format>;
