// license:BSD-3-Clause
// copyright-holders:David Haywood

#include "emu.h"
#include "elan_eu3a05gpio.h"

DEFINE_DEVICE_TYPE(ELAN_EU3A05_GPIO, elan_eu3a05gpio_device, "elan_eu3a05gpio", "Elan EU3A05 GPIO")

elan_eu3a05gpio_device::elan_eu3a05gpio_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, ELAN_EU3A05_GPIO, tag, owner, clock),
	m_write_0_callback(*this),
	m_write_1_callback(*this),
	m_write_2_callback(*this),
	m_read_0_callback(*this),
	m_read_1_callback(*this),
	m_read_2_callback(*this)
{
}

void elan_eu3a05gpio_device::device_start()
{
	m_write_0_callback.resolve_safe();
	m_write_1_callback.resolve_safe();
	m_write_2_callback.resolve_safe();
	m_read_0_callback.resolve_safe(0xff);
	m_read_1_callback.resolve_safe(0xff);
	m_read_2_callback.resolve_safe(0xff);
}

void elan_eu3a05gpio_device::device_reset()
{
	for (int i = 0; i < 3; i++)
	{
		m_ddr[i] = 0;
		m_unk[i] = 0;
	}
}

uint8_t elan_eu3a05gpio_device::read_port_data(int which)
{
	//todo, actually use the direction registers
	switch (which)
	{
	case 0: return m_read_0_callback();
	case 1: return m_read_1_callback();
	case 2: return m_read_2_callback();
	}

	return 0xff;
}

uint8_t elan_eu3a05gpio_device::read_direction(int which)
{
	return m_ddr[which];
}

uint8_t elan_eu3a05gpio_device::gpio_r(offs_t offset)
{

	int port = offset/2;
	if (!(offset&1)) return read_direction(port);
	else return read_port_data(port);
}

void elan_eu3a05gpio_device::write_port_data(int which, uint8_t data)
{
	//todo, actually use the direction registers
	logerror("%s: write_port_data (port %d) %02x (direction register %02x)\n", machine().describe_context(), which, data, m_ddr[which]);

	switch (which)
	{
	case 0: return m_write_0_callback(data); break;
	case 1: return m_write_1_callback(data); break;
	case 2: return m_write_2_callback(data); break;
	}
}

void elan_eu3a05gpio_device::write_direction(int which, uint8_t data)
{
	logerror("%s: write_direction (port %d) %02x\n", machine().describe_context(), which, data);
	m_ddr[which] = data;
}

void elan_eu3a05gpio_device::gpio_w(offs_t offset, uint8_t data)
{
	int port = offset/2;
	if (!(offset&1)) return write_direction(port, data);
	else return write_port_data(port, data);
}

void elan_eu3a05gpio_device::gpio_unk_w(offs_t offset, uint8_t data)
{
	logerror("%s: gpio_unk_w (port %d) %02x (direction register %02x)\n", machine().describe_context(), offset, data, m_ddr[offset]);
}


