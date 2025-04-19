// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

#ifndef MAME_CPU_NS32000_SLAVE_H
#define MAME_CPU_NS32000_SLAVE_H

#pragma once

class ns32000_slave_interface : public device_interface
{
public:
	auto out_scb() { return m_out_scb.bind(); }

	ns32000_slave_interface(machine_config const &mconfig, device_t &device)
		: device_interface(device, "ns32000_slave")
		, m_out_scb(*this)
	{
	}

	enum slave_status : u16
	{
		SLAVE_Q = 0x0001,
		SLAVE_L = 0x0004,
		SLAVE_F = 0x0020,
		SLAVE_Z = 0x0040,
		SLAVE_N = 0x0080,
	};

	virtual void state_add(device_state_interface &parent, int &index) = 0;
	virtual void write_id(u16 data) = 0;
	virtual void write_op(u16 data) = 0;
	virtual u16 read_st(int *icount = nullptr) = 0;
	virtual u16 read_op() = 0;

protected:
	// device_interface overrides
	virtual void interface_post_start() override
	{
		m_out_scb.resolve_safe();
	}

	devcb_write_line m_out_scb;
};

#endif // MAME_CPU_NS32000_SLAVE_H
