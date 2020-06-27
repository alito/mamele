// license:BSD-3-Clause
// copyright-holders:hap
// thanks-to:Sean Riddle
/******************************************************************************

SciSys Executive Chess, handheld chesscomputer.
Also known as Senator Chess in Germany.

Hardware notes:
- Fairchild 3870 MCU (variant with 4KB internal ROM)
- 1KB RAM (2*TC5514P)
- HLCD0538, HLCD0539, LCD screen

******************************************************************************/

#include "emu.h"

#include "cpu/f8/f8.h"
#include "machine/f3853.h"
#include "video/hlcd0538.h"
#include "video/pwm.h"

#include "screen.h"

// internal artwork
#include "saitek_exchess.lh" // clickable


namespace {

class exchess_state : public driver_device
{
public:
	exchess_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_lcd1(*this, "lcd1"),
		m_lcd2(*this, "lcd2"),
		m_display(*this, "display"),
		m_battery(*this, "battery"),
		m_inputs(*this, "IN.%u", 0)
	{ }

	void exchess(machine_config &config);

	// battery status indicator is not software controlled
	DECLARE_INPUT_CHANGED_MEMBER(battery) { m_battery = newval; }

protected:
	virtual void machine_start() override;

private:
	// devices/pointers
	required_device<cpu_device> m_maincpu;
	required_device<hlcd0538_device> m_lcd1;
	required_device<hlcd0539_device> m_lcd2;
	required_device<pwm_display_device> m_display;
	output_finder<> m_battery;
	required_ioport_array<4> m_inputs;

	void main_map(address_map &map);
	void main_io(address_map &map);

	template<int N> void lcd_output_w(u64 data);
	void lcd_data_w(u8 data);

	u16 ram_address() { return (m_ram_address[1] << 8 | m_ram_address[0]) & 0x3ff; }
	template<int N> u8 ram_address_r();
	template<int N> void ram_address_w(u8 data);
	u8 ram_data_r();
	void ram_data_w(u8 data);

	std::unique_ptr<u8[]> m_ram;
	u8 m_ram_address[2] = { 0, 0 };
	u64 m_lcd_data[2] = { 0, 0 };
};

void exchess_state::machine_start()
{
	m_battery.resolve();
	m_ram = make_unique_clear<u8[]>(0x400);

	// register for savestates
	save_pointer(NAME(m_ram), 0x400);
	save_item(NAME(m_ram_address));
	save_item(NAME(m_lcd_data));
}



/******************************************************************************
    I/O
******************************************************************************/

// LCD

template<int N>
void exchess_state::lcd_output_w(u64 data)
{
	m_lcd_data[N] = data;
	m_display->matrix(m_lcd_data[0] & 0xff, m_lcd_data[1] << 26 | m_lcd_data[0] >> 8);
}

void exchess_state::lcd_data_w(u8 data)
{
	// P40: HLCD0539 data
	// P44: HLCD0538 data
	m_lcd1->data_w(BIT(data, 4));
	m_lcd2->data_w(BIT(data, 0));

	if (~m_ram_address[1] & 4)
	{
		m_lcd1->clk_w(1); m_lcd1->clk_w(0);
		m_lcd2->clk_w(1); m_lcd2->clk_w(0);
	}
}


// 1KB RAM (port-mapped)

template<int N>
void exchess_state::ram_address_w(u8 data)
{
	// P00-P07: RAM A0-A7
	// P10-P11: RAM A8-A9
	// P12: RAM CE
	m_ram_address[N] = data;
}

template<int N>
u8 exchess_state::ram_address_r()
{
	u8 data = m_ram_address[N];

	// P13: Enter button
	return (N) ? data | (m_inputs[0]->read() & 8) : data;
}

void exchess_state::ram_data_w(u8 data)
{
	if (m_ram_address[1] & 4)
		m_ram[ram_address()] = data;
}

u8 exchess_state::ram_data_r()
{
	return (m_ram_address[1] & 4) ? m_ram[ram_address()] : 0;
}



/******************************************************************************
    Address Maps
******************************************************************************/

void exchess_state::main_map(address_map &map)
{
	map.global_mask(0xfff);
	map(0x0000, 0x0fff).rom();
}

void exchess_state::main_io(address_map &map)
{
	map(0x00, 0x00).rw(FUNC(exchess_state::ram_address_r<0>), FUNC(exchess_state::ram_address_w<0>));
	map(0x01, 0x01).rw(FUNC(exchess_state::ram_address_r<1>), FUNC(exchess_state::ram_address_w<1>));
	map(0x04, 0x07).rw("psu", FUNC(f38t56_device::read), FUNC(f38t56_device::write));
}



/******************************************************************************
    Input Ports
******************************************************************************/

static INPUT_PORTS_START( exchess )
	PORT_START("IN.0")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_ENTER) PORT_CODE(KEYCODE_ENTER_PAD) PORT_NAME("Enter")
	PORT_BIT(0xf7, IP_ACTIVE_HIGH, IPT_UNUSED)

	PORT_START("IN.1")
	PORT_BIT(0x01, 0x01, IPT_CUSTOM) PORT_CONDITION("IN.2", 0x51, NOTEQUALS, 0x00)
	PORT_BIT(0x02, 0x02, IPT_CUSTOM) PORT_CONDITION("IN.2", 0x32, NOTEQUALS, 0x00)
	PORT_BIT(0x04, 0x04, IPT_CUSTOM) PORT_CONDITION("IN.2", 0xa4, NOTEQUALS, 0x00)
	PORT_BIT(0x08, 0x08, IPT_CUSTOM) PORT_CONDITION("IN.2", 0xc8, NOTEQUALS, 0x00)
	PORT_BIT(0x30, IP_ACTIVE_HIGH, IPT_UNUSED)
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_N) PORT_NAME("New Game")
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_CODE(KEYCODE_2) PORT_CODE(KEYCODE_2_PAD) PORT_CODE(KEYCODE_F) PORT_NAME("2nd F")

	PORT_START("IN.2") // square 'd-pad' (8-way, so define joystick)
	PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT) PORT_CODE(KEYCODE_LEFT) PORT_NAME("Cursor Left")
	PORT_BIT(0x02, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP) PORT_CODE(KEYCODE_UP) PORT_NAME("Cursor Up")
	PORT_BIT(0x04, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT) PORT_CODE(KEYCODE_RIGHT) PORT_NAME("Cursor Right")
	PORT_BIT(0x08, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN) PORT_CODE(KEYCODE_DOWN) PORT_NAME("Cursor Down")
	PORT_BIT(0x10, IP_ACTIVE_HIGH, IPT_OTHER) // ul
	PORT_BIT(0x20, IP_ACTIVE_HIGH, IPT_OTHER) // ur
	PORT_BIT(0x40, IP_ACTIVE_HIGH, IPT_OTHER) // dl
	PORT_BIT(0x80, IP_ACTIVE_HIGH, IPT_OTHER) // dr

	PORT_START("IN.3")
	PORT_CONFNAME( 0x01, 0x00, "Battery Status" ) PORT_CHANGED_MEMBER(DEVICE_SELF, exchess_state, battery, 0)
	PORT_CONFSETTING(    0x01, "Low" )
	PORT_CONFSETTING(    0x00, DEF_STR( Normal ) )
