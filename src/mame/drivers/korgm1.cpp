// license:BSD-3-Clause
// copyright-holders:Angelo Salese, AJR
/***************************************************************************

    Korg M1 (c) 1988

    skeleton driver

***************************************************************************/

#include "emu.h"
#include "cpu/nec/v5x.h"
#include "machine/adc0808.h"
#include "machine/cxd1095.h"
#include "machine/nvram.h"
#include "video/hd44780.h"
#include "emupal.h"
#include "screen.h"
//#include "speaker.h"

class korgm1_state : public driver_device
{
public:
	korgm1_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_adc(*this, "adc")
		, m_lcdc(*this, "lcdc")
		, m_panel_sw(*this, "Y%u", 0U)
	{ }

	void korgm1(machine_config &config);

protected:
	// driver_device overrides
	virtual void machine_start() override;
	virtual void machine_reset() override;

	virtual void video_start() override;

private:
	HD44780_PIXEL_UPDATE(lcd_pixel_update);

	void pio_pa_w(u8 data);
	u8 panel_sw_r();
	void panel_leds_w(u8 data);

	void korgm1_map(address_map &map);
	void korgm1_io(address_map &map);

	void palette_init_korgm1(palette_device &palette);

	// devices
	required_device<v50_device> m_maincpu;
	required_device<adc0808_device> m_adc;
	required_device<hd44780_device> m_lcdc;
	optional_ioport_array<8> m_panel_sw;

	u8 m_selected_sw = 0;
};

void korgm1_state::video_start()
{
}

HD44780_PIXEL_UPDATE(korgm1_state::lcd_pixel_update)
{
	if (x < 5 && y < 8 && line < 2 && pos < 40)
		bitmap.pix16(line * 8 + y, pos * 6 + x) = state;
}

void korgm1_state::pio_pa_w(u8 data)
{
	m_selected_sw = data & 0x07;

	m_adc->address_w((data & 0x38) >> 3);
	m_adc->start_w(BIT(data, 6));

	// PA7 controls /RESET on TG I/TG II/VDF I/VDF II/MDE
}

u8 korgm1_state::panel_sw_r()
{
	return 0xc0 | m_panel_sw[m_selected_sw].read_safe(0x3f);
}

void korgm1_state::panel_leds_w(u8 data)
{
}

void korgm1_state::korgm1_map(address_map &map)
{
	map(0x00000, 0x0ffff).ram().share("nvram"); // 64 KB
//  map(0x50000, 0x57fff).ram(); // memory card 32 KB
	map(0xe0000, 0xfffff).rom().region("ipl", 0);
}

void korgm1_state::korgm1_io(address_map &map)
{
//  map(0x0000, 0x00ff); internal peripheral (?)
//  map(0x0100, 0x01ff); VDF 1 (MB87404)
//  map(0x0200, 0x02ff); VDF 2 (MB87404)
//  map(0x0500, 0x0503); MDE (MB87405)
//  map(0x0600, 0x0601); OPZ 1 (8-bit port)
//  map(0x0700, 0x0701); OPZ 2 (8-bit port)
//  map(0x0800, 0x0801); SCAN (8-bit port) (keyboard)
	map(0x0900, 0x0900).r(m_adc, FUNC(adc0808_device::data_r));
	map(0x0a00, 0x0a0f).rw("pio", FUNC(cxd1095_device::read), FUNC(cxd1095_device::write)).umask16(0x00ff);
//  map(0x0b00, 0x0b01); LCDC (8-bit port)
//  map(0x1000, 0x11ff); TG (MB87402)
//  map(0x2000, 0x23ff); SCSI
//  map(0x3000, 0x33ff); FDC
//  TG 2?
}

