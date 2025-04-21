// license:BSD-3-Clause
// copyright-holders:

/*
The CRT-300 is an extension of CRT-250/CRT-260 boards found in meritm.cpp.

TODO:
- Hanging at UART device check (PC=5e44)
- Never initializes RAMDAC;
- Never initializes CRTC on 350 games;

===================================================================================================

Merit - Multi-Action 6710-13 Touchscreen game

MERIT CRT-300 REV A:
+------------------------------------------------------------+
|       U45#                HY6264ALP-10      10.000000MHz   |
|                                                            |
|       U46                 HY6264ALP-10                     |
|                                                            |
|       U47                                   HD46505SP-2  +-|
|                                                          | |
|       U48                                   ADV476KN35   | |
|                                                          | |
|            PAL16l8ACN.U14                                |J|
|                                                          |2|
|U7**        PAL20L10NC.U8                                 | |
|                                                          | |
|DS1225Y.U6  PAL20L10NC.U1  PC16550DN                      | |
|                                                          | |
|U5*     U20*               DSW  1.84MHz                   +-|
|                                                          +-|
|                           YM2149F                        | |
|                                                          | |
|       Z8400BB1-Z80B       D8255AC-2                      | |
|                                                          |J|
|10.000MHz       DS1231-50  D8255AC-2                      |1|
|                                                          | |
|                                                          | |
|                                                          | |
|VOLUME   MB3731                                           | |
+----------------------------------------------------------+-+

  CPU: Z80B 6MHz part Clocked @ 5MHz (10MHz/2)
Video: HD46505SP-2 CRT controller (enhanced) 2MHz AKA HD68B45SP compatible with MC68B45P
       ADV476KN35 CMOS Monolithic 256x18 Color Palette RAM-DAC PS/2 & VGA compatible
Sound: AY8930
       MB3731 18-Watt BTL Power Amplifier
  RAM: 6264 8K High Speed CMOS Static RAM x 2
  OSC: 10.00MHz x 2, 1.85MHz
  DSW: 8 switch dipswitch block labeled S1
Other: PC16550DN UART with FIFO clocked @ 1.84MHz
       D8255AC Programmable Peripheral Interface chip x 2
       DS1225Y-200 Dallas 8Kx8 NVRAM @ U6
       DS1231 Power Monitor Chip

*  U5 is a 28pin female socket, U20 is 28pin male socket
** U7 is a stacked DS1216 Dallas 2Kx8 SmartWatch RTC + BENCHMARQ bq4010YMA-150 8Kx8 NVRAM

Connectors:
  J1 80-pin connector to backplane & wire harness
  J2 80-pin connector to backplane & wire harness

# Denotes unpopulated

ROMs on CRT-300 mainboard:

U-46
DC-350

U-47
DC-350

U-48
DC-350


CRT-307 rev A
+----------------+
| 28pinM  28pinF |
| U1    74LS541N |
|       SW1      |
| U2    74LS00N  |
+----------------+

Other: 8 switch dipswitch block labeled SW1
       28pinM 28pin male socket to plug into U5
       28pinF 28pin female socket to receive U20

ROMs on CRT-307 daughter board

U-1
DC-350
Ticket

U-2
DC-350
Ticket

Snooping around the U1 & U2 roms with a hex editor shows the game uses a Printer & Modem.
Game can be played in English or French
Games look to be basic Poker games, Blackjack & Super 8 Slots
Copyright is 1989

---------------------------------------------------------------------------------------------

The CRT-350 is an extension of CRT-300 that allows for memory (ROM) expansion.

MERIT CRT-350 REV C (and REV B):
+------------------------------------------------------------+
|       U45*                HY6264ALP-10      10.000000MHz   |
|                                                            |
|       U46                 HY6264ALP-10                     |
|                                                            |
|       U47                                   HD46505SP-2  +-|
|                                                          | |
|       U48                                   IMSG176P-40  | |
|                                                          | |
|U7*    PAL16l8ACN.U14                                     |J|
|                                                          |2|
|U6*    PAL20L10NC.U8                                      | |
|                                                          | |
|U5*    PAL20L10NC.U4A      PC16550DN                      | |
|                                                          | |
||===========J14==========| DSW  1.84MHz                   +-|
|                                                          +-|
|                           YM2149F                        | |
|                                                          | |
|       Z0840006PSC-Z80B    D8255AC-2                      | |
|                                                          |J|
|10.000MHz       DS1231-50  D8255AC-2                      |1|
|                                                          | |
|                                                          | |
|                                                          | |
|VOLUME   LM383T                                           | |
+----------------------------------------------------------+-+

  CPU: Z80B 6MHz part Clocked @ 5MHz (10MHz/2)
Video: HD46505SP-2 CRT controller (enhanced) 2MHz AKA HD68B45SP compatible with MC68B45P
       inmos IMS G176 High performance CMOS color look-up table
Sound: Yamaha YM2149F or AY-3-8910A
       LM383T 7-Watt Audio High Power Amplifier (rev C PCB only)
       MB3731 18-Watt BTL Power Amplifier (rev B PCB only)
  RAM: 6264 8K High Speed CMOS Static RAM x 2
  OSC: 10.00MHz x 2, 1.85MHz
  DSW: 8 switch dipswitch block labeled S1
Other: PC16550DN UART with FIFO clocked @ 1.84MHz
       D8255AC Programmable Peripheral Interface chip x 2
       DS1231 Power Monitor Chip

Connectors:
  J1 80-pin connector to CRT-351 backplane & wire harness
  J2 80-pin connector to CRT-351 backplane & wire harness
  J14 64-pin connector for CRT-352 Expansion board (96 pins, but middle row pins removed)

* Denotes unpopulated


MEMORY EXPANSION BOARD CRT-352 rev A
+--------------------------+
|     U11*         U15     |
|                          |
|     U10          U14     |
|                          |
|     U9           U13     |
|                          |
|     U8           U12     |
|                          |
| 74HC245      INS8250N    |
|                          |
| DS1225Y.U7   PAL 1.84MHz |
|                          |
| DS1216.U18   GAL20XV10B  |
|                          |
| DS1230Y.U17              |
|                          |
||===========J1===========||
|                          |
|       74HC541N    DSW    |
+--------------------------+

Other: DS1225Y-200 Dallas 8Kx8 NVRAM
       DS1230Y-200 Dallas 32Kx8 NVRAM
       DS1216 Dallas 2Kx8 SmartWatch RTC
       PC16550DN UART with FIFO clocked @ 1.84MHz
       8 switch dipswitch block labeled SW1 (enable/disable games)

Connectors:
  J1 96-pin female receiver to connect to CRT-350 main board  (64 pins used, middle row pins not connected)

* Denotes unpopulated


CRT-351
+----------------------------------------------------------------------------+
|      |=J5==| |---------------------------J3-------------------------------||
| |===J6===|                                                   |=====J4=====||
|                                                    JPR3              |-J7-||
|              |=============J2=============||=============J1===============||
+----------------------------------------------------------------------------+

J1 80-pin connector to J1 connector on the CRT-350 mainboard
J2 80-pin connector to J2 connector on the CRT-350 mainboard
J3 65-pin single row connector for wire harness
J4 40-pin dual row connector for printer
J5 16-pin dual row connector for unknown
J6 17-pin single row connector for unknown (some kind of jumper block)
J7 6-pin single row connector for hopper
JPR3 is a 3 pin jumper: Pins 1&2 = Printer, pins 2&3= Hopper

******************************************************************************

7551-20-R3T

MERIT CRT-350 REV C + MEMORY EXPANSION BOARD CRT-352 rev A

Main PCB graphics roms (on main board):

U46
DMA6
9c9a

U47
DMA6
ed62

U48
DMA6
a382


Program ROMs on Expansion board:

U11 *Empty       U15
                 7551-20-R3T
                 0ff2

U10              U14
7551-20-R3T      7551-20-R3T
a43c             a786

U9               U13
7551-20-R3T      7551-20-R3T
8f39             5443

U8               U12
7551-20-R3T      7551-20-R3T
1d98             4f74


According to U14:
 INVALID DIPSWITCH SETTING
 ENABLE AT LEAST TWO GAMES
  CS1-1 ON =JOKER POKER
  CS1-2 ON =DEUCES
  CS1-3 ON =FEVER POKER
  CS1-4 ON =JACKS POKER
  CS1-5 ON =BLACKJACK
  CS1-6 ON =KENO WILD
  CS1-7 ON =TBALL KENO
  CS1-8 ON =BINGO
 CSW1-1 ON =DOLR JACKS
 CSW1-2 ON =DOLR DEUCE
 CSW1-3 ON =5# KENO
 CSW1-4 ON =ADDEM

Dipswitch on CRT-350 main is labeled S1
Dipswitch on CRT-352 MEM is labeled SW1

******************************************************************************

Merit MULTI-ACTION 7551-21-R2P

MERIT CRT-350 REV C + MEMORY EXPANSION BOARD CRT-352 rev A

Main PCB graphics roms (on main board):

U46
NC $

U47
NC $

U48
NC $


Program ROMs on Expansion board:

U11 *Empty       U15
                 7551-21-R2P

U10              U14
7551-21-R2P      7551-21-R2P

U9               U13
7551-21-R2P      7551-21-R2P

U8               U12
7551-21-R2P      7551-21-R2P


According to U14:
 INVALID DIPSWITCH SETTING
 ENABLE AT LEAST TWO GAMES
  CS1-1 ON =JOKER POKER
  CS1-2 ON =DEUCES
  CS1-3 ON =FEVER POKER
  CS1-4 ON =JACKS POKER
  CS1-5 ON =BLACKJACK
  CS1-6 ON =KENO WILD
  CS1-7 ON =TBALL KENO
  CS1-8 ON =BINGO
 CSW1-1 ON =DOLR JACKS
 CSW1-2 ON =DOLR DEUCE
 CSW1-3 ON =5# KENO
 CSW1-4 ON =ADDEM

Dipswitch on CRT-350 main is labeled S1
Dipswitch on CRT-352 MEM is labeled SW1

******************************************************************************

Merit MULTI-ACTION 7556-00-R2

MERIT CRT-350 REV C + MEMORY EXPANSION BOARD CRT-352 rev A

Main PCB graphics roms (on main board):

U46
MLT8
ck:8bbe

U47
MLT8
ck:0262

U48
MLT8
ck:9daa

NOTE: The above ROMs are also known to be labeled as Multi-Action 7556-WV U46 through Multi-Action 7556-WV U48


Program ROMs on Expansion board:

U11 *Empty       U15
                 7556-01-r0
                 add3

U10              U14
7556-01-r0       7556-01-r0
ef8f             dff2

U9               U13
7556-01-r0       7556-01-r0
ef1e             7c21

U8               U12
7556-01-r0       7556-00-r2
23c6


According to U14:
 INVALID DIPSWITCH SETTING
 ENABLE AT LEAST TWO GAMES
  CS1-1 ON =JOKER POKER
  CS1-2 ON =DEUCES
  CS1-3 ON =FEVER POKER
  CS1-4 ON =JACKS POKER
  CS1-5 ON =BLACKJACK
  CS1-6 ON =KENO WILD
  CS1-7 ON =TBALL KENO
  CS1-8 ON =BINGO
 CSW1-1 ON =DOLR JACKS
 CSW1-2 ON =DOLR DEUCE
 CSW1-3 ON =5# KENO
 CSW1-4 ON =TREASURE

Dipswitch on CRT-350 main is labeled S1
Dipswitch on CRT-352 MEM is labeled SW1

NOTE: on this PCB pin28 on the DS1225Y was bent up so data was not correctly saved from PCB
      on this PCB pin28 on the DS1130Y was broken so data was not correctly saved from PCB

******************************************************************************

Merit MULTI-ACTION 7558-01-R0 DS

MERIT CRT-350 REV B + MEMORY EXPANSION BOARD CRT-352 rev A

Main PCB graphics roms (on main board):

Multi-Action
7556-WV
U46

Multi-Action
7556-WV
U47

Multi-Action
7556-WV
U48


Program ROMs on Expansion board:

U11 *Empty       U15
                 7558-01-R0 DS
                 cfba

U10              U14
7558-01-R0 DS    7558-01-R0 DS
88b5             a309

U9               U13
7558-01-R0 DS    7558-01-R0 DS
e651             a833

U8               U12
7558-01-R0 DS    7558-01-R0 DS
d27a             11ff


According to U12:
 INVALID DIPSW
 ENABLE 2+ GAMES
  CS1-1 ON =JOKER (Joker Poker)
  CS1-2 ON =DEUCES (Deuces Wild)
  CS1-3 ON =FEVER (Flush Fever)
  CS1-4 ON =JACKS (Jacks or Better Poker)
  CS1-5 ON =BJACK (Blackjack)
  CS1-6 ON =KNO WLD (Wild Spot Keno)
  CS1-7 ON =PWR KNO (Power Hit Keno)
  CS1-8 ON =8 LINE (Super 8)
 CSW1-1 ON =5 REEL (Five Reel)
 CSW1-2 ON =$ DEUCE (Dollar Deuces)
 CSW1-3 ON =D DOG (Dogs + Diamonds)
 CSW1-4 ON =TR7 (Treasure Sevens)

Dipswitch on CRT-350 main is labeled S1
Dipswitch on CRT-352 MEM is labeled SW1

*/