INPUT_PORTS_END



/******************************************************************************
    Machine Configs
******************************************************************************/

void exchess_state::exchess(machine_config &config)
{
	/* basic machine hardware */
	F8(config, m_maincpu, 4500000/2); // measured
	m_maincpu->set_addrmap(AS_PROGRAM, &exchess_state::main_map);
	m_maincpu->set_addrmap(AS_IO, &exchess_state::main_io);
	m_maincpu->set_irq_acknowledge_callback("psu", FUNC(f38t56_device::int_acknowledge));

	f38t56_device &psu(F38T56(config, "psu", 4500000/2));
	psu.set_int_vector(0x0020);
	psu.int_req_callback().set_inputline("maincpu", F8_INPUT_LINE_INT_REQ);
	psu.read_a().set(FUNC(exchess_state::ram_data_r));
	psu.write_a().set(FUNC(exchess_state::ram_data_w));
	psu.write_a().append(FUNC(exchess_state::lcd_data_w));
	psu.read_b().set_ioport("IN.1");

	/* video hardware */
	HLCD0538(config, m_lcd1, 310); // measured
	m_lcd1->write_cols().set(FUNC(exchess_state::lcd_output_w<0>));
	m_lcd1->write_interrupt().set(m_lcd2, FUNC(hlcd0539_device::lcd_w));

	HLCD0539(config, m_lcd2, 0);
	m_lcd2->write_cols().set(FUNC(exchess_state::lcd_output_w<1>));
	m_lcd2->write_interrupt().set("psu", FUNC(f38t56_device::ext_int_w)).invert();

	PWM_DISPLAY(config, m_display).set_size(8, 26+34);
	m_display->set_interpolation(0.2);
	config.set_default_layout(layout_saitek_exchess);

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_SVG));
	screen.set_refresh_hz(60);
	screen.set_size(1020, 1080);
	screen.set_visarea_full();
}



/******************************************************************************
    ROM Definitions
******************************************************************************/

ROM_START( exchess )
	ROM_REGION( 0x1000, "maincpu", 0 )
	ROM_LOAD("sl90553", 0x0000, 0x1000, CRC(a61b0c7e) SHA1(a13b11a93f78236223c5c0b9879a93284b7f7525) )

	ROM_REGION( 852610, "screen", 0 )
	ROM_LOAD("exchess.svg", 0, 852610, CRC(cb36f9d3) SHA1(83be9b5d906d185b7cf6895f50992e7eea390c7a) )
ROM_END

} // anonymous namespace



/******************************************************************************
    Drivers
******************************************************************************/

//    YEAR  NAME     PARENT CMP MACHINE  INPUT    STATE          INIT        COMPANY, FULLNAME, FLAGS
CONS( 1981, exchess, 0,      0, exchess, exchess, exchess_state, empty_init, "SciSys", "Executive Chess", MACHINE_NO_SOUND_HW | MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
