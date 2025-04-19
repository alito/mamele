// license:BSD-3-Clause
// copyright-holders:Vas Crabb
#include "emu.h"
#include "a2parprn.h"

//#define VERBOSE 1
//#define LOG_OUTPUT_FUNC osd_printf_info
#include "logmacro.h"

namespace {

// FIXME: get proper PROM dumps.
/*
    There are most likely multiple revisions.
    6309 and 74471 PROMs were used.

    Example silkscreen:

    SINGAPORE
    341-0005-00
    cAPPLE 78-02
    SN74S471N

    Example label:

    APPLE
    ©1978

    PROM image here is from entering a listing and zero-filling unused areas.

    ********************************
    *                              *
    *   PRINTER CARD I FIRMWARE    *
    *                              *
    *        WOZ   11/1/77         *
    *     APPLE COMPUTER INC.      *
    *     ALL RIGHTS RESERVED      *
    *                              *
    ********************************
*/
ROM_START(parprn)
	ROM_REGION(0x100, "prom", 0)
	ROM_LOAD( "prom.b4", 0x0000, 0x0100, BAD_DUMP CRC(00b742ca) SHA1(c67888354aa013f9cb882eeeed924e292734e717) )
ROM_END


INPUT_PORTS_START(parprn)
	PORT_START("CFG")
	PORT_CONFNAME(0x01, 0x00, "Acknowledge latching edge")
	PORT_CONFSETTING(   0x00, "Falling (/Y-B)")
	PORT_CONFSETTING(   0x01, "Rising (Y-B)")
	PORT_CONFNAME(0x06, 0x02, "Printer ready")
	PORT_CONFSETTING(   0x00, "Always (S5-C-D)")
	PORT_CONFSETTING(   0x02, "Acknowledge latch (Z-C-D)")
	PORT_CONFSETTING(   0x04, "ACK (Y-C-D)")
	PORT_CONFSETTING(   0x06, "/ACK (/Y-C-D)")
	PORT_CONFNAME(0x08, 0x00, "Strobe polarity")
	PORT_CONFSETTING(   0x00, "Negative (S5-A-/X, GND-X)")
	PORT_CONFSETTING(   0x08, "Positive (S5-X, GND-A-/X)")
	PORT_CONFNAME(0x10, 0x10, "Character width")
	PORT_CONFSETTING(   0x00, "7-bit")
	PORT_CONFSETTING(   0x10, "8-bit")
INPUT_PORTS_END

} // anonymous namespace



DEFINE_DEVICE_TYPE(A2BUS_PARPRN, a2bus_parprn_device, "a2parprn", "Apple II Parallel Printer Interface Card")



a2bus_parprn_device::a2bus_parprn_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock) :
	device_t(mconfig, A2BUS_PARPRN, tag, owner, clock),
	device_a2bus_card_interface(mconfig, *this),
	m_printer_conn(*this, "prn"),
	m_printer_out(*this, "prn_out"),
	m_input_config(*this, "CFG"),
	m_prom(*this, "prom"),
	m_strobe_timer(nullptr),
	m_next_strobe(0U),
	m_ack_latch(0U),
	m_ack_in(1U)
{
}



//----------------------------------------------
//  device_a2bus_card_interface implementation
//----------------------------------------------

u8 a2bus_parprn_device::read_c0nx(u8 offset)
{
	if (!machine().side_effects_disabled())
	{
		LOG("Read C0n%01X (effective open-bus write)\n", offset);

		// R/W is ignored, so it has the same effect as a write
		// the bus is open, but 74LS logic has low-impedance inputs so it's likely to latch 0xff
		write_c0nx(offset, 0xffU);
	}

	return 0x00U;
}


void a2bus_parprn_device::write_c0nx(u8 offset, u8 data)
{
	LOG("Write C0n%01X=%02X\n", offset, data);
	if (m_ack_latch)
		LOG("Clearing acknowledge latch\n");
	else
		LOG("Previous data not acknowledged\n");

	ioport_value const cfg(m_input_config->read());

	m_printer_out->write(data & (BIT(cfg, 8) ? 0xffU : 0x7fU));
	m_printer_conn->write_strobe(BIT(~cfg, 3));
	m_next_strobe = BIT(cfg, 3);
	m_ack_latch = 0U;
	m_strobe_timer->adjust(attotime::from_ticks(1, clock()));
}


u8 a2bus_parprn_device::read_cnxx(u8 offset)
{
	ioport_value const cfg(m_input_config->read());

	if (BIT(cfg, 2))
	{
		if (!BIT(offset, 6) || (BIT(offset, 7) && (BIT(cfg, 1) != m_ack_in)))
			offset |= 0x40U;
		else
			offset &= 0xbfU;
	}
	else if (BIT(cfg, 1))
	{
		if (!BIT(offset, 6) || (BIT(offset, 7) && !m_ack_latch))
			offset |= 0x40U;
		else
			offset &= 0xbfU;
	}
	else
	{
		offset ^= 0x40U;
	}

	return m_prom[offset];
}



//----------------------------------------------
//  device_t implementation
//----------------------------------------------

tiny_rom_entry const *a2bus_parprn_device::device_rom_region() const
{
	return ROM_NAME(parprn);
}


void a2bus_parprn_device::device_add_mconfig(machine_config &config)
{
	CENTRONICS(config, m_printer_conn, centronics_devices, "printer");
	m_printer_conn->ack_handler().set(FUNC(a2bus_parprn_device::ack_w));

	OUTPUT_LATCH(config, m_printer_out);
	m_printer_conn->set_output_latch(*m_printer_out);
}


ioport_constructor a2bus_parprn_device::device_input_ports() const
{
	return INPUT_PORTS_NAME(parprn);
}


void a2bus_parprn_device::device_start()
{
	m_strobe_timer = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(a2bus_parprn_device::update_strobe), this));

	save_item(NAME(m_next_strobe));
	save_item(NAME(m_ack_latch));
	save_item(NAME(m_ack_in));
}


void a2bus_parprn_device::device_reset()
{
	m_ack_latch = 1U;
}



//----------------------------------------------
//  printer status inputs
//----------------------------------------------

WRITE_LINE_MEMBER(a2bus_parprn_device::ack_w)
{
	if (bool(state) != bool(m_ack_in))
	{
		m_ack_in = state ? 1U : 0U;
		LOG("ACK=%u\n", m_ack_in);
		if (started() && (m_ack_in == BIT(m_input_config->read(), 0)))
		{
			LOG("Active ACK edge\n");
			m_ack_latch = 1U;
		}
	}
}



//----------------------------------------------
//  timer handlers
//----------------------------------------------

TIMER_CALLBACK_MEMBER(a2bus_parprn_device::update_strobe)
{
	ioport_value const cfg(m_input_config->read());

	LOG("Output /STROBE=%u\n", m_next_strobe);
	m_printer_conn->write_strobe(m_next_strobe);
	if (m_next_strobe == BIT(cfg, 3))
	{
		LOG("Start strobe timer\n");
		m_next_strobe = BIT(~cfg, 3);
		m_strobe_timer->adjust(attotime::from_ticks(1, clock()));
	}
}
