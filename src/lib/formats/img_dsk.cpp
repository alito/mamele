// license:BSD-3-Clause
// copyright-holders: F. Ulivi
/*********************************************************************

    img_dsk.cpp

    "IMG" disk format

    This format is just a raw image of every sector on SSDD 8" floppy
    disks as used in Intel MDS-II systems.
    Files with this format have no header/trailer and are exactly
    512512 bytes in size (52 sectors, 1 head, 77 tracks,
    128 bytes per sector).

*********************************************************************/

#include "img_dsk.h"

#include "coretmpl.h" // BIT


// Debugging
#define VERBOSE 0
#define LOG(...)  do { if (VERBOSE) osd_printf_info(__VA_ARGS__); } while (false)

constexpr unsigned CELL_SIZE    = 1200; // Bit cell size (1 µs)
constexpr uint8_t  INDEX_AM     = 0x0c; // Index address mark
constexpr uint8_t  ID_AM        = 0x0e; // ID address mark
constexpr uint8_t  DATA_AM      = 0x0b; // Data address mark
constexpr uint8_t  AM_CLOCK     = 0x70; // Clock pattern of AM
constexpr unsigned PREIDX_GAP   = 45;   // Size of pre-index gap
constexpr unsigned SYNC_00_LEN  = 18;   // 00's in sync (gaps 1, 2, 3)
constexpr unsigned SYNC_FF_LEN  = 10;   // FF's in sync (gaps 1, 2, 3)
constexpr int ID_DATA_OFFSET    = 35 * 16;  // Nominal distance (in cells) between ID & DATA AM
// Size of image file
constexpr unsigned IMG_IMAGE_SIZE = img_format::TRACKS * img_format::HEADS * img_format::SECTORS * img_format::SECTOR_SIZE;
constexpr uint16_t CRC_POLY     = 0x1021;   // CRC-CCITT

img_format::img_format()
{
}

int img_format::identify(io_generic *io, uint32_t form_factor)
{
	uint64_t size = io_generic_size(io);

	if (((form_factor == floppy_image::FF_8) || (form_factor == floppy_image::FF_UNKNOWN)) &&
		size == IMG_IMAGE_SIZE) {
		return 50;
	} else {
		return 0;
	}
}

bool img_format::load(io_generic *io, uint32_t form_factor, floppy_image *image)
{
	uint64_t size = io_generic_size(io);
	if (size != IMG_IMAGE_SIZE) {
		return false;
	}
	image->set_variant(floppy_image::SSDD);

	// Suck in the whole image
	std::vector<uint8_t> image_data(size);
	io_generic_read(io, (void *)image_data.data(), 0, size);

	for (unsigned cyl = 0; cyl < TRACKS; cyl++) {
		std::vector<uint32_t> track_data;

		write_gap(track_data, 0 , PREIDX_GAP);
		write_mmfm_byte(track_data , INDEX_AM , AM_CLOCK);

		// Compute interleave factor and skew for current track
		unsigned il_factor;
		unsigned skew;
		if (cyl == 0) {
			il_factor = 1;
			skew = 51;
		} else if (cyl == 1) {
			il_factor = 4;
			skew = 48;
		} else {
			il_factor = 5;
			skew = (47 + 45 * (cyl - 2)) % 52;
		}
		std::vector<uint8_t> sector_list = interleaved_sectors(il_factor);

		for (unsigned sector = 0; sector < SECTORS; sector++) {
			unsigned real_sector = sector_list[ (sector + skew) % SECTORS ];
			unsigned offset_in_image = (real_sector - 1 + cyl * SECTORS) * SECTOR_SIZE;
			write_sector(track_data , cyl , real_sector , &image_data[ offset_in_image ]);
		}
		fill_with_gap4(track_data);
		generate_track_from_levels(cyl , 0 , track_data , 0 , image);
	}
	return true;
}

bool img_format::save(io_generic *io, floppy_image *image)
{
	for (int cyl = 0; cyl < TRACKS; cyl++) {
		uint8_t bitstream[ 21000 ];
		int bitstream_size;
		generate_bitstream_from_track(cyl , 0 , CELL_SIZE , bitstream , bitstream_size , image , 0);
		int pos = 0;
		unsigned track_no , sector_no;
		uint8_t sector_data[ SECTOR_SIZE ];
		while (get_next_sector(bitstream , bitstream_size , pos , track_no , sector_no , sector_data)) {
			if (track_no == cyl && sector_no >= 1 && sector_no <= SECTORS) {
				unsigned offset_in_image = (cyl * SECTORS + sector_no - 1) * SECTOR_SIZE;
				io_generic_write(io, sector_data, offset_in_image, SECTOR_SIZE);
			}
		}
	}
	return true;
}

