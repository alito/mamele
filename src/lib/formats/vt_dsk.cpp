// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/vt_dsk.cpp

    VTech disk image formats

*********************************************************************/

#include "formats/vt_dsk.h"

// Zero = |    9187     |
// One  = | 2237 | 6950 |
// 0.5us ~= 143

void vtech_common_format::wbit(std::vector<uint32_t> &buffer, uint32_t &pos, uint32_t &mg, bool bit)
{
	buffer.push_back(pos | mg);
		mg = mg == floppy_image::MG_A ? floppy_image::MG_B : floppy_image::MG_A;
	if(bit) {
		pos += 2237;
		buffer.push_back(pos | mg);
		mg = mg == floppy_image::MG_A ? floppy_image::MG_B : floppy_image::MG_A;
		pos += 6950;
	} else
		pos += 9187;
}

void vtech_common_format::wbyte(std::vector<uint32_t> &buffer, uint32_t &pos, uint32_t &mg, uint8_t byte)
{
	for(int i = 7; i >= 0; i--)
		wbit(buffer, pos, mg, (byte >> i) & 1);
}

void vtech_common_format::image_to_flux(const std::vector<uint8_t> &bdata, floppy_image *image)
{
	static const uint8_t sector_map[16] = {
		0x0, 0xb, 0x6, 0x1, 0xc, 0x7, 0x2, 0xd, 0x8, 0x3, 0xe, 0x9, 0x4, 0xf, 0xa, 0x5
	};

	for(int track = 0; track != 40; track ++) {
		uint32_t pos = 0;
		uint32_t mg = floppy_image::MG_A;
		std::vector<uint32_t> &buffer = image->get_buffer(track, 0);
		buffer.clear();
		image->set_write_splice_position(track, 0, 0);
		// One window of pad at the start to avoid problems with the write splice
		wbit(buffer, pos, mg, 0);

		for(int sector = 0; sector != 16; sector ++) {
			uint8_t sid = sector_map[sector];
			for(int i=0; i != 7; i++)
				wbyte(buffer, pos, mg, 0x80);
			wbyte(buffer, pos, mg, 0x00);
			wbyte(buffer, pos, mg, 0xfe);
			wbyte(buffer, pos, mg, 0xe7);
			wbyte(buffer, pos, mg, 0x18);
			wbyte(buffer, pos, mg, 0xc3);
			wbyte(buffer, pos, mg, track);
			wbyte(buffer, pos, mg, sid);
			wbyte(buffer, pos, mg, track+sid);
			for(int i=0; i != 5; i++)
				wbyte(buffer, pos, mg, 0x80);
			wbyte(buffer, pos, mg, 0x00);
			wbyte(buffer, pos, mg, 0xc3);
			wbyte(buffer, pos, mg, 0x18);
			wbyte(buffer, pos, mg, 0xe7);
			wbyte(buffer, pos, mg, 0xfe);
			uint16_t chk = 0;
			const uint8_t *src = bdata.data() + 16*128*track + 128*sid;
			for(int i=0; i != 128; i++) {
				chk += src[i];
				wbyte(buffer, pos, mg, src[i]);
			}
			wbyte(buffer, pos, mg, chk);
			wbyte(buffer, pos, mg, chk >> 8);
		}
		// Rest is just not formatted
		buffer.push_back(pos | floppy_image::MG_N);
	}
}

