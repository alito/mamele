// license:BSD-3-Clause
// copyright-holders:David Haywood

/*
    Big Karnak (Modular System) (checksum 1BD1EFB)

    this appears to be a different revision than the gaelco.cpp version, so could be original code, or
    based off one that is otherwise undumped.

    as with most of the 'Modular System' setups, the hardware is heavily modified from the original
    and consists of a multi-board stack in a cage, hence different driver.

    BOARDS

 Board 5-1
 ___________________________________________________________
 |                      ________________  ________________  |
 |                      |               | |               | |
 |                      | IC21-27C010   | | IC27-27C010   | |
 |                      |_______________| |_______________| |
 |           ::::::::   ________________  ________________  |
 |                      |               | |               | |
 | __                   | IC20-27C010   | | IC26-27C010   | |
 | | |                  |_______________| |_______________| |
 | | |     __________   ________________  ________________  |
 | | |     |_GAL16V8|   |               | |               | |
 | | |        5148      | IC19-27C010   | | IC25-27C010   | |
 | | |                  |_______________| |_______________| |
 | | |     __________   ________________  ________________  |
 | | |     |_GAL16V8|   |               | |               | |
 | | |        5248      | EMPTY         | | EMPTY         | |
 | | |                  |_______________| |_______________| |
 | | |      _________   ________________  ________________  |
 | | |      |74LS138N   |               | |               | |
 | | |                  | EMPTY         | | EMPTY         | |
 | | |                  |_______________| |_______________| |
 | | |________________  ________________                    |
 | |_||               | |               |                   |
 |    | IC6-27C010    | | IC15-27C101   |                   |
 |    |_______________| |_______________|                   |
 |    ________________  ________________                    |
 |    |               | |               |                   |
 |    | IC5-27C010    | | IC14-27C101   |                   |
 |    |_______________| |_______________|                   |
 |    ________________  ________________                    |
 |    |               | |               |     MODULAR       |
 |    | IC4-27C010    | | IC13-27C101   |    SYSTEM 2       |
 |    |_______________| |_______________|                   |
 |    ________________  ________________                    |
 |    |               | |               |                   |
 |    | EMPTY         | | EMPTY         |                   |
 |    |_______________| |_______________|                   |
 |    ________________  ________________                    |
 |    |               | |               |                   |
 |    | EMPTY         | | EMPTY         |                   |
 |    |_______________| |_______________|                   |
 |    ________________  ________________                    |
 |    |               | |               |                   |
 |    | EMPTY         | | EMPTY         |                   |
 |    |_______________| |_______________|                   |
 |__________________________________________________________|

 Board 7/4
 __________________________________________________________
 | _________  _________ __________  _________  _________ __|_
 | 74LS163AN| 74LS163AN||_GAL20V8_| |74F174N_| |74S189BN||   |
 |                         7648                          |   |
 | _________  _________  _________  _________  _________ |   |
 | |74LS157N| MC74F283N  |74LS273P| 74LS290B1| |74S189BN||   |
 |                                                       |   |
 | _________  _________  _________  _________  _________ |   |
 | |74LS393N| MC74F283N 74HCTLS373N 74LS298B1| |74S189BN||   |
 |                                                       |   |
 | _________  __________  _____________  _________       |   |
 | |74LS157N| |_PAL16V8_| |84256A-10L  | |74F174N_|      |   |
 | _________  __75FL____  |____________| _________       |   |
 | |74LS283B1 |74LS245B1| _____________  |74F174N_|      |   |
 | _________  __________  |84256A-10L  | __________      |   |
 | |74LS08N_| |74LS245B1| |____________| |74LS245B1|     |   |
 | _________  __________    __________   _________       |   |
 | |74LS20N | |74LS374B1|   |74LS273N |  |74LS74AN|      |   |
 |                                                       |   |
 | _________  __________    __________   _________       |___|
 | |74LS04N_| |PALCE16V8|   |74LS374N_|  |74LS157N|        |
 |               7348                                      |
 | _________  __________    __________   _________         |
 | |74LS00N_| |_GAL10V8_|   |74LS273N_| 74HCTLS367AN     __|_
 |               7248                                    |   |
 | _________  __________    __________   _________       |   |
 | |74LS08N_| |74LS273N |   |74LS374N_|  PALCE16V8|      |   |
 |                                          7448         |   |
 | _________  __________    __________   _________       |   |
 | |74LS74AN| |74LS273N_|   74LS367AN_|  |74LS373N|      |   |
 |                                                       |   |
 | _________  __________    __________   _________       |   |
 | |74LS32N_| |74LS374N_|   74LS367AN_| GAL20V8-25LP     |   |
 |                                          7148         |   |
 | _________  _________  _________  _________  _________ |   |
 | |74LS139N  |_74F74N_| |74LS157N| |74LS20N_| 74HCT373N |   |
 | _________  _________  _________  _________  _________ |   |
 | |74LS32N_| |74LS157N|  74HCT597E 74HCT597E 74HCT597E  |   |
 | _________  _________  _________  _________  _________ |   |
 | |74LS74AN| |74LS86B1| 74HCT597N  74HCT597N 74HCT597N  |   |
 | _________  _________  _________  _________  _________ |   |
 | |74LS74AN| |74LS86B1| 74HCT597N  74HCT597N  74HCT597N |   |
 |            _________  _________  _________  _________ |   |
 |            |74LS377N| 74HCT597N  74HCT597N  74HCT597N |   |
 |            _________  _________  _________  _________ |___|
 |            |74LS273N| 74LS169BN  74LS169BN  74LS169BN   |
 |_________________________________________________________|

Board 8
 __________________________________________________________________________________
 |           :::::::: <- Jumpers                                                   |
 |          ________________  ________________  ________________  ________________ |
 | _______  |               | |               | |               | |               ||
 | 74LS175N | EMPTY         | | EMPTY         | | EMPTY         | | EMPTY         ||
 |          |_______________| |_______________| |_______________| |_______________||
 | _______  ________________  ________________  ________________  ________________ |
 | 74LS175N |               | |               | |               | |               ||
 |          | EMPTY         | | EMPTY         | | EMPTY         | | EMPTY         ||
 | _______  |_______________| |_______________| |_______________| |_______________||
 | 74LS175N ________________  ________________  ________________  ________________ |
 |          |               | |               | |               | |               ||
 | _______  | EMPTY         | | EMPTY         | | EMPTY         | | EMPTY         ||
 | 74LS175N |_______________| |_______________| |_______________| |_______________||
 |          ________________  ________________  ________________  ________________ |
 | _______  |               | |               | |               | |               ||
 | 74LS175N | EMPTY         | | EMPTY         | | EMPTY         | | EMPTY         ||
 |          |_______________| |_______________| |_______________| |_______________||
 |          ________________  ________________  ________________  ________________ |
 | _______  |               | |               | |               | |               ||
 | 74LS175N | EMPTY         | | EMPTY         | | EMPTY         | | EMPTY         ||
 |          |_______________| |_______________| |_______________| |_______________||
 |          ________________  ________________  ________________  ________________ |
 | _______  |               | |               | |               | |               ||
 | 74LS175N | KA-814        | | KA-821        | | KA-829        | | KA-836        ||
 |          |_______________| |_______________| |_______________| |_______________||
 | _______  ________________  ________________  ________________  ________________ |
 | 74LS175N |               | |               | |               | |               ||
 |          | KA-815        | | KA-822        | | KA-830        | | KA-837        ||
 |          |_______________| |_______________| |_______________| |_______________||
 |  _____________________________      _______  _________________________________  |
 |__|                            |____74LS138N_|                                |__|
    |____________________________|             |________________________________|

 CPU Board 6/1
 _____________________________________________
 |             _______________              __|_
 |             |              |             |   |
 |             | 84256A-10L   |             |   |
 |             |______________|   _______   |   |
 |  _______   ________________    74LS138N  |   |
 | 74LS367AB1 |               |             |   |
 |            | EMPTY         |             |   |
 |            |_______________|             |   |
 |            ________________    _______   |   |
 |  _______   |               |   74LS245B1 |   |
 | 74LS367AB1 | IC20-27C010   |             |   |
 |            |_______________|             |   |
 |            ________________              |   |
 |  _______   |               |   _______   |   |
 | 74LS138N   | IC17-27C010   |   74LS374N  |   |
 |            |_______________|             |   |
 |             _______________              |   |
 |             |              |   _______   |___|
 |  _______    | 84256A-10L   |   74LS245B1   |
 |  GAL16V8    |______________|               |
 |    606     ________________                |
 |  _______   |               |   _______     |
 | 74LS174N   | EMPTY         |   74LS374N    |
 |            |_______________|               |
 |  _______   ________________                |
 |  PAL16V8   |               |               |
 |    648     | IC11-27C010   |   _______     |
 |            |_______________|   74LS138N    |
 |            ________________                |
 |            |               |               |
 |            | IC8-27C010    |               |
 |            |_______________|   _______     |
 | ___________________________    74LS32N     |
 | |                          |               |
 | |       TS68000CP12        |   _______     |
 | |__________________________|   74LS20N     |
 |                                            |
 |  _______   ______   _______    _______     |
 |  |74S74N    XTAL   74LS368AN   74LS132N    |
 |           24.000MHz                        |
 |____________________________________________|


Board 51/1
 _____________________________________________
 |                                            |
 | __________  ________ ________ ________     |
 | |TO SUB 51| 74LS299N 74LS169N AM2149-35DC  |
 | |_________| ________ ________ ________     |
 |             74LS169J 74LS169N AM2149-35DC  |
 |                                            |
 | __________  ________ ________ ________     |
 | |TO SUB 51| 74LS158N 74LS169B1 82S129N <- P0502
 | |_________| ________ ________ ________     |
 |             74LS299N 74LS169B1 74LS244N    |
 |                                            |
 | __________  ________ ________ ________     |
 | |TO SUB 51| 74LS299N 74LS169B1 74LS244N    |
 | |_________| ________ ________ ________     |
 |             |74LS20N AM2149-35DC 74LS298N  |
 |                                            |
 | __________  ________ ________ ________     |
 | |TO SUB 51| 74LS299N AM2149-35DC 74LS298N  |
 | |_________|                                |
 |   ________  ________ ________ ________  __ |
 |   74LS273N  74LS00B1 |74LS86N 74LS244N  | ||
 |                                         | ||
_|_  ________  ________ ________ ________  |T||
|  | 74LS08B1  74LS158N 74LS74B1 |74LS20B1 |O||
|  |                                       | ||
|  | ________  ________ ________ ________  |S||
IC48->PAL16R6A 74LS393N 74LS368A 74LS373B1 |U||
|  |                                       |B||
|  | ________  ________ ________ ________  | ||
|  | 74LS138PC 74LS283N |UM2148_||_EMPTY_| |5||
|  |                                       |1||
|  | ________  ________ ________ ________  | ||
|  | 74LS175B1 74LS283N |UM2148_| 74LS273P | ||
|  |                                       | ||
|  | ________  ________ ________ ________  |_||
|  | 74LS298N 74LS157B1 74LS157B1 74LS273P    |
|  |                                          |
|  | ________  ________ ________ ________     |
|  | 74LS158N 74LS169B1 74LS169B1 74LS245N    |
|__|                                          |
 |____________________________________________|

Sound Board 9/2
 ____________   _____________   _______________
 |           |_|_|_|_|_|_|_|_|_|             __|_
 |                PRE-JAMMA            XTAL  |   |
 | TDA2003                          28.00000 MHz |
 |           ________   ________   ________  |   |
 |           CNY 74-4   CNY 74-4   74LS368AN |   |
 |                                           |   |
 |           ________   ________   ________  |   |
 |           CNY 74-4   CNY 74-4  |74S112N|  |   |
 |                                           |   |
 |           ________   ________   ________  |   |
 |           74LS245N   74LS245B1  74LS298B1 |   |
 |                                           |   |
 | _______   ________   ________   ________  |   |
 | |LM324N   OKI M5205  |GAL16V8   74LS298B1 |   |
 |                        9148               |   |
 |           ________   ________   ________  |   |
 |           74LS157N   74LS267AN  74LS298B1 |   |
 | _______                                   |___|
 | |LM324N   ________   ________   ________    |
 |           74LS273N   74LS138N  |74LS32N|    |
 | D  74                                       |
 | I  LS  ______        ________   ________    |
 | P  24  Y3014B        74LS174N   74LS174N    |
 | S  5N                                       |
 |                      ________   ________    |
 | D  74                74LS245N  CXK5814P-35L |
 | I  LS  __________                           |
 | P  24  |  YM3812 |   ________   ________    |
 | S  5N  |_________|   74LS374N   74LS367AN   |
 | _______   _________                         |
 | 74LS138N UM6116K-3L  ________   ________    |
 |         ____________ 74LS74AN  |GAL20V8|    |
 | _______ |IC11-27C101             9248       |
 | GAL16V8 |__________| _______ _______ ______ |
 |   9348  ____________ 74LS32N 74LS393N 74LS163AN
 | _______ |IC6-27C512|                        |
 |74LS273N |__________| _______ _______ ______ |
 |         ____________74LS393N 74F74N 74LS163AN
 | _______ |Z0840006PSC                        |
 | 74LS08N |_Z80 CPU__| _______ _______   ____ |
 |                     74LS74AN 74LS368AN XTAL |
 |________________________________________16.000 MHz

*/


