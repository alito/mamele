// license:BSD-3-Clause
// copyright-holders:hap
/******************************************************************************

La Régence, French chess computer by "France Double R". German distribution
by Sandy Electronic, who sub-titled it TSB 4 (Turniersensorbrett).

The chess engine is Richard Lang's Cyrus.

Hardware notes:
- PCB label: FRANCE DOUBLE R, MADE IN FRANCE
- Sharp LH0080A Z80A @ 4 MHz (8MHz XTAL)
- 3*4KB ROM, sockets support up to 48KB ROM
- 2KB RAM (MSM5128-15RS), 3 sockets, only middle one used
- TTL, piezo, 8*8+4 LEDs, magnetic sensors

TODO:
- verify irq source/frequency, probably a 555 ic, current approximation is from
  comparing led blink rate with a video recording

******************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/sensorboard.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "video/pwm.h"
#include "speaker.h"

// internal artwork
#include "regence.lh" // clickable


namespace {

class regence_state : public driver_device
{
public:
	regence_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_display(*this, "display"),
		m_board(*this, "board"),
		m_dac(*this, "dac"),
		m_inputs(*this, "IN.%u", 0)
	{ }

	// machine configs
	void regence(machine_config &config);

protected:
	virtual void machine_start() override;

private:
	// devices/pointers
	required_device<cpu_device> m_maincpu;
	required_device<pwm_display_device> m_display;
	required_device<sensorboard_device> m_board;
	required_device<dac_bit_interface> m_dac;
	required_ioport_array<2> m_inputs;

	// address maps
	void main_map(address_map &map);

	// I/O handlers
	void update_display();
	void control_w(u8 data);
	void leds_w(u8 data);
	u8 input_r();

	u8 m_inp_mux = 0;
	u8 m_led_data = 0;
};

void regence_state::machine_start()
{
	// register for savestates
	save_item(NAME(m_inp_mux));
	save_item(NAME(m_led_data));
}



/******************************************************************************
    I/O
******************************************************************************/

void regence_state::update_display()
{
	m_display->matrix(1 << m_inp_mux, m_led_data);
}

void regence_state::control_w(u8 data)
{
	// d0-d3: input mux/led select
	m_inp_mux = data & 0xf;
	update_display();

	// d7: speaker out
	m_dac->write(BIT(data, 7));

	// other: ?
}

void regence_state::leds_w(u8 data)
{
	// d0-d7: led data
	m_led_data = data;
	update_display();
}

u8 regence_state::input_r()
{
	u8 data = 0;

	// d0-d7: multiplexed inputs
	// read chessboard sensors
	if (m_inp_mux < 8)
		data = m_board->read_file(m_inp_mux, true);

	// read other buttons
	else if (m_inp_mux < 10)
		data = m_inputs[m_inp_mux - 8]->read();

	return data;
}



/******************************************************************************
    Address Maps
******************************************************************************/

void regence_state::main_map(address_map &map)
{
	map(0x0000, 0x0fff).rom();
	map(0x4000, 0x4fff).rom();
	map(0x8000, 0x8fff).rom();
	map(0xd000, 0xd7ff).ram();
	map(0xf000, 0xf000).rw(FUNC(regence_state::input_r), FUNC(regence_state::control_w));
	map(0xf800, 0xf800).w(FUNC(regence_state::leds_w));
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( regence )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_S) PORT_NAME("Changement de Position (Set Up)") // Veränderung
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_T) PORT_NAME(u8"Retour en Arrière (Take Back)") // Zug Zurück
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_N) PORT_NAME("Nouvelle Partie (New Game)") // Neues Spiel (press after setup)
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_NAME("King")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_NAME("Queen")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_NAME("Rook")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_NAME("Bishop")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_O) PORT_NAME("Son (Sound)") // Ton
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_L) PORT_NAME("Niveau (Level)") // Stufe
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_M) PORT_CODE(KEYCODE_H) PORT_NAME("Marche/Arret (Move/Halt)") // Zug-Halt
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_B) PORT_NAME("Noir (Black)") // Schwarz
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_W) PORT_NAME("Blanc (White)") // Weiss
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_NAME("Pawn")
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_NAME("Knight")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_UNUSED)
INPUT_PORTS_END



/******************************************************************************
    Machine Configs
******************************************************************************/

void regence_state::regence(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, 8_MHz_XTAL/2);
	m_maincpu->set_addrmap(AS_PROGRAM, &regence_state::main_map);

	m_maincpu->set_periodic_int(FUNC(regence_state::irq0_line_hold), attotime::from_hz(400)); // approximation

	SENSORBOARD(config, m_board).set_type(sensorboard_device::MAGNETS);
	m_board->init_cb().set(m_board, FUNC(sensorboard_device::preset_chess));
	m_board->set_delay(attotime::from_msec(150));

	/* video hardware */
	PWM_DISPLAY(config, m_display).set_size(10, 8);
	config.set_default_layout(layout_regence);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 0.25);
	VOLTAGE_REGULATOR(config, "vref").add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( regence )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("ic13", 0x0000, 0x1000, CRC(ac6a0a67) SHA1(52b115c7cd372dfbad14b00854aa4f6f75a937d3) )
	ROM_LOAD("ic12", 0x4000, 0x1000, CRC(5c2fb0c7) SHA1(811ab3d7cefcf872741eb2265115080aaf913f0f) )
	ROM_LOAD("ic11", 0x8000, 0x1000, CRC(e4c39dbd) SHA1(b6a6d1d39f73a2ff1ade6205bdf180be13e84df3) )
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

/*    YEAR  NAME     PARENT  COMPAT  MACHINE  INPUT    CLASS          INIT        COMPANY, FULLNAME, FLAGS */
CONS( 1982, regence, 0,      0,      regence, regence, regence_state, empty_init, "France Double R", u8"La Régence", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
