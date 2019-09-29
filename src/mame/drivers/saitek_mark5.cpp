// license:BSD-3-Clause
// copyright-holders:hap
// thanks-to:Berger
/******************************************************************************

Chess Champion: Mark V / Mark VI (aka MK V / MK VI)

Released in late 1981, the chess engine was initially written by David Broughton
for a Z80 CPU and used in a prototype. It was ported to 6502 by Mark Taylor,
I/O by Mike Johnson. Support from David Levy and Kevin O'Connell, hardware
by Nick Toop. These credits are in the ROM data.

Mark VI/Philidor was released a year later, it was a plug-in module for the Mark V.
It's not much stronger than Mark V(retroactively called Mark V/Travemunde).

When using the MAME sensorboard interface with MK VI, reset the board by pressing
CLEAR before RESET, needed on 1st power-on or when starting a new game.

Hardware notes:
- SY6502A @ ~2MHz (19.6608MHz XTAL, bunch of 74113 dividers)
- 16KB RAM (8*HM4716AP-4N)
- 256x4 battery-backed RAM (TC5501P)
- HLCD0538, 2*HLCD0539, LCD screen with chessboard

3 slots:
- ROM module at the bottom, mandatory (4*8KB)
- unused module slot next to the LCD
- sensory board at left edge, only for MK VI

Chess Champion Sensory Board:
- PCB label: SciSys 502-00
- 4KB ROM (NEC D2732), TTL
- magnet sensors, 64 leds

The slots were designed to support anything, but the only released peripheral
was the Chess Champion Sensory Board. A piece-recognition chessboard was also
announced but not released. Maybe it existed as prototype, see patent GB2103943A.

TODO:
- /2 CPU divider when accessing 0x5000 (the nvram)
- reading from 0x4400 will write to the LCD too, open bus? it wouldn't make
  sense to use it (and as expected, it never is used)
- what are the 1M/3M/4M diodes for? CPU speed? the only noticeable difference
  is beeper pitch

******************************************************************************/

#include "emu.h"
#include "cpu/m6502/m6502.h"
#include "machine/nvram.h"
#include "machine/sensorboard.h"
#include "sound/dac.h"
#include "sound/volt_reg.h"
#include "video/hlcd0538.h"
#include "video/pwm.h"

#include "screen.h"
#include "speaker.h"

// internal artwork
#include "saitek_mark5.lh" // clickable
#include "saitek_mark6.lh" // clickable


namespace {

class mark5_state : public driver_device
{
public:
	mark5_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_board(*this, "board"),
		m_cb_rom(*this, "chessboard"),
		m_display(*this, "display%u", 0),
		m_lcd(*this, "lcd%u", 0),
		m_dac(*this, "dac"),
		m_nvram(*this, "nvram"),
		m_inputs(*this, "IN.%u", 0),
		m_out_x(*this, "%u.%u.%u", 0U, 0U, 0U)
	{ }

	// machine drivers
	void mark5(machine_config &config);
	void mark6(machine_config &config);

	DECLARE_INPUT_CHANGED_MEMBER(cb_enable) { if (!newval) m_display[3]->clear(); }

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
	// devices/pointers
	required_device<cpu_device> m_maincpu;
	optional_device<sensorboard_device> m_board;
	optional_region_ptr<u8> m_cb_rom;
	optional_device_array<pwm_display_device, 3+1> m_display;
	required_device_array<hlcd0538_device, 3> m_lcd;
	required_device<dac_bit_interface> m_dac;
	required_shared_ptr<u8> m_nvram;
	required_ioport_array<7+1> m_inputs;
	output_finder<3, 8, 34> m_out_x;

	// address maps
	void mark5_map(address_map &map);
	void mark6_map(address_map &map);

	// I/O handlers
	DECLARE_WRITE8_MEMBER(nvram_w);
	DECLARE_READ8_MEMBER(nvram_r);
	DECLARE_WRITE8_MEMBER(lcd_data_w);
	DECLARE_WRITE8_MEMBER(sound_w);
	DECLARE_READ8_MEMBER(sound_r);
	DECLARE_WRITE8_MEMBER(reset_irq_w);
	DECLARE_READ8_MEMBER(reset_irq_r);
	DECLARE_READ8_MEMBER(input_r);
	DECLARE_READ8_MEMBER(cb_rom_r);
	DECLARE_WRITE8_MEMBER(cb_w);
	DECLARE_READ8_MEMBER(cb_r);