#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


class bigkarnk_ms_state : public driver_device
{
public:
	bigkarnk_ms_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_palette(*this, "palette"),
		m_screen(*this, "screen"),
		m_paletteram(*this, "palette"),
		m_gfxdecode(*this, "gfxdecode")
	{ }

	void bigkarnkm(machine_config &config);

private:
	required_device<cpu_device> m_maincpu;
	required_device<palette_device> m_palette;
	required_device<screen_device> m_screen;

	required_shared_ptr<uint16_t> m_paletteram;
	required_device<gfxdecode_device> m_gfxdecode;

	virtual void machine_start() override;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void bigkarnkm_map(address_map &map);
};


void bigkarnk_ms_state::bigkarnkm_map(address_map &map)
{
	map(0x000000, 0x07ffff).rom();
	map(0x200000, 0x2007ff).ram().w(m_palette, FUNC(palette_device::write16)).share("palette");
	map(0xff0000, 0xffffff).ram();
}


void bigkarnk_ms_state::machine_start()
{
}


uint32_t bigkarnk_ms_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}

static INPUT_PORTS_START( bigkarnkm )
INPUT_PORTS_END

static const gfx_layout tiles16x16x4_layout =
{
	16,16,
	RGN_FRAC(1,1),
	4,
	{ 0,8,16,24 },
	{ 0,1,2,3,4,5,6,7, 512,513,514,515,516,517,518,519 },
	{ STEP16(0,32) },
	16 * 16 * 4
};

