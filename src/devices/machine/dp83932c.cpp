// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

/*
 * An implementation of the National Semiconductor DP83932C SONIC™ (Systems-
 * Oriented Network Interface Controller) device.
 *
 * References:
 *
 *   http://bitsavers.org/components/national/_dataBooks/1995_National_Ethernet_Databook.pdf
 *
 * TODO
 *   - bus mode (big endian, interrupts active low)
 *   - byte count mismatch
 *   - data widths
 *   - tally counters
 *   - software reset
 *   - watchdog timers
 *   - loopback modes
 *   - programmable outputs and extended bus mode
 */

#include "emu.h"
#include "dp83932c.h"
#include "hashing.h"

#define LOG_GENERAL (1U << 0)
#define LOG_COMMAND (1U << 1)
#define LOG_FILTER  (1U << 2)
#define LOG_PACKETS (1U << 3)

//#define VERBOSE (LOG_GENERAL|LOG_COMMAND|LOG_FILTER|LOG_PACKETS)
#include "logmacro.h"

#define EA(hi, lo) ((u32(hi) << 16) | lo)

DEFINE_DEVICE_TYPE(DP83932C, dp83932c_device, "dp83932c", "National Semiconductor DP83932C SONIC")

static constexpr u32 FCS_RESIDUE = 0xdebb20e3;

static char const *const regname[] =
{
	"CR",    "DCR",   "RCR",   "TCR",   "IMR",   "ISR",   "UTDA",  "CTDA",
	"TPS",   "TFC",   "TSA0",  "TSA1",  "TFS",   "URDA",  "CRDA",  "CRBA0",
	"CRBA1", "RBWC0", "RBWC1", "EOBC",  "URRA",  "RSA",   "REA",   "RRP",
	"RWP",   "TRBA0", "TRBA1", "TBWC0", "TBWC1", "ADDR0", "ADDR1", "LLFA",

	"TTDA",  "CEP",   "CAP2",  "CAP1",  "CAP0",  "CE",    "CDP",   "CDC",
	"SR",    "WT0",   "WT1",   "RSC",   "CRCT",  "FAET",  "MPT",   "MDT",
	"30",    "31",    "32",    "33",    "34",    "35",    "36",    "37",
	"38",    "39",    "3a",    "3b",    "3c",    "3d",    "3e",    "DCR2",
};

static u16 const regmask[] =
{
	0x03bf,  0xbfff,  0xfe00,  0xf000,  0x7fff,  0x7fff,  0xffff,  0xffff,
	0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xfffe,  0xfffe,  0xfffe,
	0xfffe,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,

	0xffff,  0x000f,  0x0000,  0x0000,  0x0000,  0xffff,  0xfffe,  0x001f,
	0x0000,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0x0000,
	0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,
	0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xffff,  0xf017,
};

dp83932c_device::dp83932c_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock)
	: device_t(mconfig, DP83932C, tag, owner, clock)
	, device_network_interface(mconfig, *this, 10.0f)
	, m_bus(*this, finder_base::DUMMY_TAG, 0)
	, m_out_int(*this)
	, m_int_state(false)
{
}

void dp83932c_device::map(address_map &map)
{
	map(0x00, 0x7f).rw(FUNC(dp83932c_device::reg_r), FUNC(dp83932c_device::reg_w));
}

void dp83932c_device::device_start()
{
	m_out_int.resolve();

	m_command = machine().scheduler().timer_alloc(timer_expired_delegate(FUNC(dp83932c_device::command), this));

	save_item(NAME(m_int_state));
	save_item(NAME(m_reg));
	save_item(NAME(m_cam));

	for (u16 &reg : m_reg)
		reg = 0;

	m_reg[SR] = 6; // silicon revision for DP83932CVF
}

