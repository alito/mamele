// license:BSD-3-Clause
// copyright-holders:Miodrag Milanovic
/*********************************************************************

    formats/imd_dsk.c

    IMD disk images

*********************************************************************/

#include "imd_dsk.h"

#include <cassert>
#include <cstring>



struct imddsk_tag
{
	int heads;
	int tracks;
	int sector_size;
	uint64_t track_offsets[84*2]; /* offset within data for each track */
};


static struct imddsk_tag *get_tag(floppy_image_legacy *floppy)
{
	struct imddsk_tag *tag;
	tag = (imddsk_tag *)floppy_tag(floppy);
	return tag;
}



FLOPPY_IDENTIFY( imd_dsk_identify )
{
	uint8_t header[3];

	floppy_image_read(floppy, header, 0, 3);
	if (header[0]=='I' && header[1]=='M' && header[2]=='D') {
		*vote = 100;
	} else {
		*vote = 0;
	}
	return FLOPPY_ERROR_SUCCESS;
}

static int imd_get_heads_per_disk(floppy_image_legacy *floppy)
{
	return get_tag(floppy)->heads;
}

static int imd_get_tracks_per_disk(floppy_image_legacy *floppy)
{
	return get_tag(floppy)->tracks;
}

static uint64_t imd_get_track_offset(floppy_image_legacy *floppy, int head, int track)
{
	return get_tag(floppy)->track_offsets[(track<<1) + head];
}

static floperr_t get_offset(floppy_image_legacy *floppy, int head, int track, int sector, bool sector_is_index, uint64_t *offset)
{
	uint64_t offs = 0;
	uint8_t header[5];
	uint8_t sector_num;
	int i;


	if ((head < 0) || (head >= get_tag(floppy)->heads) || (track < 0) || (track >= get_tag(floppy)->tracks)
			|| (sector < 0) )
		return FLOPPY_ERROR_SEEKERROR;

	offs = imd_get_track_offset(floppy,head,track);
	floppy_image_read(floppy, header, offs, 5);

	sector_num = header[3];
	offs += 5 + sector_num; // skip header and sector numbering map
	if(header[2] & 0x80) offs += sector_num; // skip cylinder numbering map
	if(header[2] & 0x40) offs += sector_num; // skip head numbering map
	get_tag(floppy)->sector_size = 1 << (header[4] + 7);
	for(i=0;i<sector;i++) {
		floppy_image_read(floppy, header, offs, 1); // take sector data type
		switch(header[0]) {
			case 0: offs++; break;
			case 1:
			case 3:
			case 5:
			case 7: offs += get_tag(floppy)->sector_size + 1; break;
			default: offs += 2;
		}
	}
	if (offset)
		*offset = offs;
	return FLOPPY_ERROR_SUCCESS;
}



static floperr_t internal_imd_read_sector(floppy_image_legacy *floppy, int head, int track, int sector, bool sector_is_index, void *buffer, size_t buflen)
{
	uint64_t offset;
	floperr_t err;
	uint8_t header[1];

	// take sector offset
	err = get_offset(floppy, head, track, sector, sector_is_index, &offset);
	if (err)
		return err;

	floppy_image_read(floppy, header, offset, 1);
	switch(header[0]) {
		case 0: break;
		case 1:
		case 3:
		case 5:
		case 7:
				floppy_image_read(floppy, buffer, offset+1, buflen);
				break;

		default: // all data same
				floppy_image_read(floppy, header, offset+1, 1);
				memset(buffer,header[0],buflen);
				break;
	}

	return FLOPPY_ERROR_SUCCESS;
}


static floperr_t imd_read_sector(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buflen)
{
	return internal_imd_read_sector(floppy, head, track, sector, false, buffer, buflen);
}

static floperr_t imd_read_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector, void *buffer, size_t buflen)
{
	return internal_imd_read_sector(floppy, head, track, sector, true, buffer, buflen);
}

