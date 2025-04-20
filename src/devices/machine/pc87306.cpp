// license:BSD-3-Clause
// copyright-holders: Angelo Salese
/***************************************************************************

National Semiconductor PC87306 Super I/O

TODO:
- Barely enough to make it surpass POST test 0x05 in misc/odyssey.cpp

***************************************************************************/

#include "emu.h"
#include "bus/isa/isa.h"
//#include "machine/ds128x.h"
#include "machine/pc87306.h"

#define LOG_WARN        (1U << 1) // Show warnings

#define VERBOSE (LOG_GENERAL | LOG_WARN)
//#define LOG_OUTPUT_FUNC osd_printf_info
#include "logmacro.h"

#define LOGWARN(...)       LOGMASKED(LOG_WARN, __VA_ARGS__)

DEFINE_DEVICE_TYPE(PC87306, pc87306_device, "pc87306", "National Semiconductor PC87306 Super I/O Enhanced Sidewinder Lite")

pc87306_device::pc87306_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, PC87306, tag, owner, clock)
	, device_isa16_card_interface(mconfig, *this)
	, device_memory_interface(mconfig, *this)
	, m_space_config("superio_config_regs", ENDIANNESS_LITTLE, 8, 8, 0, address_map_constructor(FUNC(pc87306_device::config_map), this))
	, m_kbdc(*this, "pc_kbdc")
	, m_rtc(*this, "rtc")
	//, m_logical_view(*this, "logical_view")
	, m_gp20_reset_callback(*this)
	, m_gp25_gatea20_callback(*this)
	, m_irq1_callback(*this)
	, m_irq8_callback(*this)
	, m_irq9_callback(*this)
//  , m_txd1_callback(*this)
//  , m_ndtr1_callback(*this)
//  , m_nrts1_callback(*this)
//  , m_txd2_callback(*this)
//  , m_ndtr2_callback(*this)
//  , m_nrts2_callback(*this)
{ }


void pc87306_device::device_start()
{
	set_isa_device();
	//m_isa->set_dma_channel(0, this, true);
	//m_isa->set_dma_channel(1, this, true);
	//m_isa->set_dma_channel(2, this, true);
	//m_isa->set_dma_channel(3, this, true);
	// TODO: CFG0 (RTC) + MR (KBD)
	m_krr = 8 | 1;
	remap(AS_IO, 0, 0x400);
}

void pc87306_device::device_reset()
{
	m_locked_state = 2;
}

device_memory_interface::space_config_vector pc87306_device::memory_space_config() const
{
	return space_config_vector {
		std::make_pair(0, &m_space_config)
	};
}

void pc87306_device::device_add_mconfig(machine_config &config)
{
	// TODO: can bank thru bit 5 of KRR
	DS12885(config, m_rtc, 32.768_kHz_XTAL);
//  m_rtc->irq().set(FUNC(pc87306_device::irq_rtc_w));
	m_rtc->set_century_index(0x32);

	KBDC8042(config, m_kbdc);
	m_kbdc->set_keyboard_type(kbdc8042_device::KBDC8042_PS2);
	m_kbdc->set_interrupt_type(kbdc8042_device::KBDC8042_DOUBLE);
	m_kbdc->input_buffer_full_callback().set(FUNC(pc87306_device::irq_keyboard_w));
	m_kbdc->input_buffer_full_mouse_callback().set(FUNC(pc87306_device::irq_mouse_w));
	m_kbdc->system_reset_callback().set(FUNC(pc87306_device::kbdp20_gp20_reset_w));
	m_kbdc->gate_a20_callback().set(FUNC(pc87306_device::kbdp21_gp25_gatea20_w));
}

void pc87306_device::remap(int space_id, offs_t start, offs_t end)
{
	if (space_id == AS_IO)
	{
		// TODO: BADDR1/0 config pin controlled
		u16 superio_base = 0x0398;
		m_isa->install_device(superio_base, superio_base + 1, read8sm_delegate(*this, FUNC(pc87306_device::read)), write8sm_delegate(*this, FUNC(pc87306_device::write)));

		if (BIT(m_krr, 0))
		{
			m_isa->install_device(0x60, 0x60, read8sm_delegate(*m_kbdc, FUNC(kbdc8042_device::data_r)), write8sm_delegate(*m_kbdc, FUNC(kbdc8042_device::data_w)));
			m_isa->install_device(0x64, 0x64, read8sm_delegate(*this, FUNC(pc87306_device::keybc_status_r)), write8sm_delegate(*this, FUNC(pc87306_device::keybc_command_w)));
		}

		if (BIT(m_krr, 3))
			m_isa->install_device(0x70, 0x71, read8sm_delegate(*m_rtc, FUNC(ds12885_device::read)), write8sm_delegate(*m_rtc, FUNC(ds12885_device::write)));
	}
}

u8 pc87306_device::read(offs_t offset)
{
	if (m_locked_state)
	{
		if (!machine().side_effects_disabled())
			m_locked_state --;
		return (m_locked_state) ? 0x88 : 0x00;
	}

	if (offset == 0)
		return m_index;

	return space().read_byte(m_index);
}

