// license:BSD-3-Clause
// copyright-holders:AJR
/****************************************************************************

    Skeleton driver for Roland JV-80 & JV-880 synthesizers.

****************************************************************************/

#include "emu.h"
#include "cpu/h8500/h8532.h"
//#include "machine/nvram.h"

class roland_jv80_state : public driver_device
{
public:
	roland_jv80_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{
	}

	void jv880(machine_config &config);

private:
	void mem_map(address_map &map);

	required_device<h8532_device> m_maincpu;
};


void roland_jv80_state::mem_map(address_map &map)
{
	map(0x08000, 0x3ffff).rom().region("progrom", 0x8000);
}

static INPUT_PORTS_START(jv880)
INPUT_PORTS_END

void roland_jv80_state::jv880(machine_config &config)
{
	HD6435328(config, m_maincpu, 20_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &roland_jv80_state::mem_map);

	//NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0); // LC36256AML-10 (IC18) + CR2032 battery

	//TC6116(config, "pcm", 23.2_MHz_XTAL);
}

ROM_START(jv880)
	ROM_REGION(0x8000, "maincpu", ROMREGION_ERASE00)
	ROM_LOAD("roland_r15199810_6435328b86f.ic16", 0x0000, 0x8000, NO_DUMP)

	ROM_REGION(0x40000, "progrom", 0)
	ROM_LOAD("roland_jv-880_v1.01.ic17", 0x00000, 0x40000, CRC(5f19c95f) SHA1(38ec496f16dfa02d35f934cf32d8302aaf5f236e))

	ROM_REGION(0x400000, "waverom", 0)
	ROM_LOAD("roland-a_r15209312_lh5375n2.ic27", 0x000000, 0x200000, NO_DUMP)
	ROM_LOAD("roland-b_r15209313_lh5375n3.ic25", 0x200000, 0x200000, NO_DUMP)
ROM_END

//SYST(1992, jv80, 0, 0, jv80, jv80, roland_jv80_state, empty_init, "Roland", "JV-80 Multi Timbral Synthesizer", MACHINE_IS_SKELETON)
SYST(1992, jv880, 0, 0, jv880, jv880, roland_jv80_state, empty_init, "Roland", "JV-880 Multi Timbral Synthesizer Module", MACHINE_IS_SKELETON)
