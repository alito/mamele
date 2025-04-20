// license:BSD-3-Clause
// copyright-holders:Devin Acker
/*
    Casio GT913-based keyboards and MIDI modules

-------------------------------------------------------------------------------

    Celviano AP-10 digital piano (1995)

    Main board (JCM358-MA1M):
        LSI301: CPU (Casio/NEC uPD912GF)
        LSI302: DSP (Hitachi HG51B277FB)
        LSI303: 8Mbit ROM (Macronix MX23C8100MC-12)
        LSI304: 64kbit SRAM for CPU (Sanyo LC3564SM-85), battery backed
        LSI305: 256kbit SRAM for DSP (Sanyo LC333832M-70)
        LSI306: stereo DAC (NEC uPD6379GR)
        X301:   24MHz crystal for CPU
        X302:   16MHz ceramic for DSP

    Service manual with schematics, pinouts, etc.:
    https://revenant1.net/casio/manuals/upd91x/ap10.pdf

    To access the test mode (not mentioned in the service manual):
    Hold both pedals and "Transpose/Tune/MIDI" while turning on the keyboard, then release the button.
    Afterwards, press one of these buttons:
    - Transpose: LED test
    - Effect: switch test (press all front panel buttons left to right)
    - Piano: key test (press all keys left to right)
    - E.Piano: ROM test
    - Organ/Strings/Song: sound volume test
    - Record/Start/Stop: stereo test
    - Demo: MIDI loopback test
    - Harpsichord: exit test mode

    TODO: fix backup RAM getting re-initialized on every boot.
    Depends on the power switch being implemented correctly - turning the power off
    is supposed to trigger a NMI which updates the RAM checksum, but the NMI handler
    always proceeds to fully start up the system as if the power is being turned on

-------------------------------------------------------------------------------

    CTK-530/540 (1995)

    Main board (JCM460-MA1M):
        LSI101: CPU (Casio/NEC uPD912GF)
        LSI102: 8Mbit ROM (Macronix MX23C8100PC-12)
        IC103:  stereo DAC (NEC uPD6379GR)
        X301:   20MHz crystal

    Service manual with schematics, pinouts, etc.:
    https://revenant1.net/casio/manuals/upd91x/ctk530.pdf

    To access the test mode (not mentioned in the service manual):
    Hold the keypad 0 button while turning on the keyboard, then release the button.
    "TST" will appear on the LCD. Afterwards, press one of these buttons:
    - Keypad 0: switch test (press all front panel buttons in each column, top to bottom and left to right)
    - Keypad 1: key test
    - Keypad 2: ROM test
    - Keypad 4/5/6: sound volume test
    - Keypad 7/8: stereo test
    - Keypad 9: MIDI loopback test
    - Keypad +: LED/display test
    - Mode: power off

-------------------------------------------------------------------------------

    General MIDI modules (1996)

    - GZ-30M
      Basic model, small desktop module
      No 5-pin MIDI jack, only mini-DIN for RS-232 or RS-422
    - GZ-70SP
      MIDI module built into a pair of speakers w/ karaoke mic input
      Provides both standard MIDI and mini-DIN connectors
    - WG-130
      WaveBlaster-style PC daughterboard

    WG-130 board:
        LSI101: stereo DAC (NEC uPD6379GR)
        LSI102: CPU (Casio GT913F)
        LSI103: 16Mbit ROM (Casio GM16000N-C40)
        LSI104: 64kbit SRAM (Sanyo LC3564SM-85)
        LSI105: unpopulated, for DSP SRAM
        LSI106: unpopulated, for DSP
        X101: 30MHz crystal
        X102: unpopulated, for DSP

    All three of these apparently use the same mask ROM.
    This ROM was also distributed as part of Casio's SW-10 softsynth for Windows,
    which it released in early 1997 as part of the "LANA Lite" karaoke system.
    http://web.archive.org/web/20011122112757/www.casio.co.jp/lanalite/LanaSw10.exe

    The WG-130 (and possibly others) have unpopulated footprints for the same DSP
    used in some keyboards (e.g. the CTK-601). The ROM does actually support
    using the DSP if it's present, and responds to the same sysex message used to
    enable reverb on the CTK-601 and similar models (F0 44 0E 09 0x F7).

    Pulling CPU pin 53 (KI0/P24) low starts a ROM checksum test.
    The result is indicated both by sound as well as output on pin 55 (KI2/P11).

    More info and photos:
    https://piano.tyonmage.com/casio/gz-30m.html
    https://piano.tyonmage.com/casio/gz-70sp.html
    http://www.yjfy.com/museum/sound/WG-130.htm

-------------------------------------------------------------------------------

    CTK-601/611 / Concertmate 990 (1997)

    Main board (JCM462-MA1M):
        LSI1: CPU (Casio GT913F)
        LSI2: DSP (Casio GD277F / Hitachi HG51B277FB)
        LSI3: 16Mbit ROM (Macronix MX23C1610MC-12)
        LSI4: 256kbit SRAM for CPU (Toshiba TC55257DFL-70L)
        LSI5: 256kbit SRAM for DSP (same as LSI4)
        LSI6: stereo DAC (NEC uPD6379GR)
        X1:   30MHz crystal for CPU
        X2:   20MHz ceramic for DSP

    Display board (JCM462-LCD1M):
        LSI401: LCD controller (Epson SED1278F2A)

    Service manuals with schematics, pinouts, etc.:
    https://revenant1.net/casio/manuals/upd91x/ctk601.pdf
    https://revenant1.net/casio/manuals/upd91x/ctk611.pdf

    To access the test mode (not mentioned in the service manual):
    Hold the keypad 0 button while turning on the keyboard, then release the button.
    "TST" will appear on the LCD. Afterwards, press one of these buttons:
    - Keypad 0: switch test (press all front panel buttons in a specific order, generally left to right)
    - Keypad 1: pedal and key test
    - Keypad 2: ROM test
    - Keypad 4/5/6: sound volume test
    - Keypad 7/8: stereo test
    - Keypad 9: MIDI loopback test
    - Keypad +: power source test
    - Cursor Left: LCD test (all segments at once)
    - Cursor Right: LCD test (all segments individually)
    - Cursor Down: power off

-------------------------------------------------------------------------------

    CTK-551 (and related models)

    - CTK-531, CTK-533 (1999)
      Basic 61-key model
    - CTK-541, Optimus MD-1150 (1999)
      Adds velocity-sensitive keys
    - CTK-551, CTK-558, Radio Shack MD-1160 (2000)
      Adds pitch wheel and different selection of demo songs
    - CT-588 (2001)
      Chinese localized version of CTK-541
    - CT-688 (2001)
      Chinese localized version of CTK-551

    Main board (JCM453-MA1M / JCM456-MA1M):
        LSI1: CPU (Casio GT913F)
        LSI2: 8Mbit ROM (OKI MSM538002E)
        LSI3: LCD controller (HD44780 compatible)
              May be either a Samsung KS0066U-10B or Epson SED1278F2A.
        IC1:  stereo DAC (NEC uPD6379GR)
        X1:   30MHz ceramic

    Service manuals with schematics, pinouts, etc.:
    https://revenant1.net/casio/manuals/upd91x/ctk531.pdf
    https://revenant1.net/casio/manuals/upd91x/ctk541.pdf

    To access the test mode (not mentioned in the service manual):
    Hold the "Start/Stop" and keypad 0 buttons together when turning on the keyboard.
    "215dTEST" will appear on the LCD. Afterwards, press one of these buttons:
    - Tone: LCD test (press repeatedly)
    - Keypad 0: switch test (press all front panel buttons in a specific order, generally left to right)
    - Keypad 1 or Rhythm: pedal and key test
    - Keypad 2: ROM test
    - Keypad 4/5/6: sound volume test
    - Keypad 7/8: stereo test
    - Keypad 9: MIDI loopback test
    - Keypad + or Song Bank: power source test
    - Keypad -: pitch wheel test
    - FFWD: exit test mode
    - Stop: power off

 */

