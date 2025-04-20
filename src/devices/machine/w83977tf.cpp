// license:BSD-3-Clause
// copyright-holders: Angelo Salese
/***************************************************************************

Winbond W83977TF

TODO:
- PoC for a generic (LPC) Super I/O type, to be merged with fdc37c93x;
- savquest (in pciagp) fails keyboard self test (PC=e140c reads bit 0 high
  from port $64?)

***************************************************************************/

#include "emu.h"
#include "bus/isa/isa.h"
//#include "machine/ds128x.h"
#include "machine/w83977tf.h"

#define VERBOSE (LOG_GENERAL)
//#define LOG_OUTPUT_FUNC osd_printf_info
#include "logmacro.h"

DEFINE_DEVICE_TYPE(W83977TF, w83977tf_device, "w83977tf", "Winbond W83977TF Super I/O")

w83977tf_device::w83977tf_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, W83977TF, tag, owner, clock)
	, device_isa16_card_interface(mconfig, *this)
	, device_memory_interface(mconfig, *this)
	, m_space_config("superio_config_regs", ENDIANNESS_LITTLE, 8, 8, 0, address_map_constructor(FUNC(w83977tf_device::config_map), this))
	, m_kbdc(*this, "pc_kbdc")
	, m_rtc(*this, "rtc")
	, m_logical_view(*this, "logical_view")
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

void w83977tf_device::device_start()
{
	set_isa_device();
	//m_isa->set_dma_channel(0, this, true);
	//m_isa->set_dma_channel(1, this, true);
	//m_isa->set_dma_channel(2, this, true);
	//m_isa->set_dma_channel(3, this, true);
	remap(AS_IO, 0, 0x400);

}

void w83977tf_device::device_reset()
{
	m_index = 0;
	m_hefras = 0;
	m_lock_sequence = 2;
	m_keyb_address[0] = 0x60;
	m_keyb_address[1] = 0x64;
}

device_memory_interface::space_config_vector w83977tf_device::memory_space_config() const
{
	return space_config_vector {
		std::make_pair(0, &m_space_config)
	};
}

void w83977tf_device::device_add_mconfig(machine_config &config)
{
	// doc doesn't explicitly mention this being at 8, assume from intialization
	DS12885(config, m_rtc, 32.768_kHz_XTAL);
	m_rtc->irq().set(FUNC(w83977tf_device::irq_rtc_w));
	m_rtc->set_century_index(0x32);

	KBDC8042(config, m_kbdc);
	m_kbdc->set_keyboard_type(kbdc8042_device::KBDC8042_PS2);
	m_kbdc->set_interrupt_type(kbdc8042_device::KBDC8042_DOUBLE);
	m_kbdc->system_reset_callback().set(FUNC(w83977tf_device::kbdp20_gp20_reset_w));
	m_kbdc->gate_a20_callback().set(FUNC(w83977tf_device::kbdp21_gp25_gatea20_w));
	m_kbdc->input_buffer_full_callback().set(FUNC(w83977tf_device::irq_keyboard_w));
	m_kbdc->input_buffer_full_mouse_callback().set(FUNC(w83977tf_device::irq_mouse_w));
}


void w83977tf_device::remap(int space_id, offs_t start, offs_t end)
{
	if (space_id == AS_IO)
	{
		u16 superio_base = m_hefras ? 0x370 : 0x3f0;
		m_isa->install_device(superio_base, superio_base + 3, read8sm_delegate(*this, FUNC(w83977tf_device::read)), write8sm_delegate(*this, FUNC(w83977tf_device::write)));

		if (m_activate[5] & 1)
		{
			m_isa->install_device(m_keyb_address[0], m_keyb_address[0], read8sm_delegate(*m_kbdc, FUNC(kbdc8042_device::data_r)), write8sm_delegate(*m_kbdc, FUNC(kbdc8042_device::data_w)));
			m_isa->install_device(m_keyb_address[1], m_keyb_address[1], read8sm_delegate(*this, FUNC(w83977tf_device::keybc_status_r)), write8sm_delegate(*this, FUNC(w83977tf_device::keybc_command_w)));
		}

		if (m_activate[8] & 1)
		{
			// TODO: from port
			m_isa->install_device(0x70, 0x7f, read8sm_delegate(*m_rtc, FUNC(ds12885_device::read)), write8sm_delegate(*m_rtc, FUNC(ds12885_device::write)));
		}
	}
}

uint8_t w83977tf_device::read(offs_t offset)
{
	if (m_lock_sequence)
		return 0;

	if (offset == 0)
		return m_index;

	return space().read_byte(m_index);
}