void dp83932c_device::device_reset()
{
	m_reg[CR] = CR_RST | CR_STP | CR_RXDIS;
	m_reg[DCR] &= ~(DCR_EXBUS | DCR_LBR); // TODO: sample USR1,0
	m_reg[RCR] &= ~(RCR_RNT | RCR_BRD | RCR_LB);
	m_reg[TCR] |= TCR_NCRS | TCR_PTX;
	m_reg[TCR] &= ~(TCR_TPC | TCR_BCM);
	m_reg[IMR] = 0;
	m_reg[ISR] = 0;
	m_reg[EOBC] = 0x02f8;
	m_reg[CE] = 0;
	m_reg[RSC] = 0;
	m_reg[DCR2] = 0;

	update_interrupts();
}

int dp83932c_device::recv_start_cb(u8 *buf, int length)
{
	unsigned const width = (m_reg[DCR] & DCR_DW) ? 4 : 2;

	if (!(m_reg[CR] & CR_RXEN))
		return -1;

	m_reg[RCR] &= ~(RCR_MC | RCR_BC | RCR_LPKT | RCR_CRCR | RCR_FAER | RCR_LBK | RCR_PRX);

	// address filter
	if (!address_filter(buf))
		return -1;

	if ((length < 64) && !(m_reg[RCR] & RCR_RNT))
		return -1;

	u32 const fcs = util::crc32_creator::simple(buf, length);
	if (~fcs != FCS_RESIDUE)
	{
		if (m_reg[RCR] & RCR_ERR)
			m_reg[RCR] |= RCR_CRCR;
		else
			return -1;
	}
	else
		m_reg[RCR] |= RCR_PRX;

	LOG("recv_start_cb %d\n", length);

	// loopback
	if (m_reg[RCR] & RCR_LB)
		m_reg[RCR] |= RCR_LBK;

	dump_bytes(buf, length);

	if (m_reg[CRDA] & 1)
	{
		// re-read the previous descriptor link field
		m_reg[CRDA] = m_bus->read_word(EA(m_reg[URDA], m_reg[LLFA]));
		if (m_reg[CRDA] & 1)
		{
			logerror("no receive descriptor available\n");
			return -2;
		}
	}

	// save rba pointer registers
	m_reg[TRBA0] = m_reg[CRBA0];
	m_reg[TRBA1] = m_reg[CRBA1];
	m_reg[TBWC0] = m_reg[RBWC0];
	m_reg[TBWC1] = m_reg[RBWC1];

	// store data to rba
	// TODO: word/dword transfers (allow unaligned)
	// TODO: pad to word/dword size with 0xff
	// TODO: check for buffer overflow
	offs_t const rba = EA(m_reg[CRBA1], m_reg[CRBA0]);
	for (unsigned i = 0; i < length; i++)
		m_bus->write_byte(rba + i, buf[i]);

	// update remaining buffer word count
	u32 const rbwc = ((u32(m_reg[RBWC1]) << 16) | m_reg[RBWC0]) - (length + 1) / 2;
	m_reg[RBWC1] = rbwc >> 16;
	m_reg[RBWC0] = u16(rbwc);

	if (rbwc < m_reg[EOBC])
		m_reg[RCR] |= RCR_LPKT;

	// write status to rda
	// TODO: don't write the rda if rba limit exceeded (buffer overflow)
	offs_t const rda = EA(m_reg[URDA], m_reg[CRDA]);
	m_bus->write_word(rda + 0 * width, m_reg[RCR]);
	m_bus->write_word(rda + 1 * width, length);
	m_bus->write_word(rda + 2 * width, m_reg[CRBA0]);
	m_bus->write_word(rda + 3 * width, m_reg[CRBA1]);
	m_bus->write_word(rda + 4 * width, m_reg[RSC]);
	m_reg[LLFA] = m_reg[CRDA] + 5 * width;
	m_reg[CRDA] = m_bus->read_word(rda + 5 * width);

	// check for end of list
	if (m_reg[CRDA] & 1)
		m_reg[ISR] |= ISR_RDE;
	else
		m_bus->write_word(rda + 6 * width, 0);

	// handle buffer exhaustion
	if (rbwc < m_reg[EOBC])
		read_rra();
	else
		m_reg[RSC] = (m_reg[RSC] & 0xff00) | u8(m_reg[RSC] + 1);

	return length;
}

