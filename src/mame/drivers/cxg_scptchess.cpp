// license:BSD-3-Clause
// copyright-holders:hap
// thanks-to:Sean Riddle
/******************************************************************************

CXG Sensor Computachess (CXG-001 or WA-001)
CXG Portachess, Portachess II, Computachess IV, Sphinx Chess Voyager

Sensor Computachess is White & Allcock's first original chesscomputer. Cassia's
Chess Mate (aka Computachess) doesn't really count since it was a bootleg of
Fidelity Chess Challenger 10. The chess engine is by Mark Taylor, it's the same
one as in Mini Chess released by SciSys earlier that year.

Initially, it had a "Sound" button for turning the beeps off. This was later
changed to the more useful "New Game". With Portachess, they added a "Save"
switch which puts the MCU in halt state.

Hardware notes:

Sensor Computachess:
- PCB label WA 001 600 002
- Hitachi 44801A50 MCU @ ~400kHz
- buzzer, 16 leds, button sensors chessboard

Portachess II:
- PCB label CXG223-600-001 (main pcb), CXG 211 600 101 (led pcb taken from
  Advanced Star Chess, extra led row unused here)
- Hitachi HD44801C89 MCU @ ~400kHz (serial 202: from Portachess 1985 version)
- rest same as above

HD44801A50 used in:
- CXG Sensor Computachess (1981 version) - 1st use
- CXG Portachess (1983 version, has "Sound" button)
- Hanimex HCG 1500
- Schneider Sensor Chesspartner MK 3
- Systema Computachess

HD44801C89 used in:
- CXG Portachess (1985 version, "NEW 16 LEVELS") - 1st use
- CXG Sensor Computachess (198? rerelease, "NEW 16 LEVELS")
- CXG Portachess II (1986)
- CXG Computachess IV (1986)
- CXG Sphinx Chess Voyager? (1992)
- Fidelity Computachess IV
- Fidelity Mini Chess Challenger (same housing as Portachess II)
- Schneider MK 7 (same housing as Portachess II)
- Schneider Sensor Chessmaster MK 6
- Schneider Sensor Chesspartner MK 4

******************************************************************************/

#include "emu.h"
#include "cpu/hmcs40/hmcs40.h"
#include "machine/sensorboard.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "video/pwm.h"
#include "speaker.h"

// internal artwork
#include "cxg_scptchess_v1.lh" // clickable
#include "cxg_prtchess_v2.lh" // clickable


namespace {

class scptchess_state : public driver_device
{
public:
	scptchess_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_board(*this, "board"),
		m_display(*this, "display"),
		m_dac(*this, "dac"),
		m_inputs(*this, "IN.%u", 0)
	{ }