static INPUT_PORTS_START( korgm1 )
	PORT_START("Y0")
	PORT_BIT(0x03, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW1")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW5")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW2")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW6")

	PORT_START("Y1")
	PORT_BIT(0x03, IP_ACTIVE_LOW, IPT_UNUSED)
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW3")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW7")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW4")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW8")

	PORT_START("Y2")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW9")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW10")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW11")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW12")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW13")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW14")

	PORT_START("Y3")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW15")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW16")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW17")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW18")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW19")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW20")

	PORT_START("Y4")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW31")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW27")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW28")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW29")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW24")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("Y5")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW25")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW26")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW21")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW22")
	PORT_BIT(0x10, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW23")
	PORT_BIT(0x20, IP_ACTIVE_LOW, IPT_UNUSED)

	PORT_START("Y6")
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW30")
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW32")
	PORT_BIT(0x04, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW33")
	PORT_BIT(0x08, IP_ACTIVE_LOW, IPT_OTHER) PORT_NAME("SW34")
	PORT_BIT(0x30, IP_ACTIVE_LOW, IPT_UNUSED)
INPUT_PORTS_END


void korgm1_state::machine_start()
{
	save_item(NAME(m_selected_sw));
}

void korgm1_state::machine_reset()
{
}


void korgm1_state::palette_init_korgm1(palette_device &palette)
{
	palette.set_pen_color(0, rgb_t( 69,  63,  66));
	palette.set_pen_color(1, rgb_t(131, 136, 139));
}

void korgm1_state::korgm1(machine_config &config)
{
	/* basic machine hardware */
	V50(config, m_maincpu, 32_MHz_XTAL / 2); // µPD70216
	m_maincpu->set_addrmap(AS_PROGRAM, &korgm1_state::korgm1_map);
	m_maincpu->set_addrmap(AS_IO, &korgm1_state::korgm1_io);
	m_maincpu->out_handler<2>().set_inputline(m_maincpu, INPUT_LINE_IRQ2);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0); // 2x µPD43256C-15L + battery

	cxd1095_device &pio(CXD1095(config, "pio")); // CXD1095Q
	pio.out_porta_cb().set(FUNC(korgm1_state::pio_pa_w));
	pio.in_portb_cb().set(FUNC(korgm1_state::panel_sw_r));
	pio.out_portc_cb().set(FUNC(korgm1_state::panel_leds_w));
	pio.in_portd_cb().set(m_lcdc, FUNC(hd44780_device::db_r));
	pio.out_portd_cb().set(m_lcdc, FUNC(hd44780_device::db_w));
	pio.out_porte_cb().set(m_lcdc, FUNC(hd44780_device::rs_w)).bit(0);
	pio.out_porte_cb().append(m_lcdc, FUNC(hd44780_device::rw_w)).bit(1);
	pio.out_porte_cb().append(m_lcdc, FUNC(hd44780_device::e_w)).bit(2);

	M58990(config, m_adc, 32_MHz_XTAL / 32); // M58990P-1 (µPD65013G-402 divides V50 CLKOUT by 8)
	// Joystick, "value" and After Touch routes here

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_LCD));
	screen.set_color(rgb_t::green());
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(2500)); /* not accurate */
	screen.set_screen_update("lcdc", FUNC(hd44780_device::screen_update));
	screen.set_size(6*40, 8*2);
	screen.set_visarea_full();
	screen.set_palette("palette");

	PALETTE(config, "palette", FUNC(korgm1_state::palette_init_korgm1), 2);

	HD44780(config, m_lcdc, 0);
	m_lcdc->set_lcd_size(2, 40);
	m_lcdc->set_pixel_update_cb(FUNC(korgm1_state::lcd_pixel_update));

	/* sound hardware */
	//SPEAKER(config, "mono").front_center();
}


/***************************************************************************

  Game driver(s)

***************************************************************************/

