// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
/*********************************************************************

    formats/dmk_dsk.h

    DMK disk images

TODO:
- Add write/format support.
- Check support on other drivers besides msx.

*********************************************************************/

#include "dmk_dsk.h"

#include "coretmpl.h"
#include "ioprocs.h"


namespace {

uint32_t wide_fm(uint16_t val)
{
	uint32_t res = 0;
	for (int i = 15; i >= 0; i--) {
		res |= (util::BIT(val, i) << (i*2 + 1));
	}
	return res;
}

uint32_t data_to_wide_fm(uint8_t val)
{
	uint16_t res = 0xaaaa;  // clock
	for (int i = 7; i >= 0; i--) {
		res |= (util::BIT(val, i) << i*2);      // data
	}
	return wide_fm(res);
}

} // anonymous namespace



dmk_format::dmk_format()
{
}


const char *dmk_format::name() const
{
	return "dmk";
}


const char *dmk_format::description() const
{
	return "DMK disk image";
}


const char *dmk_format::extensions() const
{
	return "dmk";
}


int dmk_format::identify(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants) const
{
	uint64_t size;
	if (io.length(size))
		return 0;

	const int header_size = 16;
	uint8_t header[header_size];
	size_t actual;
	io.read_at(0, header, header_size, actual);

	int tracks = header[1];
	int track_size = ( header[3] << 8 ) | header[2];
	int heads = (header[4] & 0x10) ? 1 : 2;

	// The first header byte must be 00 or FF
	if (header[0] != 0x00 && header[0] != 0xff)
	{
		return 0;
	}

	// Bytes C-F must be zero
	if (header[0x0c] != 0 || header[0xd] != 0 || header[0xe] != 0 || header[0xf] != 0)
	{
		return 0;
	}

	// Check track size within limits
	if (track_size < 0x80 || track_size > 0x3FFF )
	{
		return 0;
	}

	if (size == header_size + heads * tracks * track_size)
	{
		return FIFID_STRUCT|FIFID_SIZE;
	}

	return 0;
}