#include "emu.h"

#include "bus/midi/midiinport.h"
#include "bus/midi/midioutport.h"
#include "cpu/h8/gt913.h"
#include "machine/nvram.h"
#include "video/hd44780.h"
#include "video/pwm.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

#include "ap10.lh"
#include "ctk530.lh"

namespace {

class ctk551_state : public driver_device
{
public:
	ctk551_state(machine_config const &mconfig, device_type type, char const *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_pwm(*this, "pwm")
		, m_lcdc(*this, "lcdc")
		, m_inputs(*this, "IN%u", 0U)
		, m_outputs(*this, "%02x.%d.%d", 0U, 0U, 0U)
		, m_led_touch(*this, "led_touch")
		, m_led_console(*this, "led_console_%d", 0U)
		, m_led_power(*this, "led_power")
	{
	}

	void ap10(machine_config& config);
	void ctk530(machine_config& config);
	void gz70sp(machine_config& config);
	void ctk601(machine_config& config);
	void ctk551(machine_config &config);

	void init_ap10();
	void init_ctk530();
	void init_gz70sp();

	TIMER_CALLBACK_MEMBER(nmi_clear) { m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE); }

	DECLARE_WRITE_LINE_MEMBER(pwm_row_w) { m_pwm->write_my(state); }
	DECLARE_WRITE_LINE_MEMBER(pwm_col_w) { m_pwm->write_mx(state ^ 0xff);  }

	DECLARE_CUSTOM_INPUT_MEMBER(lcd_r)   { return m_lcdc->db_r() >> 4; }
	DECLARE_WRITE_LINE_MEMBER(lcd_w)
	{
		m_lcd_data = state << 4;
		m_lcdc->db_w(m_lcd_data);
	}

	// some models have all 4 LCD bits wired to adjacent port bits, but some don't
	// (and even those don't always have them wired the same way -
	//  in some cases they're not even all connected to the same port)
	template <unsigned Bit>
	DECLARE_CUSTOM_INPUT_MEMBER(lcd_bit_r) { return BIT(m_lcdc->db_r(), Bit); }
	template <unsigned Bit>
	DECLARE_WRITE_LINE_MEMBER(lcd_bit_w)
	{
		m_lcd_data = (m_lcd_data & ~(1 << Bit)) | (state << Bit);
		m_lcdc->db_w(m_lcd_data);
	}

	// handle the 4-position mode switch
	// some models treat this as 3 modes plus power off,
	// while others have 4 modes and move power to a separate button instead
	DECLARE_CUSTOM_INPUT_MEMBER(switch_r)  { return m_switch; }
	DECLARE_INPUT_CHANGED_MEMBER(switch_w);
	DECLARE_INPUT_CHANGED_MEMBER(power_w);
	DECLARE_INPUT_CHANGED_MEMBER(switch_power_w);