std::vector<uint8_t> vtech_common_format::flux_to_image(floppy_image *image)
{
	std::vector<uint8_t> bdata(16*256*40, 0);

	for(int track = 0; track != 40; track++) {
		auto buffer = image->get_buffer(track, 0);
		int sz = buffer.size();
		if(sz < 128)
			continue;

		std::vector<bool> bitstream;
		int lpos = -1;
		bool looped = !((buffer[sz-1] ^ buffer[0]) & floppy_image::MG_MASK);
		int cpos = looped ? sz-1 : 0;
		while(cpos != lpos) {
			int dt = looped && cpos == sz-1 ? (200000000 - (buffer[cpos] & floppy_image::TIME_MASK)) + (buffer[1] & floppy_image::TIME_MASK) :
				cpos == sz-1 ? 200000000 - (buffer[cpos] & floppy_image::TIME_MASK) :
				(buffer[cpos+1] & floppy_image::TIME_MASK) - (buffer[cpos] & floppy_image::TIME_MASK);
			int t = dt >= 9187 - 143 ? 0 :
				dt >= 2237 - 143 && dt <= 2237 + 143 ? 1 :
				2;
			if(t <= 1) {
				if(lpos == -1)
					lpos = cpos;
				bitstream.push_back(t);
			}
			cpos += 1;
			if(cpos == sz)
				cpos = looped ? 1 : 0;
		}
		int mode = 0;
		looped = false;
		int pos = 0;
		int count = 0;
		uint8_t *dest = nullptr;
		uint16_t checksum = 0;
		uint64_t buf = 0;
		sz = bitstream.size();
		if(sz < 128)
			continue;

		for(int i=0; i != 63; i++)
			buf = (buf << 1) | bitstream[sz-64+i];
		for(;;) {
			buf = (buf << 1) | bitstream[pos];
			count ++;
			switch(mode) {
			case 0: // idle
				if(buf == 0x80808000fee718c3)
					mode = 1;
				count = 0;
				break;

			case 1: // sector header
				if(count == 24) {
					uint8_t trk = buf >> 16;
					uint8_t sector = buf >> 8;
					uint8_t chk = buf;
					if(chk != sector + trk) {
						mode = 0;
						break;
					}
					checksum = 0;
					dest = bdata.data() + 128 * 16 * trk + 128 * (sector & 0xf);
					mode = 2;
				}
				break;

			case 2: // look for sector data
				if(buf == 0x80808000fee718c3)
					mode = 1;
				else if(buf == 0x80808000c318e7fe)
					mode = 3;
				count = 0;
				break;

			case 3: // sector data
				if(count <= 128*8 && !(count & 7)) {
					uint8_t byte = buf;
					checksum += byte;
					*dest++ = byte;
				} else if(count == 128*8+16) {
					//                  uint16_t disk_checksum = buf;
					//                  printf("sector checksum %04x %04x\n", checksum, disk_checksum);
					mode = 0;
				}
				break;
			}
			if(mode == 0 && looped)
				break;
			pos++;
			if(pos == sz) {
				pos = 0;
				looped = true;
			}
		}
	}

	return bdata;
}


const char *vtech_bin_format::name() const
{
	return "vtech_bin";
}

const char *vtech_bin_format::description() const
{
	return "VTech sector disk image";
}

const char *vtech_bin_format::extensions() const
{
	return "bin";
}

const char *vtech_dsk_format::name() const
{
	return "vtech_dsk";
}

const char *vtech_dsk_format::description() const
{
	return "VTech dsk image";
}

const char *vtech_dsk_format::extensions() const
{
	return "dsk";
}

int vtech_bin_format::identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants)
{
	int size = io_generic_size(io);

	if(size == 40*16*256)
		return 50;

	return 0;
}

int vtech_dsk_format::identify(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants)
{
	int size = io_generic_size(io);
	if(size < 256)
		return 0;

	std::vector<uint8_t> bdata(size);
	io_generic_read(io, bdata.data(), 0, size);

	// Structurally validate the presence of sector headers and data
	int count_sh = 0, count_sd = 0;
	uint64_t buf = 0;
	for(uint8_t b : bdata) {
		buf = (buf << 8) | b;
		if(buf == 0x80808000fee718c3)
			count_sh++;
		else if(buf == 0x80808000c318e7fe)
			count_sd++;
	}

	return count_sh >= 30*16 && count_sd >= 30*16 ? 100 : 0;
}

bool vtech_bin_format::load(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image)
{
	int size = io_generic_size(io);
	if(size != 40*16*256)
		return false;

	std::vector<uint8_t> bdata(size);
	io_generic_read(io, bdata.data(), 0, size);

	image_to_flux(bdata, image);
	image->set_form_variant(floppy_image::FF_525, floppy_image::SSSD);
	return true;
}

