// license:BSD-3-Clause
// copyright-holders:
/*********************************************************************************************************************************************

  Skeleton driver for Cosmos PLAYC8F/PLAYC8E hardware based electromechanical machines.

  Known machines on this hardware:

  Dumped  Manufacturer                                                     Machine             Notes
  ------  ---------------------------------------------------------------  ------------------  -----------------------------------------------
  Yes     Benchmark Games, Inc.                                            Basketball          PLAYC8F + SY-MUSIC01 (music)
  Yes     Benchmark Games, Inc.                                            Ten Strike Classic  PLAYC8F (mechanical sound, no sound/music PCBs)
  No      Universal Space Amusement (HK) Co., Ltd                          Mr. Wolf            PLAYC8F
  No      HomingGame                                                       Gold Fort           PLAYC8F + SOUND03H (music, STC11F08XE-based)
  No      Family Fun Companies, Inc.                                       Basket Fortune      PLAYC8F + SY-MUSIC01 (music)
  No      Family Fun Companies, Inc.                                       Pharaoh’s Treasure  PLAYC8F + SY-MUSIC01 (music) + SC52 (sound)
  No      Family Fun Companies, Inc.                                       Football Fortune    PLAYC8F + SY-MUSIC01 (music)
  No      Universal Space Amusement Equipment Ltd.                         Bed Monsters        PLAYC8F + SY-MUSIC01 (music)
  No      Universal Space Amusement Equipment Ltd.                         Colorama 4 Player   PLAYC8F + SY-MUSIC01 (music)
  No      Universal Space Amusement Equipment Ltd. / Bromley Incorporated  Ghost Hunter        PLAYC8E + SY-MUSIC01 (music)

***********************************************************************************************************************************************/

#include "emu.h"
#include "cpu/m6805/m68hc05.h"
#include "cpu/mcs51/mcs51.h"
#include "sound/okim9810.h"
#include "speaker.h"

namespace {

class cosmos_playc8f_state : public driver_device
{
public:
	cosmos_playc8f_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_soundcpu(*this, "soundcpu")
	{
	}

	void playc8f(machine_config &config);
	void playc8f_symusic01(machine_config &config);

private:
	required_device<m68hc05_device> m_maincpu;
	optional_device<mcs51_cpu_device> m_soundcpu;
};

static INPUT_PORTS_START(playc8f)
INPUT_PORTS_END

// PLAYC8F + SY-MUSIC01 (music)
void cosmos_playc8f_state::playc8f_symusic01(machine_config &config)
{
	M68HC705C8A(config, m_maincpu, 4_MHz_XTAL);

	I80C51(config, m_soundcpu, 6_MHz_XTAL); // Actually an AT89C51

	SPEAKER(config, "mono").front_center();

	okim9810_device &oki(OKIM9810(config, "oki", 4.096_MHz_XTAL)); // Actually an OKI M9811
	oki.add_route(ALL_OUTPUTS, "mono", 0.50);
}

// PLAYC8F (without sound hardware)
void cosmos_playc8f_state::playc8f(machine_config &config)
{
	M68HC705C8A(config, m_maincpu, 4_MHz_XTAL);
}

ROM_START(bgbasketb)
	ROM_REGION(0x002000, "maincpu", 0)
	ROM_LOAD("srlqj02-2-01_2013-03-23.u13", 0x000000, 0x002000, CRC(9ab77de9) SHA1(c3507636dfe963023d5894403b850b65da2613b4)) // MC705C8ACPE

	ROM_REGION(0x001000, "soundcpu", 0)
	ROM_LOAD("mglqj-y-3-3_2004-10-22.u2",   0x000000, 0x001000, NO_DUMP) // AT89C51, protected

	ROM_REGION(0x200000, "samples", 0)
	ROM_LOAD("mglqj-y-1-3_2004-10-22.rom1", 0x000000, 0x100000, CRC(ca8ac968) SHA1(c974e16d86f0d5fbc5fe663aa81dff82b8678813)) // 27C801
	ROM_LOAD("mglqj-y-2-3_2004-10-22.rom2", 0x100000, 0x100000, CRC(b4987ea2) SHA1(77f42740bea08a72fc1a201a0ed7946c380141cf)) // 27C801
ROM_END

ROM_START(bgbasketbb)
	ROM_REGION(0x002000, "maincpu", 0)
	ROM_LOAD("basketball_2004-01-22.u13",   0x000000, 0x002000, CRC(5dd80da6) SHA1(5671d7166c7131ce0ad415da16842f271cc7ac3f)) // MC68HC705C8P

	ROM_REGION(0x001000, "soundcpu", 0)
	ROM_LOAD("mglqj-y-3-3_2004-10-22.u2",   0x000000, 0x001000, NO_DUMP) // AT89C51, protected

	ROM_REGION(0x200000, "samples", 0)
	ROM_LOAD("mglqj-y-1-3_2004-10-22.rom1", 0x000000, 0x100000, CRC(ca8ac968) SHA1(c974e16d86f0d5fbc5fe663aa81dff82b8678813)) // 27C801
	ROM_LOAD("mglqj-y-2-3_2004-10-22.rom2", 0x100000, 0x100000, CRC(b4987ea2) SHA1(77f42740bea08a72fc1a201a0ed7946c380141cf)) // 27C801
ROM_END

// This is a reissue and copy of the 1957 Williams Ten Strike
ROM_START(tenstrikc)
	ROM_REGION(0x002000, "maincpu", 0)
	ROM_LOAD("gdblq_2003-5-17.u13",         0x000000, 0x002000, CRC(dc345c3b) SHA1(50edbecf20e038f35556d315447a9d370d125ea7)) // MC68HC705C8A

	// No sound hardware (just mechanical sounds triggered by relays)
ROM_END

} // anonymous namespace

//   YEAR  NAME        PARENT     MACHINE            INPUT    CLASS                 INIT        ROT   COMPANY                  FULLNAME                               FLAGS
GAME(2013, bgbasketb,  0,         playc8f_symusic01, playc8f, cosmos_playc8f_state, empty_init, ROT0, "Benchmark Games, Inc.", "Basketball (Benchmark Games, set 1)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(2004, bgbasketbb, bgbasketb, playc8f_symusic01, playc8f, cosmos_playc8f_state, empty_init, ROT0, "Benchmark Games, Inc.", "Basketball (Benchmark Games, set 2)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(2003, tenstrikc,  0,         playc8f,           playc8f, cosmos_playc8f_state, empty_init, ROT0, "Benchmark Games, Inc.", "Ten Strike Classic",                  MACHINE_IS_SKELETON_MECHANICAL)