static floperr_t imd_expand_file(floppy_image_legacy *floppy , uint64_t offset , size_t amount)
{
		if (amount == 0) {
				return FLOPPY_ERROR_SUCCESS;
		}

		uint64_t file_size = floppy_image_size(floppy);

		if (offset > file_size) {
				return FLOPPY_ERROR_INTERNAL;
		}

	uint64_t size_after_off = file_size - offset;

	if (size_after_off == 0) {
		return FLOPPY_ERROR_SUCCESS;
	}

	auto buffer = global_alloc_array(uint8_t , size_after_off);

	// Read the part of file after offset
	floppy_image_read(floppy , buffer , offset , size_after_off);

	// Add zeroes
	floppy_image_write_filler(floppy , 0 , offset , amount);

	// Write back the part of file after offset
	floppy_image_write(floppy, buffer, offset + amount, size_after_off);

	global_free_array(buffer);

	// Update track offsets
	struct imddsk_tag *tag = get_tag(floppy);
	for (int track = 0; track < tag->tracks; track++) {
		for (int head = 0; head < tag->heads; head++) {
			uint64_t *track_off = &(tag->track_offsets[ (track << 1) + head ]);
			if (*track_off >= offset) {
				*track_off += amount;
			}
		}
	}

	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t imd_write_indexed_sector(floppy_image_legacy *floppy, int head, int track, int sector_index, const void *buffer, size_t buflen, int ddam)
{
	uint64_t offset;
	floperr_t err;
	uint8_t header[1];

	// take sector offset
	err = get_offset(floppy, head, track, sector_index, true, &offset);
	if (err)
		return err;

	floppy_image_read(floppy, header, offset, 1);

	switch (header[ 0 ]) {
	case 0:
		return FLOPPY_ERROR_SEEKERROR;

	default:
		// Expand image file (from 1 byte to a whole sector)
		err = imd_expand_file(floppy , offset , buflen - 1);
		if (err) {
			return err;
		}
		// Fall through!

	case 1:
	case 3:
	case 5:
	case 7:
		// Turn every kind of sector into type 1 (normal data)
		header[ 0 ] = 1;
		floppy_image_write(floppy, header, offset, 1);
		// Write sector
		floppy_image_write(floppy, buffer, offset + 1, buflen);
		break;
	}

	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t imd_get_sector_length(floppy_image_legacy *floppy, int head, int track, int sector, uint32_t *sector_length)
{
	floperr_t err;
	err = get_offset(floppy, head, track, sector, false, nullptr);
	if (err)
		return err;

	if (sector_length) {
		*sector_length = get_tag(floppy)->sector_size;
	}
	return FLOPPY_ERROR_SUCCESS;
}

static floperr_t imd_get_indexed_sector_info(floppy_image_legacy *floppy, int head, int track, int sector_index, int *cylinder, int *side, int *sector, uint32_t *sector_length, unsigned long *flags)
{
	uint64_t offset;
	uint8_t header[5];
	uint8_t hd;
	uint8_t tr;
	uint32_t sector_size;
	uint8_t sector_num;

	offset = imd_get_track_offset(floppy,head,track);
	floppy_image_read(floppy, header, offset, 5);
	tr = header[1];
	hd = header[2];
	sector_num = header[3];
	sector_size = 1 << (header[4] + 7);
	if (sector_index >= sector_num) return FLOPPY_ERROR_SEEKERROR;
	if (cylinder) {
		if (head & 0x80) {
			floppy_image_read(floppy, header, offset + 5 + sector_num+ sector_index, 1);
			*cylinder = header[0];
		} else {
			*cylinder = tr;
		}
	}
	if (side) {
		if (head & 0x40) {
			if (head & 0x80) {
				floppy_image_read(floppy, header, offset + 5 + 2 * sector_num+sector_index, 1);
			} else {
				floppy_image_read(floppy, header, offset + 5 + sector_num+sector_index, 1);
			}
			*side = header[0];
		} else {
			*side = hd & 1;
		}
	}
	if (sector) {
		floppy_image_read(floppy, header, offset + 5 + sector_index, 1);
		*sector = header[0];
	}
	if (sector_length) {
		*sector_length = sector_size;
	}
	if (flags) {
		uint8_t skip;
		if (head & 0x40) {
			if (head & 0x80) {
				skip = 3;
			} else {
				skip = 2;
			}
		} else {
			skip = 1;
		}
		floppy_image_read(floppy, header, offset + 5 + skip * sector_num, 1);
		*flags = 0;
		if ((header[0]-1) & 0x02) *flags |= ID_FLAG_DELETED_DATA;
		if ((header[0]-1) & 0x04) *flags |= ID_FLAG_CRC_ERROR_IN_DATA_FIELD;
	}
	return FLOPPY_ERROR_SUCCESS;
}


FLOPPY_CONSTRUCT( imd_dsk_construct )
{
	struct FloppyCallbacks *callbacks;
	struct imddsk_tag *tag;
	uint8_t header[0x100];
	uint64_t pos = 0;
	int sector_size = 0;
	int sector_num;
	int i;
	if(params)
	{
		// create
		return FLOPPY_ERROR_UNSUPPORTED;
	}

	tag = (struct imddsk_tag *) floppy_create_tag(floppy, sizeof(struct imddsk_tag));
	if (!tag)
		return FLOPPY_ERROR_OUTOFMEMORY;

	floppy_image_read(floppy, header, pos, 1);
	while(header[0]!=0x1a) {
		pos++;
		floppy_image_read(floppy, header, pos, 1);
	}
	pos++;
	tag->tracks = 0;
	tag->heads = 1;
	do {
		floppy_image_read(floppy, header, pos, 5);
		if ((header[2] & 1)==1) tag->heads = 2;
		tag->track_offsets[(header[1]<<1) + (header[2] & 1)] = pos;
		sector_num = header[3];
		pos += 5 + sector_num; // skip header and sector numbering map
		if(header[2] & 0x80) pos += sector_num; // skip cylinder numbering map
		if(header[2] & 0x40) pos += sector_num; // skip head numbering map
		sector_size = 1 << (header[4] + 7);
		for(i=0;i<sector_num;i++) {
			floppy_image_read(floppy, header, pos, 1); // take sector data type
			switch(header[0]) {
				case 0: pos++; break;
				case 1:
				case 3:
				case 5:
				case 7: pos += sector_size + 1; break;
				default: pos += 2;break;
			}
		}
		tag->tracks += 1;
	} while(pos < floppy_image_size(floppy));
	if (tag->heads==2) {
		tag->tracks = tag->tracks / 2;
	}
	callbacks = floppy_callbacks(floppy);
	callbacks->read_sector = imd_read_sector;
	callbacks->read_indexed_sector = imd_read_indexed_sector;
	callbacks->write_indexed_sector = imd_write_indexed_sector;
	callbacks->get_sector_length = imd_get_sector_length;
	callbacks->get_heads_per_disk = imd_get_heads_per_disk;
	callbacks->get_tracks_per_disk = imd_get_tracks_per_disk;
	callbacks->get_indexed_sector_info = imd_get_indexed_sector_info;

	return FLOPPY_ERROR_SUCCESS;
}


// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/*********************************************************************

    formats/imd_dsk.c

    IMD disk images

*********************************************************************/

imd_format::imd_format()
{
}

const char *imd_format::name() const
{
	return "imd";
}

const char *imd_format::description() const
{
	return "IMD disk image";
}

const char *imd_format::extensions() const
{
	return "imd";
}

void imd_format::fixnum(char *start, char *end) const
{
	end--;
	if(*end != '0')
		return;
	while(end > start) {
		end--;
		if(*end == ' ')
			*end = '0';
		else if(*end != '0')
			return;
	};
}

int imd_format::identify(io_generic *io, uint32_t form_factor)
{
	char h[4];

	io_generic_read(io, h, 0, 4);
	if(!memcmp(h, "IMD ", 4))
		return 100;

	return 0;
}

bool imd_format::load(io_generic *io, uint32_t form_factor, floppy_image *image)
{
	uint64_t size = io_generic_size(io);
	std::vector<uint8_t> img(size);
	io_generic_read(io, &img[0], 0, size);

	uint64_t pos;
	for(pos=0; pos < size && img[pos] != 0x1a; pos++) {};
	pos++;

	m_comment.resize(pos);
	memcpy(&m_comment[0], &img[0], pos);

	if(pos >= size)
		return false;

	int tracks, heads;
	image->get_maximal_geometry(tracks, heads);

	while(pos < size) {
		m_mode.push_back(img[pos++]);
		m_track.push_back(img[pos++]);
		m_head.push_back(img[pos++]);
		m_sector_count.push_back(img[pos++]);
		m_ssize.push_back(img[pos++]);

		if(m_track.back() >= tracks)
		{
			osd_printf_error("imd_format: Track %d exceeds maximum of %d\n", m_track.back(), tracks);
			return false;
		}

		if((m_head.back() & 0x3f) >= heads)
		{
			osd_printf_error("imd_format: Head %d exceeds maximum of %d\n", m_head.back() & 0x3f, heads);
			return false;
		}

		if(m_ssize.back() == 0xff)
		{
			osd_printf_error("imd_format: Unsupported variable sector size on track %d head %d", m_track.back(), m_head.back() & 0x3f);
			return false;
		}

		uint32_t actual_size = m_ssize.back() < 7 ? 128 << m_ssize.back() : 8192;

		static const int rates[3] = { 500000, 300000, 250000 };
		bool fm = m_mode.back() < 3;
		int rate = rates[m_mode.back() % 3];
		int rpm = form_factor == floppy_image::FF_8 || (form_factor == floppy_image::FF_525 && rate >= 300000) ? 360 : 300;
		int cell_count = (fm ? 1 : 2)*rate*60/rpm;

		//const uint8_t *snum = &img[pos];
		m_snum.push_back(std::vector<uint8_t>(m_sector_count.back()));
		memcpy(&m_snum.back()[0], &img[pos], m_sector_count.back());
		pos += m_sector_count.back();

		//const uint8_t *tnum = head & 0x80 ? &img[pos] : nullptr;
		if (m_head.back() & 0x80)
		{
			m_tnum.push_back(std::vector<uint8_t>(m_sector_count.back()));
			memcpy(&m_tnum.back()[0], &img[pos], m_sector_count.back());
			pos += m_sector_count.back();
		}
		else
		{
			m_tnum.push_back(std::vector<uint8_t>(0));
		}

		//const uint8_t *hnum = head & 0x40 ? &img[pos] : nullptr;
		if (m_head.back() & 0x40)
		{
			m_hnum.push_back(std::vector<uint8_t>(m_sector_count.back()));
			memcpy(&m_hnum.back()[0], &img[pos], m_sector_count.back());
			pos += m_sector_count.back();
		}
		else
		{
			m_hnum.push_back(std::vector<uint8_t>(0));
		}

		uint8_t head = m_head.back() & 0x3f;

		int gap_3 = calc_default_pc_gap3_size(form_factor, actual_size);

		desc_pc_sector sects[256];

		for(int i=0; i<m_sector_count.back(); i++) {
			uint8_t stype        = img[pos++];
			sects[i].track       = m_tnum.back().size() ? m_tnum.back()[i] : m_track.back();
			sects[i].head        = m_hnum.back().size() ? m_hnum.back()[i] : m_head.back();
			sects[i].sector      = m_snum.back()[i];
			sects[i].size        = m_ssize.back();
			sects[i].actual_size = actual_size;

			if(stype == 0 || stype > 8) {
				sects[i].data = nullptr;
			} else {
				sects[i].deleted = stype == 3 || stype == 4 || stype == 7 || stype == 8;
				sects[i].bad_crc = stype == 5 || stype == 6 || stype == 7 || stype == 8;

				if(stype == 2 || stype == 4 || stype == 6 || stype == 8) {
					sects[i].data = global_alloc_array(uint8_t, actual_size);
					memset(sects[i].data, img[pos++], actual_size);
				} else {
					sects[i].data = &img[pos];
					pos += actual_size;
				}
			}
		}

		if(m_sector_count.back()) {
			if(fm) {
				build_pc_track_fm(m_track.back(), head, image, cell_count, m_sector_count.back(), sects, gap_3);
			} else {
				build_pc_track_mfm(m_track.back(), head, image, cell_count, m_sector_count.back(), sects, gap_3);
			}
		}

		for(int i=0; i< m_sector_count.back(); i++)
			if(sects[i].data && (sects[i].data < &img[0] || sects[i].data >= (&img[0] + size)))
				global_free_array(sects[i].data);
	}

	return true;
}

bool can_compress(const uint8_t* buffer, uint8_t ptrn, uint64_t size)
{
	for (; size > 0; size--)
		if (*buffer++ != ptrn)
			return false;
	return true;
}

bool imd_format::save(io_generic* io, floppy_image* image)
{
	uint64_t pos = 0;
	io_generic_write(io, &m_comment[0], pos, m_comment.size());
	pos += m_comment.size();

	for (int i = 0; i < m_mode.size(); i++)
	{
		io_generic_write(io, &m_mode[i], pos++, 1);
		io_generic_write(io, &m_track[i], pos++, 1);
		io_generic_write(io, &m_head[i], pos++, 1);
		io_generic_write(io, &m_sector_count[i], pos++, 1);
		io_generic_write(io, &m_ssize[i], pos++, 1);

		io_generic_write(io, &m_snum[i][0], pos, m_sector_count[i]);
		pos += m_sector_count[i];

		if (m_tnum[i].size())
		{
			io_generic_write(io, &m_tnum[i][0], pos, m_sector_count[i]);
			pos += m_sector_count[i];
		}

		if (m_hnum[i].size())
		{
			io_generic_write(io, &m_hnum[i][0], pos, m_sector_count[i]);
			pos += m_sector_count[i];
		}

		uint32_t actual_size = m_ssize[i] < 7 ? 128 << m_ssize[i] : 8192;
		uint8_t head = m_head[i] & 0x3f;

		bool fm = m_mode[i]< 3;

		uint8_t bitstream[500000 / 8];
		uint8_t sector_data[50000];
		desc_xs sectors[256];
		int track_size;
		generate_bitstream_from_track(m_track[i], head, 2000, bitstream, track_size, image);
		if (fm)
			extract_sectors_from_bitstream_fm_pc(bitstream, track_size, sectors, sector_data, sizeof(sector_data));
		else
			extract_sectors_from_bitstream_mfm_pc(bitstream, track_size, sectors, sector_data, sizeof(sector_data));

		uint8_t sdata[8192];
		for (int j = 0; j < m_sector_count[i]; j++) {

			desc_xs& xs = sectors[m_snum[i][j]];

			uint8_t mode;
			if (!xs.data)
			{
				mode = 0;
				io_generic_write(io, &mode, pos++, 1);
				continue;
			}
			else if (xs.size < actual_size) {
				memcpy((void*)sdata, xs.data, xs.size);
				memset((uint8_t*)sdata + xs.size, 0, xs.size - actual_size);
			}
			else
				memcpy((void*)sdata, xs.data, actual_size);

			if (can_compress(sdata, sdata[0], actual_size))
			{
				mode = 2;
				io_generic_write(io, &mode, pos++, 1);
				io_generic_write(io, &sdata[0], pos++, 1);
			}
			else
			{
				mode = 1;
				io_generic_write(io, &mode, pos++, 1);
				io_generic_write(io, &sdata, pos, actual_size);
				pos += actual_size;
			}
		}
	}

	return true;
}

bool imd_format::supports_save() const
{
	return true;
}

const floppy_format_type FLOPPY_IMD_FORMAT = &floppy_image_format_creator<imd_format>;
