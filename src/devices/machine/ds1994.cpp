// license:BSD-3-Clause
// copyright-holders:Aaron Giles, smf, Grull Osgo
/*
 * DS1994
 *
 * Dallas Semiconductor
 * 1-Wire Protocol
 * RTC + BACKUP RAM
 *
 */

// FIXME: convert to device_rtc_interface and remove time.h

#include "emu.h"
#include "machine/ds1994.h"

#include <ctime>

#define LOG_ERRORS          (1U << 1)
#define LOG_1WIRE           (1U << 2)
#define LOG_ROM_COMMANDS    (1U << 3)
#define LOG_COMMANDS        (1U << 4)
#define LOG_STATES          (1U << 5)
#define LOG_RESETS          (1U << 6)
#define LOG_WRITES          (1U << 7)
#define LOG_READS           (1U << 8)

#define VERBOSE (0)
#include "logmacro.h"

// device type definition
DEFINE_DEVICE_TYPE(DS1994, ds1994_device, "ds1994", "DS1994 iButton 4Kb Memory Plus Time")

ds1994_device::ds1994_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, DS1994, tag, owner, clock), device_nvram_interface(mconfig, *this)
	, m_timer_main(nullptr)
	, m_timer_reset(nullptr)
	, m_timer_clock(nullptr)
	, m_ref_year(0)
	, m_ref_month(0)
	, m_ref_day(0)
	, m_address(0)
	, m_offset(0)
	, m_a1(0)
	, m_a2(0)
	, m_bit(0)
	, m_shift(0)
	, m_byte(0)
	, m_rx(false)
	, m_tx(false)
	, m_state_ptr(0)
	, m_offs_ro(false)
{
	memset(m_ram, 0, sizeof(m_ram));
}

void ds1994_device::device_start()
{
	// Reference time setup
	struct tm ref_tm;

	memset(&ref_tm, 0, sizeof(ref_tm));
	ref_tm.tm_year = m_ref_year - 1900;
	ref_tm.tm_mon = m_ref_month - 1;
	ref_tm.tm_mday = m_ref_day;

	time_t ref_time = mktime(&ref_tm);

	time_t current_time;
	time(&current_time);
	current_time -= ref_time;

	m_rtc[0] = 0x0;
	m_rtc[1] = (current_time >> 0) & 0xff;
	m_rtc[2] = (current_time >> 8) & 0xff;
	m_rtc[3] = (current_time >> 16) & 0xff;
	m_rtc[4] = (current_time >> 24) & 0xff;

	// 1-wire timmings
	t_samp = attotime::from_usec(30);
	t_rdv  = attotime::from_usec(30);
	t_rstl = attotime::from_usec(480);
	t_pdh  = attotime::from_usec(30);
	t_pdl  = attotime::from_usec(120);

	// 1-wire states
	m_rx = true;
	m_tx = true;

	// 1-Wire related
	save_item(NAME(m_bit));
	save_item(NAME(m_byte));
	save_item(NAME(m_shift));
	save_item(NAME(m_rx));
	save_item(NAME(m_tx));

	// ds1994 specific
	save_item(NAME(m_rom));
	save_item(NAME(m_sram));
	save_item(NAME(m_ram));
	save_item(NAME(m_rtc));
	save_item(NAME(m_address));
	save_item(NAME(m_a1));
	save_item(NAME(m_a2));
	save_item(NAME(m_offset));

	// state machine
	save_item(NAME(m_state));
	save_item(NAME(m_state_ptr));
	for (auto & elem : m_state)
	elem = STATE_IDLE;

	// timers
	m_timer_main  = timer_alloc(FUNC(ds1994_device::main_tick), this);
	m_timer_reset = timer_alloc(FUNC(ds1994_device::reset_tick), this);
	m_timer_clock = timer_alloc(FUNC(ds1994_device::clock_tick), this);

	m_timer_clock->adjust(attotime::from_hz(256), 0, attotime::from_hz(256));
}

