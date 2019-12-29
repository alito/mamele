// license:BSD-3-Clause
// copyright-holders:

/*
    Raiden (Modular System)

    As with most of the 'Modular System' setups, the hardware is heavily modified from the original
    and consists of a multi-board stack in a cage, hence different driver.

    For this game the Modular System cage contains 8 main boards and 1 sub board.

    Unmarked red board - 2 V30s, unpopulated M68000 socket + 6 ROMs + RAMs.
    MOD-6/1 - RAMs, 20 MHz XTAL and unpopulated M68000 socket.
    MOD 21/1 - 24 MHz XTAL.
    MOD 1/5 - Sound board (Z80, 2xYM2203C). 2 8-dips banks + small sub board with OKI M5205.
    MOD 51/3 - Sprite board, has logic + 4 sprite ROMs.
    MOD 4/3 - Tilemap board, has logic + 4 tilemap ROMs, long thin sub-board (CAR-0484/1 SOLD) with no chips, just routing along one edge.
    MOD 4/3 - Tilemap board, has logic + 4 tilemap ROMs, long thin sub-board (CAR-0484/1 SOLD) with no chips, just routing along one edge.
    MOD 4/3 - Tilemap board, has logic + 4 tilemap ROMs, long thin sub-board (CAR-0484/1 SOLD) with no chips, just routing along one edge.

    PCBs pictures and dip listing are available at: http://www.recreativas.org/modular-system-raiden-4315-gaelco-sa
*/


#include "emu.h"
#include "cpu/nec/nec.h"
#include "cpu/z80/z80.h"
#include "machine/gen_latch.h"
#include "sound/2203intf.h"
#include "sound/msm5205.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"


class raiden_ms_state : public driver_device
{
public:
	raiden_ms_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_screen(*this, "screen")
	{ }

	void raidenm(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<screen_device> m_screen;

	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	void raidenm_map(address_map &map);
};


void raiden_ms_state::raidenm_map(address_map &map)
{
	map(0xa0000, 0xfffff).rom();
}


void raiden_ms_state::machine_start()
{
}


uint32_t raiden_ms_state::screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	return 0;
}

static INPUT_PORTS_START( raidenm )
INPUT_PORTS_END

static GFXDECODE_START( gfx_raiden_ms )
GFXDECODE_END

void raiden_ms_state::raidenm(machine_config &config)
{
	/* basic machine hardware */
	V30(config, m_maincpu, 20_MHz_XTAL / 2); // divisor unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &raiden_ms_state::raidenm_map);

	V30(config, "subcpu", 20_MHz_XTAL / 2).set_disable(); // divisor unknown

	Z80(config, "audiocpu", 24_MHz_XTAL / 8).set_disable(); // divisor unknown, no XTAL on the PCB, might also use the 20 MHz one

	/* video hardware */
	SCREEN(config, m_screen, SCREEN_TYPE_RASTER); // all wrong
	m_screen->set_refresh_hz(60);
	m_screen->set_vblank_time(ATTOSECONDS_IN_USEC(2500) /* not accurate */);
	m_screen->set_size(256, 256);
	m_screen->set_visarea(0, 256-1, 0, 256-32-1);
	m_screen->set_screen_update(FUNC(raiden_ms_state::screen_update));
	m_screen->set_palette("palette");

	PALETTE(config, "palette").set_format(palette_device::xBRG_444, 1024);

	GFXDECODE(config, "gfxdecode", "palette", gfx_raiden_ms);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	GENERIC_LATCH_8(config, "soundlatch");

	YM2203(config, "ym1", 24_MHz_XTAL / 8).add_route(ALL_OUTPUTS, "mono", 0.15); // divisor unknown, no XTAL on the PCB, might also use the 20 MHz one

	YM2203(config, "ym2", 24_MHz_XTAL / 8).add_route(ALL_OUTPUTS, "mono", 0.15); // divisor unknown, no XTAL on the PCB, might also use the 20 MHz one

	MSM5205(config, "msm", 24_MHz_XTAL / 8).add_route(ALL_OUTPUTS, "mono", 0.15); // divisor unknown, no XTAL on the PCB, might also use the 20 MHz one
}