	DECLARE_WRITE_LINE_MEMBER(inputs_w) { m_input_sel = state; }
	DECLARE_CUSTOM_INPUT_MEMBER(inputs_r);

	void dsp_data_w(uint8_t data);
	void dsp_cmd_w(uint8_t cmd);

	DECLARE_WRITE_LINE_MEMBER(led_touch_w) { m_led_touch = state; }
	void led_console_w(uint8_t state);
	DECLARE_WRITE_LINE_MEMBER(apo_w);

private:
	void ap10_map(address_map& map);
	void ctk530_map(address_map& map);
	void gz70sp_map(address_map& map);
	void ctk601_map(address_map& map);

	void ap10_io_map(address_map& map);
	void ctk530_io_map(address_map& map);
	void gz70sp_io_map(address_map& map);
	void ctk551_io_map(address_map &map);

	virtual void driver_start() override;

	required_device<gt913_device> m_maincpu;
	optional_device<pwm_display_device> m_pwm;
	optional_device<hd44780_device> m_lcdc;

	emu_timer* m_nmi_timer = nullptr;

	optional_ioport_array<4> m_inputs;

	output_finder<64, 8, 5> m_outputs;
	output_finder<> m_led_touch;
	output_finder<6> m_led_console;
	output_finder<> m_led_power;

	ioport_value m_switch{};
	ioport_value m_input_sel{};

	uint8_t m_lcd_data{};
	uint32_t m_dsp_data{};

	DECLARE_WRITE_LINE_MEMBER(render_w);
};

INPUT_CHANGED_MEMBER(ctk551_state::switch_w)
{
	if (!oldval && newval)
		m_switch = param;
}

INPUT_CHANGED_MEMBER(ctk551_state::power_w)
{
	if (newval)
	{
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
		m_nmi_timer->adjust(attotime::never);
	}
	else
	{
		// give the CPU enough time to switch NMI to active-high so it fires again
		// otherwise, releasing the power button too quickly may be ignored
		m_nmi_timer->adjust(attotime::from_msec(100));
	}
}

INPUT_CHANGED_MEMBER(ctk551_state::switch_power_w)
{
	if (!oldval && newval)
	{
		if (m_switch == 0x1 && param != m_switch)
			m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
		else
			m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);

		m_switch = param;
	}
}

CUSTOM_INPUT_MEMBER(ctk551_state::inputs_r)
{
	uint8_t result = 0xff;
	for (unsigned i = 0U; i < m_inputs.size(); i++)
		if (!BIT(m_input_sel, i))
			result &= m_inputs[i].read_safe(0xff);

	return result;
}

void ctk551_state::dsp_data_w(uint8_t data)
{
	m_dsp_data >>= 8;
	m_dsp_data |= (data << 24);
}

void ctk551_state::dsp_cmd_w(uint8_t data)
{
	logerror("dsp_cmd_w: addr = %02x, data = %08x\n", data, m_dsp_data);
}

void ctk551_state::led_console_w(uint8_t state)
{
	for (unsigned i = 0; i < 6; i++)
		m_led_console[i] = !BIT(state, i);
}

WRITE_LINE_MEMBER(ctk551_state::apo_w)
{
	logerror("apo_w: %x\n", state);
	/* auto power off - disable the LCD and speakers
	the CPU will go to sleep until the power switch triggers a NMI */
	if (!state)
	{
		if (m_pwm.found())
			m_pwm->clear();
		if (m_lcdc.found())
			m_lcdc->reset();
	}
	m_led_power = state;
	m_maincpu->set_output_gain(ALL_OUTPUTS, state ? 1.0 : 0.0);
}


WRITE_LINE_MEMBER(ctk551_state::render_w)
{
	if(!state)
		return;

	const u8 *render = m_lcdc->render();
	for(int x=0; x != 64; x++) {
		for(int y=0; y != 8; y++) {
			u8 v = *render++;
			for(int z=0; z != 5; z++)
				m_outputs[x][y][z] = (v >> z) & 1;
		}
		render += 8;
	}
}


void ctk551_state::ap10_map(address_map& map)
{
	map(0x000000, 0x0fffff).rom().region("maincpu", 0).mirror(0x100000);
	map(0x300000, 0x301fff).ram().share("nvram").mirror(0x07e000);
	// TODO: DSP
	map(0x380000, 0x380000).w(FUNC(ctk551_state::dsp_data_w));
	map(0x380001, 0x380001).w(FUNC(ctk551_state::dsp_cmd_w));
	map(0x380002, 0x380003).noprw();
	map(0x380003, 0x380003).w(FUNC(ctk551_state::led_console_w));
}

void ctk551_state::ctk530_map(address_map& map)
{
	map(0x000000, 0x0fffff).rom().region("maincpu", 0).mirror(0x100000);
}

void ctk551_state::gz70sp_map(address_map& map)
{
	map(0x000000, 0x1fffff).rom().region("maincpu", 0);
	map(0x300000, 0x301fff).ram().mirror(0x07e000);
	map(0x380000, 0x380003).noprw(); // DSP is mapped here, but not actually present
}

void ctk551_state::ctk601_map(address_map& map)
{
	map(0x000000, 0x1fffff).rom().region("maincpu", 0);
	map(0x300000, 0x307fff).ram().mirror(0x078000);
	// TODO: DSP
	map(0x380000, 0x380000).w(FUNC(ctk551_state::dsp_data_w));
	map(0x380001, 0x380001).w(FUNC(ctk551_state::dsp_cmd_w));
	map(0x380002, 0x380003).noprw();
	map(0x380002, 0x380003).portr("PB").portw("PA").umask16(0x00ff);
}