void ds1994_device::device_reset()
{
	m_bit = 0;
	m_byte = 0;
	m_shift = 0;
	m_rx = true;
	m_tx = true;
	m_state_ptr = 0;
	m_state[m_state_ptr] = STATE_IDLE;

	memory_region *region = memregion(DEVICE_SELF);
	if (region != nullptr)
	{
		if (region->bytes() == ROM_SIZE + SPD_SIZE + DATA_SIZE + RTC_SIZE + REGS_SIZE)
		{
			memcpy(m_rom,  region->base(), ROM_SIZE);
			memcpy(m_ram,  region->base() + ROM_SIZE, SPD_SIZE);
			memcpy(m_sram, region->base() + ROM_SIZE + SPD_SIZE, DATA_SIZE);
			memcpy(m_rtc,  region->base() + ROM_SIZE + SPD_SIZE + DATA_SIZE, RTC_SIZE);
			memcpy(m_regs, region->base() + ROM_SIZE + SPD_SIZE + DATA_SIZE + RTC_SIZE, REGS_SIZE);
			return;
		}
		LOGMASKED(LOG_ERRORS, "ds1994 %s: Wrong region length for data, expected 0x%x, got 0x%x\n", tag(), ROM_SIZE + SPD_SIZE + DATA_SIZE + RTC_SIZE + REGS_SIZE, region->bytes());
	}
	else
	{
		LOGMASKED(LOG_ERRORS, "ds1994 %s: Warning, no id provided, answer will be all zeroes.\n", tag());
		memset(m_rom,  0, ROM_SIZE);
		memset(m_ram,  0, SPD_SIZE);
		memset(m_sram, 0, DATA_SIZE);
		memset(m_rtc,  0, RTC_SIZE);
		memset(m_regs, 0, REGS_SIZE);
	}
}

/********************************************/
/*                                          */
/*   1-wire protocol - Tx/Rx Bit Routines   */
/*                                          */
/********************************************/

bool ds1994_device::one_wire_tx_bit(uint8_t value)
{
	if (!m_bit)
	{
		m_shift = value;
		LOGMASKED(LOG_1WIRE, "one_wire_tx_bit: Byte to send %02x\n", m_shift);
	}
	m_tx = m_shift & 1;
	m_shift >>= 1;
	LOGMASKED(LOG_1WIRE, "one_wire_tx_bit: State %d\n", m_tx);
	m_bit++;
	if (m_bit == 8) return true;
	else
		return false;
}

bool ds1994_device::one_wire_rx_bit(void)
{
	m_shift >>= 1;
	if (m_rx)
	{
		m_shift |= 0x80;
	}
	LOGMASKED(LOG_1WIRE, "one_wire_rx_bit: State %d\n", m_rx);
	m_bit++;
	if (m_bit == 8)
	{
		LOGMASKED(LOG_1WIRE, "one_wire_rx_bit: Byte Received %02x\n", m_shift);
		return true;
	}
	 else
		return false;
}

/********************************************/
/*                                          */
/*   Internal states - Rom Commands         */
/*                                          */
/********************************************/

void ds1994_device::handle_rom_cmd(void)
{
	LOGMASKED(LOG_ROM_COMMANDS, "rom_cmd\n");
	if (one_wire_rx_bit())
	{
		switch (m_shift)
		{
			case ROMCMD_READROM:
				LOGMASKED(LOG_ROM_COMMANDS, "rom_cmd readrom\n");
				m_bit = 0;
				m_byte = 0;
				m_state[0] = STATE_READROM;
				m_state_ptr = 0;
				break;
			case ROMCMD_SKIPROM:
				LOGMASKED(LOG_ROM_COMMANDS, "rom_cmd skiprom\n");
				m_bit = 0;
				m_byte = 0;
				m_state[0] = STATE_COMMAND;
				m_state_ptr = 0;
				break;
			case ROMCMD_MATCHROM:
				LOGMASKED(LOG_ROM_COMMANDS, "rom_cmd matchrom\n");
				m_bit = 0;
				m_byte = 0;
				m_state[0] = STATE_MATCHROM;
				m_state_ptr = 0;
				break;
			case ROMCMD_SEARCHROM:
			case ROMCMD_SEARCHINT:
				LOGMASKED(LOG_ERRORS, "rom_command not implemented %02x\n", m_shift);
				m_state[m_state_ptr] = STATE_COMMAND;
				break;
			default:
				LOGMASKED(LOG_ERRORS, "rom_command not found %02x\n", m_shift);
				m_state[m_state_ptr] = STATE_IDLE;
				break;
		}
	}
}

/********************************************/
/*                                          */
/*   Internal states - DS1994 Commands      */
/*                                          */
/********************************************/

