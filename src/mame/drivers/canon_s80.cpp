// license:GPL-2.0+
// copyright-holders:FelipeSanches
/*
 * canon_s80.cpp
 *
 *    CANON S-80 electronic typewriter
 *
 * skeleton driver by:
 *    Felipe Correa da Silva Sanches <juca@members.fsf.org>
 *
 * known issues:
 *  - memory-map is uncertain
 *  - maincpu clock was only verified for canonts3, but it's probably the same for canons80
 *  - still lacks description of the keyboard inputs
 *  - as well as a "paper" device to plot the output of the dot matrix print head

*************************************************************************************
Canon Typestar 3 Electronic Typewriter, Canon 1989/1990.
Hardware info by Guru
Last updated: 21st May 2021
---------------------------

This is an electronic typewriter made by Canon.
It uses a thermal printing process with ribbon cartridges containing a coil
of plastic tape coated with black 'ink' that is fused onto the paper via
heat from the thermal print-head.
The LCD has an alpha-numeric 15 character display with full editing capability and
some other areas that show fixed functions and operation modes.
It appears to be functionally identical to the Canon S80 Electronic Typewriter.

PCB Layout
----------

CANON NH1-0275-04
|------------------------------------------|
|CN1                 CN2              CN6  |
|            PIEZO       VR2  FUSE(3A)   DC|
|  T1719A                                  |
| CN3     CN4    M5233  M54523P         SW1|
|         CN5                              |
|                         LC3518           |
| MB64H192   HD63A01X0P   NH4-0268.IC6  VR1|
|           6MHz                           |
|------------------------------------------|
Notes:
         CN1 - Head left-travel end-of-stroke switch connector (2 wires)
               On power-up the head moves left to touch this switch then moves right about 1 inch then stops.
         CN2 - Multi-pin flat cable for carriage power and data
         CN3 - Multi-pin flat cable for LCD
     CN4/CN5 - Multi-pin flat cables for keyboard
         CN6 - Connector for battery power input (6V via 4x 1.5V D-Cells). There are 3 wires so
               the CPU can detect when the battery is low and give the user a warning about it.
         VR1 - Print darkness adjustment pot
         VR2 - Piezo volume adjustment pot
       PIEZO - Piezo speaker/beeper
      LC3518 - Sanyo LC3518BL-15 2kb x8-bit SRAM, equivalent to 6116
NH4-0268.IC6 - Toshiba TC53257P 32kb x8-bit mask ROM marked 'NH4-0268' at location IC6
               The ROM supports all characters in English, German, French, Spanish and Italian languages.
  HD63A01X0P - Hitachi HD63A01X0P micro-controller with 4kb x8-bit internal mask ROM. Clock input 6MHz.
               HD63A01 has an on-chip divide-by-4 clock divider so the MCU runs at 1.5MHz internally.
               MCU is marked with a Canon part number 'NH4-0021' at location IC5.
               The internal ROM was trojan-dumped using the undocumented test mode.
      T1719A - Toshiba T1719A. Datasheet not available. This is most likely the keyboard and LCD
               controller and probably made exclusively for Canon.
    MB64H192 - Fujitsu MB64H192. Datasheet not available. This is most likely the carriage
               motor controller and probably made exclusively for Canon.
       M5233 - Mitsubishi M5233 Dual Comparitor, equivalent to LM4558
     M54523P - Mitsubishi M54523P 7-Unit 500mA Darlington Transistor Array with clamp diode
          DC - 6V DC power input 5.5mm barrel jack
         SW1 - Power on/off switch

Keyboard Layout
---------------

Keyboard is made by Matsushita
Part#: NS5-0594
|--------------------------------------------------------------------------------|
|MARGIN    KB                 |---------------------|           L/MARGIN R/MARGIN|
|RELEASE                      |         LCD         |    VR                      |
|          MODE               |                     |           TABSET    TABCLR |
|                             |---------------------|                            |
|     !     @     #     $     %     Yen   &     *     (     )     _ +   <-    -> |
|+-   1     2     3     4     5     6     7     8     9     0     - =            |
|                                                                    1/4         |
| TAB    Q     W     E     R     T     Y     U     I     O     P     1/2     BS  |
|                                                                                |
|                                                                :     "[        |
| LOCK     A     S     D     F     G     H     J     K     L     ;     ']        |
|                                                                          RETURN|
|                                                                  ?3            |
| SHIFT      Z     X     C     V     B     N     M     ,     .     /2   SHIFT    |
|                                                                                |
|        REPEAT                   SPACE                         RELOC        CODE|
|--------------------------------------------------------------------------------|
Notes:
      In the diagram above not all characters are shown due to being special or
      international characters that are not available on US keyboards, however every
      character used in the languages included in the ROM are available from this keyboard.

      There are several keys with up to 4 different characters on them. They are
      accessed by holding the key modifier SHIFT and KB keys.
      KB is used to select the characters on the left side of a key or the right side
      of a key. On the LCD fixed lower display, it will show I or II denoting either
      the left (I) or right (II) side of the key is active. When typing a key the lower
      character is normally active and the upper character is accessed by holding SHIFT.

      VR2 - LCD brightness adjustment pot

      MODE KEY FUNCTIONS (* = power-on default)
      ---------|---------------------------------------------------
    * MODE +   |    +-     Sets line spacing mode to 1
      MODE +   |    1      Sets line spacing mode to 1 1/2
      MODE +   |    2      Sets line spacing mode to 2
      MODE +   |    3      Sets typing mode to C (character by character printing like a mechanical typewriter, characters do not appear on the LCD)
    * MODE +   |    4      Sets typing mode to L (line printing, characters show on LCD and can be edited, then printed as one line by pressing RETURN)
    * MODE +   |    5      Sets printing mode to normal print
      MODE +   |    6      Sets printing mode to double-width print
      MODE +   |    7      Sets printing mode to underlined print
      MODE +   |    9      Sets manual carriage return mode
    * MODE +   |    0      Sets automatic carriage return mode
      MODE +   |    =      Sets justified text mode
      MODE +   |    Q      Moves the carriage while using the display for manual carriage positioning (for example typing in columns etc)
    * MODE +   |    <-     Sets typestyle mode to A (COURIER 10)
      MODE +   |    ->     Sets typestyle mode to B (CUBIC PS)
      MODE +   |    Z      Sets/cancels shading print mode. A 2nd menu will appear showing 4 shading modes that are selected by pressing numbers 1 - 4
      ---------|---------------------------------------------------


      CODE KEY FUNCTIONS
      ---------|---------------------------------------------------
      CODE +   |    1      Sets centering between margins
      CODE +   |    2      Sets centering between tabs
      CODE +   |    4      Executes right margin alignment
      CODE +   |    5      Sets/clears indent position
      CODE +   |    6      Executes decimal tab
      CODE +   |    0      Executes automatic paper feed (i.e. loads paper to a fixed start position automatically)
      CODE +   | TABSET    Sets decimal tab
      CODE +   |   TAB     Confirms decimal tab position
      CODE +   |TAB+TABCLR Clears specific decimal tab
      CODE +   | TABCLR    Clears all tabs and decimal tabs
      CODE +   |    =      Executes permanent hyphen
      CODE +   |    B      Executes permanent space
      CODE +   |  SPACE    Executes controlled carriage movement
      CODE +   | RETURN    Returns carriage without line feed
      CODE +   |    D      Prints a demo on the paper
      ---------|---------------------------------------------------

*************************************************************************************
 */