void ctk551_state::ap10_io_map(address_map& map)
{
	map(h8_device::PORT_1, h8_device::PORT_1).portrw("P1").umask16(0x00ff);
	map(h8_device::PORT_2, h8_device::PORT_4).noprw();
	map(h8_device::ADC_0, h8_device::ADC_1).nopr();
}

void ctk551_state::ctk530_io_map(address_map& map)
{
	map(h8_device::PORT_1, h8_device::PORT_1).portrw("P1").umask16(0x00ff);
	map(h8_device::PORT_2, h8_device::PORT_3).noprw();
	map(h8_device::PORT_4, h8_device::PORT_4).portw("PLE");
	map(h8_device::ADC_0, h8_device::ADC_1).nopr();
}

void ctk551_state::gz70sp_io_map(address_map& map)
{
	map(h8_device::PORT_1, h8_device::PORT_1).portrw("P1").umask16(0x00ff);
	map(h8_device::PORT_2, h8_device::PORT_2).portrw("P2").umask16(0x00ff);
	map(h8_device::PORT_3, h8_device::PORT_4).noprw();
	map(h8_device::ADC_0, h8_device::ADC_1).nopr();
}

void ctk551_state::ctk551_io_map(address_map &map)
{
	map(h8_device::PORT_1, h8_device::PORT_1).portr("P1_R").portw("P1_W").umask16(0x00ff);
	map(h8_device::PORT_2, h8_device::PORT_2).portrw("P2").umask16(0x00ff);
	map(h8_device::PORT_3, h8_device::PORT_4).noprw(); // port 3 pins are shared w/ key matrix
	map(h8_device::ADC_0,  h8_device::ADC_0).portr("AN0");
	map(h8_device::ADC_1,  h8_device::ADC_1).portr("AN1");
}

void ctk551_state::driver_start()
{
	m_led_touch.resolve();
	m_led_console.resolve();
	m_led_power.resolve();
	m_outputs.resolve();

	m_nmi_timer = timer_alloc(FUNC(ctk551_state::nmi_clear), this);

	m_input_sel = 0xf;

	save_item(NAME(m_switch));
	save_item(NAME(m_input_sel));
	save_item(NAME(m_lcd_data));
	save_item(NAME(m_dsp_data));
}


void ctk551_state::ap10(machine_config& config)
{
	// CPU
	GT913(config, m_maincpu, 24_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_DATA, &ctk551_state::ap10_map);
	m_maincpu->set_addrmap(AS_IO, &ctk551_state::ap10_io_map);
	m_maincpu->add_route(0, "lspeaker", 1.0);
	m_maincpu->add_route(1, "rspeaker", 1.0);

	NVRAM(config, "nvram");

	// TODO: DSP

	// MIDI
	auto& mdin(MIDI_PORT(config, "mdin"));
	midiin_slot(mdin);
	mdin.rxd_handler().set("maincpu:sci0", FUNC(h8_sci_device::rx_w));

	auto& mdout(MIDI_PORT(config, "mdout"));
	midiout_slot(mdout);
	m_maincpu->subdevice<h8_sci_device>("sci0")->tx_handler().set(mdout, FUNC(midi_port_device::write_txd));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	config.set_default_layout(layout_ap10);
}

void ctk551_state::ctk530(machine_config& config)
{
	// CPU
	GT913(config, m_maincpu, 20_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_DATA, &ctk551_state::ctk530_map);
	m_maincpu->set_addrmap(AS_IO, &ctk551_state::ctk530_io_map);
	m_maincpu->add_route(0, "lspeaker", 1.0);
	m_maincpu->add_route(1, "rspeaker", 1.0);

	// MIDI
	auto& mdin(MIDI_PORT(config, "mdin"));
	midiin_slot(mdin);
	mdin.rxd_handler().set("maincpu:sci0", FUNC(h8_sci_device::rx_w));

	auto& mdout(MIDI_PORT(config, "mdout"));
	midiout_slot(mdout);
	m_maincpu->subdevice<h8_sci_device>("sci0")->tx_handler().set(mdout, FUNC(midi_port_device::write_txd));

	PWM_DISPLAY(config, m_pwm, 0);
	m_pwm->set_size(4, 8);
	m_pwm->set_segmask(0x7, 0xff);

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	config.set_default_layout(layout_ctk530);
}

void ctk551_state::gz70sp(machine_config& config)
{
	// CPU
	GT913(config, m_maincpu, 30_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_DATA, &ctk551_state::gz70sp_map);
	m_maincpu->set_addrmap(AS_IO, &ctk551_state::gz70sp_io_map);
	m_maincpu->add_route(0, "lspeaker", 1.0);
	m_maincpu->add_route(1, "rspeaker", 1.0);

	// MIDI (sci0 for RS232/422, sci1 for standard MIDI)
	auto& mdin(MIDI_PORT(config, "mdin"));
	midiin_slot(mdin);
	mdin.rxd_handler().set("maincpu:sci1", FUNC(h8_sci_device::rx_w));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
}