ROM_START( korgm1 )
	ROM_REGION16_LE( 0x20000, "ipl", 0 )
	ROM_LOAD16_BYTE( "rev19-ic23-880219.bin", 0x00001, 0x10000, CRC(397bce2d) SHA1(8df016ae172904eb94bc782a054bd783878c6f75) )
	ROM_LOAD16_BYTE( "rev19-ic32-880319.bin", 0x00000, 0x10000, CRC(4e33fc09) SHA1(2b96e054a08bf1091310d9367e35fbcf52003a80) )

	ROM_REGION( 0x8000, "tgrom", ROMREGION_ERASE00 )
	ROM_LOAD( "mb83512-15p-259.ic30", 0x0000, 0x8000, NO_DUMP )
	ROM_IGNORE( 0x8000 )

	ROM_REGION( 0x4000, "vdf1", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic26", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_IGNORE( 0x4000 )

	ROM_REGION( 0x4000, "vdf2", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic20", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
ROM_END

ROM_START( korgm1ex )
	ROM_REGION16_LE( 0x20000, "ipl", 0 )
	ROM_LOAD16_BYTE( "v. 1.29 27c512 hi ic23.bin", 0x00001, 0x10000, CRC(ff96bae7) SHA1(8e84755e0b48e47b9fa46958ff33b4111602950b) )
	ROM_LOAD16_BYTE( "v. 1.29 27c512 lo ic32.bin", 0x00000, 0x10000, CRC(730cedf1) SHA1(ae1e5aee1e296714dc518dc0184c7b9b398df11d) )

	ROM_REGION( 0x8000, "tgrom", ROMREGION_ERASE00 )
	ROM_LOAD( "mb83512-15p-259.ic30", 0x0000, 0x8000, NO_DUMP )
	ROM_IGNORE( 0x8000 )

	ROM_REGION( 0x4000, "vdf1", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic26", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_IGNORE( 0x4000 )

	ROM_REGION( 0x4000, "vdf2", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic20", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
ROM_END

ROM_START( korgm1r )
	ROM_REGION16_LE( 0x20000, "ipl", 0 )
	ROM_LOAD16_BYTE( "880506_ic23 high _27c512_ m1r.bin", 0x00001, 0x10000, CRC(c8b09803) SHA1(30579364d224e8422941acd15b8c7cfc5ffc0e8d) )
	ROM_LOAD16_BYTE( "880606_ic32 low _27c512_ m1r.bin",  0x00000, 0x10000, CRC(ad3d3c21) SHA1(6f0e4778edc48017dcd813c2688967e362c14328) )

	ROM_REGION( 0x8000, "tgrom", ROMREGION_ERASE00 )
	ROM_LOAD( "mb83512-15p-259.ic30", 0x0000, 0x8000, NO_DUMP )
	ROM_IGNORE( 0x8000 )

	ROM_REGION( 0x4000, "vdf1", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic26", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_IGNORE( 0x4000 )

	ROM_REGION( 0x4000, "vdf2", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic20", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
ROM_END

ROM_START( korgm1rex )
	ROM_REGION16_LE( 0x20000, "ipl", 0 )
	ROM_LOAD16_BYTE( "880512 _ic23 high _27c512_ m1rex.bin", 0x00001, 0x10000, CRC(67059dc6) SHA1(91c387689323f6bf9a03ba18df76f29106f87e4a) )
	ROM_LOAD16_BYTE( "880612 _ic32 low _27c512_ m1rex.bin",  0x00000, 0x10000, CRC(056112b2) SHA1(4d7b552034b1a6f09d7ff30b74f4351ddecc2e24) )

	ROM_REGION( 0x8000, "tgrom", ROMREGION_ERASE00 )
	ROM_LOAD( "mb83512-15p-259.ic30", 0x0000, 0x8000, NO_DUMP )
	ROM_IGNORE( 0x8000 )

	ROM_REGION( 0x4000, "vdf1", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic26", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_IGNORE( 0x4000 )

	ROM_REGION( 0x4000, "vdf2", ROMREGION_ERASE00 )
	ROM_LOAD( "upd23c512-039.ic20", 0x0000, 0x4000, NO_DUMP )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
ROM_END

ROM_START( korgm1p1 )
	ROM_REGION16_LE( 0x20000, "ipl", 0 )
	ROM_LOAD16_BYTE( "plus-1_lsb.bin", 0x00000, 0x10000, CRC(13a2add2) SHA1(7993ace91a10f0f523113538bb7be6fa29ad52f8) )
	ROM_LOAD16_BYTE( "plus-1_msb.bin", 0x00001, 0x10000, CRC(ce6d8473) SHA1(eb45dde07d78ed086da01bb0431c1d8d2dc9a15e) )

	ROM_REGION( 0x8000, "tgrom", 0 )
	ROM_LOAD( "ic30.bin", 0x0000, 0x8000, CRC(8b23ba0f) SHA1(4759e8918961f080f2607d8213a789c7a81228d0) )
	ROM_IGNORE( 0x8000 )

	ROM_REGION( 0x4000, "vdf1", 0 )
	ROM_LOAD( "ic26.bin", 0x0000, 0x4000, CRC(8b23ba0f) SHA1(4759e8918961f080f2607d8213a789c7a81228d0) )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_IGNORE( 0x4000 )

	ROM_REGION( 0x4000, "vdf2", 0 )
	ROM_LOAD( "ic20.bin", 0x0000, 0x4000, CRC(8b23ba0f) SHA1(4759e8918961f080f2607d8213a789c7a81228d0) )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )
	ROM_CONTINUE( 0x0000, 0x4000 )

	ROM_REGION16_LE( 0x400000, "pcm", 0 )
	ROM_LOAD16_BYTE( "ic36.bin", 0x000000, 0x080000, CRC(5a3adc4c) SHA1(964fd245e46b4adf63b9b03b0490117b6df5e272) )
	ROM_LOAD16_BYTE( "ic40.bin", 0x000001, 0x080000, CRC(77ac122b) SHA1(aa5a99ac697d673cf758eae323cbaeca0bb6f243) )
	ROM_LOAD16_BYTE( "ic37.bin", 0x100000, 0x080000, CRC(fe15f7ca) SHA1(aa339c0972662d38773cdb533ceb0feb30b62eda) )
	ROM_LOAD16_BYTE( "ic41.bin", 0x100001, 0x080000, CRC(e0dce6ad) SHA1(6952b5b8c2a72c3a515444f1d1841dc27fadd62a) )
	ROM_LOAD16_BYTE( "ic38.bin", 0x200000, 0x080000, CRC(14554a91) SHA1(d2ecd6e7a8fdf6b52fc083628b92233a13db8198) )
	ROM_LOAD16_BYTE( "ic42.bin", 0x200001, 0x080000, CRC(bf404899) SHA1(9bda08cf5bddb733f203ed2f6622f18d9635d575) )
	ROM_LOAD16_BYTE( "ic39.bin", 0x300000, 0x080000, CRC(ef8324c4) SHA1(6cbbd8421f777f3d39644474daac2b15f9636810) )
	ROM_LOAD16_BYTE( "ic43.bin", 0x300001, 0x080000, CRC(77c39278) SHA1(bf87f16fa6af1a3076c19c12901b02cbefac3fce) )

	ROM_REGION( 0x8000, "progcard", 0 )
	ROM_LOAD( "plus-1_prog_card.bin", 0x0000, 0x8000, CRC(c31b3bce) SHA1(ef341868800bc98c9677f6f9227e8a2342504391) )

	ROM_REGION( 0x400000, "multisound", 0 )
	ROM_LOAD( "u201.bin", 0x000000, 0x100000, CRC(19fc1c37) SHA1(2badf71d0f104341436f4a35b1255b2c4d3e67bf) )
	ROM_LOAD( "u202.bin", 0x100000, 0x100000, CRC(bb272f7e) SHA1(4c73c97dc313313ec2142e5b73defd38d1fcbddf) )
	ROM_LOAD( "u203.bin", 0x200000, 0x100000, CRC(65eb825d) SHA1(8289b06759cf6e4eafe04c940f11312466352e14) )
	ROM_LOAD( "u204.bin", 0x300000, 0x100000, CRC(ff8d6307) SHA1(3021fe2ce88884e57b397bc5dfb4b29afc56eec4) )
ROM_END

SYST(1988, korgm1,    0,      0, korgm1, korgm1, korgm1_state, empty_init, "Korg",                 "M1 Music Workstation (Rev 19)",    MACHINE_IS_SKELETON)
SYST(1988, korgm1ex,  korgm1, 0, korgm1, korgm1, korgm1_state, empty_init, "Korg",                 "M1 EX Music Workstation (v1.29)",  MACHINE_IS_SKELETON)
SYST(1988, korgm1r,   korgm1, 0, korgm1, korgm1, korgm1_state, empty_init, "Korg",                 "M1R Music Workstation (v1.06)",    MACHINE_IS_SKELETON)
SYST(1988, korgm1rex, korgm1, 0, korgm1, korgm1, korgm1_state, empty_init, "Korg",                 "M1R EX Music Workstation (v1.12)", MACHINE_IS_SKELETON)
SYST(1993, korgm1p1,  korgm1, 0, korgm1, korgm1, korgm1_state, empty_init, "InVision Interactive", "M1 Plus+1 Music Workstation",      MACHINE_IS_SKELETON)