void ds1994_device::handle_cmd(void)
{
	LOGMASKED(LOG_COMMANDS, "state_command\n");
	if (one_wire_rx_bit())
	{
		switch (m_shift)
		{
			case COMMAND_READ_MEMORY:
				LOGMASKED(LOG_COMMANDS, "cmd read_memory\n");
				m_bit = 0;
				m_byte = 0;
				m_state[0] = STATE_ADDRESS1;
				m_state[1] = STATE_ADDRESS2;
				m_state[2] = STATE_INIT_COMMAND;
				m_state[3] = STATE_READ_MEMORY;
				m_state_ptr = 0;
				break;
			case COMMAND_WRITE_SCRATCHPAD:
				LOGMASKED(LOG_COMMANDS, "cmd write_scratchpad\n");
				m_bit = 0;
				m_byte = 0;
				m_offs_ro = false;
				m_state[0] = STATE_ADDRESS1;
				m_state[1] = STATE_ADDRESS2;
				m_state[2] = STATE_INIT_COMMAND;
				m_state[3] = STATE_WRITE_SCRATCHPAD;
				m_state_ptr = 0;
				break;
			case COMMAND_READ_SCRATCHPAD:
				LOGMASKED(LOG_COMMANDS, "cmd read_scratchpad\n");
				m_bit = 0;
				m_byte = 0;
				m_state[0] = STATE_TXADDRESS1;
				m_state[1] = STATE_TXADDRESS2;
				m_state[2] = STATE_TXOFFSET;
				m_state[3] = STATE_INIT_COMMAND;
				m_state[4] = STATE_READ_SCRATCHPAD;
				m_state_ptr = 0;
				break;
			case COMMAND_COPY_SCRATCHPAD:
				LOGMASKED(LOG_COMMANDS, "cmd copy_scratchpad\n");
				m_bit = 0;
				m_byte = 0;
				m_offs_ro = true;
				m_auth = true;
				m_state[0] = STATE_ADDRESS1;
				m_state[1] = STATE_ADDRESS2;
				m_state[2] = STATE_OFFSET;
				m_state[3] = STATE_INIT_COMMAND;
				m_state[4] = STATE_COPY_SCRATCHPAD;
				m_state_ptr = 0;
				break;
			default:
				LOGMASKED(LOG_COMMANDS, "command not handled %02x\n", m_shift);
				m_state[m_state_ptr] = STATE_IDLE;
				break;
		}
	}
}

/********************************************/
/*                                          */
/*   Internal Routines - Memory R/W         */
/*                                          */
/********************************************/

uint8_t ds1994_device::readmem()
{
	if (m_address < 0x200)
	{
		return m_sram[m_address];
	}
	else
	{
		if (m_address >= 0x202 && m_address <= 0x206)
		{
			return m_rtc[m_address - 0x202];
		}
	}
	return 0;
}

void ds1994_device::writemem(uint8_t value)
{
	if (m_address < 0x200)
	{
		m_sram[m_address] = value;
	}
	else
	{
		if (m_address >= 0x202 && m_address <= 0x206)
		{
			m_rtc[m_address - 0x202] = value;
		}
	}
}

/*************************************************/
/*                                               */
/*   Internal states - Timer controlled Events   */
/*                                               */
/*************************************************/