	template<int N> DECLARE_WRITE8_MEMBER(pwm_output_w);
	template<int N> DECLARE_WRITE64_MEMBER(lcd_output_w);

	u8 m_dac_data;
	u8 m_lcd_lcd;
	u8 m_lcd_rowsel;
	u8 m_cb_mux;

	emu_timer *m_irqtimer;
	TIMER_CALLBACK_MEMBER(interrupt);
	void write_lcd(int state);
};

void mark5_state::machine_start()
{
	m_out_x.resolve();
	m_irqtimer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(mark5_state::interrupt),this));

	// zerofill
	m_dac_data = 0;
	m_lcd_lcd = 0;
	m_lcd_rowsel = 0;
	m_cb_mux = 0;

	// register for savestates
	save_item(NAME(m_dac_data));
	save_item(NAME(m_lcd_lcd));
	save_item(NAME(m_lcd_rowsel));
	save_item(NAME(m_cb_mux));
}

void mark5_state::machine_reset()
{
	reset_irq_w(machine().dummy_space(), 0, 0);
}



/******************************************************************************
    I/O
******************************************************************************/

WRITE8_MEMBER(mark5_state::nvram_w)
{
	// nvram is only d0-d3
	m_nvram[offset] = data & 0xf;
}

READ8_MEMBER(mark5_state::nvram_r)
{
	return m_nvram[offset] & 0xf;
}

template<int N>
WRITE8_MEMBER(mark5_state::pwm_output_w)
{
	m_out_x[N][offset & 0x3f][offset >> 6] = data;
}

template<int N>
WRITE64_MEMBER(mark5_state::lcd_output_w)
{
	if (N == 0)
	{
		// HLCD0538 R pins
		m_lcd_rowsel = data & 0xff;
		data >>= 8;
	}

	m_display[N]->matrix(m_lcd_rowsel, data);
}

void mark5_state::write_lcd(int state)
{
	for (int i = 0; i < 3; i++)
		m_lcd[i]->lcd_w(state);

	m_lcd_lcd = state;
}

TIMER_CALLBACK_MEMBER(mark5_state::interrupt)
{
	// master clock to MC14020, Q12 to IRQ @ 480Hz
	// irq active ~34.61us (470pF, 100K to GND)
	m_irqtimer->adjust(attotime::from_hz(19.6608_MHz_XTAL / 10 / 0x1000));
	m_maincpu->pulse_input_line(0, attotime::from_nsec(34610));

	// MC14020 Q13(1 stage further than IRQ) goes to LCD "LCD" pins
	write_lcd(m_lcd_lcd ^ 1);
}

WRITE8_MEMBER(mark5_state::reset_irq_w)
{
	// MC14020 R
	m_irqtimer->adjust(attotime::from_hz((19.6608_MHz_XTAL / 10 / 0x1000) * 2));
	write_lcd(0);
}

READ8_MEMBER(mark5_state::reset_irq_r)
{
	if (!machine().side_effects_disabled())
		reset_irq_w(space, offset, 0);

	return 0xff;
}

WRITE8_MEMBER(mark5_state::sound_w)
{
	// 7474 to speaker out
	m_dac_data ^= 1;
	m_dac->write(m_dac_data & m_inputs[7]->read());
}

READ8_MEMBER(mark5_state::sound_r)
{
	if (!machine().side_effects_disabled())
		sound_w(space, offset, 0);

	return 0xff;
}

WRITE8_MEMBER(mark5_state::lcd_data_w)
{
	// d0,d2,d4: LCD data
	for (int i = 0; i < 3; i++)
	{
		m_lcd[i]->data_w(BIT(data, i*2));

		m_lcd[i]->clk_w(1);
		m_lcd[i]->clk_w(0);
	}
}