static GFXDECODE_START( gfx_bigkarnk_ms )
	GFXDECODE_ENTRY( "gfx1", 0, tiles16x16x4_layout, 0, 16 )
	GFXDECODE_ENTRY( "gfx2", 0, tiles16x16x4_layout, 0, 16 )
GFXDECODE_END

void bigkarnk_ms_state::bigkarnkm(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 12_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &bigkarnk_ms_state::bigkarnkm_map);
	m_maincpu->set_vblank_int("screen", FUNC(bigkarnk_ms_state::irq6_line_hold));

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500) /* not accurate */);
	m_screen->set_size(256, 256);
	m_screen->set_visarea(0, 256-1, 0, 256-32-1);
	m_screen->set_screen_update(FUNC(bigkarnk_ms_state::screen_update));
	m_screen->set_palette(m_palette);

	PALETTE(config, m_palette).set_format(palette_device::xRGB_444, 0x400);

	GFXDECODE(config, m_gfxdecode, "palette", gfx_bigkarnk_ms);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
}

ROM_START( bigkarnkm )
	ROM_REGION( 0x080000, "maincpu", 0 )    /* 68000 code */
	ROM_LOAD16_BYTE( "cpu_ka_6.ic8",   0x000001, 0x020000, CRC(ab71c1d3) SHA1(3174f1c68e4aa5b6053b118da1fed1f4001193b0) )
	ROM_LOAD16_BYTE( "cpu_ka_6.ic17",  0x000000, 0x020000, CRC(9f5c0dac) SHA1(a8089f58e34d7ba581303f7cf819297d21867a6a) )
	ROM_LOAD16_BYTE( "cpu_ka_6.ic11",  0x040001, 0x020000, CRC(30674ef3) SHA1(d1b29337068ed7323c104a48de593c9ac4668e66) )
	ROM_LOAD16_BYTE( "cpu_ka_6.ic20",  0x040000, 0x020000, CRC(332d6dea) SHA1(cd7e402642f57c12cb7405c49b75bfaa0d104421) )

	ROM_REGION( 0x010000, "audiocpu", 0 )    /* Z80 code (uses YM3812 + M5205) */
	ROM_LOAD( "snd_ka.ic6",   0x000000, 0x010000, CRC(48a66be8) SHA1(0ca8e4ef5b5e257d56afda6946c5f2a0712917a3) )

	ROM_REGION( 0x020000, "audiodata", 0 )
	ROM_LOAD( "snd_ka.ic11",   0x000000, 0x020000, CRC(8e53a6b8) SHA1(5082bbcb042216a6d58c654a52c98d75df700ac8) )

	ROM_REGION( 0x180000, "gfx1", ROMREGION_ERASEFF ) // sprites (same rom subboard type as galpanic_ms.cpp)
	ROM_LOAD32_BYTE( "5_ka.ic4",         0x000003, 0x020000, CRC(2bee07ea) SHA1(afd8769955314768db894e4e98f65422fc0dbb4f) )
	ROM_LOAD32_BYTE( "5_ka.ic13",        0x000002, 0x020000, CRC(d55e3024) SHA1(71c84a76b08f8983f65ac4b99430eeb30dc3f8ea) )
	ROM_LOAD32_BYTE( "5_ka.ic19",        0x000001, 0x020000, CRC(fc682c21) SHA1(c3fa9907fbe276bc4b74b79dda52713e702e441c) )
	ROM_LOAD32_BYTE( "5_ka.ic25",        0x000000, 0x020000, CRC(1157b739) SHA1(fdea10f808f258409e19e41dedfb3d4699e7daa2) )
	ROM_LOAD32_BYTE( "5_ka.ic5",         0x080003, 0x020000, CRC(507056f7) SHA1(e754c803f85862a37d2a48be6554ff5bc4128b4d) )
	ROM_LOAD32_BYTE( "5_ka.ic14",        0x080002, 0x020000, CRC(ef936e76) SHA1(ed93b04a45c38c0fa7333f182beba33fafe17f38) )
	ROM_LOAD32_BYTE( "5_ka.ic20",        0x080001, 0x020000, CRC(38854cd6) SHA1(b32efc1c621a9d8559c294f7431c219a05a37db6) )
	ROM_LOAD32_BYTE( "5_ka.ic26",        0x080000, 0x020000, CRC(3f63c4ed) SHA1(e0dd3ec27e7aa0b7db1587e83d20d1b9333ca405) )
	ROM_LOAD32_BYTE( "5_ka.ic6",         0x100003, 0x020000, CRC(2fdbc484) SHA1(6e8ac1a8bde8189b7ebf32c59185425c512ab911) )
	ROM_LOAD32_BYTE( "5_ka.ic15",        0x100002, 0x020000, CRC(802128e4) SHA1(20cfdf28aa7ada404ceca236c6eb554dcaa8e633) )
	ROM_LOAD32_BYTE( "5_ka.ic21",        0x100001, 0x020000, CRC(5ccc0f99) SHA1(ae2b2d4b2aa77a099ad2711032e6a05ab52789b9) )
	ROM_LOAD32_BYTE( "5_ka.ic27",        0x100000, 0x020000, CRC(55509d96) SHA1(ddd064695ca7e8c2377f13484e385bf7ea7df610) )

	ROM_REGION( 0x100000, "gfx2", 0 )
	ROM_LOAD32_BYTE( "8_ka_814.ic14",      0x000003, 0x020000, CRC(50e6cab6) SHA1(5af8b27f35a59611484ea35a2883b1e59d5c7517) )
	ROM_LOAD32_BYTE( "8_ka_821.ic21",      0x000002, 0x020000, CRC(90c1d93e) SHA1(581a1e2f30e8b467c8d8f5c8e528c78c0c3904f2) )
	ROM_LOAD32_BYTE( "8_ka_829.ic29",      0x000001, 0x020000, CRC(8c5df0ec) SHA1(15a5b847d6d035f27300435a03bd254dd9b3f99c) )
	ROM_LOAD32_BYTE( "8_ka_836.ic36",      0x000000, 0x020000, CRC(43de75db) SHA1(419e7702d17c52365addb8bfda582e916762ead5) )
	ROM_LOAD32_BYTE( "8_ka_815.ic15",      0x080003, 0x020000, CRC(59d79b33) SHA1(70b9c60a72e517ac70f807c918f0ad4dd6c98f98) )
	ROM_LOAD32_BYTE( "8_ka_822.ic22",      0x080002, 0x020000, CRC(12fc89c0) SHA1(883144d0c453cd8f829b2209d9a8028b7f87d0d5) )
	ROM_LOAD32_BYTE( "8_ka_830.ic30",      0x080001, 0x020000, CRC(9904ae87) SHA1(5df3b35185c53a64c0647d297a19b9c013a3b3c2) )
	ROM_LOAD32_BYTE( "8_ka_837.ic37",      0x080000, 0x020000, CRC(f475eaa7) SHA1(8e5c7f0231d7f84bc377b756b99d055a4791e3bf) )

	ROM_REGION( 0x100, "prom", ROMREGION_ERASEFF )
	ROM_LOAD( "51_p0502_n82s129n.ic10",      0x000, 0x100, CRC(15085e44) SHA1(646e7100fcb112594023cf02be036bd3d42cc13c) ) // common PROM found on all? Modular System sets?

	ROM_REGION( 0x100000, "pals", 0 )
	ROM_LOAD( "51_p0503_pal16r6acn.ic48",       0x000, 0x104, CRC(07eb86d2) SHA1(482eb325df5bc60353bac85412cf45429cd03c6d) ) // matches one of the Euro League Modular System PALs

	ROM_REGION( 0x100, "protgal", 0 ) // all read protected
	ROM_LOAD( "5_5148_gal16v8-25lnc.ic9", 0, 1, NO_DUMP )
	ROM_LOAD( "5_5248_gal16v8-25lnc.ic8", 0, 1, NO_DUMP )
	ROM_LOAD( "7_75flv_gal16v8-25hb1.ic59", 0, 1, NO_DUMP )
	ROM_LOAD( "7_7148_gal20v8-25lp.ic7", 0, 1, NO_DUMP )
	ROM_LOAD( "7_7248_gal20v8-25lp.ic54", 0, 1, NO_DUMP )
	ROM_LOAD( "7_7348_gal16v8-25hb1.ic55", 0, 1, NO_DUMP )
	ROM_LOAD( "7_7448_gal16v8-25hb1.ic9", 0, 1, NO_DUMP )
	ROM_LOAD( "7_7648_gal20v8-25lp.ic44", 0, 1, NO_DUMP )
	ROM_LOAD( "cpu_606_gal16v8-25hb1.ic13", 0, 1, NO_DUMP )
	ROM_LOAD( "cpu_648_gal16v8-25ln.ic7", 0, 1, NO_DUMP )
	ROM_LOAD( "snd_9148_gal16v8-25hb1.ic142", 0, 1, NO_DUMP )
	ROM_LOAD( "snd_9248_gal20v8-25lp.ic18", 0, 1, NO_DUMP )
	ROM_LOAD( "snd_9348_gal16v8-25hb1.ic10", 0, 1, NO_DUMP )
ROM_END

GAME( 1991, bigkarnkm,  bigkarnk,  bigkarnkm,  bigkarnkm,  bigkarnk_ms_state, empty_init, ROT0, "Gaelco", "Big Karnak (Modular System)", MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
