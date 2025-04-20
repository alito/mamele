// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#include "emu.h"
#include "arc.h"


DEFINE_DEVICE_TYPE(MSX_CART_ARC, msx_cart_arc_device, "msx_cart_arc", "MSX Cartridge - Arc")


msx_cart_arc_device::msx_cart_arc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, MSX_CART_ARC, tag, owner, clock)
	, msx_cart_interface(mconfig, *this)
	, m_7f(0)
{
}

void msx_cart_arc_device::device_start()
{
	save_item(NAME(m_7f));

	// Install IO read/write handlers
	io_space().install_write_handler(0x7f, 0x7f, write8smo_delegate(*this, FUNC(msx_cart_arc_device::io_7f_w)));
	io_space().install_read_handler(0x7f, 0x7f, read8smo_delegate(*this, FUNC(msx_cart_arc_device::io_7f_r)));
}

void msx_cart_arc_device::device_reset()
{
	m_7f = 0;
}

void msx_cart_arc_device::initialize_cartridge()
{
	if (get_rom_size() != 0x8000)
	{
		fatalerror("arc: Invalid ROM size\n");
	}

	page(1)->install_rom(0x4000, 0x7fff, get_rom_base());
	page(2)->install_rom(0x8000, 0xbfff, get_rom_base() + 0x4000);
}

void msx_cart_arc_device::io_7f_w(u8 data)
{
	if (data == 0x35)
		m_7f++;
}

u8 msx_cart_arc_device::io_7f_r()
{
	return ((m_7f & 0x03) == 0x03) ? 0xda : 0xff;
}
