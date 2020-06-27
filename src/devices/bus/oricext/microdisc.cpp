// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#include "emu.h"
#include "microdisc.h"
#include "formats/oric_dsk.h"

DEFINE_DEVICE_TYPE(ORIC_MICRODISC, oric_microdisc_device, "oric_microdisc", "Microdisc floppy drive interface")

ROM_START( microdisc )
	ROM_REGION( 0x2000, "microdisc", 0 )
	ROM_LOAD ("microdis.rom", 0, 0x02000, CRC(a9664a9c) SHA1(0d2ef6e67322f48f4b7e08d8bbe68827e2074561) )
ROM_END

FLOPPY_FORMATS_MEMBER( oric_microdisc_device::floppy_formats )
	FLOPPY_ORIC_DSK_FORMAT
FLOPPY_FORMATS_END

static void microdisc_floppies(device_slot_interface &device)
{
	device.option_add("3dsdd", FLOPPY_3_DSDD);
}

void oric_microdisc_device::map(address_map &map)
{
	map(0x310, 0x313).rw("fdc", FUNC(fd1793_device::read), FUNC(fd1793_device::write));
	map(0x314, 0x314).rw(FUNC(oric_microdisc_device::port_314_r), FUNC(oric_microdisc_device::port_314_w));
	map(0x318, 0x318).r(FUNC(oric_microdisc_device::port_318_r));
}

oric_microdisc_device::oric_microdisc_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, ORIC_MICRODISC, tag, owner, clock),
	device_oricext_interface(mconfig, *this),
	fdc(*this, "fdc"), microdisc_rom(nullptr), port_314(0), intrq_state(false), drq_state(false), hld_state(false)
{
}

oric_microdisc_device::~oric_microdisc_device()
{
}

void oric_microdisc_device::device_start()
{
	microdisc_rom = device().machine().root_device().memregion(this->subtag("microdisc").c_str())->base();
	cpu->space(AS_PROGRAM).install_device(0x0000, 0xffff, *this, &oric_microdisc_device::map);

	for(int i=0; i<4; i++) {
		char name[32];
		sprintf(name, "fdc:%d", i);
		floppies[i] = subdevice<floppy_connector>(name)->get_device();
	}
	intrq_state = drq_state = hld_state = false;
}

void oric_microdisc_device::device_reset()
{
	port_314 = 0x00;
	irq_w(false);
	remap();
	fdc->set_floppy(floppies[0]);

	// The bootstrap checksums part of the high ram and if the sum is
	// 0 it goes wrong.
	ram[0xe000] = 0x42;
}

const tiny_rom_entry *oric_microdisc_device::device_rom_region() const
{
	return ROM_NAME( microdisc );
}

void oric_microdisc_device::device_add_mconfig(machine_config &config)
{
	FD1793(config, fdc, 8_MHz_XTAL / 8);
	fdc->intrq_wr_callback().set(FUNC(oric_microdisc_device::fdc_irq_w));
	fdc->drq_wr_callback().set(FUNC(oric_microdisc_device::fdc_drq_w));
	fdc->hld_wr_callback().set(FUNC(oric_microdisc_device::fdc_hld_w));
	fdc->set_force_ready(true);

	FLOPPY_CONNECTOR(config, "fdc:0", microdisc_floppies, "3dsdd", oric_microdisc_device::floppy_formats);
	FLOPPY_CONNECTOR(config, "fdc:1", microdisc_floppies, nullptr, oric_microdisc_device::floppy_formats);
	FLOPPY_CONNECTOR(config, "fdc:2", microdisc_floppies, nullptr, oric_microdisc_device::floppy_formats);
	FLOPPY_CONNECTOR(config, "fdc:3", microdisc_floppies, nullptr, oric_microdisc_device::floppy_formats);
}

void oric_microdisc_device::remap()
{
	if(port_314 & P_ROMDIS) {
		bank_c000_r->set_base(rom+0x0000);
		bank_e000_r->set_base(rom+0x2000);
		bank_f800_r->set_base(rom+0x3800);
		bank_c000_w->set_base(junk_write);
		bank_e000_w->set_base(junk_write);
		bank_f800_w->set_base(junk_write);
	} else {
		bank_c000_r->set_base(ram+0xc000);
		bank_c000_w->set_base(ram+0xc000);
		if(port_314 & P_EPROM) {
			bank_e000_r->set_base(ram+0xe000);
			bank_f800_r->set_base(ram+0xf800);
			bank_e000_w->set_base(ram+0xe000);
			bank_f800_w->set_base(ram+0xf800);
		} else {
			bank_e000_r->set_base(microdisc_rom+0x0000);
			bank_f800_r->set_base(microdisc_rom+0x1800);
			bank_e000_w->set_base(junk_write);
			bank_f800_w->set_base(junk_write);
		}
	}
}

void oric_microdisc_device::port_314_w(uint8_t data)
{
	port_314 = data;
	remap();
	floppy_image_device *floppy = floppies[(port_314 >> 5) & 3];
	fdc->set_floppy(floppy);
	fdc->dden_w(port_314 & P_DDEN);
	if(floppy) {
		floppy->ss_w(port_314 & P_SS ? 1 : 0);
		floppy->mon_w(0);
	}
	irq_w(intrq_state && (port_314 & P_IRQEN));
}

uint8_t oric_microdisc_device::port_314_r()
{
	return (intrq_state && (port_314 & P_IRQEN)) ? 0x7f : 0xff;
}

uint8_t oric_microdisc_device::port_318_r()
{
	return drq_state ? 0x7f : 0xff;
}

WRITE_LINE_MEMBER(oric_microdisc_device::fdc_irq_w)
{
	intrq_state = state;
	irq_w(intrq_state && (port_314 & P_IRQEN));
}

WRITE_LINE_MEMBER(oric_microdisc_device::fdc_drq_w)
{
	drq_state = state;
}

WRITE_LINE_MEMBER(oric_microdisc_device::fdc_hld_w)
{
	logerror("hld %d\n", state);
	hld_state = state;
	floppies[(port_314 >> 5) & 3]->mon_w(!hld_state);
}