READ8_MEMBER(mark5_state::input_r)
{
	u8 data = 0;

	// _a6: configuration diodes
	// a0-a5: multiplexed inputs
	for (int i = 0; i < 7; i++)
		if (BIT(offset ^ 0x40, i))
			data |= m_inputs[i]->read();

	return ~data;
}

WRITE8_MEMBER(mark5_state::cb_w)
{
	if (~m_inputs[6]->read() & 0x20)
		return;

	// d0-d2: chessboard led mux 1/input mux
	// d3-d5: chessboard led mux 2
	// d6: led state
	m_display[3]->matrix(1 << (data & 7), 1 << (data >> 3 & 0xf));
	m_cb_mux = data;
}

READ8_MEMBER(mark5_state::cb_r)
{
	if (~m_inputs[6]->read() & 0x20)
		return 0xff;

	// read chessboard sensors
	return ~m_board->read_file(m_cb_mux & 7);
}

READ8_MEMBER(mark5_state::cb_rom_r)
{
	return (m_inputs[6]->read() & 0x20) ? m_cb_rom[offset] : 0xff;
}



/******************************************************************************
    Address Maps
******************************************************************************/

void mark5_state::mark5_map(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x3fff).ram();
	map(0x4000, 0x407f).mirror(0x0380).r(FUNC(mark5_state::input_r));
	map(0x4400, 0x4400).mirror(0x03ff).w(FUNC(mark5_state::lcd_data_w));
	map(0x4800, 0x4800).mirror(0x03ff).rw(FUNC(mark5_state::sound_r), FUNC(mark5_state::sound_w));
	map(0x4c00, 0x4c00).mirror(0x03ff).rw(FUNC(mark5_state::reset_irq_r), FUNC(mark5_state::reset_irq_w));
	map(0x5000, 0x50ff).mirror(0x0f00).ram().rw(FUNC(mark5_state::nvram_r), FUNC(mark5_state::nvram_w)).share("nvram");
	map(0x8000, 0xffff).rom();
}