void ctk551_state::ctk601(machine_config& config)
{
	// CPU
	GT913(config, m_maincpu, 30_MHz_XTAL / 2);
	m_maincpu->set_addrmap(AS_DATA, &ctk551_state::ctk601_map);
	m_maincpu->set_addrmap(AS_IO, &ctk551_state::ctk551_io_map);
	m_maincpu->add_route(0, "lspeaker", 1.0);
	m_maincpu->add_route(1, "rspeaker", 1.0);

	// TODO: DSP

	// MIDI
	auto& mdin(MIDI_PORT(config, "mdin"));
	midiin_slot(mdin);
	mdin.rxd_handler().set("maincpu:sci0", FUNC(h8_sci_device::rx_w));

	auto& mdout(MIDI_PORT(config, "mdout"));
	midiout_slot(mdout);
	m_maincpu->subdevice<h8_sci_device>("sci0")->tx_handler().set(mdout, FUNC(midi_port_device::write_txd));

	// LCD
	HD44780(config, m_lcdc, 0);
	m_lcdc->set_lcd_size(2, 8);

	auto& screen = SCREEN(config, "screen", SCREEN_TYPE_SVG);
	screen.set_refresh_hz(60);
	screen.set_size(1000, 424);
	screen.set_visarea_full();
	screen.screen_vblank().set(FUNC(ctk551_state::render_w));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	m_switch = 0x8;
}

void ctk551_state::ctk551(machine_config &config)
{
	// CPU
	GT913(config, m_maincpu, 30'000'000 / 2);
	m_maincpu->set_addrmap(AS_DATA, &ctk551_state::ctk530_map);
	m_maincpu->set_addrmap(AS_IO, &ctk551_state::ctk551_io_map);
	m_maincpu->add_route(0, "lspeaker", 1.0);
	m_maincpu->add_route(1, "rspeaker", 1.0);

	// MIDI
	auto &mdin(MIDI_PORT(config, "mdin"));
	midiin_slot(mdin);
	mdin.rxd_handler().set("maincpu:sci0", FUNC(h8_sci_device::rx_w));

	auto &mdout(MIDI_PORT(config, "mdout"));
	midiout_slot(mdout);
	m_maincpu->subdevice<h8_sci_device>("sci0")->tx_handler().set(mdout, FUNC(midi_port_device::write_txd));

	// LCD
	HD44780(config, m_lcdc, 0);
	m_lcdc->set_lcd_size(2, 8);

	auto &screen = SCREEN(config, "screen", SCREEN_TYPE_SVG);
	screen.set_refresh_hz(60);
	screen.set_size(1000, 737);
	screen.set_visarea_full();
	screen.screen_vblank().set(FUNC(ctk551_state::render_w));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	m_switch = 0x2;
}

INPUT_PORTS_START(ap10)
	PORT_START("maincpu:kbd:FI0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A0")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A0#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B0")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C1")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C1#")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D1")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D1#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E1")

	PORT_START("maincpu:kbd:FI1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F1")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F1#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G1")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G1#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A1")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A1#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B1")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C2")

	PORT_START("maincpu:kbd:FI2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C2#")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D2")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D2#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E2")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F2")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F2#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G2")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G2#")

	PORT_START("maincpu:kbd:FI3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A2")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A2#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B2")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C3#")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D3")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D3#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E3")

	PORT_START("maincpu:kbd:FI4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F3#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G3")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G3#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A3")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A3#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B3")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C4")

	PORT_START("maincpu:kbd:FI5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C4#")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D4")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D4#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E4")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F4")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F4#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G4")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G4#")

	PORT_START("maincpu:kbd:FI6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A4")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A4#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B4")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C5")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C5#")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D5")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D5#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E5")

	PORT_START("maincpu:kbd:FI7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F5")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F5#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G5")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G5#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A5")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A5#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B5")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C6")

	PORT_START("maincpu:kbd:FI8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C6#")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D6")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D6#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E6")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F6")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F6#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G6")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G6#")

	PORT_START("maincpu:kbd:FI9")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A6")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A6#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B6")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C7")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C7#")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D7")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D7#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E7")

	PORT_START("maincpu:kbd:FI10")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F7")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F7#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G7")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G7#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A7")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A7#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B7")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C8")

	PORT_START("maincpu:kbd:KI0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Transpose / Tune / MIDI") PORT_CODE(KEYCODE_1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Digital Effect")          PORT_CODE(KEYCODE_2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Piano")                   PORT_CODE(KEYCODE_3)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("E. Piano")                PORT_CODE(KEYCODE_4)
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:KI1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Harpsichord") PORT_CODE(KEYCODE_5)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Pipe Organ")  PORT_CODE(KEYCODE_6)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Strings")     PORT_CODE(KEYCODE_7)
	PORT_BIT( 0xf8, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:KI2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Song")         PORT_CODE(KEYCODE_8)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Record")       PORT_CODE(KEYCODE_9)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Start / Stop") PORT_CODE(KEYCODE_0)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Demo")         PORT_CODE(KEYCODE_MINUS)
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:VELOCITY")
	PORT_BIT( 0x7f, 0x7f, IPT_POSITIONAL ) PORT_NAME("Key Velocity") PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_CENTERDELTA(0)

	PORT_START("P1")
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, apo_w)
	PORT_BIT( 0x38, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER  ) PORT_NAME("Damper Pedal")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER  ) PORT_NAME("Soft/Sostenuto Pedal")
INPUT_PORTS_END