#include "emu.h"
#include "cpu/m6800/m6801.h"
#include "video/hd44780.h"
#include "emupal.h"
#include "screen.h"


namespace {

class canons80_state : public driver_device
{
public:
	canons80_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
	{ }

	void canons80(machine_config &config);

private:
	HD44780_PIXEL_UPDATE(pixel_update);

	void canons80_map(address_map &map);
};


HD44780_PIXEL_UPDATE(canons80_state::pixel_update)
{
	if (pos < 16)
		bitmap.pix(line * 8 + y, pos * 6 + x) = state;
}

void canons80_state::canons80_map(address_map &map)
{
	map(0x0000, 0x001f).m("maincpu", FUNC(hd6301x0_cpu_device::hd6301x_io));
	map(0x0040, 0x00ff).ram();
	map(0x0100, 0x07ff).ram();
	map(0x2000, 0x2001).rw("lcdc", FUNC(hd44780_device::read), FUNC(hd44780_device::write));
	map(0x4000, 0x7fff).rom().region("external", 0x4000);
	map(0x8000, 0xbfff).rom().region("external", 0);
	map(0xf000, 0xffff).rom().region("maincpu", 0);
}

void canons80_state::canons80(machine_config &config)
{
	// basic machine hardware
	hd6301x0_cpu_device &maincpu(HD6301X0(config, "maincpu", 6_MHz_XTAL)); // hd63a01xop
	maincpu.set_addrmap(AS_PROGRAM, &canons80_state::canons80_map);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_LCD));
	screen.set_refresh_hz(50);
	screen.set_screen_update("lcdc", FUNC(hd44780_device::screen_update));
	screen.set_size(16*6, 16);
	screen.set_visarea(0, 16*6-1, 0, 16-1);
	screen.set_palette("palette");

	hd44780_device &hd44780(HD44780(config, "lcdc"));
	hd44780.set_lcd_size(2, 16);
	hd44780.set_pixel_update_cb(FUNC(canons80_state::pixel_update));

	PALETTE(config, "palette").set_entries(2);
}