void mark5_state::mark6_map(address_map &map)
{
	mark5_map(map);

	map(0x7000, 0x77ff).r(FUNC(mark5_state::cb_rom_r));
	map(0x7800, 0x7800).mirror(0x07ff).rw(FUNC(mark5_state::cb_r), FUNC(mark5_state::cb_w));
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( mark5 )
	PORT_START("IN.0")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_S) PORT_NAME("Enter Position")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_N) PORT_NAME("New Game")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_Z) PORT_NAME("Draw")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_L) PORT_NAME("Peripheral")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_I) PORT_NAME("Next Simult")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_W) PORT_NAME("Swap")

	PORT_START("IN.1")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_X) PORT_NAME("Next Best")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_U) PORT_NAME("Auto")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_T) PORT_NAME("Comment")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_Y) PORT_NAME("Analysis")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_SLASH) PORT_NAME("Clock")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_STOP) PORT_NAME("Stop Clock")

	PORT_START("IN.2")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_0) PORT_CODE(KEYCODE_0_PAD) PORT_CODE(KEYCODE_MINUS) PORT_CODE(KEYCODE_MINUS_PAD) PORT_NAME("No / 0 / CS")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_1) PORT_CODE(KEYCODE_1_PAD) PORT_CODE(KEYCODE_A) PORT_NAME("A / 1 / Pawn")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_CODE(KEYCODE_B) PORT_NAME("B / 2 / Knight")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_3) PORT_CODE(KEYCODE_3_PAD) PORT_CODE(KEYCODE_C) PORT_NAME("C / 3 / Bishop")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_4) PORT_CODE(KEYCODE_4_PAD) PORT_CODE(KEYCODE_D) PORT_NAME("D / 4 / Rook")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_5) PORT_CODE(KEYCODE_5_PAD) PORT_CODE(KEYCODE_E) PORT_NAME("E / 5 / Queen")

	PORT_START("IN.3")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_6) PORT_CODE(KEYCODE_6_PAD) PORT_CODE(KEYCODE_F) PORT_NAME("F / 6 / King")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_7) PORT_CODE(KEYCODE_7_PAD) PORT_CODE(KEYCODE_G) PORT_NAME("G / 7 / Black")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_8) PORT_CODE(KEYCODE_8_PAD) PORT_CODE(KEYCODE_H) PORT_NAME("H / 8 / White")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_9) PORT_CODE(KEYCODE_9_PAD) PORT_CODE(KEYCODE_EQUALS) PORT_CODE(KEYCODE_PLUS_PAD) PORT_NAME("Yes / 9 / CB")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_PGDN) PORT_NAME("Backward")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_PGUP) PORT_NAME("Forward")

	PORT_START("IN.4")
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_R) PORT_NAME("Replay")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_O) PORT_NAME("Go")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_ENTER) PORT_CODE(KEYCODE_ENTER_PAD) PORT_NAME("Enter")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_DEL) PORT_CODE(KEYCODE_BACKSPACE) PORT_NAME("CE")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_M) PORT_NAME("Mode")
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_COMMA) PORT_NAME("Start Clock")

	PORT_START("IN.5") // square 'd-pad' (8-way, so define joystick)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP) PORT_CODE(KEYCODE_UP) PORT_NAME("Cursor Up")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN) PORT_CODE(KEYCODE_DOWN) PORT_NAME("Cursor Down")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT) PORT_CODE(KEYCODE_RIGHT) PORT_NAME("Cursor Right")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT) PORT_CODE(KEYCODE_LEFT) PORT_NAME("Cursor Left")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("IN.6") // diodes
	PORT_BIT(0x01, IP_ACTIVE_LOW, IPT_CUSTOM) // 1M/3M/4M
	PORT_BIT(0x02, IP_ACTIVE_LOW, IPT_CUSTOM) // "
	PORT_CONFNAME( 0x0c, 0x00, DEF_STR( Language ) )
	PORT_CONFSETTING(    0x00, DEF_STR( English ) )
	PORT_CONFSETTING(    0x04, DEF_STR( German ) )
	PORT_CONFSETTING(    0x08, DEF_STR( French ) )
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_CUSTOM) // $6000 edge connector
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_CUSTOM) // $7000 "

	PORT_START("IN.7") // switches
	PORT_CONFNAME( 0x01, 0x01, "Sound" )
	PORT_CONFSETTING(    0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x01, DEF_STR( On ) )
	PORT_CONFNAME( 0x02, 0x02, "LCD Light" )
	PORT_CONFSETTING(    0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x02, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( mark6 )
	PORT_INCLUDE( mark5 )

	PORT_MODIFY("IN.6")
	PORT_CONFNAME( 0x20, 0x20, "Sensory Board" ) PORT_CHANGED_MEMBER(DEVICE_SELF, mark5_state, cb_enable, 0)
	PORT_CONFSETTING(    0x00, DEF_STR( Off ) )
	PORT_CONFSETTING(    0x20, DEF_STR( On ) )
INPUT_PORTS_END



/******************************************************************************
    Machine Drivers
******************************************************************************/

void mark5_state::mark5(machine_config &config)
{
	/* basic machine hardware */
	M6502(config, m_maincpu, 19.6608_MHz_XTAL / 10);
	m_maincpu->set_addrmap(AS_PROGRAM, &mark5_state::mark5_map);

	NVRAM(config, "nvram", nvram_device::DEFAULT_ALL_0);

	/* video hardware */
	HLCD0538(config, m_lcd[0]).write_cols().set(FUNC(mark5_state::lcd_output_w<0>));
	PWM_DISPLAY(config, m_display[0]).set_size(8, 26);
	m_display[0]->output_x().set(FUNC(mark5_state::pwm_output_w<0>));

	HLCD0539(config, m_lcd[1]).write_cols().set(FUNC(mark5_state::lcd_output_w<1>));
	PWM_DISPLAY(config, m_display[1]).set_size(8, 34);
	m_display[1]->output_x().set(FUNC(mark5_state::pwm_output_w<1>));

	HLCD0539(config, m_lcd[2]).write_cols().set(FUNC(mark5_state::lcd_output_w<2>));
	PWM_DISPLAY(config, m_display[2]).set_size(8, 34);
	m_display[2]->output_x().set(FUNC(mark5_state::pwm_output_w<2>));

	for (int i = 0; i < 3; i++)
		m_display[i]->set_bri_maximum(0.1);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_SVG));
	screen.set_refresh_hz(60);
	screen.set_size(942, 1080);
	screen.set_visarea_full();

	config.set_default_layout(layout_saitek_mark5);

	/* sound hardware */
	SPEAKER(config, "speaker").front_center();
	DAC_1BIT(config, m_dac).add_route(ALL_OUTPUTS, "speaker", 0.25);
	VOLTAGE_REGULATOR(config, "vref").add_route(0, "dac", 1.0, DAC_VREF_POS_INPUT);
}