ROM_START( raidenm )
	ROM_REGION( 0x100000, "maincpu", 0 ) // on red board
	ROM_LOAD16_BYTE( "msraid_6-1-8086-1_rd604.u7",   0x0a0000, 0x10000, CRC(a4b12785) SHA1(446314e82ce01315cb3e3d1f323eaa2ad6fb48dd) )
	ROM_LOAD16_BYTE( "msraid_6-1-8086-1_rd603.u8",   0x0a0001, 0x10000, CRC(17640bd5) SHA1(5bbc99900426b1a072b52537ae9a50220c378a0d) )
	ROM_LOAD16_BYTE( "msraid_6-1-8086-1_rd602b.u5",  0x0c0000, 0x20000, CRC(2af64e22) SHA1(4aef347ef6396c3e12fe27b82046ee0efb274602) )
	ROM_LOAD16_BYTE( "msraid_6-1-8086-1_rd601b.u6",  0x0c0001, 0x20000, CRC(bf6245e1) SHA1(8eccd42f1eef012a5c937d77d2f877de14dcf39c) )

	ROM_REGION( 0x100000, "subcpu", 0 ) // on red board
	ROM_LOAD16_BYTE( "msraid_6-1-8086-1_rd606b.u19", 0x0c0000, 0x20000, CRC(58eac0b6) SHA1(618813c7593d13271d2826739f24e08dda400b0d) )
	ROM_LOAD16_BYTE( "msraid_6-1-8086-1_rd605b.u18", 0x0c0001, 0x20000, CRC(251fef93) SHA1(818095a77cb94fd4acc9eb26954615ee93e8432c) )

	ROM_REGION( 0x10000, "audiocpu", 0 ) // on MOD 1/5 board
	ROM_LOAD( "msraid_1-5_rd101.ic12",  0x00000, 0x10000, CRC(2b76e371) SHA1(4c9732950f576e498d02fde485ba92fb293d5594) )

	// dumper's note: ROMs [rd4b1, rd4b2, rb4b3, rd4b4] and [rd4a1, rd4a2, rb4a3, rd4a4] have a strange setup
	// with pins 32, 31 and 31 soldered together and pin 2 connected between all four chips, while the sockets are for 28 pin chips
	// (with 27C512 silkscreened on the PCB behind the chips)
	ROM_REGION( 0x80000, "gfx1", 0 ) // on one of the MOD 4/3 boards
	ROM_LOAD( "msraid_4-3-1_rd4b1.ic17",  0x00000, 0x20000, CRC(ff35b830) SHA1(fb552b2aa50aed12c3adb6ef9032a438adf6f37f) )
	ROM_LOAD( "msraid_4-3-1_rd4b2.ic16",  0x20000, 0x20000, CRC(da0b2fca) SHA1(4ffe177587759ea03e73bcc5c36bedc869653ce5) )
	ROM_LOAD( "msraid_4-3-1_rd4b3.ic15",  0x40000, 0x20000, CRC(00e5953f) SHA1(488ac889a587bf4108be424faa9123abe73b5246) )
	ROM_LOAD( "msraid_4-3-1_rd4b4.ic14",  0x60000, 0x20000, CRC(932f8407) SHA1(0c7a0b18bbc3ac1f30bdb0ba4466c1e253130305) )

	ROM_REGION( 0x80000, "gfx2", 0 ) // on another MOD 4/3 board
	ROM_LOAD( "msraid_4-3-2_rd4a1.ic17",  0x00000, 0x20000, CRC(32892554) SHA1(8136626bf7073cdf19a8a38d19f6d0c52405df16) )
	ROM_LOAD( "msraid_4-3-2_rd4a2.ic16",  0x20000, 0x20000, CRC(cb325246) SHA1(f0a7bf8b1b5145541af78eba0375392a3030e2d9) )
	ROM_LOAD( "msraid_4-3-2_rd4a3.ic15",  0x40000, 0x20000, CRC(7554acef) SHA1(ec418ef2246c889d0e308925e11b1d3328bd83f9) )
	ROM_LOAD( "msraid_4-3-2_rd4a4.ic14",  0x60000, 0x20000, CRC(1e1537a5) SHA1(3f17a85c185dd8be7f7b3a5adb28d508f9f059a5) )

	ROM_REGION( 0x20000, "gfx3", 0 ) // on a third MOD 4/3 board, all 1st and 2nd half identical
	ROM_LOAD( "msraid_4-3-3_rd404.ic17",  0x00000, 0x08000, CRC(caec39f5) SHA1(c61bd1f02515c6597d276dfcb21ed0969b556b8b) )
	ROM_LOAD( "msraid_4-3-3_rd403.ic16",  0x08000, 0x08000, CRC(0e817530) SHA1(efbc05d31ab38d0213387f4e61977f5203e19ace) )
	ROM_LOAD( "msraid_4-3-3_rd402.ic15",  0x10000, 0x08000, CRC(55dd887b) SHA1(efa687afe0b19fc741626b8fe5cd6ab541874396) )
	ROM_LOAD( "msraid_4-3-3_rd401.ic14",  0x18000, 0x08000, CRC(da82ab5d) SHA1(462db31a3cc1494fdc163d5abdf6d74a182a1421) )

	ROM_REGION( 0x80000, "gfx4", 0 ) // on MOD 51/3 board
	ROM_LOAD( "msraid_51-3_rd501.ic43",   0x00000, 0x20000, CRC(fcd1fc21) SHA1(465036348f19dd3ff999c12e6591b172a7fb621e) )
	ROM_LOAD( "msraid_51-3_rd502.ic42",   0x20000, 0x20000, CRC(faba2544) SHA1(1d32b370f43bf6f7cb3bbe052775e52403a7ccb6) )
	ROM_LOAD( "msraid_51-3_rd503.ic41",   0x40000, 0x20000, CRC(ae4001e9) SHA1(5f21a042cad1807d2ef5e7f4f2cfd86cadc0503b) )
	ROM_LOAD( "msraid_51-3_rd504.ic40",   0x60000, 0x20000, CRC(0452eb10) SHA1(3b998da404bd7133d12aadcadd57ee21a0cfc226) )

	ROM_REGION( 0x0700, "proms", 0 )    /* PROMs (function unknown) */
	ROM_LOAD( "msraid_1-5_110_82s123.ic20",      0x0000, 0x0020, CRC(e26e680a) SHA1(9bbe30e98e952a6113c64e1171330153ddf22ce7) )
	ROM_LOAD( "msraid_21-1_211_82s129.ic4",      0x0100, 0x0100, CRC(4f8c3e63) SHA1(0aa68fa1de6ca945027366a06752e834bbbc8d09) )
	ROM_LOAD( "msraid_21-1_p0202_82s129.ic12",   0x0200, 0x0100, CRC(e434128a) SHA1(ef0f6d8daef8b25211095577a182cdf120a272c1) )
	ROM_LOAD( "msraid_51-3_502_82129.ic10",      0x0600, 0x0100, CRC(15085e44) SHA1(646e7100fcb112594023cf02be036bd3d42cc13c) )

	ROM_REGION( 0x1000, "plds", ROMREGION_ERASEFF )
	ROM_LOAD( "msraid_4-3-1_pc403_pal16r8.ic29",    0x000, 0x104, CRC(506156cc) SHA1(5560671fc2c9872ed28620491af5dc486909fc6e) )
	ROM_LOAD( "msraid_4-3-2_403_pal16r8.ic29",      0x000, 0x104, CRC(16379b0d) SHA1(5379560b0ec7c67cbe131a581a347b86395f34ac) )
	ROM_LOAD( "msraid_4-3-3_p0403_pal16r8.ic29",    0x000, 0x104, CRC(506156cc) SHA1(5560671fc2c9872ed28620491af5dc486909fc6e) ) // yes, same as the first one
	ROM_LOAD( "msraid_51-3_503_gal16v8.ic46",       0x000, 0x117, CRC(11470ea1) SHA1(cfcafbcc7e55be717348f895df61e144fdd0cc9b) )
	ROM_LOAD( "msraid_6-1_645b_gal16v8a.ic7",       0x000, 0x117, NO_DUMP )
	ROM_LOAD( "msraid_6-1_686_ga16v8.ic13",         0x000, 0x117, NO_DUMP )
	ROM_LOAD( "msraid_6-1-8086-1_645c_gal16v8.u33", 0x000, 0x117, NO_DUMP )
	ROM_LOAD( "msraid_6-1-8086-1_645d_gal16v8.u27", 0x000, 0x117, NO_DUMP )
ROM_END

GAME( 199?, raidenm,  raiden,  raidenm,  raidenm,  raiden_ms_state, empty_init, ROT0, "bootleg (Gaelco / Ervisa)", "Raiden (Modular System)", MACHINE_IS_SKELETON )