ROM_START( canons80 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD( "hd63a1x0p.bin", 0x0000, 0x1000, NO_DUMP )
	ROM_FILL( 0xfec, 1, 0xbf )
	ROM_FILL( 0xfed, 1, 0xf2 )
	ROM_FILL( 0xff4, 1, 0xbf )
	ROM_FILL( 0xff5, 1, 0xf5 )
	ROM_FILL( 0xffa, 1, 0xbf )
	ROM_FILL( 0xffb, 1, 0xf8 )
	ROM_FILL( 0xffe, 1, 0xbf )
	ROM_FILL( 0xfff, 1, 0xfb )

	ROM_REGION( 0x8000, "external", 0 )
	ROM_LOAD( "canon_8735kx_nh4-0029_064.ic6", 0x0000, 0x8000, CRC(b6cd2ff7) SHA1(e47a136300c826e480fac1be7fc090523078a2a6) )
ROM_END

ROM_START( canonts3 )
	ROM_REGION( 0x1000, "maincpu", 0 )
	// every 0x100 range was read via trojan 7 times, but reads weren't totally consistent. This was reconstructed by taking the most consistent reads for each byte.
	ROM_LOAD( "nh4-0021.ic5", 0x0000, 0x1000, BAD_DUMP CRC(b859b7d4) SHA1(3a5a80b1b8040fe0b13c0fb52b93f738a06eff16) )

	ROM_REGION( 0x8000, "external", 0 )
	ROM_LOAD( "nh4-0268.ic6", 0x0000, 0x8000, CRC(bbdd9f74) SHA1(347fa0d37f4df0c175ff1d7feb634f681739804f) )
ROM_END

} // Anonymous namespace


//    YEAR  NAME      PARENT  COMPAT  MACHINE   INPUT  CLASS           INIT        COMPANY  FULLNAME                                  FLAGS
COMP( 1988, canons80, 0,      0,      canons80, 0,     canons80_state, empty_init, "Canon", "Canon S-80 electronic typewriter",       MACHINE_NOT_WORKING | MACHINE_NO_SOUND)
COMP( 1988, canonts3, 0,      0,      canons80, 0,     canons80_state, empty_init, "Canon", "Typestar 3",                             MACHINE_NOT_WORKING | MACHINE_NO_SOUND)