TIMER_CALLBACK_MEMBER(ds1994_device::main_tick)
{
	switch (m_state[m_state_ptr])
	{
		case STATE_RESET1:
			LOGMASKED(LOG_STATES, "timer_main state_reset1 %d\n", m_rx);
			m_tx = false;
			m_state[m_state_ptr] = STATE_RESET2;
			m_timer_main->adjust(t_pdl);
			break;
		case STATE_RESET2:
			LOGMASKED(LOG_STATES, "timer_main state_reset2 %d\n", m_rx);
			m_tx = true;
			m_bit = 0;
			m_shift = 0;
			m_state[m_state_ptr] = STATE_ROMCMD;
			break;
		case STATE_ROMCMD:
			handle_rom_cmd();
			break;
		case STATE_COMMAND:
			handle_cmd();
			break;
		case STATE_MATCHROM:
			LOGMASKED(LOG_STATES, "timer_main state_matchrom - Data to match: <- %d\n", m_rx);
			if (one_wire_rx_bit())
			{
				if (m_rom[7- m_byte] == m_shift)
				{
					m_byte++;
					m_bit = 0;
					LOGMASKED(LOG_STATES, "timer_main state_matchrom: datamatch %x - byte=%d\n", m_shift, m_byte);
				}
				else
				{
					m_state[m_state_ptr] = STATE_IDLE;
					LOGMASKED(LOG_STATES, "timer_main state_matchrom: no match rx=%x <> mem=%x - byte:%d\n", m_shift, m_rom[7 - m_byte], m_byte);
				}
			}
			if (m_byte == ROM_SIZE)
			{
				LOGMASKED(LOG_STATES, "timer_main matchrom finished\n");
				m_state[m_state_ptr] = STATE_COMMAND;
			}
			break;
		case STATE_ADDRESS1:
			LOGMASKED(LOG_STATES, "timer_main state_address1\n");
			if (one_wire_rx_bit())
			{
				m_bit = 0;
				if (m_offs_ro)
				{
					if (m_a1 != m_shift) m_auth = false;
					LOGMASKED(LOG_STATES, "timer_main state_address1 - TA1=%02x  - Auth_Code 1=%02x\n", m_a1, m_shift);
				}
				else
				{
					m_a1 = m_shift & 0xff;
					LOGMASKED(LOG_STATES, "timer_main state_address1 - Address1=%02x\n", m_a1);
				}
				m_state_ptr++;
			}
			break;
		case STATE_ADDRESS2:
			LOGMASKED(LOG_STATES, "timer_main state_address2\n");
			if (one_wire_rx_bit())
			{
				m_bit = 0;
				if (m_offs_ro)
				{
					if ( m_a2 != m_shift )
						m_auth = false;
					LOGMASKED(LOG_STATES, "timer_main state_address1 - TA2=%02x  - Auth_Code 2=%02x\n", m_a1, m_shift);
				}
				else
				{
					m_a2 = m_shift & 0xff;
					LOGMASKED(LOG_STATES, "timer_main state_address2 - Address2=%02x\n", m_a2);
				}
				m_state_ptr++;
			}
			break;
		case STATE_OFFSET:
			LOGMASKED(LOG_STATES, "timer_main state_offset\n");
			if (one_wire_rx_bit())
			{
				m_bit = 0;
				if (m_offs_ro)
				{
					if (m_a2 != m_shift)
						m_auth = false;
					LOGMASKED(LOG_STATES, "timer_main state_address1 - OFS_ES=%02x  - Auth_Code 3=%02x\n", m_offset, m_shift);
				}
				else
				{
					m_offset = m_shift & 0x1f;
					LOGMASKED(LOG_STATES, "timer_main state_address2 - Offset=%02x\n", m_offset);
				}
				m_state_ptr++;
			}
			break;
		case STATE_WRITE_SCRATCHPAD:
			LOGMASKED(LOG_STATES, "timer_main state_write_scratchpad\n");
			if (one_wire_rx_bit())
			{
				m_bit = 0;
				m_ram[m_offset & 0x1f] = m_shift & 0xff;
				m_offset++;
			}
			LOGMASKED(LOG_STATES, "timer_main state_write_scratchpad %d Offs=%02x\n", m_rx, m_offset);
			break;
		case STATE_READROM:
			m_tx = true;
			if (m_byte == ROM_SIZE)
			{
				LOGMASKED(LOG_STATES, "timer_main readrom finished\n");
				m_state[m_state_ptr] = STATE_COMMAND;
			}
			else
				LOGMASKED(LOG_STATES, "timer_main readrom window closed\n");
			break;
		case STATE_TXADDRESS1:
			m_tx = true;
			if (m_byte == 1)
			{
				LOGMASKED(LOG_STATES, "timer_main txaddress1 finished  m_byte=%d\n", m_byte);
				m_byte = 0;
				m_state_ptr++;
			}
			else
				LOGMASKED(LOG_STATES, "timer_main txaddress1 window closed\n");
			break;
		case STATE_TXADDRESS2:
			m_tx = true;
			if (m_byte == 1)
			{
				LOGMASKED(LOG_STATES, "timer_main txaddress2 finished m_byte=%d\n", m_byte);
				m_byte = 0;
				m_state_ptr++;
			}
			else
				LOGMASKED(LOG_STATES, "timer_main txaddress2 window closed\n");
			break;
		case STATE_TXOFFSET:
			m_tx = true;
			if (m_byte == 1)
			{
				LOGMASKED(LOG_STATES, "timer_main txoffset finished  - m_byte=%d\n", m_byte);
				m_byte = 0;
				m_state_ptr++;
			}
			else
				LOGMASKED(LOG_STATES, "timer_main txoffset window closed\n");
			break;
		case STATE_READ_MEMORY:
			LOGMASKED(LOG_STATES, "timer_main state_readmemory\n");
			break;
		case STATE_COPY_SCRATCHPAD:
			LOGMASKED(LOG_STATES, "timer_main state_copy_scratchpad\n");
			break;
		case STATE_READ_SCRATCHPAD:
			LOGMASKED(LOG_STATES, "timer_main state_read_scratchpad\n");
			break;
		default:
			LOGMASKED(LOG_ERRORS, "timer_main state not handled: %d\n", m_state[m_state_ptr]);
			break;
	}

	if (m_state[m_state_ptr] == STATE_INIT_COMMAND)
	{
		switch (m_state[m_state_ptr + 1])
		{
			case STATE_IDLE:
			case STATE_COMMAND:
			case STATE_ADDRESS1:
			case STATE_ADDRESS2:
			case STATE_OFFSET:
				break;
			case STATE_READ_MEMORY:
				LOGMASKED(LOG_STATES, "timer_main (init_cmd) -> state_read_memory - set address\n");
				m_address = (m_a2 << 8) | m_a1;
				break;
			case STATE_WRITE_SCRATCHPAD:
				LOGMASKED(LOG_STATES, "timer_main (init_cmd) -> state_write_scratchpad - set address\n");
				m_offs_ro = false;
				m_offset = 0;
				break;
			case STATE_READ_SCRATCHPAD:
				LOGMASKED(LOG_STATES, "timer_main (init_cmd) -> state_read_scratchpad - set address\n");
				m_address = 0;
				break;
			case STATE_COPY_SCRATCHPAD:
				LOGMASKED(LOG_STATES, "timer_main (init_cmd) -> state_copy_scratchpad - do copy\n");
				if (m_auth)
				{
					m_address = (m_a2 << 8) | m_a1;
					for (int i = 0; i <= m_offset; i++)
					{
						writemem(m_ram[i]);
						m_address++;
					}
				}
				else
					LOGMASKED(LOG_STATES, "timer_main (init_cmd) -> state_copy_scratchpad - Auth-Rejected\n");
				break;
		}
		m_state_ptr++;
	}
}