void dp83932c_device::recv_complete_cb(int result)
{
	if (result > 0)
	{
		m_reg[ISR] |= ISR_PKTRX;
		update_interrupts();
	}
}

void dp83932c_device::reg_w(offs_t offset, u16 data)
{
	LOG("reg_w register %s data 0x%04x (%s)\n", regname[offset], data, machine().describe_context());

	// TODO: can only write during reset: DCR, DCR2

	switch (offset)
	{
	case CR:
		if (m_reg[CR] & CR_RST)
		{
			if (!(data & CR_RST))
			{
				LOGMASKED(LOG_COMMAND, "exit software reset\n");

				m_reg[CR] &= ~CR_RST;
			}

			return;
		}

		if (data & CR_RST)
		{
			LOGMASKED(LOG_COMMAND, "enter software reset\n");

			m_command->adjust(attotime::never);

			m_reg[CR] &= ~(CR_LCAM | CR_RRRA | CR_TXP | CR_HTX);
			m_reg[CR] |= (CR_RST | CR_RXDIS);

			return;
		}

		m_reg[offset] |= data & regmask[offset];
		m_command->adjust(attotime::zero, data & regmask[offset]);
		break;

	case RCR:
		m_reg[offset] = (m_reg[offset] & ~regmask[offset]) | (data & regmask[offset]);
		set_loopback(bool(m_reg[offset] & RCR_LB));
		break;

	case IMR:
		m_reg[offset] = (m_reg[offset] & ~regmask[offset]) | (data & regmask[offset]);
		update_interrupts();
		break;

	case ISR:
		m_reg[offset] &= ~(data & regmask[offset]);
		// TODO: reload rra after RBE cleared
		update_interrupts();
		break;

	case CRCT:
	case FAET:
	case MPT:
		// inverted
		m_reg[offset] = ~data;
		break;

	default:
		if (regmask[offset])
			m_reg[offset] = (m_reg[offset] & ~regmask[offset]) | (data & regmask[offset]);
		else
			logerror("write to read-only register %s data 0x%04x (%s)\n",
				regname[offset], data, machine().describe_context());
		break;
	}
}

void dp83932c_device::command(void *ptr, s32 param)
{
	if (param & CR_HTX)
	{
		LOGMASKED(LOG_COMMAND, "halt transmission\n");
		m_reg[CR] &= ~CR_TXP;
	}

	if (param & CR_TXP)
	{
		LOGMASKED(LOG_COMMAND, "transmit\n");
		m_reg[CR] &= ~CR_HTX;

		transmit();
	}

	if (param & CR_RXDIS)
	{
		LOGMASKED(LOG_COMMAND, "receiver disable\n");
		m_reg[CR] &= ~CR_RXEN;
	}

	if (param & CR_RXEN)
	{
		LOGMASKED(LOG_COMMAND, "receiver enable\n");
		m_reg[CR] &= ~CR_RXDIS;
	}

	if (param & CR_STP)
	{
		LOGMASKED(LOG_COMMAND, "stop timer\n");
		m_reg[CR] &= ~CR_ST;
	}

	if (param & CR_ST)
	{
		LOGMASKED(LOG_COMMAND, "start timer\n");
		m_reg[CR] &= ~CR_STP;
	}

	if (param & CR_RRRA)
	{
		LOGMASKED(LOG_COMMAND, "read rra\n");
		read_rra(true);
	}

	if (param & CR_LCAM)
	{
		LOGMASKED(LOG_COMMAND, "load cam\n");
		load_cam();
	}

	update_interrupts();
}