bool vtech_dsk_format::load(io_generic *io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image)
{
	int size = io_generic_size(io);
	std::vector<uint8_t> bdata(size);
	io_generic_read(io, bdata.data(), 0, size);

	std::vector<uint8_t> bdatax(128*16*40, 0);

	int mode = 0;
	int count = 0;
	uint16_t checksum = 0;
	uint64_t buf = 0;
	uint8_t *dest = nullptr;

	for(uint8_t b : bdata) {
		buf = (buf << 8) | b;
		count ++;
		switch(mode) {
		case 0: // idle
			if(buf == 0x80808000fee718c3)
				mode = 1;
			count = 0;
			break;

		case 1: // sector header
			if(count == 3) {
				uint8_t trk = buf >> 16;
				uint8_t sector = buf >> 8;
				uint8_t chk = buf;
				if(chk != sector + trk) {
					mode = 0;
					break;
				}
				dest = bdatax.data() + 128*16*trk + sector*128;
				checksum = 0;
				mode = 2;
			}
			break;

		case 2: // look for sector data
			if(buf == 0x80808000fee718c3)
				mode = 1;
			else if(buf == 0x80808000c318e7fe)
				mode = 3;
			count = 0;
			break;

		case 3: // sector data
			if(count <= 128) {
				uint8_t byte = buf;
				checksum += byte;
				*dest++ = byte;

			} else if(count == 128+2) {
				uint16_t disk_checksum = buf;
				disk_checksum = (disk_checksum << 8) | (disk_checksum >> 8);
				mode = 0;
			}
			break;
		}
	}

	image_to_flux(bdatax, image);
	image->set_form_variant(floppy_image::FF_525, floppy_image::SSSD);
	return true;
}

bool vtech_bin_format::save(io_generic *io, const std::vector<uint32_t> &variants, floppy_image *image)
{
	int tracks, heads;
	image->get_maximal_geometry(tracks, heads);
	if(tracks < 40)
		return false;

	auto bdata = flux_to_image(image);
	io_generic_write(io, bdata.data(), 0, bdata.size());
	return true;
}

bool vtech_dsk_format::save(io_generic *io, const std::vector<uint32_t> &variants, floppy_image *image)
{
	int tracks, heads;
	image->get_maximal_geometry(tracks, heads);
	if(tracks < 40)
		return false;

	auto bdata = flux_to_image(image);
	std::vector<uint8_t> bdatax(0x9b*16*40);

	// Format is essentially an idealized version of what's written on the disk

	static const uint8_t sector_map[16] = {
		0x0, 0xb, 0x6, 0x1, 0xc, 0x7, 0x2, 0xd, 0x8, 0x3, 0xe, 0x9, 0x4, 0xf, 0xa, 0x5
	};

	int pos = 0;
	for(int track = 0; track != 40; track ++) {
		for(int sector = 0; sector != 16; sector ++) {
			uint8_t sid = sector_map[sector];
			for(int i=0; i != 7; i++)
				bdatax[pos++] = 0x80;
			bdatax[pos++] = 0x00;
			bdatax[pos++] = 0xfe;
			bdatax[pos++] = 0xe7;
			bdatax[pos++] = 0x18;
			bdatax[pos++] = 0xc3;
			bdatax[pos++] = track;
			bdatax[pos++] = sid;
			bdatax[pos++] = track+sid;
			for(int i=0; i != 5; i++)
				bdatax[pos++] = 0x80;
			bdatax[pos++] = 0x00;
			bdatax[pos++] = 0xc3;
			bdatax[pos++] = 0x18;
			bdatax[pos++] = 0xe7;
			bdatax[pos++] = 0xfe;
			uint16_t chk = 0;
			const uint8_t *src = bdata.data() + 16*128*track + 128*sid;
			for(int i=0; i != 128; i++) {
				chk += src[i];
				bdatax[pos++] = src[i];
			}
			bdatax[pos++] = chk;
			bdatax[pos++] = chk >> 8;
		}
	}

	io_generic_write(io, bdatax.data(), 0, bdatax.size());
	return true;
}


const floppy_format_type FLOPPY_VTECH_BIN_FORMAT = &floppy_image_format_creator<vtech_bin_format>;
const floppy_format_type FLOPPY_VTECH_DSK_FORMAT = &floppy_image_format_creator<vtech_dsk_format>;
