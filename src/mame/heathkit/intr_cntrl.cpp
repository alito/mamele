// license:BSD-3-Clause
// copyright-holders:Mark Garlanger
/***************************************************************************

  Heathkit Interrupt controller for H89

****************************************************************************/

#include "emu.h"

#include "intr_cntrl.h"

DEFINE_DEVICE_TYPE(HEATH_INTR_CNTRL, heath_intr_cntrl, "heath_intr_cntrl", "Heath H/Z-89 Interrupt Controller");
DEFINE_DEVICE_TYPE(HEATH_Z37_INTR_CNTRL, z37_intr_cntrl, "heath_z37_intr_cntrl", "Heath H/Z-89 with Z-37 Interrupt Controller");

DEFINE_DEVICE_TYPE(HEATH_INTR_SOCKET, heath_intr_socket, "heath_intr_socket", "Heath Interrupt Socket");

/**
 * Heath interrupt interface
 */
device_heath_intr_interface::device_heath_intr_interface(const machine_config &mconfig, device_t &device) :
	device_interface(device, "heathintrdevice"),
	m_socket(dynamic_cast<heath_intr_socket *>(device.owner()))
{
}

/**
 * Original Heath interrrupt controller
 *
 */
heath_intr_cntrl::heath_intr_cntrl(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock):
	heath_intr_cntrl(mconfig, HEATH_INTR_CNTRL, tag, owner, clock)
{
}

heath_intr_cntrl::heath_intr_cntrl(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock):
	device_t(mconfig, type, tag, owner, 0),
	device_heath_intr_interface(mconfig, *this)
{
}

void heath_intr_cntrl::device_start()
{
	save_item(NAME(m_intr_lines));

	m_intr_lines = 0;
}

void heath_intr_cntrl::update_intr_line()
{
	if (m_socket)
	{
		m_socket->raise_irq((m_intr_lines == 0) ? 0 : 1);
	}
}

void heath_intr_cntrl::set_irq_level(uint8_t level, int data)
{
	// only 0 to 7 is valid
	level &= 0x7;

	if (data == 0)
	{
		m_intr_lines &= ~(1 << level);
	}
	else
	{
		m_intr_lines |= 1 << level;
	}

	update_intr_line();
}

uint8_t heath_intr_cntrl::get_instruction()
{

	// determine top priority instruction
	if (!m_intr_lines)
	{
		// should not occur.
		// NO-OP ?
		logerror("get instruct: bad m_intr_lines\n");

		return 0x00;
	}

	// ideally this would be handled with a function like ffs()
	uint8_t level = 0;
	uint8_t mask = 0x01;

	while (mask)
	{
		if (m_intr_lines & mask)
		{
			break;
		}
		level++;
		mask <<= 1;
	}

	if (level > 7)
	{
		logerror("bad level: %d\n", level);
	}

	// return RST based on level
	return 0xc7 | ((level & 0x7) << 3);
}

/**
 * Interrupt controller for the Z37 soft-sectored controller.
 *
 * It will take control of the interrupt system and block all other
 * interrupts while it is waiting for Z37 events.
 */
z37_intr_cntrl::z37_intr_cntrl(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock):
	heath_intr_cntrl(mconfig, HEATH_Z37_INTR_CNTRL, tag, owner, clock)
{
}

void z37_intr_cntrl::update_intr_line()
{
	m_socket->raise_irq(
		(m_irq_raised || m_drq_raised ||
		(!m_intr_blocked && (m_intr_lines != 0))) ? 1 : 0);
}

uint8_t z37_intr_cntrl::get_instruction()
{

	if (m_drq_raised)
	{
		// EI
		return 0xfb;
	}

	if (m_irq_raised)
	{
		// RST 20H (Interrupt 4)
		return 0xe7;
	}

	if (!m_intr_blocked)
	{
		return heath_intr_cntrl::get_instruction();
	}

	// shouldn't get here - NO-OP?
	logerror("Warning: z37 intr get_instruction: fd: %d dr: %d ib: %d\n", m_irq_raised, m_drq_raised, m_intr_blocked);
	return 0x00;
}

void z37_intr_cntrl::set_drq(int state)
{
	m_drq_raised = bool(state);

	update_intr_line();
}


void z37_intr_cntrl::set_irq(int state)
{
	m_irq_raised = bool(state);

	update_intr_line();
}

void z37_intr_cntrl::device_start()
{
	heath_intr_cntrl::device_start();

	save_item(NAME(m_intr_blocked));
	save_item(NAME(m_drq_raised));
	save_item(NAME(m_irq_raised));

	m_intr_blocked = false;
	m_drq_raised = false;
	m_irq_raised = false;
}

void z37_intr_cntrl::block_interrupts(uint8_t data)
{
	m_intr_blocked = bool(data);

	update_intr_line();
}


/**
 * Heath Interrupt socket
 *
 * Allows choice of interrupt controllers for Heath 8-bit computers.
 */
heath_intr_socket::heath_intr_socket(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, HEATH_INTR_SOCKET, tag, owner, clock),
	device_single_card_slot_interface(mconfig, *this),
	m_irq_line(*this),
	m_cntrl(nullptr)
{
}

heath_intr_socket::~heath_intr_socket()
{
}

void heath_intr_socket::device_start()
{
	m_cntrl = get_card_device();
}

IRQ_CALLBACK_MEMBER(heath_intr_socket::irq_callback)
{
	// assume NO-OP
	uint8_t instr = 0x00;

	if (m_cntrl)
	{
		instr = m_cntrl->get_instruction();
	}

	return instr;
}