void dp83932c_device::transmit()
{
	unsigned const width = (m_reg[DCR] & DCR_DW) ? 4 : 2;

	m_reg[TTDA] = m_reg[CTDA];
	offs_t const tda = EA(m_reg[UTDA], m_reg[CTDA]);
	unsigned word = 1;

	// read control information from tda and load registers
	u16 const tcr = m_reg[TCR];
	m_reg[TCR] = m_bus->read_word(tda + word++ * width) & TCR_TPC;
	m_reg[TPS] = m_bus->read_word(tda + word++ * width);
	m_reg[TFC] = m_bus->read_word(tda + word++ * width);

	// check for programmable interrupt
	if ((m_reg[TCR] & TCR_PINT) && !(tcr & TCR_PINT))
		m_reg[ISR] |= ISR_PINT;

	// FIXME: abort if tps > buffer size
	u8 buf[1520];
	unsigned length = 0;

	// read fragments into buffer
	for (unsigned fragment = 0; fragment < m_reg[TFC]; fragment++)
	{
		// read fragment address and size
		m_reg[TSA0] = m_bus->read_word(tda + word++ * width);
		m_reg[TSA1] = m_bus->read_word(tda + word++ * width);
		m_reg[TFS] = m_bus->read_word(tda + word++ * width);

		offs_t const tsa = EA(m_reg[TSA1], m_reg[TSA0]);

		// FIXME: word/dword transfers (allow unaligned)
		for (unsigned byte = 0; byte < m_reg[TFS]; byte++)
			buf[length++] = m_bus->read_byte(tsa + byte);
	}

	// append fcs if not inhibited
	if (!(m_reg[TCR] & TCR_CRCI))
	{
		u32 const crc = util::crc32_creator::simple(buf, length);

		// insert the fcs
		buf[length++] = crc >> 0;
		buf[length++] = crc >> 8;
		buf[length++] = crc >> 16;
		buf[length++] = crc >> 24;
	}

	// advance ctda to the link field
	m_reg[CTDA] += word * width;

	// transmit data
	dump_bytes(buf, length);
	send(buf, length);
}

void dp83932c_device::send_complete_cb(int result)
{
	// TODO: errors
	if (result > 0)
	{
		// TODO: number of collisions

		m_reg[TCR] |= TCR_PTX;
	}

	// write descriptor status
	m_bus->write_word(EA(m_reg[UTDA], m_reg[TTDA]), m_reg[TCR] & TCR_TPS);

	// check for halt
	if (!(m_reg[CR] & CR_HTX))
	{
		// load next descriptor address
		m_reg[CTDA] = m_bus->read_word(EA(m_reg[UTDA], m_reg[CTDA]));

		// check for end of list
		if (m_reg[CTDA] & 1)
		{
			m_reg[ISR] |= ISR_TXDN;
			m_reg[CR] &= ~CR_TXP;

			update_interrupts();
		}
		else
			// transmit next packet
			if (m_command->enabled())
				m_command->set_param(m_command->param() | CR_TXP);
			else
				m_command->adjust(attotime::zero, CR_TXP);
	}
	else
		m_reg[CR] &= ~CR_TXP;
}

void dp83932c_device::read_rra(bool command)
{
	unsigned const width = (m_reg[DCR] & DCR_DW) ? 4 : 2;

	offs_t const rrp = EA(m_reg[URRA], m_reg[RRP]);

	m_reg[CRBA0] = m_bus->read_word(rrp + 0 * width);
	m_reg[CRBA1] = m_bus->read_word(rrp + 1 * width);
	m_reg[RBWC0] = m_bus->read_word(rrp + 2 * width);
	m_reg[RBWC1] = m_bus->read_word(rrp + 3 * width);

	LOG("read_rra crba 0x%08x rbwc 0x%08x\n",
		EA(m_reg[CRBA1], m_reg[CRBA0]), EA(m_reg[RBWC1], m_reg[RBWC0]));

	// advance rrp
	m_reg[RRP] += 4 * width;

	// check for wrapping and resource exhaustion
	if (m_reg[RRP] == m_reg[REA])
		m_reg[RRP] = m_reg[RSA];

	if (m_reg[RRP] == m_reg[RWP])
		m_reg[ISR] |= ISR_RBE;

	if (command)
		m_reg[CR] &= ~CR_RRRA;
	else
		m_reg[RSC] = (m_reg[RSC] & 0xff00) + 0x100;
}