void pc87306_device::write(offs_t offset, u8 data)
{
	if (offset == 0)
	{
		m_index = data;
	}
	else
	{
		// TODO: two writes, first one just unlocks?
		space().write_byte(m_index, data);
	}
}

void pc87306_device::config_map(address_map &map)
{
//  map(0x00, 0x00) FER Function Enable Register
//  map(0x01, 0x01) FAR Function Address Register
//  map(0x02, 0x02) PTR Power and Test Register
//  map(0x03, 0x03) FCR Function Control Register
//  map(0x04, 0x04) PCR Printer Control Register
	map(0x05, 0x05).rw(FUNC(pc87306_device::krr_r), FUNC(pc87306_device::krr_w));
//  map(0x06, 0x06) PMC Power Management Control Register
//  map(0x07, 0x07) TUP Tape, UART and Parallel Port Configuration Register
	// SID Super I/O Identification Register
	// bits 7-3 -> 01110 TL/C/12379-27
	// bits 2-0 -> <undefined>
	map(0x08, 0x08).lr8(
		NAME([] (offs_t offset) { return 0x70; })
	);
//  map(0x09, 0x09) ASC Advanced Super I/O Configuration Register
//  map(0x0a, 0x0a) CS0LA Chip Select 0 Low Address
//  map(0x0b, 0x0b) CS0CF Chip Select 0 Configuration Address
//  map(0x0c, 0x0c) CS1LA Chip Select 1 Low Address
//  map(0x0d, 0x0d) CS1CF Chip Select 1 Configuration Address
//  map(0x0e, 0x0e) IRC InfraRed Configuration Register
//  map(0x0f, 0x0f) GPBA General Purpose I/O Port Base Address
//  map(0x10, 0x10) CS0HA Chip Select 0 High Address
//  map(0x11, 0x11) CS1HA Chip Select 1 High Address
//  map(0x12, 0x12) SCF0 Super I/O Configuration Register 0
//  map(0x18, 0x18) SCF1 Super I/O Configuration Register 1
//  map(0x19, 0x19) LPTBA LPT Base Address

//  map(0x1b, 0x1b) PNP0 Plug and Play Configuration 0 Register
//  map(0x1c, 0x1c) PNP1 Plug and Play Configuration 1 Register
}

// [0x05] KRR KBC and RTC Control Register
/*
 * x--- ---- KBC clock source select (0) X1 clock (1) SYSCLK
 * --x- ---- RAMSREL RTC bank select
 * ---- x--- RTC enable bit
 * ---- -1-- <unknown> must be 1 for KBC to work
 * ---- --x- KBC speed control (0) X1 / 3 (1) X1 / 2, ignored when SYSCLK selected
 * ---- ---x KBC Enable bit
 *
 */
u8 pc87306_device::krr_r(offs_t offset)
{
	return m_krr;
}

void pc87306_device::krr_w(offs_t offset, u8 data)
{
	m_krr = data;
	LOG("krr %02x\n", data);
	if (data & 0x20)
		LOGWARN("RAMSREL active\n");
	remap(AS_IO, 0, 0x400);
}

u8 pc87306_device::keybc_status_r(offs_t offset)
{
	return (m_kbdc->data_r(4) & 0xfb) | 0x10; // bios needs bit 2 to be 0 as powerup and bit 4 to be 1
}

void pc87306_device::keybc_command_w(offs_t offset, u8 data)
{
	m_kbdc->data_w(4, data);
}

/*
 * KBC lines
 */
// Unlike other Super I/Os this doesn't seem to have irq line relocation

void pc87306_device::kbdp21_gp25_gatea20_w(int state)
{
	if (!BIT(m_krr, 0))
		return;
	m_gp25_gatea20_callback(state);
}

void pc87306_device::kbdp20_gp20_reset_w(int state)
{
	if (!BIT(m_krr, 0))
		return;
	m_gp20_reset_callback(state);
}

void pc87306_device::irq_keyboard_w(int state)
{
	if (!BIT(m_krr, 0))
		return;
	request_irq(1, state ? ASSERT_LINE : CLEAR_LINE);
}

void pc87306_device::irq_mouse_w(int state)
{
	if (!BIT(m_krr, 0))
		return;
	request_irq(12, state ? ASSERT_LINE : CLEAR_LINE);
}

void pc87306_device::request_irq(int irq, int state)
{
	switch (irq)
	{
	case 1:
		m_irq1_callback(state);
		break;
	case 3:
		m_isa->irq3_w(state);
		break;
	case 4:
		m_isa->irq4_w(state);
		break;
	case 5:
		m_isa->irq5_w(state);
		break;
	case 6:
		m_isa->irq6_w(state);
		break;
	case 7:
		m_isa->irq7_w(state);
		break;
	case 8:
		m_irq8_callback(state);
		break;
	case 9:
		m_irq9_callback(state);
		break;
	case 10:
		m_isa->irq10_w(state);
		break;
	case 11:
		m_isa->irq11_w(state);
		break;
	case 12:
		m_isa->irq12_w(state);
		break;
	case 14:
		m_isa->irq14_w(state);
		break;
	case 15:
		m_isa->irq15_w(state);
		break;
	}
}