TIMER_CALLBACK_MEMBER(ds1994_device::reset_tick)
{
	LOGMASKED(LOG_RESETS, "timer_reset\n");
	m_state[m_state_ptr] = STATE_RESET;
	m_timer_reset->adjust(attotime::never);
}

TIMER_CALLBACK_MEMBER(ds1994_device::clock_tick)
{
	for (uint8_t &elem : m_rtc)
	{
		elem++;
		if (elem != 0)
		{
			break;
		}
	}
}

/*********************/
/*                   */
/*   Write Handler   */
/*                   */
/*********************/

WRITE_LINE_MEMBER(ds1994_device::write)
{
	LOGMASKED(LOG_WRITES, "write(%d)\n", state);
	if (!state && m_rx)
	{
		switch (m_state[m_state_ptr])
		{
			case STATE_IDLE:
			case STATE_INIT_COMMAND:
				break;
			case STATE_ROMCMD:
				LOGMASKED(LOG_WRITES, "state_romcommand\n");
				m_timer_main->adjust(t_samp);
				break;
			case STATE_COMMAND:
				LOGMASKED(LOG_WRITES, "state_command\n");
				m_timer_main->adjust(t_samp);
				break;
			case STATE_ADDRESS1:
				LOGMASKED(LOG_WRITES, "state_address1\n");
				m_timer_main->adjust(t_samp);
				break;
			case STATE_ADDRESS2:
				LOGMASKED(LOG_WRITES, "state_address2\n");
				m_timer_main->adjust(t_samp);
				break;
			case STATE_OFFSET:
				LOGMASKED(LOG_WRITES, "state_offset\n");
				m_timer_main->adjust(t_samp);
				break;
			case STATE_TXADDRESS1:
				LOGMASKED(LOG_WRITES, "state_txaddress1\n");
				if (one_wire_tx_bit(m_a1))
				{
					m_bit = 0;
					m_byte++;
				}
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_TXADDRESS2:
				LOGMASKED(LOG_WRITES, "state_txaddress2\n");
				if (one_wire_tx_bit(m_a2))
				{
					m_bit = 0;
					m_byte++;
				}
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_TXOFFSET:
				LOGMASKED(LOG_WRITES, "state_txoffset\n");
				if (one_wire_tx_bit(m_offset))
				{
					m_bit = 0;
					m_byte++;
				}
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_READROM:
				LOGMASKED(LOG_WRITES, "state_readrom\n");
				if (one_wire_tx_bit(m_rom[7 - m_byte]))
				{
					m_bit = 0;
					m_byte++;
				}
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_READ_MEMORY:
				LOGMASKED(LOG_WRITES, "state_read_memory\n");
				if (one_wire_tx_bit(readmem()))
				{
					m_bit = 0;
					if (m_address < DATA_SIZE + RTC_SIZE + REGS_SIZE)
						m_address++;
					else
						m_tx = true;
				}
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_MATCHROM:
				LOGMASKED(LOG_WRITES, "state_matchrom\n");
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_COPY_SCRATCHPAD:
				if (m_auth)
				{
					LOGMASKED(LOG_WRITES, "state_copy_scratchpad Auth_Code Match: %d\n", m_tx);
					m_tx = true;
					m_auth = false;
				}
				else
				{
					m_tx = false;
					LOGMASKED(LOG_WRITES, "state_copy_scratchpad Auth_Code No Match: %d\n", m_tx);
				}
				m_timer_main->adjust(t_rdv);
				break;
			case STATE_WRITE_SCRATCHPAD:
				LOGMASKED(LOG_WRITES, "state_write_scratchpad\n");
				m_timer_main->adjust(t_samp);
				break;
			case STATE_READ_SCRATCHPAD:
				LOGMASKED(LOG_WRITES, "state_read_scratchpad\n");
				if (one_wire_tx_bit(m_ram[m_address]))
				{
					m_bit = 0;
					if (m_address <= m_offset)
						m_address++;
					else
						m_tx = true;
				}
				m_timer_main->adjust(t_rdv);
				break;
			default:
				LOGMASKED(LOG_ERRORS | LOG_WRITES, "state not handled: %d\n", m_state[m_state_ptr]);
				break;
		}
		m_timer_reset->adjust(t_rstl);
	}
	else
	if (state && !m_rx)
	{
		switch (m_state[m_state_ptr])
		{
		case STATE_RESET:
			m_state[m_state_ptr] = STATE_RESET1;
			m_timer_main->adjust(t_pdh);
			break;
		}
		m_timer_reset->adjust(attotime::never);
	}
	m_rx = state;
}