INPUT_PORTS_START(gz70sp)
	PORT_START("maincpu:kbd:FI0")
	PORT_START("maincpu:kbd:FI1")
	PORT_START("maincpu:kbd:FI2")
	PORT_START("maincpu:kbd:FI3")
	PORT_START("maincpu:kbd:FI4")
	PORT_START("maincpu:kbd:FI5")
	PORT_START("maincpu:kbd:FI6")
	PORT_START("maincpu:kbd:FI7")
	PORT_START("maincpu:kbd:FI8")
	PORT_START("maincpu:kbd:FI9")
	PORT_START("maincpu:kbd:FI10")
	PORT_START("maincpu:kbd:KI0")
	PORT_START("maincpu:kbd:KI1")
	PORT_START("maincpu:kbd:KI2")

	PORT_START("P1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OUTPUT ) // test mode output (1 = in progress / OK, 0 = error)
	PORT_BIT( 0xfc, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_POWER_ON ) PORT_NAME("Demo") PORT_TOGGLE
	PORT_BIT( 0x0e, IP_ACTIVE_LOW, IPT_UNUSED )
	/*
	this is actually a serial mode switch for the mini-DIN connector
	PORT_CONFNAME( 0x0e, 0x0e, "Connection to Host" )
	PORT_CONFSETTING(    0x06, "PC 1 (RS-232, 31250 baud)" )
	PORT_CONFSETTING(    0x0a, "PC 2 (RS-232, 38400 baud)" )
	PORT_CONFSETTING(    0x0c, "Mac (RS-422, 31250 baud)" )
	PORT_CONFSETTING(    0x0e, "MIDI" )
	*/
	PORT_BIT( 0x10, IP_ACTIVE_LOW, IPT_SERVICE )  PORT_NAME("ROM Test")
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNUSED )
INPUT_PORTS_END

INPUT_PORTS_START(base_61key)
	PORT_START("maincpu:kbd:FI0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C2")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C2#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D2")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D2#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E2")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F2")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F2#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G2")

	PORT_START("maincpu:kbd:FI1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G2#")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A2")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A2#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B2")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C3")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C3#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D3")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D3#")

	PORT_START("maincpu:kbd:FI2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F3")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F3#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G3")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G3#")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A3")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A3#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B3")

	PORT_START("maincpu:kbd:FI3")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C4")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C4#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D4")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D4#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E4")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F4")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F4#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G4")

	PORT_START("maincpu:kbd:FI4")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G4#")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A4")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A4#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B4")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C5")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C5#")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D5")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D5#")

	PORT_START("maincpu:kbd:FI5")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E5")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F5")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F5#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G5")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G5#")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A5")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A5#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B5")

	PORT_START("maincpu:kbd:FI6")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C6")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C6#")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D6")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("D6#")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("E6")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F6")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("F6#")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G6")

	PORT_START("maincpu:kbd:FI7")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("G6#")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A6")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("A6#")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("B6")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("C7")
	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

INPUT_PORTS_START(ctk530)
	PORT_INCLUDE(base_61key)

	PORT_START("maincpu:kbd:FI8")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:FI9")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:FI10")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad +") PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad -") PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 0") PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Accomp Volume Up")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Main Volume Up")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Mode")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:KI0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Demo")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 3") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 2") PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 1") PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tempo Up")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Main Volume Down")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Intro / Fill In")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:KI1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Transpose / Tune / MIDI")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 6") PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 5") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 4") PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Rhythm")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Accomp Volume Down")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Synchro / Ending")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:KI2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Touch Response")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 9") PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 8") PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 7") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tone")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tempo Down")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Start / Stop")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:VELOCITY")
	PORT_BIT( 0x7f, 0x7f, IPT_POSITIONAL ) PORT_NAME("Key Velocity") PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_CENTERDELTA(0)

	PORT_START("SWITCH")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_POWER_ON ) PORT_NAME("Power") PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, power_w, 0)

	PORT_START("P1")
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, apo_w)
	PORT_BIT( 0x78, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_CONFNAME( 0x80, 0x80, "Power Source" )
	PORT_CONFSETTING(    0x80, "AC Adapter" )
	PORT_CONFSETTING(    0x00, "Battery" )

	PORT_START("PLE")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, pwm_col_w)
	PORT_BIT( 0x0f00, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, pwm_row_w)
	PORT_BIT( 0xf000, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

INPUT_PORTS_START(ctk601)
	PORT_INCLUDE(base_61key)

	PORT_START("maincpu:kbd:FI8")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:FI9")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:FI10")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Drum Pad 3")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Drum Pad 1")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Pitch Bend Down")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Cursor Down / Enter")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 9") PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 6") PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad +") PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tone")

	PORT_START("maincpu:kbd:KI0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Drum Pad 4")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Drum Pad 2")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Pitch Bend Up")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Start / Stop")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 8") PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 3") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad -") PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Rhythm")

	PORT_START("maincpu:kbd:KI1")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Drum Pad 5")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Step")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tempo Down")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Cursor Right")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 7") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 2") PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 0") PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Demo")

	PORT_START("maincpu:kbd:KI2")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Drum Pad 6")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Memory")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tempo Up")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Cursor Left")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 4") PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 5") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 1") PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Transpose / Tune / MIDI")

	PORT_START("maincpu:kbd:VELOCITY")
	PORT_BIT( 0x7f, 0x7f, IPT_POSITIONAL ) PORT_NAME("Key Velocity") PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_CENTERDELTA(0)

	PORT_START("P1_R")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(ctk551_state, lcd_bit_r<4>)
	PORT_BIT( 0x06, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(ctk551_state, lcd_bit_r<5>)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(ctk551_state, lcd_bit_r<6>)
	PORT_BIT( 0x60, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(ctk551_state, lcd_bit_r<7>)

	PORT_START("P1_W")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, lcd_bit_w<4>)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, apo_w)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, lcd_bit_w<5>)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, lcd_bit_w<6>)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OUTPUT ) // DSP reset
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, lcd_bit_w<7>)

	PORT_START("P2")
	PORT_BIT( 0x07, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_DEVICE_MEMBER("lcdc", hd44780_device, e_w)
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("AN0")
	PORT_CONFNAME( 0xff, 0x00, "Power Source" )
	PORT_CONFSETTING(    0x00, "AC Adapter" )
	PORT_CONFSETTING(    0xff, "Battery" )

	PORT_START("AN1")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	// DSP ports
	PORT_START("PA")
	PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_MEMBER(ctk551_state, inputs_w)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("lcdc", hd44780_device, rs_w)
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("lcdc", hd44780_device, rw_w)
	PORT_BIT( 0xc0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("PB")
	PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(ctk551_state, inputs_r)
	PORT_BIT( 0x10, IP_ACTIVE_LOW,  IPT_OTHER )  PORT_NAME("Pedal")
	PORT_BIT( 0xe0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("SWITCH")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Mode (Full Range Chord)") PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_w, 0x1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Mode (Fingered)")         PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_w, 0x2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Mode (Casio Chord)")      PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_w, 0x4)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )    PORT_NAME("Mode (Normal)")           PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_w, 0x8)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_POWER_ON ) PORT_NAME("Power")                   PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, power_w, 0)

	PORT_START("IN0")
	PORT_BIT( 0x0f, IP_ACTIVE_LOW, IPT_CUSTOM )  PORT_CUSTOM_MEMBER(ctk551_state, switch_r)

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Intro")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Normal / Fill In")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Synchro / Ending")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Variation / Fill In")

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Touch Response")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Free Session")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Layer")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Split")

	PORT_START("IN3")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Reverb")
	PORT_BIT( 0x02, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Accomp Volume")
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Synth")
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_KEYPAD )  PORT_NAME("Mixer")
INPUT_PORTS_END