#include "emu.h"

#include "cpu/z80/z80.h"
#include "machine/ds1204.h"
#include "machine/i8255.h"
#include "machine/ins8250.h"
#include "machine/microtch.h"
#include "machine/nvram.h"
#include "sound/ay8910.h"
#include "video/bt47x.h"
#include "video/mc6845.h"

#include "screen.h"
#include "speaker.h"
#include "tilemap.h"


namespace {

class merit3xx_state : public driver_device
{
public:
	merit3xx_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_rombank(*this, "rombank")
		, m_gfxdecode(*this, "gfxdecode")
		, m_ymsnd(*this, "ymsnd")
		, m_gfx(*this, "gfx1")
		, m_charram(*this, "charram")
		, m_attram(*this, "attrram")
	{ }

	void merit300(machine_config &config);
	void merit350(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	MC6845_UPDATE_ROW(update_row);

	void ppi1_pa_w(u8 data);
	void crt350_rombank_w(u8 data);

	void main_map(address_map &map);
	void io_map(address_map &map);
	void crt350_main_map(address_map &map);
	void crt350_io_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_memory_bank m_rombank;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<ym2149_device> m_ymsnd;
	required_region_ptr<uint8_t> m_gfx;
	required_shared_ptr<uint8_t> m_charram;
	required_shared_ptr<uint8_t> m_attram;
};

MC6845_UPDATE_ROW(merit3xx_state::update_row)
{
	uint16_t x = 0;
	uint8_t const *const data = m_gfx;

	for (uint8_t cx = 0; cx < x_count; cx++)
	{
		const u32 base_addr = (ma + cx) & 0x1fff;
		int const attr = m_attram[base_addr];
		// TODO: bit 0 comes from an unknown bit in attr (bit 0?), bit 1-2 used with "TOD CLOCK ERROR" / "COIN JAM" messages
		u32 tile_addr = (m_charram[base_addr] << 1) | ((attr & 0x40) << 4);
		tile_addr <<= 3;
		tile_addr += (ra & 7);

		for (int i = 7; i >= 0; i--)
		{
			// TODO: may be banked, need RAMDAC colors to tell
			int col = 0;

			// TODO: looks 6bpp from GFX decoding (cfr. 0x*000 - 0x*800 tiles)
			col |= (BIT(data[0x00000 | tile_addr], i) << 2);
			col |= (BIT(data[0x10000 | tile_addr], i) << 1);
			col |= (BIT(data[0x20000 | tile_addr], i) << 0);

			// TODO: ramdac has no palette set (?) so cheating for now
			const u32 pen = (BIT(col, 2) ? 0xff : 0) | (BIT(col, 1) ? 0xff00 : 0) | (BIT(col, 0) ? 0xff0000 : 0);

			bitmap.pix(y, x) = pen;

			x++;
		}
	}
}

void merit3xx_state::ppi1_pa_w(u8 data)
{
	m_rombank->set_entry((data & 0x04) >> 1 | (~data & 0x01));
}

void merit3xx_state::crt350_rombank_w(u8 data)
{
	m_rombank->set_entry(data & 0x07);
}

void merit3xx_state::main_map(address_map &map)
{
//  map.unmap_value_high();
	map(0x0000, 0x7fff).bankr("rombank");
	map(0x8000, 0x9fff).ram().share("nvram");
	// definitely accesses RAM here, would drop to "RAM error" with unmap high
	map(0xa000, 0xbfff).ram();
	map(0xc000, 0xdfff).ram().share("charram");
	map(0xe000, 0xffff).ram().share("attrram");
}

void merit3xx_state::io_map(address_map &map)
{
	map.global_mask(0xff);
	map(0x00, 0x03).rw("ppi0", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x04, 0x07).rw("ppi1", FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x10, 0x17).rw("uart", FUNC(ns16550_device::ins8250_r), FUNC(ns16550_device::ins8250_w));
	map(0x18, 0x1b).m("ramdac", FUNC(bt476_device::map));
	map(0x40, 0x40).rw("crtc", FUNC(hd6845s_device::status_r), FUNC(hd6845s_device::address_w));
	map(0x41, 0x41).rw("crtc", FUNC(hd6845s_device::register_r), FUNC(hd6845s_device::register_w));
	map(0x80, 0x80).r(m_ymsnd, FUNC(ym2149_device::data_r));
	map(0x80, 0x81).w(m_ymsnd, FUNC(ym2149_device::address_data_w));
}

void merit3xx_state::crt350_main_map(address_map &map)
{
	main_map(map);
//  map(0xa000, 0xbfff).ram();
}

void merit3xx_state::crt350_io_map(address_map &map)
{
	io_map(map);
	map(0x20, 0x20).w(FUNC(merit3xx_state::crt350_rombank_w));
}

static INPUT_PORTS_START( merit3xx )
	PORT_START("IN0")
	PORT_DIPNAME( 0x01, 0x01, "IN0" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")
	PORT_DIPNAME( 0x01, 0x01, "IN1" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) ) // "TOD clock failure" if enabled
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN2")
	PORT_DIPNAME( 0x01, 0x01, "IN2" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("PB")
	PORT_DIPNAME( 0x01, 0x01, "PB" )
	PORT_DIPSETTING(    0x01, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x02, 0x02, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x04, 0x04, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x20, 0x20, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
INPUT_PORTS_END

static const gfx_layout gfx_8x8x3 =
{
	8,8,
	RGN_FRAC(1, 3),
	3,
	{ RGN_FRAC(0, 3), RGN_FRAC(1, 3), RGN_FRAC(2, 3) },
	{ STEP8(0, 1) },
	{ STEP8(0, 8) },
	8*8
};

static GFXDECODE_START( gfx_merit300 )
	GFXDECODE_ENTRY( "gfx1", 0, gfx_8x8x3, 0, 1 )
GFXDECODE_END

void merit3xx_state::machine_start()
{
	memory_region *rom = memregion("maincpu");
	m_rombank->configure_entries(0, rom->bytes() / 0x8000, rom->base(), 0x8000);
}

void merit3xx_state::machine_reset()
{
	m_rombank->set_entry(0);
}

void merit3xx_state::merit300(machine_config &config)
{
	Z80(config, m_maincpu, 10_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &merit3xx_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &merit3xx_state::io_map);
	m_maincpu->set_vblank_int("screen", FUNC(merit3xx_state::irq0_line_hold));

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	i8255_device &ppi0(I8255(config, "ppi0"));
	ppi0.in_pa_callback().set_ioport("IN0");
	ppi0.in_pb_callback().set_ioport("IN1");
	ppi0.in_pc_callback().set_ioport("IN2");

	i8255_device &ppi1(I8255(config, "ppi1"));
	ppi1.out_pa_callback().set(FUNC(merit3xx_state::ppi1_pa_w));

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(10_MHz_XTAL, 616, 0, 512, 270, 0, 256);
	screen.set_screen_update("crtc", FUNC(hd6845s_device::screen_update));

	hd6845s_device &crtc(HD6845S(config, "crtc", 10_MHz_XTAL / 8));
	crtc.set_screen("screen");
	crtc.set_show_border_area(false);
	crtc.set_char_width(8);
	crtc.set_update_row_callback(FUNC(merit3xx_state::update_row));

	BT476(config, "ramdac", 10_MHz_XTAL);

	NS16550(config, "uart", 1.8432_MHz_XTAL);

	GFXDECODE(config, m_gfxdecode, "ramdac", gfx_merit300);

	SPEAKER(config, "speaker").front_center();

	YM2149(config, m_ymsnd, XTAL(1'843'200));
	m_ymsnd->port_b_read_callback().set_ioport("PB");
	m_ymsnd->add_route(ALL_OUTPUTS, "speaker", 0.5);

}

void merit3xx_state::merit350(machine_config &config)
{
	merit300(config);

	m_maincpu->set_addrmap(AS_PROGRAM, &merit3xx_state::crt350_main_map);
	m_maincpu->set_addrmap(AS_IO, &merit3xx_state::crt350_io_map);

	subdevice<i8255_device>("ppi1")->out_pa_callback().set_nop();
}



ROM_START( ma6710 )
	ROM_REGION(0x20000, "maincpu", 0)
	ROM_LOAD( "u-1_dc-350_ticket.u1", 0x00000, 0x10000, CRC(33aa53ce) SHA1(828d6f4828d5d90777c573a6870d800ae6a51425) )
	ROM_LOAD( "u-2_dc-350_ticket.u2", 0x10000, 0x10000, CRC(fcac2391) SHA1(df9a1834441569fef876594aaef7d364831dbae6) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "u-46_dc-350.u46", 0x00000, 0x10000, CRC(3765a026) SHA1(cdb47d4b3775bec4b3ab16636d795ad737344166) )
	ROM_LOAD( "u-47_dc-350.u47", 0x10000, 0x10000, CRC(bbcf8280) SHA1(83c6fd84bdd09dd82506d81be1cbae797fd59347) )
	ROM_LOAD( "u-48_dc-350.u48", 0x20000, 0x10000, CRC(b93a0481) SHA1(df60d81fb68bd868ce94f8b313896d6d31e54ad4) )

	ROM_REGION( 0x2000, "nvram", 0 )
	ROM_LOAD( "ds1225y.u6", 0x0000, 0x2000, CRC(78fd0284) SHA1(37aa7deaafc6faad7505cd56a442913b35f54166) )

	// DS1216?
	ROM_REGION( 0x2000, "unk", 0 )
	ROM_LOAD( "bq4010.u5",  0x0000, 0x2000, CRC(003ea272) SHA1(3f464a0189af49470b33825a00905df6b156913f) )
ROM_END


ROM_START( ma7551t ) // all ROMs reads matched printed checksum
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_LOAD( "u8_7551-20-r3t_1d98.u8",   0x00000, 0x08000, CRC(a130ec60) SHA1(7d09faf1c6a5df63890eb22317bb4a5ad55d8b8f) )
	ROM_LOAD( "u9_7551-20-r3t_8f39.u9",   0x08000, 0x08000, CRC(6758e2f9) SHA1(f114bd78e1d940190bc2771d90642dba566d47ed) )
	ROM_LOAD( "u10_7551-20-r3t_a43c.u10", 0x10000, 0x08000, CRC(3efb3bb4) SHA1(6b39acecd577eb2f4b44c8421390f4035e5d5d84) )
	// u11 not populated
	ROM_LOAD( "u15_7551-20-r3t_0ff2.u15", 0x20000, 0x08000, CRC(39203dd0) SHA1(885424a7c0bdb85891188ca575c0c3ca3ecca04a) )
	ROM_LOAD( "u14_7551-20-r3t_a786.u14", 0x28000, 0x08000, CRC(63baf2a5) SHA1(402f8bac78cdbe6d6df90db3e77bd9e97615ae21) )
	ROM_LOAD( "u13_7551-20-r3t_5443.u13", 0x30000, 0x08000, CRC(88f89dd9) SHA1(0ce29f56f5a3643a2fb204ce2b919bea6f5dd3b5) )
	ROM_LOAD( "u12_7551-20-r3t_4f74.u12", 0x38000, 0x08000, CRC(9b818bb4) SHA1(5f1228f500618d5de93c82dbc9c710651bdb22f6) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "u46_dma6_9c9a.u46", 0x00000, 0x10000, CRC(138d1cc7) SHA1(2043fcc580269966031d86dc445e03bddf83a412) )
	ROM_LOAD( "u47_dma6_ed62.u47", 0x10000, 0x10000, CRC(4312f851) SHA1(281f0fdf5ec0519c5fbdf73f2d8d567da626b13e) )
	ROM_LOAD( "u48_dma6_a382.u48", 0x20000, 0x10000, CRC(fd256128) SHA1(e32da5242a8f0c68074326336938c60991d98fdc) )

	ROM_REGION( 0x2000, "nvram", 0 )
	ROM_LOAD( "dallas_ds1225y-150.u7",  0x0000, 0x2000, CRC(d7d46736) SHA1(98c7d6905f30e351583c90103aae0ca742ba070f) )

	ROM_REGION( 0x8000, "nvram2", 0 )
	ROM_LOAD( "dallas_ds1230y-120.u17", 0x0000, 0x8000, CRC(6fcc7313) SHA1(6ee2dd8898e4b567a27ee5b8ed54e0cdc56f9553) )
ROM_END


ROM_START( ma7551p )
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_LOAD( "u8_7551-21-r2p.u8",   0x00000, 0x08000, CRC(a2ae7a03) SHA1(2d923cf068fd1b9bd5f48a110f5155b876b9ba37) )
	ROM_LOAD( "u9_7551-21-r2p.u9",   0x08000, 0x08000, CRC(2e669bc9) SHA1(376e808a62e92169a5ae34b9ef808fe4eda6c13c) )
	ROM_LOAD( "u10_7551-21-r2p.u10", 0x10000, 0x08000, CRC(e9425269) SHA1(030a3d9beafd08c5a571672fb6987525c8d9a0f5) )
	// u11 not populated
	ROM_LOAD( "u15_7551-21-r2p.u15", 0x20000, 0x08000, CRC(31283190) SHA1(153601d5df7fbbc116f876399ce194797175be2f) )
	ROM_LOAD( "u14_7551-21-r2p.u14", 0x28000, 0x08000, CRC(fe993b57) SHA1(4c872b3dff278298558493f6fd9a64be63613956) )
	ROM_LOAD( "u13_7551-21-r2p.u13", 0x30000, 0x08000, CRC(9194d993) SHA1(52d094f55c329a7f0b4bf1dd02a7784e9a9faa12) )
	ROM_LOAD( "u12_7551-21-r2p.u12", 0x38000, 0x08000, CRC(8ca19c9c) SHA1(a694a9be8b6d2beea8ee171dcfb2fa64eb6af14c) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "u46_nc+.u46", 0x00000, 0x10000, CRC(5140ca67) SHA1(0f5f7062cd874529630fd6f58e640c11f0692786) )
	ROM_LOAD( "u47_nc+.u47", 0x10000, 0x10000, CRC(5f1d8ffa) SHA1(c8fe36f91ddd634e6d66434342b8dafdc1ffa332) )
	ROM_LOAD( "u48_nc+.u48", 0x20000, 0x10000, CRC(1ef22a70) SHA1(f33db37dc6e2ded3a39907eb5f5ea6306fd6f8b0) )

	ROM_REGION( 0x2000, "nvram", 0 )
	ROM_LOAD( "dallas_ds1225y-150.u7",  0x0000, 0x2000, CRC(2526c25c) SHA1(fe7d54e65dc7bd93576f496160f63b3c8e8c128b) )

	ROM_REGION( 0x8000, "nvram2", 0 )
	ROM_LOAD( "dallas_ds1230y-120.u17", 0x0000, 0x8000, CRC(54099035) SHA1(2a8854a862bc24ff72470660e60e9e4228158b42) )
ROM_END


ROM_START( ma7556 ) // all ROMs reads matched printed checksum
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_LOAD( "u8_7556-01-r0_23c6.u8",   0x00000, 0x08000, CRC(4dfca3d2) SHA1(2d8cc59edad12368dbc267b763af46e095599bc0) )
	ROM_LOAD( "u9_7556-01-r0_ef1e.u9",   0x08000, 0x08000, CRC(142370d6) SHA1(cb32f204b7bf78874990ef438fd5115cc3ed140e) )
	ROM_LOAD( "u10_7556-01-r0_ef8f.u10", 0x10000, 0x08000, CRC(f2dfb326) SHA1(b50a234ad649d41fb50c6eec345fa9414de6cec9) )
	// u11 not populated
	ROM_LOAD( "u15_7556-01-r0_add3.u15", 0x20000, 0x08000, CRC(83e5f4cd) SHA1(15b999169b28fb267ec8a265c915c1d366e57655) )
	ROM_LOAD( "u14_7556-01-r0_dff2.u14", 0x28000, 0x08000, CRC(9e5518c1) SHA1(37ed33118d87f0699845f84c820569666ac8c533) )
	ROM_LOAD( "u13_7556-01-r0_7c21.u13", 0x30000, 0x08000, CRC(5288eecc) SHA1(efd569beb22b8a9354520e7755bd797724593a0a) )
	ROM_LOAD( "u12_7556-00-r2.u12",      0x38000, 0x08000, CRC(34357c5d) SHA1(f71db3cd5ced70a709ecb8de1328c12666abc047) ) // rev 2, other program ROMs are rev 0

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "multi-action_7556-wv_u46.u46", 0x00000, 0x10000, CRC(32c11634) SHA1(26f3c5c220b45e8eedad940ff94dc5ef6f89e3fa) ) // also known to be labeled: U46  MLT8  cs:8bbe
	ROM_LOAD( "multi-action_7556-wv_u47.u47", 0x10000, 0x10000, CRC(5781bdd7) SHA1(e3f920dd1c247f92044100e28fc39d48b02b6a4b) ) // also known to be labeled: U47  MLT8  cs:0262
	ROM_LOAD( "multi-action_7556-wv_u48.u48", 0x20000, 0x10000, CRC(52ac8411) SHA1(9941388b90b6b91c1dab9286db588f0032620ea4) ) // also known to be labeled: U48  MLT8  cs:9daa

	ROM_REGION( 0x2000, "nvram", 0 )
	ROM_LOAD( "dallas_ds1225y-200.u7",  0x0000, 0x2000, BAD_DUMP CRC(5b635a95) SHA1(dd347258ba9e000963da75af5ac383c09b60be0b) )

	ROM_REGION( 0x8000, "nvram2", 0 )
	ROM_LOAD( "dallas_ds1230y-200.u17", 0x0000, 0x8000, BAD_DUMP CRC(e0c07037) SHA1(c6674a79a51f5aacca4a9e9bd19a2ce475c98b47) )
ROM_END


ROM_START( ma7558 ) // all ROMs reads matched printed checksum
	ROM_REGION(0x40000, "maincpu", 0)
	ROM_LOAD( "u8_7558-01-r0_ds_d27a.u8",   0x00000, 0x08000, CRC(ff59d929) SHA1(902ba35967a49b73a6b7c1990c736ac922e25672) )
	ROM_LOAD( "u9_7558-01-r0_ds_e651.u9",   0x08000, 0x08000, CRC(e02f8c98) SHA1(d04351535f86907129b97811a02a590f96f108b9) )
	ROM_LOAD( "u10_7558-01-r0_ds_88b5.u10", 0x10000, 0x08000, CRC(33994802) SHA1(041190e01115abd7e629335486f7ba3070a29635) )
	// u11 not populated
	ROM_LOAD( "u15_7558-01-r0_ds_cfba.u15", 0x20000, 0x08000, CRC(fb698a84) SHA1(57d8ff484691b0227034815bac0c4d99bae7d067) )
	ROM_LOAD( "u14_7558-01-r0_ds_a309.u14", 0x28000, 0x08000, CRC(25431b2b) SHA1(9ecd04b00d6531f41913f67fef848f2d1e6d7766) )
	ROM_LOAD( "u13_7558-01-r0_ds_a833.u13", 0x30000, 0x08000, CRC(55accddc) SHA1(33c845b3b730126a1e3e26483a05e2e186925199) )
	ROM_LOAD( "u12_7558-01-r0_ds_11ff.u12", 0x38000, 0x08000, CRC(9172a8a0) SHA1(b0ef6f8a706f48de9896929647ef30e3555c797b) )

	ROM_REGION( 0x30000, "gfx1", 0 )
	ROM_LOAD( "multi-action_7556-wv_u46.u46", 0x00000, 0x10000, CRC(32c11634) SHA1(26f3c5c220b45e8eedad940ff94dc5ef6f89e3fa) ) // also known to be labeled: U46  MLT8  cs:8bbe
	ROM_LOAD( "multi-action_7556-wv_u47.u47", 0x10000, 0x10000, CRC(5781bdd7) SHA1(e3f920dd1c247f92044100e28fc39d48b02b6a4b) ) // also known to be labeled: U47  MLT8  cs:0262
	ROM_LOAD( "multi-action_7556-wv_u48.u48", 0x20000, 0x10000, CRC(52ac8411) SHA1(9941388b90b6b91c1dab9286db588f0032620ea4) ) // also known to be labeled: U48  MLT8  cs:9daa

	ROM_REGION( 0x2000, "nvram", 0 )
	ROM_LOAD( "dallas_ds1225y-200.u7",  0x0000, 0x2000, CRC(142c5cea) SHA1(39d787109e0b782fda5a18ff3a56cf8428cb2437) )

	ROM_REGION( 0x8000, "nvram2", 0 )
	ROM_LOAD( "dallas_ds1230y-200.u17", 0x0000, 0x8000, CRC(9d196d52) SHA1(21fd5acd7652ba10ae6b4ae520abcc7c34eb37d1) )
ROM_END

} // anonymous namespace

// CRT-300 games
GAME( 1989, ma6710, 0, merit300, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 6710-13", MACHINE_IS_SKELETON )

// CRT-350 games
GAME( 199?, ma7551t, 0, merit350, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 7551-20-R3T", MACHINE_IS_SKELETON )
GAME( 199?, ma7551p, 0, merit350, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 7551-21-R2P", MACHINE_IS_SKELETON )
GAME( 199?, ma7556,  0, merit350, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 7556-00-R2",  MACHINE_IS_SKELETON )
GAME( 199?, ma7558,  0, merit350, merit3xx, merit3xx_state, empty_init, ROT0, "Merit", "Multi-Action 7558-01-R0",  MACHINE_IS_SKELETON )
