// license:BSD-3-Clause
// copyright-holders:Dirk Verwiebe, Cowering, hap
/******************************************************************************

Mephisto 4 + 5 Chess Computer
2007 Dirk V.

TODO:
- rebel5 unknown read from 0x4002, looks like leftover bookrom check
- need to emulate TurboKit properly, also for mm5p (it's not as simple as a CPU
  overclock plus ROM patch)

===============================================================================

Hardware notes:
- CPU: R65C02P3/R65C02P4 or G65SC02P-4
- Clock: 4.9152 MHz
- NMI CLK: 600 Hz
- IRQ Line is set to VSS
- 8 KByte SRAM Sony CXK5864-15L

1-CD74HC4060E: 14 Bit Counter
1-CD74HC166E
1-CD74HC251E
1-SN74HC138N TI
1-SN74HC139N TI
1-74HC14AP Toshiba
1-74HC02AP Toshiba
1-74HC00AP Toshiba
1-CD74HC259E

LCD module:
PCB label HGS 10 122 01
1-CD4011
4-CD4015

$0000-$1fff S-RAM
$2000 LCD 4 Byte Shift Register writeonly right to left
every 2nd char xor'd by $FF

2c00-2c07 Keyboard (8to1 Multiplexer) 74HCT251
2*8 Matrix
Adr. 0x3407
==0 !=0
2c00 CL E5
2c01 POS F6
2c02 MEMO G7
2c03 INFO A1
2c04 LEV H8
2c05 ENT B2
2c06 >0 C3
2c07 <9 D4

$3400-$3407 LED 1-6, Buzzer, Keyboard select

$2400 // Chess Board
$2800 // Chess Board
$3000 // Chess Board

$4000-7FFF Opening Modul HG550
$8000-$FFF ROM

===============================================================================

Mephisto 4 Turbo Kit 18mhz - (mm4tk)
    This is a replacement rom combining the turbo kit initial rom with the original MM IV.
    The Turbo Kit powers up to it's tiny rom, copies itself to ram, banks in normal rom,
    copies that to faster SRAM, then patches the checksum and the LED blink delays.
    If someone else wants to code up the power up banking, feel free

    There is an undumped MM V Turbo Kit, which will be the exact same except for location of
    the patches. The mm5tk just needs the normal mm5 ROM swapped out for that one to
    blinks the LEDs a little slower.

    -- Cowering (2011)

The MM V prototype was the program that Ed Schroeder participated with as "Rebel" at the
1989 WMCCC in Portorose. It was used with the TK20 TurboKit.
For more information, see: http://chesseval.com/ChessEvalJournal/PrototypeMMV.htm

MM VI (Saitek, 1994) is on different hardware, H8 CPU.

******************************************************************************/

#include "emu.h"

#include "bus/generic/slot.h"
#include "bus/generic/carts.h"
#include "cpu/m6502/r65c02.h"
#include "machine/74259.h"
#include "machine/mmboard.h"
#include "sound/dac.h"
#include "video/mmdisplay1.h"

#include "softlist.h"
#include "speaker.h"

// internal artwork
#include "mephisto_mm2.lh"
#include "mephisto_bup.lh"


namespace {

class mm2_state : public driver_device
{
public:
	mm2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_outlatch(*this, "outlatch")
		, m_display(*this, "display")
		, m_dac(*this, "dac")
		, m_keys(*this, "KEY.%u", 0)
	{ }

	DECLARE_INPUT_CHANGED_MEMBER(reset_button);

	void rebel5(machine_config &config);
	void mm4(machine_config &config);
	void mm4tk(machine_config &config);
	void mm5(machine_config &config);
	void mm5p(machine_config &config);
	void mm2(machine_config &config);
	void bup(machine_config &config);

protected:
	virtual void machine_reset() override;

private:
	required_device<cpu_device> m_maincpu;
	required_device<hc259_device> m_outlatch;
	required_device<mephisto_display1_device> m_display;
	required_device<dac_bit_interface> m_dac;
	required_ioport_array<2> m_keys;

	void bup_mem(address_map &map);
	void mm2_mem(address_map &map);
	void mm4_mem(address_map &map);
	void mm5p_mem(address_map &map);
	void rebel5_mem(address_map &map);

