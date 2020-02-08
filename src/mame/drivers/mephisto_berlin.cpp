// license:BSD-3-Clause
// copyright-holders:Sandro Ronco
/******************************************************************************

Mephisto Berlin 68000 / Berlin Professional 68020
Berlin Professional has the same engine as Mephisto Genius.

TODO:
- does it have ROM waitstates like mephisto_modular?

Undocumented buttons:
- holding ENTER and LEFT cursor on boot runs diagnostics
- holding CLEAR on boot clears battery backed RAM

******************************************************************************/

#include "emu.h"

#include "cpu/m68000/m68000.h"
#include "machine/bankdev.h"
#include "machine/nvram.h"
#include "machine/mmboard.h"
#include "video/mmdisplay2.h"

// internal artwork
#include "mephisto_berlin.lh"


class berlin_state : public driver_device
{
public:
	berlin_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_board(*this, "board")
		, m_keys(*this, "KEY")
	{ }

	void berlin(machine_config &config);
	void berlinp(machine_config &config);

private:
	DECLARE_READ8_MEMBER(input_r);

	void berlin_mem(address_map &map);
	void berlinp_mem(address_map &map);
	void nvram_map(address_map &map);

	required_device<cpu_device> m_maincpu;
	required_device<mephisto_board_device> m_board;
	required_ioport m_keys;
};



/******************************************************************************
    I/O
******************************************************************************/

READ8_MEMBER(berlin_state::input_r)
{
	if (m_board->mux_r(space, offset) == 0xff)
		return m_keys->read();
	else
		return m_board->input_r(space, offset) ^ 0xff;
}



/******************************************************************************
    Address Maps
******************************************************************************/

void berlin_state::nvram_map(address_map &map)
{
	// nvram is 8-bit (8KB)
	map(0x0000, 0x1fff).ram().share("nvram");
}

void berlin_state::berlin_mem(address_map &map)
{
	map(0x000000, 0x01ffff).rom();
	map(0x800000, 0x87ffff).ram();
	map(0x900000, 0x903fff).m("nvram_map", FUNC(address_map_bank_device::amap8)).umask16(0xff00);
	map(0xa00000, 0xa00000).r(FUNC(berlin_state::input_r));
	map(0xb00000, 0xb00000).w(m_board, FUNC(mephisto_board_device::mux_w));
	map(0xc00000, 0xc00000).w("display", FUNC(mephisto_display_module2_device::latch_w));
	map(0xd00008, 0xd00008).w("display", FUNC(mephisto_display_module2_device::io_w));
	map(0xe00000, 0xe00000).w(m_board, FUNC(mephisto_board_device::led_w));
}

void berlin_state::berlinp_mem(address_map &map)
{
	map(0x000000, 0x03ffff).rom();
	map(0x400000, 0x4fffff).ram();
	map(0x800000, 0x800000).r(FUNC(berlin_state::input_r));
	map(0x900000, 0x900000).w(m_board, FUNC(mephisto_board_device::mux_w));
	map(0xa00000, 0xa00000).w(m_board, FUNC(mephisto_board_device::led_w));
	map(0xb00000, 0xb00000).w("display", FUNC(mephisto_display_module2_device::io_w));
	map(0xc00000, 0xc00000).w("display", FUNC(mephisto_display_module2_device::latch_w));
	map(0xd00000, 0xd07fff).m("nvram_map", FUNC(address_map_bank_device::amap8)).umask32(0xff000000);
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( berlin )
	PORT_START("KEY")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("ENTER") PORT_CODE(KEYCODE_ENTER)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CLEAR") PORT_CODE(KEYCODE_BACKSPACE) PORT_CODE(KEYCODE_DEL)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("UP")    PORT_CODE(KEYCODE_UP)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("DOWN")  PORT_CODE(KEYCODE_DOWN)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("LEFT")  PORT_CODE(KEYCODE_LEFT)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RIGHT") PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("NEW GAME (1/2)") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_F1)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("NEW GAME (2/2)") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_F1)
INPUT_PORTS_END



/******************************************************************************
    Machine Configs
******************************************************************************/

void berlin_state::berlin(machine_config &config)
{
	/* basic machine hardware */
	M68000(config, m_maincpu, 12_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &berlin_state::berlin_mem);
	m_maincpu->set_periodic_int(FUNC(berlin_state::irq2_line_hold), attotime::from_hz(750));

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);
	ADDRESS_MAP_BANK(config, "nvram_map").set_map(&berlin_state::nvram_map).set_options(ENDIANNESS_BIG, 8, 13);

	MEPHISTO_BUTTONS_BOARD(config, m_board);
	MEPHISTO_DISPLAY_MODULE2(config, "display");
	config.set_default_layout(layout_mephisto_berlin);
}

