// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
/**********************************************************************************

TODO:
- Verify real contents of the kanji roms. The current dumps seem to have
  been read from inside a running MSX machine. The content retrieved that
  way may not be how they are stored in the rom.

**********************************************************************************/

#include "emu.h"
#include "kanji.h"

#include "speaker.h"


#define VERBOSE 0
#include "logmacro.h"


DEFINE_DEVICE_TYPE(MSX_CART_KANJI, msx_cart_kanji_device, "msx_cart_kanji", "MSX Cartridge - Kanji")
DEFINE_DEVICE_TYPE(MSX_CART_MSXWRITE, msx_cart_msxwrite_device, "msx_cart_msxwrite", "MSX Cartridge - MSXWRITE")


msx_cart_kanji_device::msx_cart_kanji_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: msx_cart_kanji_device(mconfig, MSX_CART_KANJI, tag, owner, clock)
{
}

msx_cart_kanji_device::msx_cart_kanji_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock)
	: device_t(mconfig, type, tag, owner, clock)
	, msx_cart_interface(mconfig, *this)
	, m_kanji_mask(0)
	, m_kanji_address(0)
{
}

void msx_cart_kanji_device::device_reset()
{
	m_kanji_address = 0;
}

void msx_cart_kanji_device::device_start()
{
	save_item(NAME(m_kanji_address));
}

void msx_cart_kanji_device::initialize_cartridge()
{
	u32 size = get_kanji_size();

	if (size != 0x20000)
	{
		fatalerror("kanji: Invalid ROM size\n");
	}

	m_kanji_mask = size - 1;

	// Install IO read/write handlers
	io_space().install_write_handler(0xd8, 0xd9, write8sm_delegate(*this, FUNC(msx_cart_kanji_device::kanji_w)));
	io_space().install_read_handler(0xd9, 0xd9, read8sm_delegate(*this, FUNC(msx_cart_kanji_device::kanji_r)));
}

u8 msx_cart_kanji_device::kanji_r(offs_t offset)
{
	u8 result = get_kanji_base()[m_kanji_address];

	if (!machine().side_effects_disabled())
	{
		m_kanji_address = (m_kanji_address & ~0x1f) | ((m_kanji_address + 1) & 0x1f);
	}
	return result;
}

void msx_cart_kanji_device::kanji_w(offs_t offset, u8 data)
{
	if (offset)
		m_kanji_address = (m_kanji_address & 0x007e0) | ((data & 0x3f) << 11);
	else
		m_kanji_address = (m_kanji_address & 0x1f800) | ((data & 0x3f) << 5);

	m_kanji_address = m_kanji_address & m_kanji_mask;
}



static INPUT_PORTS_START(msxwrite_kanji_enable_switch)
	PORT_START("KANJI")
	PORT_CONFNAME(0x01, 0x01, "Kanji is")
	PORT_CONFSETTING(0x00, "disabled")
	PORT_CONFSETTING(0x01, "enabled")
INPUT_PORTS_END


msx_cart_msxwrite_device::msx_cart_msxwrite_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
	: msx_cart_kanji_device(mconfig, MSX_CART_MSXWRITE, tag, owner, clock)
	, m_rombank(*this, "rombank%u", 0U)
	, m_kanji_switch(*this, "KANJI")
	, m_bank_mask(0)
{
}

ioport_constructor msx_cart_msxwrite_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(msxwrite_kanji_enable_switch);
}

void msx_cart_msxwrite_device::device_reset()
{
	msx_cart_kanji_device::device_reset();

	m_rombank[0]->set_entry(0);
	m_rombank[1]->set_entry(0);

	if (BIT(m_kanji_switch->read(), 0))
		msx_cart_kanji_device::initialize_cartridge();
}

void msx_cart_msxwrite_device::initialize_cartridge()
{
	u32 size = get_rom_size();
	u16 banks = size / BANK_SIZE;

	if (size > 256 * BANK_SIZE || size != banks * BANK_SIZE || (~(banks - 1) % banks))
	{
		fatalerror("msxwrite: Invalid ROM size\n");
	}

	m_bank_mask = banks - 1;

	for (int i = 0; i < 2; i++)
		m_rombank[i]->configure_entries(0, banks, get_rom_base(), BANK_SIZE);

	page(1)->install_read_bank(0x4000, 0x7fff, m_rombank[0]);
	// The rom writes to 6fff and 7fff for banking, unknown whether
	// other locations also trigger banking.
	page(1)->install_write_handler(0x6fff, 0x6fff, write8smo_delegate(*this, FUNC(msx_cart_msxwrite_device::bank_w<0>)));
	page(1)->install_write_handler(0x7fff, 0x7fff, write8smo_delegate(*this, FUNC(msx_cart_msxwrite_device::bank_w<1>)));
	page(2)->install_read_bank(0x8000, 0xbfff, m_rombank[1]);
}

template <int Bank>
void msx_cart_msxwrite_device::bank_w(u8 data)
{
	m_rombank[Bank]->set_entry(data & m_bank_mask);
}