void w83977tf_device::write(offs_t offset, u8 data)
{
	if (offset == 0)
	{
		if (m_lock_sequence)
		{
			if (data == 0x87)
			{
				m_lock_sequence --;
				//if (m_lock_sequence == 0)
				//  LOG("Config unlocked\n");
			}
		}
		else
		{
			if (data == 0xaa)
			{
				//LOG("Config locked\n");
				m_lock_sequence = 2;
				return;
			}
			m_index = data;
		}
	}
	else
	{
		if (!m_lock_sequence)
			space().write_byte(m_index, data);
	}
}

void w83977tf_device::config_map(address_map &map)
{
//  map(0x02, 0x02) configuration control (bit 0 soft reset)
	map(0x07, 0x07).lr8(NAME([this] () { return m_logical_index; })).w(FUNC(w83977tf_device::logical_device_select_w));
	map(0x20, 0x20).lr8(NAME([] () { return 0x97; })); // device ID
	map(0x21, 0x21).lr8(NAME([] () { return 0x73; })); // revision
//  map(0x22, 0x22) device power down control
//  map(0x23, 0x23) global immediate power down
//  map(0x24, 0x24)
//  map(0x25, 0x25)
	map(0x26, 0x26).rw(FUNC(w83977tf_device::cr26_r), FUNC(w83977tf_device::cr26_w));
//  map(0x28, 0x28)
//  map(0x2a, 0x2a)
//  map(0x2b, 0x2b)
//  map(0x2c, 0x2c)
//  map(0x2d, 0x2f) Test Modes

	map(0x30, 0xff).view(m_logical_view);
	// FDC
	m_logical_view[0](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<0>), FUNC(w83977tf_device::activate_w<0>));
	m_logical_view[0](0x31, 0xff).unmaprw();
	// PRT
	m_logical_view[1](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<1>), FUNC(w83977tf_device::activate_w<1>));
	m_logical_view[1](0x31, 0xff).unmaprw();
	// UART1
	m_logical_view[2](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<2>), FUNC(w83977tf_device::activate_w<2>));
	m_logical_view[2](0x31, 0xff).unmaprw();
	// UART2
	m_logical_view[3](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<3>), FUNC(w83977tf_device::activate_w<3>));
	m_logical_view[3](0x31, 0xff).unmaprw();
	// <reserved>
	m_logical_view[4](0x30, 0xff).unmaprw();
	// KBC
	m_logical_view[5](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<5>), FUNC(w83977tf_device::activate_w<5>));
	m_logical_view[5](0x60, 0x63).rw(FUNC(w83977tf_device::keyb_io_address_r), FUNC(w83977tf_device::keyb_io_address_w));
	m_logical_view[5](0x70, 0x70).rw(FUNC(w83977tf_device::keyb_irq_r), FUNC(w83977tf_device::keyb_irq_w));
	m_logical_view[5](0x72, 0x72).rw(FUNC(w83977tf_device::mouse_irq_r), FUNC(w83977tf_device::mouse_irq_w));
	m_logical_view[5](0xf0, 0xf0).lw8(
		NAME([this] (offs_t offset, u8 data) {
			// xx-- ---- KBC clock rate (00 = 6 MHz, 01 = 8 MHz, 10 = 12 MHz, 11 16 MHz)
			// ---- -x-- Enable port $92
			// ---- --xx Enables HW A20/reset from port $92
			LOG("keyboard_hwcontrol_w: %02x\n", data);
		})
	);
	// <reserved>
	m_logical_view[6](0x30, 0xff).unmaprw();
	// GPIO1
	m_logical_view[7](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<7>), FUNC(w83977tf_device::activate_w<7>));
	m_logical_view[7](0x31, 0xff).unmaprw();
	// GPIO2
	m_logical_view[8](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<8>), FUNC(w83977tf_device::activate_w<8>));
	m_logical_view[8](0x70, 0x70).rw(FUNC(w83977tf_device::rtc_irq_r), FUNC(w83977tf_device::rtc_irq_w));
	// GPIO3
	m_logical_view[9](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<9>), FUNC(w83977tf_device::activate_w<9>));
	m_logical_view[9](0x31, 0xff).unmaprw();
	// ACPI
	m_logical_view[0xa](0x30, 0x30).rw(FUNC(w83977tf_device::activate_r<0xa>), FUNC(w83977tf_device::activate_w<0xa>));
	m_logical_view[0xa](0x31, 0xff).unmaprw();
}