	void lcd_irqack_w(u8 data);
	u8 keys_r(offs_t offset);
};

void mm2_state::machine_reset()
{
	m_display->reset();
	m_maincpu->set_input_line(0, CLEAR_LINE);
}

INPUT_CHANGED_MEMBER(mm2_state::reset_button)
{
	// RES buttons in serial tied to CPU RESET
	if (ioport("RESET")->read() == 3)
	{
		m_maincpu->pulse_input_line(INPUT_LINE_RESET, attotime::zero);
		machine_reset();
	}
}



/******************************************************************************
    I/O
******************************************************************************/

void mm2_state::lcd_irqack_w(u8 data)
{
	m_display->data_w(data);

	// accessing 0x2800 also clears irq
	m_maincpu->set_input_line(0, CLEAR_LINE);
}

u8 mm2_state::keys_r(offs_t offset)
{
	return ~(BIT(m_keys[m_outlatch->q7_r()]->read(), offset) << 7);
}



/******************************************************************************
    Address Maps
******************************************************************************/

void mm2_state::bup_mem(address_map &map)
{
	map(0x0000, 0x0fff).ram();
	map(0x1000, 0x1007).w("outlatch", FUNC(hc259_device::write_d7));
	map(0x1800, 0x1807).r(FUNC(mm2_state::keys_r));
	map(0x2000, 0x2000).r("board", FUNC(mephisto_board_device::input_r));
	map(0x2800, 0x2800).w(FUNC(mm2_state::lcd_irqack_w));
	map(0x3000, 0x3000).w("board", FUNC(mephisto_board_device::led_w));
	map(0x3800, 0x3800).w("board", FUNC(mephisto_board_device::mux_w));
	map(0x8000, 0xffff).rom();
}

void mm2_state::mm2_mem(address_map &map)
{
	bup_mem(map);
	map(0x4000, 0x7fff).r("cartslot", FUNC(generic_slot_device::read_rom)); // opening library
}

void mm2_state::rebel5_mem(address_map &map)
{
	map(0x0000, 0x1fff).ram();
	map(0x2000, 0x2007).w("outlatch", FUNC(hc259_device::write_d7)).nopr();
	map(0x3000, 0x3007).r(FUNC(mm2_state::keys_r));
	map(0x4000, 0x4000).r("board", FUNC(mephisto_board_device::input_r));
	map(0x5000, 0x5000).w(m_display, FUNC(mephisto_display1_device::data_w));
	map(0x6000, 0x6000).w("board", FUNC(mephisto_board_device::led_w));
	map(0x7000, 0x7000).w("board", FUNC(mephisto_board_device::mux_w));
	map(0x8000, 0xffff).rom();
}

void mm2_state::mm5p_mem(address_map &map)
{
	map(0x0000, 0x1fff).ram();
	map(0x2000, 0x2000).w(m_display, FUNC(mephisto_display1_device::data_w));
	map(0x2400, 0x2400).w("board", FUNC(mephisto_board_device::led_w));
	map(0x2800, 0x2800).w("board", FUNC(mephisto_board_device::mux_w));
	map(0x2c00, 0x2c07).r(FUNC(mm2_state::keys_r));
	map(0x3000, 0x3000).r("board", FUNC(mephisto_board_device::input_r));
	map(0x3400, 0x3407).w("outlatch", FUNC(hc259_device::write_d7)).nopr();
	map(0x3800, 0x3800).nopw(); // N/C
	map(0x4000, 0xffff).rom();
}

void mm2_state::mm4_mem(address_map &map)
{
	mm5p_mem(map);
	map(0x4000, 0x7fff).r("cartslot", FUNC(generic_slot_device::read_rom));
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( mm2 )
	PORT_START("KEY.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CL") PORT_CODE(KEYCODE_DEL) PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("POS") PORT_CODE(KEYCODE_O)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("MEM") PORT_CODE(KEYCODE_M)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("INFO") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("LEV") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("ENT") PORT_CODE(KEYCODE_ENTER) PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Right / White / 0") PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(KEYCODE_RIGHT)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Left / Black / 9") PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD) PORT_CODE(KEYCODE_LEFT)

	PORT_START("KEY.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("E / 5 / Queen") PORT_CODE(KEYCODE_E) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("F / 6 / King") PORT_CODE(KEYCODE_F) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("G / 7") PORT_CODE(KEYCODE_G) PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("A / 1 / Pawn") PORT_CODE(KEYCODE_A) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("H / 8") PORT_CODE(KEYCODE_H) PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("B / 2 / Knight") PORT_CODE(KEYCODE_B) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("C / 3 / Bishop") PORT_CODE(KEYCODE_C) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("D / 4 / Rook") PORT_CODE(KEYCODE_D) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD)

	PORT_START("RESET")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RES 1") PORT_CODE(KEYCODE_Z) PORT_CODE(KEYCODE_F1) PORT_CHANGED_MEMBER(DEVICE_SELF, mm2_state, reset_button, 0)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RES 2") PORT_CODE(KEYCODE_X) PORT_CODE(KEYCODE_F1) PORT_CHANGED_MEMBER(DEVICE_SELF, mm2_state, reset_button, 0)