const char *img_format::name() const
{
	return "img";
}

const char *img_format::description() const
{
	return "MDS-II floppy disk image";
}

const char *img_format::extensions() const
{
	return "img";
}

bool img_format::supports_save() const
{
	return true;
}

std::vector<uint8_t> img_format::interleaved_sectors(unsigned il_factor)
{
	std::vector<uint8_t> out(SECTORS);
	unsigned idx = 0;
	for (unsigned s = 0; s < SECTORS; s++) {
		while (out[ idx ] != 0) {
			if (++idx >= SECTORS) {
				idx = 0;
			}
		}
		out[ idx ] = s + 1;
		idx += il_factor;
		if (idx >= SECTORS) {
			idx -= SECTORS;
		}
	}
	return out;
}

void img_format::write_mmfm_bit(std::vector<uint32_t> &buffer , bool data_bit , bool clock_bit)
{
	bool had_transition = buffer.size() < 2 ? false : bit_r(buffer, buffer.size() - 1) || bit_r(buffer , buffer.size() - 2);
	clock_bit = !data_bit && (clock_bit || !had_transition);
	bit_w(buffer , clock_bit , CELL_SIZE);
	bit_w(buffer , data_bit , CELL_SIZE);

	if (util::BIT(m_crc , 15) ^ data_bit) {
		m_crc = (m_crc << 1) ^ CRC_POLY;
	} else {
		m_crc <<= 1;
	}
}

void img_format::write_mmfm_byte(std::vector<uint32_t> &buffer , uint8_t data , uint8_t clock)
{
	for (int i = 7; i >= 0; i--) {
		write_mmfm_bit(buffer , util::BIT(data , i) , util::BIT(clock , i));
	}
}

void img_format::write_sync(std::vector<uint32_t> &buffer)
{
	write_gap(buffer , SYNC_00_LEN , SYNC_FF_LEN);
}

void img_format::write_crc(std::vector<uint32_t> &buffer , uint16_t crc)
{
	// Note that CRC is stored with MSB (x^15) first
	for (unsigned i = 0; i < 16; i++) {
		write_mmfm_bit(buffer , util::BIT(crc , 15 - i) , 0);
	}
}

void img_format::write_gap(std::vector<uint32_t> &buffer , unsigned size_00 , unsigned size_ff)
{
	for (unsigned i = 0; i < size_00; ++i) {
		write_mmfm_byte(buffer, 0);
	}
	for (unsigned i = 0; i < size_ff; ++i) {
		write_mmfm_byte(buffer, 0xff);
	}
}

void img_format::write_sector(std::vector<uint32_t> &buffer , uint8_t track_no , uint8_t sect_no , const uint8_t *sect_data)
{
	// **** On-disk format of a sector ****
	//
	// | Offset | Size | Value | Content              |
	// |--------+------+-------+----------------------|
	// |      0 |   18 |    00 | Gap 1/3              |
	// |     18 |   10 |    ff | Gap 1/3              |
	// |     28 |    1 |    0e | ID AM (clock = 70)   |
	// |     29 |    1 |    xx | Track no.            |
	// |     30 |    1 |    00 | N/U                  |
	// |     31 |    1 |    xx | Sector no.           |
	// |     32 |    1 |    00 | N/U                  |
	// |     33 |    2 |    xx | ID CRC               |
	// |     35 |   18 |    00 | Gap 2                |
	// |     53 |   10 |    ff | Gap 2                |
	// |     63 |    1 |    0b | Data AM (clock = 70) |
	// |     64 |  128 |    xx | Sector data          |
	// |    192 |    2 |    xx | Data CRC             |
	// |    194 |      |       |                      |

	// Gap1
	write_sync(buffer);
	// ID AM
	m_crc = 0;
	write_mmfm_byte(buffer , ID_AM , AM_CLOCK);
	// Track #
	write_mmfm_byte(buffer , track_no);
	write_mmfm_byte(buffer , 0);
	// Sector #
	write_mmfm_byte(buffer , sect_no);
	write_mmfm_byte(buffer , 0);
	// ID CRC
	write_crc(buffer , m_crc);
	// Gap 2
	write_sync(buffer);
	// Data AM
	m_crc = 0;
	write_mmfm_byte(buffer , DATA_AM , AM_CLOCK);
	for (unsigned i = 0; i < SECTOR_SIZE; i++) {
		// Data
		write_mmfm_byte(buffer , sect_data[ i ]);
	}
	// Data CRC
	write_crc(buffer , m_crc);
}