void berlin_state::berlinp(machine_config &config)
{
	berlin(config);

	/* basic machine hardware */
	M68EC020(config.replace(), m_maincpu, 24.576_MHz_XTAL); // M68EC020RP25
	m_maincpu->set_addrmap(AS_PROGRAM, &berlin_state::berlinp_mem);

	const attotime irq_period = attotime::from_hz(24.576_MHz_XTAL / 0x8000); // 750Hz
	m_maincpu->set_periodic_int(FUNC(berlin_state::irq2_line_hold), irq_period);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( berl16 )
	ROM_REGION16_BE( 0x20000, "maincpu", 0 )
	ROM_SYSTEM_BIOS( 0, "v003", "V0.03" ) // B003 8C60 CA47
	ROMX_LOAD("berlin_68000_even.bin",     0x00000, 0x10000, CRC(31337f15) SHA1(0dcacb153a6f8376e6f1c2f3e57e60aad4370740), ROM_SKIP(1) | ROM_BIOS(0) )
	ROMX_LOAD("berlin_68000_odd_b003.bin", 0x00001, 0x10000, CRC(cc146819) SHA1(e4b2c6e496eff4a657a0718be292f563fb4e5688), ROM_SKIP(1) | ROM_BIOS(0) )
	ROM_SYSTEM_BIOS( 1, "v002", "V0.02" ) // B002 8C59 CA47
	ROMX_LOAD("berlin_68000_even.bin",     0x00000, 0x10000, CRC(31337f15) SHA1(0dcacb153a6f8376e6f1c2f3e57e60aad4370740), ROM_SKIP(1) | ROM_BIOS(1) )
	ROMX_LOAD("berlin_68000_odd_b002.bin", 0x00001, 0x10000, CRC(513a95f2) SHA1(cbaef0078a119163577e76a78b2110939b17be6b), ROM_SKIP(1) | ROM_BIOS(1) )
ROM_END

ROM_START( berlinp ) // B400 8AA1 E785
	ROM_REGION32_BE( 0x40000, "maincpu", 0 )
	ROM_LOAD("berlin_020_v400.u2", 0x00000, 0x40000, CRC(82fbaf6e) SHA1(729b7cef3dfaecc4594a6178fc4ba6015afa6202) )
ROM_END

ROM_START( berl16l ) // B500 ABD5 CA47
	ROM_REGION16_BE( 0x20000, "maincpu", 0 )
	ROM_LOAD16_BYTE("berlin_68000_london_even.bin", 0x00000, 0x10000, CRC(0ccddbc6) SHA1(90effdc9f2811a24d450b74ccfb24995ce896b86) )
	ROM_LOAD16_BYTE("berlin_68000_london_odd.bin",  0x00001, 0x10000, CRC(5edac658) SHA1(18ebebc5ceffd9a01798d8a3709875120bd096f7) )
ROM_END

ROM_START( berlinpl ) // B500 53CA 3DCE
	ROM_REGION32_BE( 0x40000, "maincpu", 0 )
	ROM_LOAD("berlin_020_london.u2", 0x00000, 0x40000, CRC(d75e170f) SHA1(ac0ebdaa114abd4fef87361a03df56928768b1ae) )
ROM_END



/******************************************************************************
    Drivers
******************************************************************************/

/*    YEAR  NAME      PARENT   COMPAT  MACHINE  INPUT   CLASS         INIT        COMPANY             FULLNAME                                  FLAGS */
CONS( 1992, berl16,   0,       0,      berlin,  berlin, berlin_state, empty_init, "Hegener + Glaser", "Mephisto Berlin 68000",                  MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1994, berlinp,  0,       0,      berlinp, berlin, berlin_state, empty_init, "Hegener + Glaser", "Mephisto Berlin Professional 68020",     MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1996, berl16l,  berl16,  0,      berlin,  berlin, berlin_state, empty_init, "Richard Lang",     "Mephisto Berlin 68000 (London upgrade)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1996, berlinpl, berlinp, 0,      berlinp, berlin, berlin_state, empty_init, "Richard Lang",     "Mephisto Berlin Professional 68020 (London upgrade)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