void dp83932c_device::load_cam()
{
	unsigned const width = (m_reg[DCR] & DCR_DW) ? 4 : 2;

	while (m_reg[CDC])
	{
		offs_t const cdp = EA(m_reg[URRA], m_reg[CDP]);

		u16 const cep = m_bus->read_word(cdp + 0 * width) & 0xf;
		u16 const cap0 = m_bus->read_word(cdp + 1 * width);
		u16 const cap1 = m_bus->read_word(cdp + 2 * width);
		u16 const cap2 = m_bus->read_word(cdp + 3 * width);

		// FIXME: documented byte/word order doesn't match emulation

		LOG("load_cam entry %2d %02x:%02x:%02x:%02x:%02x:%02x\n",
			cep, u8(cap0), cap0 >> 8, u8(cap1), cap1 >> 8, u8(cap2), cap2 >> 8);

		m_cam[cep] =
			(u64(u8(cap0 >> 0)) << 40) | (u64(u8(cap0 >> 8)) << 32) | (u64(u8(cap1 >> 0)) << 24) |
			(u64(u8(cap1 >> 8)) << 16) | (u64(u8(cap2 >> 0)) << 8)  | (u64(u8(cap2 >> 8)) << 0);

		m_reg[CDP] += 4 * width;
		m_reg[CDC]--;
	}

	// read cam enable
	m_reg[CE] = m_bus->read_word(EA(m_reg[URRA], m_reg[CDP]));
	LOG("load_cam enable 0x%04x\n", m_reg[CE]);

	m_reg[CR] &= ~CR_LCAM;
	m_reg[ISR] |= ISR_LCD;
}

void dp83932c_device::update_interrupts()
{
	bool const int_state = bool(m_reg[ISR] & m_reg[IMR]);

	if (int_state != m_int_state)
	{
		m_int_state = int_state;
		m_out_int(m_int_state);
	}
}

bool dp83932c_device::address_filter(u8 *buf)
{
	if (m_reg[RCR] & RCR_PRO)
	{
		LOGMASKED(LOG_FILTER, "address_filter accepted (promiscuous)\n");

		return true;
	}

	u64 const address =
		(u64(buf[0]) << 40) | (u64(buf[1]) << 32) | (u64(buf[2]) << 24) |
		(u64(buf[3]) << 16) | (u64(buf[4]) << 8) | (u64(buf[5]) << 0);

	// broadcast
	if ((address == 0xffff'ffffffffULL) && (m_reg[RCR] & (RCR_AMC | RCR_BRD)))
	{
		LOGMASKED(LOG_FILTER, "address_filter accepted (broadcast) %02x-%02x-%02x-%02x-%02x-%02x\n",
			buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

		m_reg[RCR] |= RCR_BC;
		return true;
	}

	// multicast
	if ((address & 0x0100'00000000ULL) && (m_reg[RCR] & RCR_AMC))
	{
		LOGMASKED(LOG_FILTER, "address_filter accepted (multicast) %02x-%02x-%02x-%02x-%02x-%02x\n",
			buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

		m_reg[RCR] |= RCR_MC;
		return true;
	}

	// content addressable memory
	for (unsigned i = 0; i < 16; i++)
	{
		if ((address == m_cam[i]) && BIT(m_reg[CE], i))
		{
			LOGMASKED(LOG_FILTER, "address_filter accepted (cam entry %d match) %02x-%02x-%02x-%02x-%02x-%02x\n",
				i, buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

			return true;
		}
	}

	return false;
}

void dp83932c_device::dump_bytes(u8 *buf, int length)
{
	if (VERBOSE & LOG_PACKETS)
	{
		// pad with zeros to 8-byte boundary
		for (int i = 0; i < 8 - (length % 8); i++)
			buf[length + i] = 0;

		// dump length / 8 (rounded up) groups of 8 bytes
		for (int i = 0; i < (length + 7) / 8; i++)
			LOGMASKED(LOG_PACKETS, "%02x %02x %02x %02x %02x %02x %02x %02x\n",
				buf[i * 8 + 0], buf[i * 8 + 1], buf[i * 8 + 2], buf[i * 8 + 3],
				buf[i * 8 + 4], buf[i * 8 + 5], buf[i * 8 + 6], buf[i * 8 + 7]);
	}
}