INPUT_PORTS_START(ctk551)
	PORT_INCLUDE(base_61key)

	PORT_START("maincpu:kbd:FI8")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 9") PORT_CODE(KEYCODE_9_PAD)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 8") PORT_CODE(KEYCODE_8_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 7") PORT_CODE(KEYCODE_7_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad +") PORT_CODE(KEYCODE_PLUS_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Touch Response")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Song Bank")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Rhythm")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tone")

	PORT_START("maincpu:kbd:FI9")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 6") PORT_CODE(KEYCODE_6_PAD)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 5") PORT_CODE(KEYCODE_5_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 4") PORT_CODE(KEYCODE_4_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad -") PORT_CODE(KEYCODE_MINUS_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Accomp Volume")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Transpose / Tune / MIDI")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Chord Book")

	PORT_START("maincpu:kbd:FI10")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 3") PORT_CODE(KEYCODE_3_PAD)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 2") PORT_CODE(KEYCODE_2_PAD)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 1") PORT_CODE(KEYCODE_1_PAD)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Keypad 0") PORT_CODE(KEYCODE_0_PAD)
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Play / Pause")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Rewind")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Start / Stop")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Sync / Fill In")

	PORT_START("maincpu:kbd:KI0")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Right Hand On/Off")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Left Hand On/Off")
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Fast Forward")
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Stop")
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tempo Down")
	PORT_BIT( 0x20, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Volume Down")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Tempo Up")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_KEYPAD ) PORT_NAME("Volume Up")

	PORT_START("maincpu:kbd:KI1")
	PORT_BIT( 0x0f, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_CUSTOM_MEMBER(ctk551_state, switch_r)

	PORT_START("maincpu:kbd:KI2")
	PORT_BIT( 0xff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("maincpu:kbd:VELOCITY")
	PORT_BIT( 0x7f, 0x7f, IPT_POSITIONAL ) PORT_NAME("Key Velocity") PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_CENTERDELTA(0)

	PORT_START("SWITCH")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("Power Off")          PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_power_w, 0x1)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("Mode (Normal)")      PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_power_w, 0x2)
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("Mode (Casio Chord)") PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_power_w, 0x4)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OTHER )  PORT_NAME("Mode (Fingered)")    PORT_CHANGED_MEMBER(DEVICE_SELF, ctk551_state, switch_power_w, 0x8)

	PORT_START("P1_R")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x02, IP_ACTIVE_LOW,  IPT_OTHER )   PORT_NAME("Pedal")
	PORT_BIT( 0x0c, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_CUSTOM )  PORT_CUSTOM_MEMBER(ctk551_state, lcd_r)

	PORT_START("P1_W")
	PORT_BIT( 0x01, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_MEMBER(ctk551_state, led_touch_w)
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_MEMBER(ctk551_state, apo_w)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_DEVICE_MEMBER("lcdc", hd44780_device, e_w)
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_MEMBER(ctk551_state, lcd_w)

	PORT_START("P2")
	PORT_BIT( 0x03, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x04, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_DEVICE_MEMBER("lcdc", hd44780_device, rs_w)
	PORT_BIT( 0x08, IP_ACTIVE_HIGH, IPT_OUTPUT )  PORT_WRITE_LINE_DEVICE_MEMBER("lcdc", hd44780_device, rw_w)
	PORT_BIT( 0xf0, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("AN0")
	PORT_CONFNAME( 0xff, 0x00, "Power Source" )
	PORT_CONFSETTING(    0x00, "AC Adapter" )
	PORT_CONFSETTING(    0xff, "Battery" )

	PORT_START("AN1")
	PORT_BIT( 0xff, 0x7f, IPT_PADDLE ) PORT_NAME("Pitch Wheel") PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_MINMAX(0x00, 0xff)
INPUT_PORTS_END


ROM_START(ap10)
	ROM_REGION(0x100000, "maincpu", 0)
	ROM_LOAD16_WORD_SWAP("ap10.lsi303", 0x000000, 0x100000, CRC(39caa214) SHA1(3b484628c1e6f0ad7c11e2ec7eff664294f9ec83)) // MX23C8100MC-12CA27
ROM_END

ROM_START(ctk530)
	ROM_REGION(0x100000, "maincpu", ROMREGION_ERASE00)

	ROM_REGION16_BE(0x100000, "lsi102", 0)
	ROM_LOAD16_WORD_SWAP("ctk530.lsi102", 0x000000, 0x100000, CRC(961bff85) SHA1(adfd46ef96fb53981b1b66cb89e3d716b0792ef0)) // MX23C8100PC-12CA19
ROM_END

ROM_START(gz70sp)
	ROM_REGION(0x200000, "maincpu", 0)
	ROM_LOAD("romsxgm.bin", 0x000000, 0x200000, CRC(c392cf89) SHA1(93ebe213ea7a085c67d88974ed39ac3e9bf8059b)) // from the SW-10 softsynth
ROM_END

ROM_START(ctk601)
	ROM_REGION(0x200000, "maincpu", 0)
	ROM_LOAD16_WORD_SWAP("ctk601.lsi3", 0x000000, 0x200000, CRC(23ae6ab1) SHA1(c1a8a1b9af19888360b56587c58602c26ad5029e)) // MX23C1610MC-12CA62

	ROM_REGION(366949, "screen", 0)
	ROM_LOAD("ctk601.svg", 0, 366949, CRC(f150ca5a) SHA1(203fc05171ae6f5ef69c13dc4c0f538fb1ea152b))
ROM_END

ROM_START(ctk551)
	ROM_REGION(0x100000, "maincpu", 0)
	ROM_LOAD16_WORD_SWAP("ctk551.lsi2", 0x000000, 0x100000, CRC(66fc34cd) SHA1(47e9559edc106132f8a83462ed17a6c5c3872157)) // MSM538002E-T6

	ROM_REGION(285279, "screen", 0)
	ROM_LOAD("ctk551lcd.svg", 0, 285279, CRC(1bb5da03) SHA1(a0cf22c6577c4ff0119ee7bb4ba8b487e23872d4))
ROM_END


void ctk551_state::init_ap10()
{
	uint16_t* rom = (uint16_t*)memregion("maincpu")->base();
	for (uint32_t addr = 0; addr < 0x80000; addr++)
		rom[addr] = bitswap(rom[addr], 15, 14, 13, 10, 11, 12, 9, 8, 7, 6, 2, 3, 4, 5, 1, 0);
}

void ctk551_state::init_ctk530()
{
	uint16_t* dest = (uint16_t*)memregion("maincpu")->base();
	const uint16_t* src = (uint16_t*)memregion("lsi102")->base();
	for (uint32_t i = 0; i < 0x80000; i++)
	{
		const uint32_t addr = bitswap(i, 8, 9, 0, 2, 4, 6, 17, 16, 14, 12, 10, 11, 13, 15, 18, 7, 5, 3, 1);
		dest[addr] = bitswap(src[i], 0, 2, 15, 13, 4, 6, 11, 9, 1, 3, 14, 12, 5, 7, 10, 8);
	}
}

void ctk551_state::init_gz70sp()
{
	/*
	the version of this ROM bundled with the SW-10 softsynth has little endian samples, so byteswap them
	(and stop at the end of sample data, not the end of the whole ROM, otherwise the ROM test fails)
	*/
	uint16_t* rom = (uint16_t*)memregion("maincpu")->base();
	for (uint32_t addr = 0x2f000 >> 1; addr < 0x1fe8c2 >> 1; addr++)
		rom[addr] = swapendian_int16(rom[addr]);
}

} // anonymous namespace

// models with MACHINE_IMPERFECT_SOUND are missing DSP emulation
//    YEAR  NAME     PARENT  COMPAT  MACHINE  INPUT   CLASS         INIT         COMPANY  FULLNAME          FLAGS
SYST( 1995, ap10,    0,      0,      ap10,    ap10,   ctk551_state, init_ap10,   "Casio", "Celviano AP-10", MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK | MACHINE_IMPERFECT_SOUND )
SYST( 1995, ctk530,  0,      0,      ctk530,  ctk530, ctk551_state, init_ctk530, "Casio", "CTK-530",        MACHINE_SUPPORTS_SAVE | MACHINE_CLICKABLE_ARTWORK )
SYST( 1996, gz70sp,  0,      0,      gz70sp,  gz70sp, ctk551_state, init_gz70sp, "Casio", "GZ-70SP",        MACHINE_SUPPORTS_SAVE )
SYST( 1997, ctk601,  0,      0,      ctk601,  ctk601, ctk551_state, empty_init,  "Casio", "CTK-601",        MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND )
SYST( 2000, ctk551,  0,      0,      ctk551,  ctk551, ctk551_state, empty_init,  "Casio", "CTK-551",        MACHINE_SUPPORTS_SAVE )