void img_format::fill_with_gap4(std::vector<uint32_t> &buffer)
{
	// Cell count in a track (1 µs cells in a 1/6 s track)
	unsigned cell_count = (500000 * 120) / 360;
	unsigned cells_in_buffer = buffer.size();
	// Size of gap 4
	unsigned gap_4 = (cell_count - cells_in_buffer) / 16;
	// Gap 4
	write_gap(buffer , gap_4 , 0);
	// Last cell to round everything up to 2E+8
	if (buffer.size() * CELL_SIZE < 200000000) {
		bit_w(buffer , false , 200000000 - buffer.size() * CELL_SIZE);
	}
}

std::vector<uint8_t> img_format::get_next_id_n_block(const uint8_t *bitstream , int bitstream_size , int& pos , int& start_pos)
{
	std::vector<uint8_t> res;
	uint8_t data_sr;
	uint8_t clock_sr;

	// Look for either sync + ID AM or sync + DATA AM
	do {
		unsigned cnt_trans = 0;
		while (pos < bitstream_size && cnt_trans < 34) {
			bool bit = util::BIT(bitstream[ pos >> 3 ] , ~pos & 7);
			pos++;
			if (cnt_trans < 32) {
				if (!(util::BIT(cnt_trans , 0) ^ bit)) {
					cnt_trans++;
				} else {
					cnt_trans = 0;
				}
			} else if (cnt_trans == 32) {
				if (!bit) {
					start_pos = pos;
					cnt_trans++;
				} else {
					cnt_trans = 0;
				}
			} else if (!bit) {
				cnt_trans++;
			} else {
				cnt_trans = 32;
			}
		}

		if (pos == bitstream_size) {
			// End of track reached
			return res;
		}

		// Get AM
		data_sr = clock_sr = 0;
		for (unsigned i = 0; i < 7; ++i) {
			bool bit = util::BIT(bitstream[ pos >> 3 ] , ~pos & 7);
			pos++;
			clock_sr = (clock_sr << 1) | bit;
			bit = util::BIT(bitstream[ pos >> 3 ] , ~pos & 7);
			pos++;
			data_sr = (data_sr << 1) | bit;
		}
		if (pos >= bitstream_size) {
			return res;
		}
	} while (clock_sr != AM_CLOCK);

	// ID blocks: Track no. + 0 + sector no. + 0 + CRC
	// Data blocks: Sector data + CRC
	res.push_back(data_sr);
	unsigned to_dump;
	if (data_sr == ID_AM) {
		to_dump = 4;
	} else if (data_sr == DATA_AM) {
		to_dump = SECTOR_SIZE;
	} else {
		to_dump = 0;
	}

	pos++;
	for (unsigned i = 0; i < to_dump && pos < bitstream_size; i++) {
		data_sr = 0;
		unsigned j;
		for (j = 0; j < 8 && pos < bitstream_size; j++) {
			bool bit = util::BIT(bitstream[ pos >> 3 ] , ~pos & 7);
			pos += 2;
			data_sr = (data_sr << 1) | bit;
		}
		if (j == 8) {
			res.push_back(data_sr);
		}
	}
	return res;
}

bool img_format::get_next_sector(const uint8_t *bitstream , int bitstream_size , int& pos , unsigned& track , unsigned& sector , uint8_t *sector_data)
{
	std::vector<uint8_t> block;
	while (true) {
		// Scan for ID block first
		int id_pos = 0;
		while (true) {
			if (block.size() == 0) {
				block = get_next_id_n_block(bitstream , bitstream_size , pos , id_pos);
				if (block.size() == 0) {
					return false;
				}
			}
			if (block[ 0 ] == ID_AM && block.size() >= 5) {
				track = block[ 1 ];
				sector = block[ 3 ];
				break;
			} else {
				block.clear();
			}
		}
		// Then for DATA block
		int data_pos = 0;
		block = get_next_id_n_block(bitstream , bitstream_size , pos , data_pos);
		if (block.size() == 0) {
			return false;
		}
		if (block[ 0 ] == DATA_AM && block.size() >= (SECTOR_SIZE + 1) && abs((data_pos - id_pos) - ID_DATA_OFFSET) <= 64) {
			break;
		}
	}

	memcpy(sector_data , block.data() + 1 , SECTOR_SIZE);

	return true;
}

const floppy_format_type FLOPPY_IMG_FORMAT = &floppy_image_format_creator<img_format>;
