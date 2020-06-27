// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#include "emu.h"
#include "h8_port.h"

DEFINE_DEVICE_TYPE(H8_PORT, h8_port_device, "h8_digital_port", "H8 digital port")

h8_port_device::h8_port_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, H8_PORT, tag, owner, clock),
	cpu(*this, DEVICE_SELF_OWNER), io(nullptr), address(0), default_ddr(0), ddr(0), pcr(0), odr(0), mask(0), dr(0), last_output(0)
{
}

void h8_port_device::set_info(int _address, uint8_t _default_ddr, uint8_t _mask)
{
	address = _address;
	default_ddr = _default_ddr;
	mask = _mask;
}

void h8_port_device::ddr_w(uint8_t data)
{
	//  logerror("ddr_w %02x\n", data);
	ddr = data;
	update_output();
}

void h8_port_device::dr_w(uint8_t data)
{
	//  logerror("dr_w %02x\n", data);
	dr = data;
	update_output();
}

uint8_t h8_port_device::dr_r()
{
	//  logerror("dr_r %02x\n", (dr | mask) & 0xff);
	return dr | mask;
}

uint8_t h8_port_device::port_r()
{
	uint8_t res = mask | (dr & ddr);
	if((ddr & ~mask) != uint8_t(~mask))
		res |= io->read_word(address) & ~ddr;

	//  logerror("port_r %02x (%02x %02x)\n", res, ddr & ~mask, uint8_t(~mask));
	return res;
}

void h8_port_device::pcr_w(uint8_t data)
{
	logerror("pcr_w %02x\n", data);
	pcr = data;
}

uint8_t h8_port_device::pcr_r()
{
	logerror("dr_r %02x\n", (pcr | mask) & 0xff);
	return pcr | mask;
}

void h8_port_device::odr_w(uint8_t data)
{
	logerror("odr_w %02x\n", data);
	odr = data;
}

uint8_t h8_port_device::odr_r()
{
	logerror("odr_r %02x\n", (odr | mask) & 0xff);
	return odr | ~mask;
}

void h8_port_device::update_output()
{
	uint8_t res = dr & ddr & ~mask;
	if(res != last_output) {
		last_output = res;
		io->write_word(address, res);
	}
}

void h8_port_device::device_start()
{
	io = &cpu->space(AS_IO);
	save_item(NAME(ddr));
	save_item(NAME(dr));
	save_item(NAME(pcr));
	save_item(NAME(odr));
	save_item(NAME(last_output));
	last_output = 0;
}

void h8_port_device::device_reset()
{
	dr = 0;
	ddr = default_ddr;
	pcr = 0;
	odr = 0;
	update_output();
}