	void scptchess_v1(machine_config &config);
	void prtchess_v2(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(reset_button);
	DECLARE_INPUT_CHANGED_MEMBER(save_switch) { update_halt(); }

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override { update_halt(); }

private:
	// devices/pointers
	required_device<hmcs40_cpu_device> m_maincpu;
	required_device<sensorboard_device> m_board;
	required_device<pwm_display_device> m_display;
	required_device<dac_bit_interface> m_dac;
	required_ioport_array<2> m_inputs;

	void update_halt();
	void update_display();
	template<int N> void mux_w(u8 data);
	void leds_w(u16 data);
	u16 input_r();

	u8 m_inp_mux = 0;
	u8 m_led_data = 0;
};

void scptchess_state::machine_start()
{
	save_item(NAME(m_inp_mux));
	save_item(NAME(m_led_data));
}

void scptchess_state::update_halt()
{
	// power switch to "Save" disables the leds and halts the MCU
	m_maincpu->set_input_line(HMCS40_INPUT_LINE_HLT, (m_inputs[1]->read() & 2) ? ASSERT_LINE : CLEAR_LINE);
	m_display->clear();
}

INPUT_CHANGED_MEMBER(scptchess_state::reset_button)
{
	// New Game button is directly tied to MCU reset
	if (~m_inputs[1]->read() & 2)
		m_maincpu->set_input_line(INPUT_LINE_RESET, newval ? ASSERT_LINE : CLEAR_LINE);
}



/******************************************************************************
    I/O
******************************************************************************/

void scptchess_state::update_display()
{
	m_display->matrix(m_inp_mux, m_led_data);
}

template<int N>
void scptchess_state::mux_w(u8 data)
{
	// R2x,R3x: input mux, led select
	m_inp_mux = (m_inp_mux & ~(0xf << (N*4))) | (data << (N*4));
	update_display();
}

void scptchess_state::leds_w(u16 data)
{
	// D2,D3: led data
	m_led_data = ~data >> 2 & 3;
	update_display();

	// D0: speaker out
	m_dac->write(data & 1);
}

u16 scptchess_state::input_r()
{
	u16 data = 0;

	// D7: read buttons
	if (m_inp_mux & m_inputs[0]->read())
		data |= 0x80;

	// D8-D15: read chessboard
	for (int i = 0; i < 8; i++)
		if (BIT(m_inp_mux, i))
			data |= m_board->read_file(i ^ 7) << 8;

	return ~data;
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( shared_v1 )
	PORT_START("IN.0")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_S) PORT_NAME("Sound") // only hooked up on 1st version
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_R) PORT_NAME("Reverse Play")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_L) PORT_NAME("Level")

	PORT_START("IN.1") // these are not available up on 1st version
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_N) PORT_CHANGED_MEMBER(DEVICE_SELF, scptchess_state, reset_button, 0) PORT_NAME("New Game")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_OTHER) PORT_CODE(KEYCODE_F1) PORT_TOGGLE PORT_CHANGED_MEMBER(DEVICE_SELF, scptchess_state, save_switch, 0) PORT_NAME("Save Switch")
INPUT_PORTS_END

static INPUT_PORTS_START( shared_v2 )
	PORT_INCLUDE( shared_v1 )

	PORT_MODIFY("IN.0")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END



/******************************************************************************
    Machine Configs
******************************************************************************/

void scptchess_state::scptchess_v1(machine_config &config)
{
	/* basic machine hardware */
	HD44801(config, m_maincpu, 400000);
	m_maincpu->write_r<2>().set(FUNC(scptchess_state::mux_w<0>));
	m_maincpu->write_r<3>().set(FUNC(scptchess_state::mux_w<1>));
	m_maincpu->write_d().set(FUNC(scptchess_state::leds_w));
	m_maincpu->read_d().set(FUNC(scptchess_state::input_r));

	SENSORBOARD(config, m_board).set_type(sensorboard_device::BUTTONS);
	m_board->init_cb().set(m_board, FUNC(sensorboard_device::preset_chess));
	m_board->set_delay(attotime::from_msec(150));

	/* video hardware */
	PWM_DISPLAY(config, m_display).set_size(8, 2);
	config.set_default_layout(layout_cxg_scptchess_v1);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 0.25);
	VOLTAGE_REGULATOR(config, "vref").add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}

void scptchess_state::prtchess_v2(machine_config &config)
{
	scptchess_v1(config);
	config.set_default_layout(layout_cxg_prtchess_v2);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( scptchess )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD("white_allcock_44801a50", 0x0000, 0x2000, CRC(c5c53e05) SHA1(8fa9b8e48ca54f08585afd83ae78fb1970fbd382) )
ROM_END

ROM_START( prtchess )
	ROM_REGION( 0x2000, "maincpu", 0 )
	ROM_LOAD("202_newcrest_16_hd44801c89", 0x0000, 0x2000, CRC(56b48f70) SHA1(84ec62323c6d3314e0515bccfde2f65f6d753e99) )
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

//    YEAR  NAME       PARENT    CMP MACHINE       INPUT      STATE            INIT        COMPANY, FULLNAME, FLAGS
CONS( 1981, scptchess, 0,         0, scptchess_v1, shared_v1, scptchess_state, empty_init, "CXG Systems / White & Allcock", "Sensor Computachess (1981 version)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1985, prtchess,  scptchess, 0, prtchess_v2,  shared_v2, scptchess_state, empty_init, "CXG Systems / Newcrest Technology", "Portachess (1985 version)", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