INPUT_PORTS_END

static INPUT_PORTS_START( bup )
	PORT_START("KEY.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("1 / Pawn") PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("2 / Knight") PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("3 / Bishop") PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("4 / Rook") PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("5 / Queen") PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("6 / King") PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("White") PORT_CODE(KEYCODE_W)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("Black") PORT_CODE(KEYCODE_B)

	PORT_START("KEY.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("CL") PORT_CODE(KEYCODE_DEL) PORT_CODE(KEYCODE_BACKSPACE)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("ENT") PORT_CODE(KEYCODE_ENTER) PORT_CODE(KEYCODE_ENTER_PAD)
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("BEST") PORT_CODE(KEYCODE_S)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("INFO") PORT_CODE(KEYCODE_I)
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("MON") PORT_CODE(KEYCODE_N)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("POS") PORT_CODE(KEYCODE_O)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("LEV") PORT_CODE(KEYCODE_L)
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("MEM") PORT_CODE(KEYCODE_M)

	PORT_START("RESET")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RES 1") PORT_CODE(KEYCODE_Z) PORT_CODE(KEYCODE_F1) PORT_CHANGED_MEMBER(DEVICE_SELF, mm2_state, reset_button, 0)
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME("RES 2") PORT_CODE(KEYCODE_X) PORT_CODE(KEYCODE_F1) PORT_CHANGED_MEMBER(DEVICE_SELF, mm2_state, reset_button, 0)
INPUT_PORTS_END



/******************************************************************************
    Machine Configs
******************************************************************************/

void mm2_state::rebel5(machine_config &config)
{
	/* basic machine hardware */
	R65C02(config, m_maincpu, 9.8304_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &mm2_state::rebel5_mem);

	const attotime nmi_period = attotime::from_hz(9.8304_MHz_XTAL / 2 / 0x2000); // 600Hz
	m_maincpu->set_periodic_int(FUNC(mm2_state::nmi_line_pulse), nmi_period);

	HC259(config, m_outlatch);
	m_outlatch->q_out_cb<0>().set_output("led100");
	m_outlatch->q_out_cb<1>().set_output("led101");
	m_outlatch->q_out_cb<2>().set_output("led102");
	m_outlatch->q_out_cb<3>().set_output("led103");
	m_outlatch->q_out_cb<4>().set_output("led104");
	m_outlatch->q_out_cb<5>().set_output("led105");
	m_outlatch->q_out_cb<6>().set(m_dac, FUNC(dac_bit_interface::write));
	m_outlatch->q_out_cb<7>().set(m_display, FUNC(mephisto_display1_device::strobe_w)).invert();

	MEPHISTO_SENSORS_BOARD(config, "board");
	MEPHISTO_DISPLAY_MODULE1(config, m_display);
	config.set_default_layout(layout_mephisto_mm2);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 0.25);
}

void mm2_state::mm5p(machine_config &config)
{
	rebel5(config);

	/* basic machine hardware */
	m_maincpu->set_clock(4.9152_MHz_XTAL);
	m_maincpu->set_addrmap(AS_PROGRAM, &mm2_state::mm5p_mem);

	const attotime nmi_period = attotime::from_hz(4.9152_MHz_XTAL / 0x2000); // 600Hz
	m_maincpu->set_periodic_int(FUNC(mm2_state::nmi_line_pulse), nmi_period);
}

void mm2_state::mm4(machine_config &config)
{
	mm5p(config);

	/* basic machine hardware */
	m_maincpu->set_addrmap(AS_PROGRAM, &mm2_state::mm4_mem);

	GENERIC_CARTSLOT(config, "cartslot", generic_plain_slot, "mephisto_cart");
	SOFTWARE_LIST(config, "cart_list").set_original("mephisto_mm4");
}

void mm2_state::mm4tk(machine_config &config)
{
	mm4(config);
	m_maincpu->set_clock(18000000);
}

void mm2_state::mm5(machine_config &config)
{
	mm4(config);
	SOFTWARE_LIST(config.replace(), "cart_list").set_original("mephisto_mm5");
}

void mm2_state::bup(machine_config &config)
{
	rebel5(config);

	/* basic machine hardware */
	m_maincpu->set_clock(7.3728_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_PROGRAM, &mm2_state::bup_mem);

	const attotime irq_period = attotime::from_hz(7.3728_MHz_XTAL / 2 / 0x2000); // 450Hz from 4020 Q13
	m_maincpu->set_periodic_int(FUNC(mm2_state::irq0_line_assert), irq_period);

	m_outlatch->q_out_cb<7>().set(m_display, FUNC(mephisto_display1_device::strobe_w));

	config.set_default_layout(layout_mephisto_bup);
}

void mm2_state::mm2(machine_config &config)
{
	bup(config);

	/* basic machine hardware */
	m_maincpu->set_addrmap(AS_PROGRAM, &mm2_state::mm2_mem);

	config.set_default_layout(layout_mephisto_mm2);

	GENERIC_CARTSLOT(config, "cartslot", generic_plain_slot, "mephisto_cart");
	SOFTWARE_LIST(config, "cart_list").set_original("mephisto_mm2");
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( mm2 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("400", 0x8000, 0x8000, CRC(e8c1f431) SHA1(c32dfa66eefbf3e539438d2fe6e6916f78a128be) ) // HN27C256G-20
ROM_END

ROM_START( mm2a ) // binary identical to mm2
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("mm2_v4_1.bin", 0x8000, 0x4000, CRC(e9adcb8f) SHA1(498f48f96678f7bf429fd43e4f392ec6dd649fc6) )
	ROM_LOAD("mm2_v4_2.bin", 0xc000, 0x4000, CRC(d40cbfc2) SHA1(4e9b19b1a0ad97868b31d7a55143a1778110cc96) )
ROM_END

ROM_START( mm2b )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("300", 0x8000, 0x8000, CRC(60c777d4) SHA1(a77d678be60094073275558b4e8f0d34b43dd9ae) ) // D27C256D-20
ROM_END

ROM_START( mm2c )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("mm2_v2_1.bin", 0x8000, 0x4000, CRC(e2daac82) SHA1(c9fa59ca92362f8ee770733073bfa2ab8c7904ad) )
	ROM_LOAD("mm2_v2_2.bin", 0xc000, 0x4000, CRC(5e296939) SHA1(badd2a377259cf738cd076d8fb245c3dc284c24d) )
ROM_END

ROM_START( mm2d )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("mm2_v1_1.bin", 0x8000, 0x4000, CRC(b91dab77) SHA1(67762304afe51fb8f1eb91259567b2451bf9bbfd) )
	ROM_LOAD("mm2_v1_2.bin", 0xc000, 0x4000, CRC(01143cc1) SHA1(f78474b410dbecb209aa23ef81e9f894e8b54942) )
ROM_END


ROM_START( bup )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("bup_v2_1.bin", 0x8000, 0x4000, CRC(e1e9625a) SHA1(8a757e28b7afca2a092f8ff419087e06b07b743e) )
	ROM_LOAD("bup_v2_2.bin", 0xc000, 0x4000, CRC(6db30b80) SHA1(df4b379c4e916dff6b4110ec9c3591a9620c3424) )
ROM_END

ROM_START( bupa )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("bup_v1_1.bin", 0x8000, 0x4000, CRC(e1e9625a) SHA1(8a757e28b7afca2a092f8ff419087e06b07b743e) )
	ROM_LOAD("bup_v1_2.bin", 0xc000, 0x4000, CRC(708338ea) SHA1(d617c4aa2161865a22b4b0646ba793f8a1fda863) )
ROM_END


ROM_START( rebel5 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("rebel5_v2.rom", 0x8000, 0x8000, CRC(17232752) SHA1(3cd6893c0071f3dc02785bf99f1950eed81eba39) )
ROM_END

ROM_START( rebel5a )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("rebell_5.12.86", 0x8000, 0x8000, CRC(8d02e1ef) SHA1(9972c75936613bd68cfd3fe62bd222e90e8b1083) )
ROM_END


ROM_START( mm4 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("710", 0x8000, 0x8000, CRC(f68a4124) SHA1(d1d03a9aacc291d5cb720d2ee2a209eeba13a36c) )
ROM_END

ROM_START( mm4a )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("700", 0x8000, 0x8000, CRC(c97da840) SHA1(10bd2a391338ed2e417b35dcb6396ab4a4e360f0) )
ROM_END

ROM_START( mm4b )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("600", 0x8000, 0x8000, CRC(837d49b9) SHA1(9fb2dfaaeca2559ce582211137635c069180e95f) )
ROM_END

ROM_START( mm4tk )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("mm4tk.rom", 0x8000, 0x8000, CRC(51cb36a4) SHA1(9e184b4e85bb721e794b88d8657ae8d2ff5a24af) )
ROM_END


ROM_START( mm5 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("mephisto5.rom", 0x8000, 0x8000, CRC(89c3d9d2) SHA1(77cd6f8eeb03c713249db140d2541e3264328048) )
ROM_END

ROM_START( mm5a )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("mm50.rom", 0x8000, 0x8000, CRC(fcfa7e6e) SHA1(afeac3a8c957ba58cefaa27b11df974f6f2066da) )
ROM_END

ROM_START( mm5p )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("buch.bin",     0x0000, 0x8000, CRC(534607c7) SHA1(d0347a5f8dc4cf6001f649aa13e7a7fe75bec5b9) ) // 1st half empty
	ROM_LOAD("programm.bin", 0x8000, 0x8000, CRC(ee22b974) SHA1(37267507be30ee84051bc94c3a63fb1298a00261) )
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

/*    YEAR  NAME     PARENT  COMPAT  MACHINE   INPUT  CLASS      INIT        COMPANY, FULLNAME, FLAGS */
CONS( 1984, mm2,     0,      0,      mm2,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM II (set 1, v4.00 1 EPROM)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1984, mm2a,    mm2,    0,      mm2,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM II (set 2, v4.00 2 EPROMs)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1984, mm2b,    mm2,    0,      mm2,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM II (set 3, v3.00)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1984, mm2c,    mm2,    0,      mm2,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM II (set 4)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1984, mm2d,    mm2,    0,      mm2,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM II (set 5)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )

CONS( 1985, bup,     0,      0,      bup,      bup,   mm2_state, empty_init, "Hegener + Glaser", u8"Mephisto Blitz- und Problemlösungs-Modul (set 1)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1985, bupa,    bup,    0,      bup,      bup,   mm2_state, empty_init, "Hegener + Glaser", u8"Mephisto Blitz- und Problemlösungs-Modul (set 2)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )

CONS( 1986, rebel5,  0,      0,      rebel5,   mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto Rebell 5,0 (set 1)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK ) // aka MM III
CONS( 1986, rebel5a, rebel5, 0,      rebel5,   mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto Rebell 5,0 (set 2)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK ) // "

CONS( 1987, mm4,     0,      0,      mm4,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM IV (v7.10)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1987, mm4a,    mm4,    0,      mm4,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM IV (v7.00)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1987, mm4b,    mm4,    0,      mm4,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM IV (v6.00)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1987, mm4tk,   mm4,    0,      mm4tk,    mm2,   mm2_state, empty_init, "hack",             "Mephisto MM IV (TurboKit)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_TIMING )

CONS( 1990, mm5,     0,      0,      mm5,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM V (set 1)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1990, mm5a,    mm5,    0,      mm5,      mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM V (set 2)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1989, mm5p,    mm5,    0,      mm5p,     mm2,   mm2_state, empty_init, "Hegener + Glaser", "Mephisto MM V (Portorose TM version)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_TIMING ) // aka Rebel
