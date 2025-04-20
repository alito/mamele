// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#include "emu.h"
#include "slotexpander.h"
#include "bus/msx/cart/cartridge.h"
#include "machine/input_merger.h"

/*
Emulation of a 4 slot expander for the MSX system.

Some slot expanders have the ability to enable/disable subslots through
jumpers/switches, or adjustable volume per slot. None of that is emulated.

Known slot expanders:
- 8bits4ever Slot x4 - audio mixer with adjustable volume per slot not emulated.
- CIEL Mini Slot Expander - Jumpers and switches not emulated.
- Digital Design Expansor de slots DDX
- ECC Expansion Computer Case - 8 slot version not emulated
- Front Line Slot Expander
- G.DOS Slotexpander
- Hans Oranje slotexpander - Switches not emulated
- Incompel Expansor de slots
- MAD Expander Slot Box MXE-MAIN-A / MXE-MAIN4-A - Switches not emulated
- MK Slotexpander - Switches not emulated
- MSX Club Gouda Slotexpander - Switches not emulated
- Mitsubishi ML-20EB
- Neos EX-4
- Padial LPE-4EXP-V3SC
- Padial LPE-4EXP-V4SC
- Repro Factory My Super Expander 4X
- Sinfox Slotexpander
- Sony HBI-50 - Only expands to 2 slots
- Sunrise 8x SlotExpander - only 4 slots emulated
- Supersoniqs Modulon
- Toshiba HX-E601
- Victor HC-A703E - Only expands to 2 slots?
- Zemina Expansion slot - Only expands to 2 slots

*/

namespace {

class msx_cart_slotexpander_device : public device_t, public msx_cart_interface
{
public:
	msx_cart_slotexpander_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock)
		: device_t(mconfig, MSX_CART_SLOTEXPANDER, tag, owner, clock)
		, msx_cart_interface(mconfig, *this)
		, m_cartslot(*this, "cartslot%u", 1)
		, m_irq_out(*this, "irq_out")
		, m_view0(*this, "view0")
		, m_view1(*this, "view1")
		, m_view2(*this, "view2")
		, m_view3(*this, "view3")
		, m_secondary_slot(0)
	{ }

protected:
	virtual void device_start() override;
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_resolve_objects() override;
	virtual void device_config_complete() override;

private:
	template<int Slot> void add_cartslot(machine_config &mconfig);
	u8 secondary_slot_r();
	void secondary_slot_w(u8 data);

	required_device_array<msx_slot_cartridge_device, 4> m_cartslot;
	required_device<input_merger_any_high_device> m_irq_out;
	memory_view m_view0;
	memory_view m_view1;
	memory_view m_view2;
	memory_view m_view3;
	u8 m_secondary_slot;
};

template<int Slot>
void msx_cart_slotexpander_device::add_cartslot(machine_config &mconfig)
{
	MSX_SLOT_CARTRIDGE(mconfig, m_cartslot[Slot], DERIVED_CLOCK(1, 1));
	m_cartslot[Slot]->option_reset();
	msx_cart(*m_cartslot[Slot], true);
	m_cartslot[Slot]->set_default_option(nullptr);
	m_cartslot[Slot]->set_fixed(false);
	m_cartslot[Slot]->irq_handler().set(m_irq_out, FUNC(input_merger_device::in_w<Slot>));
	if (parent_slot())
	{
		m_cartslot[Slot]->add_route(ALL_OUTPUTS, soundin(), 1.0);
	}
}

void msx_cart_slotexpander_device::device_add_mconfig(machine_config &mconfig)
{
	add_cartslot<0>(mconfig);
	add_cartslot<1>(mconfig);
	add_cartslot<2>(mconfig);
	add_cartslot<3>(mconfig);

	INPUT_MERGER_ANY_HIGH(mconfig, m_irq_out).output_handler().set(*this, FUNC(msx_cart_slotexpander_device::irq_out));
}

void msx_cart_slotexpander_device::device_config_complete()
{
	if (parent_slot())
	{
		for (auto &subslot : m_cartslot)
		{
			auto target = subslot.finder_target();
			parent_slot()->configure_subslot(*target.first.subdevice<msx_slot_cartridge_device>(target.second));
		}
	}
}

void msx_cart_slotexpander_device::device_resolve_objects()
{
	page(0)->install_view(0x0000, 0x3fff, m_view0);
	page(1)->install_view(0x4000, 0x7fff, m_view1);
	page(2)->install_view(0x8000, 0xbfff, m_view2);
	page(3)->install_view(0xc000, 0xffff, m_view3);
	page(3)->install_readwrite_handler(0xffff, 0xffff, emu::rw_delegate(*this, FUNC(msx_cart_slotexpander_device::secondary_slot_r)), emu::rw_delegate(*this, FUNC(msx_cart_slotexpander_device::secondary_slot_w)));

	for (int page = 0; page < 4; page++)
	{
		m_view0[page];
		m_view1[page];
		m_view2[page];
		m_view3[page];
	}

	for (int i = 0; i < 4; i++)
		m_cartslot[i]->install(&m_view0[i], &m_view1[i], &m_view2[i], &m_view3[i]);
}

void msx_cart_slotexpander_device::device_start()
{
	save_item(NAME(m_secondary_slot));
	m_secondary_slot = 0;

	m_view0.select(0);
	m_view1.select(0);
	m_view2.select(0);
	m_view3.select(0);
}

u8 msx_cart_slotexpander_device::secondary_slot_r()
{
	return ~m_secondary_slot;
}

void msx_cart_slotexpander_device::secondary_slot_w(u8 data)
{
	m_view0.select((data >> 0) & 0x03);
	m_view1.select((data >> 2) & 0x03);
	m_view2.select((data >> 4) & 0x03);
	m_view3.select((data >> 6) & 0x03);
	m_secondary_slot = data;
}

} // anonymous namespace


DEFINE_DEVICE_TYPE_PRIVATE(MSX_CART_SLOTEXPANDER, msx_cart_interface, msx_cart_slotexpander_device, "msx_cart_slotexpander", "MSX Slot Expander")
