// license:BSD-3-Clause
// copyright-holders:Frank Palazzolo

// MAME Reference driver for Grant Searle's Simple Z80 Computer
// http://www.searle.wales/

// All the common emulator stuff is here
#include "emu.h"

// Two member devices referenced in state class, a Z80 and a UART
#include "cpu/z80/z80.h"
#include "machine/6850acia.h"

// Two more devices needed, a clock device for the UART, and RS-232 devices
#include "machine/clock.h"
#include "bus/rs232/rs232.h"

// State class - derives from driver_device
class gsz80_state : public driver_device
{
public:
	gsz80_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")   // Tag name for Z80 is "maincpu"
		, m_acia(*this, "acia")         // Tag name for UART is "acia"
	{ }

	// This function sets up the machine configuration
	void gsz80(machine_config &config);

private:
	// address maps for program memory and io memory
	void gsz80_mem(address_map &map);
	void gsz80_io(address_map &map);

	// two member devices required here
	required_device<cpu_device> m_maincpu;
	required_device<acia6850_device> m_acia;
};

// Trivial memory map for program memory
void gsz80_state::gsz80_mem(address_map &map)
{
	map(0x0000, 0x1fff).rom();
	map(0x2000, 0xffff).ram();
}

void gsz80_state::gsz80_io(address_map &map)
{
	map.global_mask(0xff);  // use 8-bit ports
	map.unmap_value_high(); // unmapped addresses return 0xff
	map(0x80, 0xbf).rw("acia", FUNC(acia6850_device::read), FUNC(acia6850_device::write));
}

// This is here only to configure our terminal for interactive use
static DEVICE_INPUT_DEFAULTS_START( terminal )
	DEVICE_INPUT_DEFAULTS( "RS232_RXBAUD", 0xff, RS232_BAUD_115200 )
	DEVICE_INPUT_DEFAULTS( "RS232_TXBAUD", 0xff, RS232_BAUD_115200 )
	DEVICE_INPUT_DEFAULTS( "RS232_STARTBITS", 0xff, RS232_STARTBITS_1 )
	DEVICE_INPUT_DEFAULTS( "RS232_DATABITS", 0xff, RS232_DATABITS_8 )
	DEVICE_INPUT_DEFAULTS( "RS232_PARITY", 0xff, RS232_PARITY_NONE )
	DEVICE_INPUT_DEFAULTS( "RS232_STOPBITS", 0xff, RS232_STOPBITS_1 )
DEVICE_INPUT_DEFAULTS_END

void gsz80_state::gsz80(machine_config &config)
{
	/* basic machine hardware */

	// Configure member Z80 (via m_maincpu)
	Z80(config, m_maincpu, XTAL(7'372'800));
	m_maincpu->set_addrmap(AS_PROGRAM, &gsz80_state::gsz80_mem);
	m_maincpu->set_addrmap(AS_IO, &gsz80_state::gsz80_io);

	// Configure UART (via m_acia)
	ACIA6850(config, m_acia, 0);
	m_acia->txd_handler().set("rs232", FUNC(rs232_port_device::write_txd));
	m_acia->rts_handler().set("rs232", FUNC(rs232_port_device::write_rts));
	m_acia->irq_handler().set_inputline("maincpu", INPUT_LINE_IRQ0); // Connect interrupt pin to our Z80 INT line

	// Create a clock device to connect to the transmit and receive clock on the 6850
	clock_device &acia_clock(CLOCK(config, "acia_clock", 7'372'800));
	acia_clock.signal_handler().set("acia", FUNC(acia6850_device::write_txc));
	acia_clock.signal_handler().append("acia", FUNC(acia6850_device::write_rxc));

	// Configure a "default terminal" to connect to the 6850, so we have a console
	rs232_port_device &rs232(RS232_PORT(config, "rs232", default_rs232_devices, "terminal"));
	rs232.rxd_handler().set(m_acia, FUNC(acia6850_device::write_rxd));
	rs232.dcd_handler().set(m_acia, FUNC(acia6850_device::write_dcd));
	rs232.cts_handler().set(m_acia, FUNC(acia6850_device::write_cts));
	rs232.set_option_device_input_defaults("terminal", DEVICE_INPUT_DEFAULTS_NAME(terminal)); // must be below the DEVICE_INPUT_DEFAULTS_START block
}

// ROM mapping is trivial, this binary was created from the HEX file on Grant's website
ROM_START(gsz80)
	ROM_REGION(0x2000, "maincpu",0)
	ROM_LOAD("gsz80.bin",   0x0000, 0x2000, CRC(6f4bc7e5) SHA1(9008fe3b9754ec5537b3ad90f748096602ba008e))
ROM_END

// This ties everything together
//    YEAR  NAME         PARENT    COMPAT  MACHINE   INPUT    CLASS        INIT           COMPANY           FULLNAME                FLAGS
COMP( 201?, gsz80,       0,        0,      gsz80,    0,       gsz80_state, empty_init,    "Grant Searle",   "Simple Z-80 Machine",  MACHINE_NO_SOUND_HW )
