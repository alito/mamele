// license:BSD-3-Clause
// copyright-holders:
/***************************************************************************

 Skeleton driver for MCS51-based crane coinops from Compumatic.
 The same PCB was used on machines from different manufacturers, like OMVending
 and Covielsa.

 Display is just a four digits 7-segments (on a small PCB called "Plumadig").

 Different hardware revisions from Compumatic, called "GANCHONEW" PCB.
 From V2 to V8 hardware with the following layout (minor changes, like the power
 supply connector, moving from PC AT to PC ATX):

 COMPUMATIC "GANCHONEW V8" CPU
                               COUNTERS
  ______________________________________
 |     ______  ______  ______  ········ |
 |     ST8251  ST8251  ST8251           |
 | __________    ____                   |
 | ULN2803APG    LM358N                 |
 |                                      |
 | __________    __________     __ __  _|_
 | SN74HC2       SN74HC244N    | || | |   |
 |                             | || | | C |
 | __________    __________    |FUSES | O |
 | SN74HC737N    SN74HC244N    | || | | N |
 |                             |_||_| | N |
 | __________    __________           |___|
 | SN74HC737N    |_GAL16V8_|            |
 |                                    oo|
 | ________________    ____     ____  oo|
 || EPROM         | TL7705ACP         oo|
 ||_______________|    ____           oo|<- ATX Power
 |                    24C64           oo|   Supply conn
 | ___________________   Xtal         oo|
 || 80C32            |   12MHz  TEST  oo|
 ||__________________|           SW   oo|
 | ....  ...... ..  ..... .......       |
 |______________________________________|
  DISPLAY SENSOR SPK SELECT JOYSTICK
          +V RET

 The MCU on the older PCBs can differ between 80C32 compatible models
 (found with a Winbond W78C32C-40 and with a TS80C32X2-MCA).

 V2 uses a 24C16 SEEPROM, while later models use a 24C64.

 "GANCHONEW" V1 PCB has a different layout.

***************************************************************************/

#include "emu.h"
#include "cpu/mcs51/mcs51.h"
#include "speaker.h"

namespace
{

class compucranes_state : public driver_device
{
public:
	compucranes_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{
	}

	void ganchonew(machine_config &config);

protected:
	required_device<mcs51_cpu_device> m_maincpu;
};

INPUT_PORTS_START(ganchonew)
INPUT_PORTS_END

void compucranes_state::ganchonew(machine_config &config)
{
	I80C32(config, m_maincpu, 6_MHz_XTAL);

	SPEAKER(config, "mono").front_center();
}

// "GANCHONEW V8" PCB with ATX PSU connector
ROM_START(mastcrane)
	ROM_REGION(0x80000, "maincpu", 0)
	ROM_LOAD("v8.ic3",      0x00000, 0x20000, CRC(c47d11ad) SHA1(b0e784b7f68492f2872c06674f92d582def9cd26))

	ROM_REGION(0x00117, "pld", 0)
	ROM_LOAD("gal16v8.ic4", 0x00000, 0x00117, CRC(4d665a06) SHA1(504f0107482f636cd216579e982c6162c0b120a7)) // Verified to be the same on all known PCB revisions
ROM_END

// "GANCHONEW V7" PCB with AT PSU connector
ROM_START(mastcranea)
	ROM_REGION(0x80000, "maincpu", 0)
	ROM_LOAD("v7.ic3",      0x00000, 0x20000, CRC(c3a5a2fe) SHA1(1f89508420b8eb829081bdee17b3248030063d20))

	ROM_REGION(0x00117, "pld", 0)
	ROM_LOAD("atf16v8.ic4", 0x00000, 0x00117, CRC(4d665a06) SHA1(504f0107482f636cd216579e982c6162c0b120a7)) // Verified to be the same on all known PCB revisions
ROM_END

// "GANCHONEW V2" PCB with AT PSU connector
ROM_START(mastcraneb)
	ROM_REGION(0x80000, "maincpu", 0)
	ROM_LOAD("505.ic3",     0x00000, 0x20000, CRC(3dbb83f1) SHA1(3536762937332add0ca942283cc22ff301884a4a))

	ROM_REGION(0x00117, "pld", 0)
	ROM_LOAD("gal16v8.ic4", 0x00000, 0x00117, CRC(4d665a06) SHA1(504f0107482f636cd216579e982c6162c0b120a7)) // Verified to be the same on all known PCB revisions
ROM_END

// "GANCHONEW V2" PCB with AT PSU connector
ROM_START(octopussy)
	ROM_REGION(0x80000, "maincpu", 0)
	ROM_LOAD("w29c011.ic5", 0x00000, 0x20000, CRC(47da93e8) SHA1(aa821dd22c1912ec2942ca6afd989d61df4387d7))

	ROM_REGION(0x00117, "pld", 0)
	ROM_LOAD("atf16v8.ic4", 0x00000, 0x00117, CRC(4d665a06) SHA1(504f0107482f636cd216579e982c6162c0b120a7))
ROM_END

} // anonymous namespace

//    YEAR  NAME        PARENT     MACHINE    INPUT      CLASS              INIT        ROT   COMPANY       FULLNAME                FLAGS
GAME( 199?, mastcrane,  0,         ganchonew, ganchonew, compucranes_state, empty_init, ROT0, "Compumatic", "Master Crane (set 1)", MACHINE_IS_SKELETON_MECHANICAL )
GAME( 199?, mastcranea, mastcrane, ganchonew, ganchonew, compucranes_state, empty_init, ROT0, "Compumatic", "Master Crane (set 2)", MACHINE_IS_SKELETON_MECHANICAL )
GAME( 199?, mastcraneb, mastcrane, ganchonew, ganchonew, compucranes_state, empty_init, ROT0, "Compumatic", "Master Crane (set 3)", MACHINE_IS_SKELETON_MECHANICAL )
GAME( 1990, octopussy,  mastcrane, ganchonew, ganchonew, compucranes_state, empty_init, ROT0, "Covielsa",   "Octopussy",            MACHINE_IS_SKELETON_MECHANICAL )