void mark5_state::mark6(machine_config &config)
{
	mark5(config);

	/* basic machine hardware */
	m_maincpu->set_addrmap(AS_PROGRAM, &mark5_state::mark6_map);

	SENSORBOARD(config, m_board).set_type(sensorboard_device::MAGNETS);
	m_board->init_cb().set(m_board, FUNC(sensorboard_device::preset_chess));
	m_board->set_delay(attotime::from_msec(150));

	PWM_DISPLAY(config, m_display[3]).set_size(8, 8);
	m_display[3]->set_bri_levels(0.001);
	config.set_default_layout(layout_saitek_mark6);
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( ccmk5 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("c47024_syp_2364-3-y51", 0x8000, 0x2000, CRC(c210f530) SHA1(60ba3809ed3054024508344f654a6846061fafd5) ) // 2364
	ROM_LOAD("c47025_syp_2364-3-y5a", 0xa000, 0x2000, CRC(3239c96b) SHA1(6a23713b30c48546d993a0de8998c8de9044e48c) ) // "
	ROM_LOAD("c47026_syp_2364-3-y5c", 0xc000, 0x2000, CRC(1754ccab) SHA1(d246b6aa2e2a1858dd6608a4dbf496778f79b22e) ) // "
	ROM_LOAD("c47027_syp_2364-3-y5d", 0xe000, 0x2000, CRC(7c0f7bd8) SHA1(68b4566f0501005f6b1739bb24a4bec990421a6f) ) // "

	ROM_REGION( 1887311, "screen", 0)
	ROM_LOAD( "ccmk5.svg", 0, 1887311, CRC(3261bcb2) SHA1(46b38a2877faa36ef1adea2b8f484a97b46ea529) )
ROM_END

ROM_START( ccmk6 )
	ROM_REGION( 0x10000, "maincpu", 0 )
	ROM_LOAD("y6_80", 0x8000, 0x2000, CRC(8144dd71) SHA1(8d6fbb4aa9757149c81d2bf533085dc5203f0751) ) // 2764
	ROM_LOAD("y6_a0", 0xa000, 0x2000, CRC(dd77dd90) SHA1(844aee56e1941f05bdf046d95c5ae687707a2c95) ) // "
	ROM_LOAD("y6_c0", 0xc000, 0x2000, CRC(705e5718) SHA1(513bba3e7344194efaaf022a7934d32d8cba3cb5) ) // "
	ROM_LOAD("y6_e0", 0xe000, 0x2000, CRC(b92c3eb3) SHA1(99a20f5e971b8c4228e0eda0a4c05750d46b95f6) ) // "

	ROM_REGION( 0x1000, "chessboard", 0 )
	ROM_LOAD("d2732c-e.u1", 0x0000, 0x1000, CRC(93221b4c) SHA1(8561b52c80cab7c04d30eaa14f9520a362d7f822) ) // no label, identical halves

	ROM_REGION( 1887311, "screen", 0)
	ROM_LOAD( "ccmk5.svg", 0, 1887311, CRC(3261bcb2) SHA1(46b38a2877faa36ef1adea2b8f484a97b46ea529) )
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

//    YEAR  NAME   PARENT CMP MACHINE INPUT  STATE        INIT        COMPANY, FULLNAME, FLAGS
CONS( 1981, ccmk5, 0,      0, mark5,  mark5, mark5_state, empty_init, "SciSys / Philidor Software", "Chess Champion: Mark V", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
CONS( 1982, ccmk6, ccmk5,  0, mark6,  mark6, mark5_state, empty_init, "SciSys / Philidor Software", "Chess Champion: Mark VI/Philidor", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
