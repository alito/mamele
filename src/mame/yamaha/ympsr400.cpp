// license:BSD-3-Clause
// copyright-holders:AJR
/*******************************************************************************

    Skeleton driver for Yamaha PSR-400 & PSR-500 PortaTone keyboards.

*******************************************************************************/

#include "emu.h"
#include "cpu/m6805/m6805.h"
#include "cpu/mn1880/mn1880.h"
#include "sound/multipcm.h"
#include "speaker.h"

namespace {

class psr400_state : public driver_device
{
public:
	psr400_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_mpscpu(*this, "mpscpu")
	{
	}

	void psr500(machine_config &config);

protected:
	virtual void driver_start() override;

private:
	void program_map(address_map &map);
	void data_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_device<cpu_device> m_mpscpu;
};

void psr400_state::program_map(address_map &map)
{
	// 2 MB external program memory space using MMU
	map(0x000000, 0x1fffff).rom().region("program", 0);
}

void psr400_state::data_map(address_map &map)
{
	// 2 MB external data memory space using MMU
	map(0x000080, 0x03ffff).mirror(0xc0000).ram(); // 2x 1M-bit PSRAM (only one on PSR-400)
	map(0x100000, 0x10000f).mirror(0xffff0).rw("gew8", FUNC(multipcm_device::read), FUNC(multipcm_device::write));
}

static INPUT_PORTS_START(psr500)
INPUT_PORTS_END

void psr400_state::psr500(machine_config &config)
{
	MN18801A(config, m_maincpu, 10_MHz_XTAL); // MN18801A (also has 500 kHz secondary resonator)
	m_maincpu->set_addrmap(AS_PROGRAM, &psr400_state::program_map);
	m_maincpu->set_addrmap(AS_DATA, &psr400_state::data_map);

	HD63705(config, m_mpscpu, 8_MHz_XTAL).set_disable(); // HD63B05V0D73P (mislabeled HD63B50 on schematic)

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	multipcm_device &gew8(MULTIPCM(config, "gew8", 9.4_MHz_XTAL)); // YMW-258-F
	gew8.add_route(1, "lspeaker", 1.0);
	gew8.add_route(0, "rspeaker", 1.0);

	//YM3413(config, "ldsp"); // PSR-500 only (has its own 256K-bit PSRAM)
}

ROM_START(psr500)
	ROM_REGION(0x200000, "program", 0)
	ROM_LOAD("xj920c0.ic4", 0x000000, 0x040000, CRC(bd45d962) SHA1(fe46ceae5584b56e36f31f27bedd9e7d578eb35b))
	ROM_RELOAD(0x040000, 0x040000)
	ROM_RELOAD(0x080000, 0x040000)
	ROM_RELOAD(0x0c0000, 0x040000)
	ROM_LOAD("xj921b0.ic5", 0x100000, 0x100000, CRC(dd1a8afc) SHA1(5d5b47577faeed165f0bd73283f148d112e4d1e9))

	ROM_REGION(0x100000, "gew8", 0)
	ROM_LOAD("xj426b0.ic3", 0x000000, 0x100000, CRC(ef566734) SHA1(864f5689dbaa82bd8a1be4e53bdb21ec71be03cc))

	ROM_REGION(0x1000, "mpscpu", 0)
	ROM_LOAD("xj450a00.ic1", 0x0000, 0x1000, NO_DUMP)
ROM_END

void psr400_state::driver_start()
{
	memory_region *region = memregion("program");
	u8 *program = static_cast<u8 *>(region->base());
	std::vector<u8> buf(0x20000);

	// A8-A14 & A16 are scrambled
	for (u32 offset = 0; offset < region->bytes(); )
	{
		std::copy(&program[offset], &program[offset + 0x20000], buf.begin());
		for (int blocks = 0x200; blocks-- > 0; offset += 0x100)
			std::copy_n(&buf[bitswap<9>(offset, 9, 15, 10, 11, 8, 14, 16, 13, 12) << 8], 0x100, &program[offset]);
	}
}

} // anonymous namespace

SYST(1991, psr500, 0, 0, psr500, psr500, psr400_state, empty_init, "Yamaha", "PSR-500", MACHINE_NO_SOUND | MACHINE_NOT_WORKING)