/*********************/
/*                   */
/*   Read  Handler   */
/*                   */
/*********************/

READ_LINE_MEMBER(ds1994_device::read)
{
	LOGMASKED(LOG_READS, "read %d\n", m_tx && m_rx);
	return m_tx && m_rx;
}

//-------------------------------------------------
//  nvram_default - called to initialize NVRAM to
//  its default state
//-------------------------------------------------

void ds1994_device::nvram_default()
{
	memset(m_rom,  0, ROM_SIZE);
	memset(m_ram,  0, SPD_SIZE);
	memset(m_sram, 0, DATA_SIZE);
	memset(m_rtc,  0, RTC_SIZE);
	memset(m_regs, 0, REGS_SIZE);
}

//-------------------------------------------------
//  nvram_read - called to read NVRAM from the
//  .nv file
//-------------------------------------------------

bool ds1994_device::nvram_read(util::read_stream &file)
{
	size_t actual;
	bool result =      !file.read(m_rom,  ROM_SIZE,  actual) && actual == ROM_SIZE;
	result = result && !file.read(m_ram,  SPD_SIZE,  actual) && actual == SPD_SIZE;
	result = result && !file.read(m_sram, DATA_SIZE, actual) && actual == DATA_SIZE;
	result = result && !file.read(m_rtc,  RTC_SIZE,  actual) && actual == RTC_SIZE;
	result = result && !file.read(m_regs, REGS_SIZE, actual) && actual == REGS_SIZE;
	return result;
}

//-------------------------------------------------
//  nvram_write - called to write NVRAM to the
//  .nv file
//-------------------------------------------------

bool ds1994_device::nvram_write(util::write_stream &file)
{
	size_t actual;
	bool result =      !file.write(m_rom,  ROM_SIZE,  actual) && actual == ROM_SIZE;
	result = result && !file.write(m_ram,  SPD_SIZE,  actual) && actual == SPD_SIZE;
	result = result && !file.write(m_sram, DATA_SIZE, actual) && actual == DATA_SIZE;
	result = result && !file.write(m_rtc,  RTC_SIZE,  actual) && actual == RTC_SIZE;
	result = result && !file.write(m_regs, REGS_SIZE, actual) && actual == REGS_SIZE;
	return result;
}