/*
 * Global register space
 */

void w83977tf_device::logical_device_select_w(offs_t offset, u8 data)
{
	m_logical_index = data;
	if (m_logical_index <= 0xa)
		m_logical_view.select(m_logical_index);
	else
		LOG("Attempt to select an unmapped device with %02x\n", data);
}

u8 w83977tf_device::cr26_r()
{
	return m_hefras << 6 | m_lockreg << 5;
}

void w83977tf_device::cr26_w(offs_t offset, u8 data)
{
	m_hefras = BIT(data, 6);
	// TODO: disable R/W on logical devices?
	m_lockreg = BIT(data, 5);
}

template <unsigned N> u8 w83977tf_device::activate_r(offs_t offset)
{
	return m_activate[N];
}

template <unsigned N> void w83977tf_device::activate_w(offs_t offset, u8 data)
{
	m_activate[N] = data & 1;
	LOG("%d Device %s\n", N, data & 1 ? "enabled" : "disabled");
	remap(AS_IO, 0, 0x400);
}

void w83977tf_device::request_irq(int irq, int state)
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

/*
 * Device #5 (Keyboard)
 */

void w83977tf_device::kbdp21_gp25_gatea20_w(int state)
{
	if (m_activate[5] == false)
		return;
	m_gp25_gatea20_callback(state);
}

void w83977tf_device::kbdp20_gp20_reset_w(int state)
{
	if (m_activate[5] == false)
		return;
	m_gp20_reset_callback(state);
}

void w83977tf_device::irq_keyboard_w(int state)
{
	if (m_activate[5] == false)
		return;
	request_irq(m_keyb_irq_line, state ? ASSERT_LINE : CLEAR_LINE);
}

void w83977tf_device::irq_mouse_w(int state)
{
	if (m_activate[5] == false)
		return;
	request_irq(m_mouse_irq_line, state ? ASSERT_LINE : CLEAR_LINE);
}

u8 w83977tf_device::keyb_irq_r(offs_t offset)
{
	return m_keyb_irq_line;
}

void w83977tf_device::keyb_irq_w(offs_t offset, u8 data)
{
	m_keyb_irq_line = data & 0xf;
	LOG("keyb irq routed to %02x\n", m_keyb_irq_line);
}

u8 w83977tf_device::mouse_irq_r(offs_t offset)
{
	return m_mouse_irq_line;
}

void w83977tf_device::mouse_irq_w(offs_t offset, u8 data)
{
	m_mouse_irq_line = data & 0xf;
	LOG("mouse irq routed to %02x\n", m_mouse_irq_line);
}

u8 w83977tf_device::keybc_status_r(offs_t offset)
{
	return (m_kbdc->data_r(4) & 0xfb) | 0x10; // bios needs bit 2 to be 0 as powerup and bit 4 to be 1
}

void w83977tf_device::keybc_command_w(offs_t offset, u8 data)
{
	m_kbdc->data_w(4, data);
}

// $60-$61 selects data port, $62-$63 command port
u8 w83977tf_device::keyb_io_address_r(offs_t offset)
{
	return m_keyb_address[(offset & 2) >> 1] >> ((offset & 1) ? 0 : 8) & 0xff;
}

void w83977tf_device::keyb_io_address_w(offs_t offset, u8 data)
{
	const u8 which = (offset & 2) >> 1;
	if (offset & 1)
	{
		m_keyb_address[which] &= 0xff00;
		m_keyb_address[which] |= data;
		LOG("keyb_io_address_w[1] %04x (%04x & 0x00ff)\n", which, m_keyb_address[which], data);
	}
	else
	{
		m_keyb_address[which] &= 0xff;
		m_keyb_address[which] |= data << 8;
		LOG("keyb_io_address_w[0] %04x (%04x & 0xff00)\n", which, m_keyb_address[which], data << 8);
	}
	remap(AS_IO, 0, 0x400);
}

/*
 * Device #8 (RTC)
 */

void w83977tf_device::irq_rtc_w(int state)
{
	if (m_activate[8] == false)
		return;
	request_irq(m_rtc_irq_line, state ? ASSERT_LINE : CLEAR_LINE);
}

u8 w83977tf_device::rtc_irq_r(offs_t offset)
{
	return m_rtc_irq_line;
}

void w83977tf_device::rtc_irq_w(offs_t offset, u8 data)
{
	m_rtc_irq_line = data & 0xf;
	LOG("RTC irq routed to %02x\n", m_rtc_irq_line);
}
