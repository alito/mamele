// license:BSD-3-Clause
// copyright-holders:

/*
Namco 'M188 MAIN PCB'
1656960100 (1656970100)

Main components:

1x H8/3007 5B4 HD6413007F20
1x RTC 72423 A EPSON E220E
1x Oki M9810B
1x 14.7456 OSC near the H8
1x HRD051R5 DC/DC converter
2x MTB011 High Output Interface Driver ICs
1x 4-dip bank
11x connectors

Sweet Land 4 video: https://www.youtube.com/watch?v=Zj8_RRGlCI4
*/


#include "emu.h"

#include "cpu/h8/h83006.h"
#include "machine/msm6242.h"
#include "sound/okim9810.h"
//#include "video/hd44780.h"

//#include "emupal.h"
//#include "screen.h"
#include "speaker.h"


namespace {

class sweetland4_state : public driver_device
{
public:
	sweetland4_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{}

	void sweetland4(machine_config &config);

private:
	required_device<h83007_device> m_maincpu;
	//required_device<hd44780_device> m_lcdc;

	void lcdc_w(u8 data);

	void program_map(address_map &map);
};


void sweetland4_state::lcdc_w(u8 data)
{
	//m_lcdc->db_w(data << 4);
	//m_lcdc->rs_w(BIT(data, 7));
	//m_lcdc->e_w(BIT(data, 5));
}

void sweetland4_state::program_map(address_map &map)
{
	map(0x000000, 0x07ffff).rom().region("maincpu", 0);
	map(0x200000, 0x20000f).rw("rtc", FUNC(rtc72423_device::read), FUNC(rtc72423_device::write));
	map(0x40000f, 0x40000f).w(FUNC(sweetland4_state::lcdc_w));
}


static INPUT_PORTS_START( sweetld4 )
	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x40, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x80, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("DSW1") // only one bank of dips on PCB
	PORT_DIPNAME( 0x01, 0x01, "DSW1-01" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, "DSW1-02" )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, "DSW1-04" )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, "DSW1-08" )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END


void sweetland4_state::sweetland4(machine_config &config)
{
	H83007(config, m_maincpu, 14.7456_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &sweetland4_state::program_map);

	RTC72423(config, "rtc", 32'768); // no evident XTAL on PCB

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	okim9810_device &oki(OKIM9810(config, "oki", 4'096'000)); // no evident XTAL on PCB
	oki.add_route(0, "lspeaker", 1.00);
	oki.add_route(1, "rspeaker", 1.00);
}


ROM_START( sweetld4 )
	ROM_REGION( 0x80000, "maincpu", 0 )
	ROM_LOAD( "swb 1 mpr0b.19f", 0x00000, 0x80000, CRC(8a3e9e87) SHA1(8b1532bac5e1668bdf719a66fdc8b9165cf0722a) )

	ROM_REGION( 0x200000, "oki", 0 )
	ROM_LOAD( "swb 1 snd0.19d", 0x000000, 0x200000, CRC(39294dc7) SHA1(6094a53f10f220d1df8c3e35df11e0566e9c099c) ) // 11xxxxxxxxxxxxxxxxx = 0x00
ROM_END

} // anonymous namespace


GAME( 2004, sweetld4, 0, sweetland4, sweetld4, sweetland4_state, empty_init, ROT0, "Namco", "Sweet Land 4 Bright (ver 2004.9.29)", MACHINE_IS_SKELETON_MECHANICAL )