bool dmk_format::load(util::random_read &io, uint32_t form_factor, const std::vector<uint32_t> &variants, floppy_image *image) const
{
	size_t actual;

	const int header_size = 16;
	uint8_t header[header_size];
	io.read_at(0, header, header_size, actual);

	const int tracks = header[1];
	const int track_size = ( header[3] << 8 ) | header[2];
	const int heads = (header[4] & 0x10) ? 1 : 2;
	const bool is_sd = (header[4] & 0x40) ? true : false;

	auto variant = floppy_image::SSDD;
	if (is_sd)
	{
		if (heads == 2)
		{
			variant = floppy_image::DSSD;
		}
		else
		{
			variant = floppy_image::SSSD;
		}
	}
	else
	{
		if (heads == 2)
		{
			variant = floppy_image::DSDD;
		}
		else
		{
			variant = floppy_image::SSDD;
		}
	}
	image->set_variant(variant);

	int fm_stride = is_sd ? 1 : 2;

	for (int track = 0; track < tracks; track++)
	{
		for (int head = 0; head < heads; head++)
		{
			int fm_loss = 0;
			std::vector<uint8_t> track_data(track_size);
			std::vector<uint32_t> raw_track_data;
			int mark_location[64*2+1];
			uint8_t mark_value[64*2+1];
			bool mark_is_mfm[64*2+1];
			int iam_location = -1;

			// Read track
			io.read_at(header_size + (heads * track + head) * track_size, &track_data[0], track_size, actual);

			for (int i = 0; i < 64*2+1; i++)
			{
				mark_location[i] = -1;
				mark_value[i] = 0xfe;
				mark_is_mfm[i] = !is_sd;    // Use default encoding
			}
			int mark_count = 0;

			// Find IDAM/DAM locations
			uint16_t track_header_offset = 0;
			uint16_t track_offset = ( ( track_data[track_header_offset + 1] << 8 ) | track_data[track_header_offset] ) & 0x3fff;
			bool idam_is_mfm = (track_data[track_header_offset + 1] & 0x80) ? true : false;
			track_header_offset += 2;

			while ( track_offset != 0 && track_offset >= 0x83 && track_offset < track_size && track_header_offset < 0x80 )
			{
				// Assume 3 bytes before IDAM pointers are the start of IDAM indicators
				int mark_offset = idam_is_mfm ? 3 : 0;
				mark_location[mark_count] = track_offset - mark_offset;
				mark_value[mark_count] = 0xfe;
				mark_is_mfm[mark_count] = idam_is_mfm;
				mark_count++;

				int stride = idam_is_mfm ? 1 : fm_stride;
				// Scan for DAM location
				for (int i = track_offset + 10*stride; i < track_offset + 53*stride; i++)
				{
					if ((track_data[i] >= 0xf8 && track_data[i] <= 0xfb))
					{
						if (!idam_is_mfm || (track_data[i-1] == 0xa1 && track_data[i-2] == 0xa1))
						{
							mark_location[mark_count] = i - mark_offset;
							mark_value[mark_count] = track_data[i];
							mark_is_mfm[mark_count] = idam_is_mfm;
							mark_count++;

							break;
						}
					}
				}

				idam_is_mfm = (track_data[track_header_offset + 1] & 0x80) ? true : false;
				track_offset = ( ( track_data[track_header_offset + 1] << 8 ) | track_data[track_header_offset] ) & 0x3fff;
				track_header_offset += 2;
			}

			// Prevent encoding from switching after last sector
			if (mark_count > 0)
			{
				mark_is_mfm[mark_count] = mark_is_mfm[mark_count - 1];
			}

			// Find IAM location
			for(int i = mark_location[0] - 1; i >= 3; i--)
			{
				// It's usually 3 bytes but several dumped tracks seem to contain only 2 bytes
				if (track_data[i] == 0xfc && (is_sd || (track_data[i-1] == 0xc2 && track_data[i-2] == 0xc2)))
				{
					iam_location = i - (is_sd ? 0 : 3);
					break;
				}
			}

			int curr_mark = 0;
			bool enc_mfm = mark_is_mfm[curr_mark];
			for (int offset = 0x80; offset < track_size; offset++)
			{
				if (offset == iam_location)
				{
					if (!is_sd)
					{
						// Write IAM
						raw_w(raw_track_data, 16, 0x5224);
						raw_w(raw_track_data, 16, 0x5224);
						raw_w(raw_track_data, 16, 0x5224);
						offset += 3;
					}
					else
					{
						raw_w(raw_track_data, 32, wide_fm(0xf77a)); // FC clocked with D7
						offset += fm_stride;
					}
				}

				// If close to mark, switch encoding
				if (offset + 8 >= mark_location[curr_mark])
				{
					bool new_enc = mark_is_mfm[curr_mark];
					if (new_enc != enc_mfm)
					{
						enc_mfm = new_enc;
					}
				}
				if (offset == mark_location[curr_mark]
					|| (!enc_mfm && offset - fm_stride + 1 == mark_location[curr_mark])
					)
				{
					if (enc_mfm)
					{
						raw_w(raw_track_data, 16, 0x4489);
						raw_w(raw_track_data, 16, 0x4489);
						raw_w(raw_track_data, 16, 0x4489);
						offset += 3;
						if (fm_stride == 1)
						{
							fm_loss += 3;
						}
					}
					else
					{
						uint16_t mark;
						switch (mark_value[curr_mark])
						{
							default:
							case 0xfb: mark = 0xf56f; break;
							case 0xfa: mark = 0xf56e; break;
							case 0xf9: mark = 0xf56b; break;
							case 0xf8: mark = 0xf56a; break;
							case 0xfe: mark = 0xf57e; break;
						}
						raw_w(raw_track_data, 32, wide_fm(mark));
						offset += fm_stride;
					}
					curr_mark++;
				}

				if (enc_mfm)
				{
					if (fm_stride == 1)
					{
						fm_loss++;
					}
					mfm_w(raw_track_data, 8, track_data[offset]);
				}
				else
					raw_w(raw_track_data, 32, data_to_wide_fm(track_data[offset]));


				if (!enc_mfm)
					offset += fm_stride - 1;
			}

			if (fm_loss != 0)
			{
				if (enc_mfm)
				{
					for (int jj = 0; jj < fm_loss; jj++)
					{
						mfm_w(raw_track_data, 8, 0x4e);
					}
				}
				else
				{
					for (int jj = 0; jj < fm_loss/2; jj++)
					{
						raw_w(raw_track_data, 32, data_to_wide_fm(0xff));
					}
				}
			}

			generate_track_from_levels(track, head, raw_track_data, 0, image);
		}
	}

	return true;
}

bool dmk_format::supports_save() const
{
	return false;
}


const dmk_format FLOPPY_DMK_FORMAT;
