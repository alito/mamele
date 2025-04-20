// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#include "emu.h"
#include "halnote.h"

namespace {

class msx_cart_halnote_device : public device_t, public msx_cart_interface
{
public:
	msx_cart_halnote_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: device_t(mconfig, MSX_CART_HALNOTE, tag, owner, clock)
		, msx_cart_interface(mconfig, *this)
		, m_rombank(*this, "rombank%u", 0U)
		, m_view0(*this, "view0")
		, m_view1(*this, "view1")
	{ }

	virtual std::error_condition initialize_cartridge(std::string &message) override;

protected:
	// device_t implementation
	virtual void device_start() override { }
	virtual void device_reset() override;

private:
	static constexpr u8 BANK_MASK = (0x100000 / 0x2000) - 1;

	void bank0_w(u8 data);
	void bank1_w(u8 data);
	void bank2_w(u8 data);
	void bank3_w(u8 data);
	void bank4_w(u8 data);
	void bank5_w(u8 data);

	memory_bank_array_creator<6> m_rombank;
	memory_view m_view0;
	memory_view m_view1;
};

void msx_cart_halnote_device::device_reset()
{
	for (int i = 0; i < 6; i++)
		m_rombank[i]->set_entry(0);

	m_view0.disable();
	m_view1.select(0);
}

std::error_condition msx_cart_halnote_device::initialize_cartridge(std::string &message)
{
	if (!cart_rom_region())
	{
		message = "msx_cart_halnote_device: Required region 'rom' was not found.";
		return image_error::INTERNAL;
	}

	if (!cart_sram_region())
	{
		message = "msx_cart_halnote_device: Required region 'sram' was not found.";
		return image_error::INTERNAL;
	}

	if (cart_rom_region()->bytes() != 0x100000)
	{
		message = "msx_cart_halnote_device: Region 'rom' has unsupported size.";
		return image_error::INVALIDLENGTH;
	}

	if (cart_sram_region()->bytes() < 0x4000)
	{
		message = "msx_cart_halnote_device: Region 'sram' has unsupported size.";
		return image_error::BADSOFTWARE;
	}

	for (int i = 0; i < 4; i++)
	{
		m_rombank[i]->configure_entries(0, 0x80, cart_rom_region()->base(), 0x2000);
	}
	m_rombank[4]->configure_entries(0, 0x100, cart_rom_region()->base() + 0x80000, 0x800);
	m_rombank[5]->configure_entries(0, 0x100, cart_rom_region()->base() + 0x80000, 0x800);

	page(0)->install_view(0x0000, 0x3fff, m_view0);
	m_view0[0];
	m_view0[1].install_ram(0x0000, 0x3fff, cart_sram_region()->base());
	page(1)->install_read_bank(0x4000, 0x5fff, m_rombank[0]);
	page(1)->install_write_handler(0x4fff, 0x4fff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank0_w)));
	page(1)->install_view(0x6000, 0x7fff, m_view1);
	m_view1[0].install_read_bank(0x6000, 0x7fff, m_rombank[1]);
	m_view1[0].install_write_handler(0x6fff, 0x6fff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank1_w)));
	m_view1[0].install_write_handler(0x77ff, 0x77ff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank4_w)));
	m_view1[0].install_write_handler(0x7fff, 0x7fff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank5_w)));
	m_view1[1].install_read_bank(0x6000, 0x6fff, m_rombank[1]);
	m_view1[1].install_write_handler(0x6fff, 0x6fff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank1_w)));
	m_view1[1].install_read_bank(0x7000, 0x77ff, m_rombank[4]);
	m_view1[1].install_write_handler(0x77ff, 0x77ff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank4_w)));
	m_view1[1].install_read_bank(0x7800, 0x7fff, m_rombank[5]);
	m_view1[1].install_write_handler(0x7fff, 0x7fff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank5_w)));
	page(2)->install_read_bank(0x8000, 0x9fff, m_rombank[2]);
	page(2)->install_write_handler(0x8fff, 0x8fff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank2_w)));
	page(2)->install_read_bank(0xa000, 0xbfff, m_rombank[3]);
	page(2)->install_write_handler(0xafff, 0xafff, emu::rw_delegate(*this, FUNC(msx_cart_halnote_device::bank3_w)));

	return std::error_condition();
}

void msx_cart_halnote_device::bank0_w(u8 data)
{
	m_rombank[0]->set_entry(data & 0x7f);
	m_view0.select(BIT(data, 7) ? 1 : 0);
}

void msx_cart_halnote_device::bank1_w(u8 data)
{
	m_rombank[1]->set_entry(data & 0x7f);
}

void msx_cart_halnote_device::bank2_w(u8 data)
{
	m_rombank[2]->set_entry(data & 0x7f);
}

void msx_cart_halnote_device::bank3_w(u8 data)
{
	m_rombank[3]->set_entry(data & 0x7f);
	m_view1.select(BIT(data, 7) ? 1 : 0);
}

void msx_cart_halnote_device::bank4_w(u8 data)
{
	m_rombank[4]->set_entry(data);
}

void msx_cart_halnote_device::bank5_w(u8 data)
{
	m_rombank[5]->set_entry(data);
}

} // anonymous namespace

DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_HALNOTE, msx_cart_interface, msx_cart_halnote_device, "msx_cart_halnote", "MSX Cartridge - Halnote")
