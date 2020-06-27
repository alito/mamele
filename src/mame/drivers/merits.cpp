// license:BSD-3-Clause
// copyright-holders:
/*******************************************************************************

    Skeleton driver for Merit Scorpion darts machines

    Hardware overview:
    Main CPU: Dallas DS80C3202-UM or compatible
    Sound: DAC?
    NVRAM: Dallas DS1220Y-120 or compatible
    Other: Dallas DS1232 MicroMonitor
           Dallas DS1204U-3 Electronic Key (not populated)
    OSCs: 12.000 MHz, 3.2768 MHz
    Dips: 2 x 8 dips banks

*******************************************************************************/

#include "emu.h"
#include "cpu/mcs51/mcs51.h"
#include "machine/nvram.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "speaker.h"


class merits_state : public driver_device
{
public:
	merits_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{
	}

	void scrpiond(machine_config &config);

private:
	void mem_map(address_map &map);
	void io_map(address_map &map);

	required_device<cpu_device> m_maincpu;
};

void merits_state::mem_map(address_map &map)
{
	map(0x0000, 0xffff).rom().region("maincpu", 0);
}

void merits_state::io_map(address_map &map)
{
	map(0x8000, 0x87ff).ram().share("nvram");
	//map(0x9000, 0x9000).r();
	//map(0xa000, 0xa000).r();
	//map(0xc000, 0xc000).w();
	//map(0xd000, 0xd000).w();
	//map(0xe000, 0xe000).w();
	//map(0xf000, 0xf000).w();
	//map(0xf800, 0xf800).w();
}

static INPUT_PORTS_START(scrpiond)
INPUT_PORTS_END

void merits_state::scrpiond(machine_config &config)
{
	DS80C320(config, m_maincpu, 12_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &merits_state::mem_map);
	m_maincpu->set_addrmap(AS_IO, &merits_state::io_map);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0); // DS1220Y
}

ROM_START(scrpiond)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "27c512.u7", 0x00000, 0x10000, CRC(06cdf965) SHA1(4cdac131063fc0dd954eaaee2ae40d5731f83469) )
ROM_END

ROM_START(scrpionda)
	ROM_REGION(0x10000, "maincpu", 0)
	ROM_LOAD( "4978-22_u7-r5_c1997_mii.u7", 0x00000, 0x10000, CRC(e647a17e) SHA1(4a7b9e2af3656a1b6f4ffd8c17b68eec5c534776) )
ROM_END


GAME(1999, scrpiond,         0, scrpiond, scrpiond, merits_state, empty_init, ROT0, "Merit", "Scorpion (Jun 15, 1999)", MACHINE_IS_SKELETON_MECHANICAL)
GAME(1997, scrpionda, scrpiond, scrpiond, scrpiond, merits_state, empty_init, ROT0, "Merit", "Scorpion (Oct 01, 1997)", MACHINE_IS_SKELETON_MECHANICAL)
