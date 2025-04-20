// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#include "emu.h"
#include "h8_port.h"

DEFINE_DEVICE_TYPE(H8_PORT, h8_port_device, "h8_digital_port", "H8 digital port")

h8_port_device::h8_port_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, H8_PORT, tag, owner, clock),
	m_cpu(*this, DEVICE_SELF_OWNER), m_address(0), m_default_ddr(0), m_ddr(0), m_pcr(0), m_odr(0), m_mask(0), m_dr(0), m_last_output(0)
{
}

void h8_port_device::ddr_w(uint8_t data)
{
	//  logerror("ddr_w %02x\n", data);
	m_ddr = data;
	update_output();
}

uint8_t h8_port_device::ddr_r()
{
	return m_ddr;
}

void h8_port_device::dr_w(uint8_t data)
{
	//  logerror("dr_w %02x\n", data);
	m_dr = data;
	update_output();
}

uint8_t h8_port_device::dr_r()
{
	//  logerror("dr_r %02x\n", (dr | mask) & 0xff);
	return m_dr | m_mask;
}

uint8_t h8_port_device::port_r()
{
	uint8_t res = m_mask | (m_dr & m_ddr);
	if((m_ddr & ~m_mask) != uint8_t(~m_mask))
		res |= m_cpu->do_read_port(m_address) & ~m_ddr;

	//  logerror("port_r %02x (%02x %02x)\n", res, ddr & ~mask, uint8_t(~mask));
	return res;
}

void h8_port_device::pcr_w(uint8_t data)
{
	logerror("pcr_w %02x\n", data);
	m_pcr = data;
}

uint8_t h8_port_device::pcr_r()
{
	logerror("dr_r %02x\n", (m_pcr | m_mask) & 0xff);
	return m_pcr | m_mask;
}

void h8_port_device::odr_w(uint8_t data)
{
	logerror("odr_w %02x\n", data);
	m_odr = data;
}

uint8_t h8_port_device::odr_r()
{
	logerror("odr_r %02x\n", (m_odr | ~m_mask) & 0xff);
	return m_odr | ~m_mask;
}

void h8_port_device::update_output()
{
	uint8_t res = m_dr & m_ddr & ~m_mask;
	if(res != m_last_output) {
		m_last_output = res;
		m_cpu->do_write_port(m_address, res);
	}
}

void h8_port_device::device_start()
{
	save_item(NAME(m_ddr));
	save_item(NAME(m_dr));
	save_item(NAME(m_pcr));
	save_item(NAME(m_odr));
	save_item(NAME(m_last_output));
	m_last_output = 0;
}

void h8_port_device::device_reset()
{
	m_dr = 0;
	m_ddr = m_default_ddr;
	m_pcr = 0;
	m_odr = 0;
	update_output();
}
