// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    h83003.h

    H8/3003

    H8/300H-based mcus.


***************************************************************************/

#ifndef MAME_CPU_H8_H83003_H
#define MAME_CPU_H8_H83003_H

#pragma once

#include "h8h.h"
#include "h8_adc.h"
#include "h8_dma.h"
#include "h8_port.h"
#include "h8_intc.h"
#include "h8_timer16.h"
#include "h8_sci.h"
#include "h8_watchdog.h"

class h83003_device : public h8h_device {
public:
	h83003_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto tend0() { return m_tend_cb[0].bind(); }
	auto tend1() { return m_tend_cb[1].bind(); }
	auto tend2() { return m_tend_cb[2].bind(); }
	auto tend3() { return m_tend_cb[3].bind(); }

	auto read_port4()  { return m_read_port [PORT_4].bind(); }
	auto write_port4() { return m_write_port[PORT_4].bind(); }
	auto read_port5()  { return m_read_port [PORT_5].bind(); }
	auto write_port5() { return m_write_port[PORT_5].bind(); }
	auto read_port6()  { return m_read_port [PORT_6].bind(); }
	auto write_port6() { return m_write_port[PORT_6].bind(); }
	auto read_port7()  { return m_read_port [PORT_7].bind(); }
	auto read_port8()  { return m_read_port [PORT_8].bind(); }
	auto write_port8() { return m_write_port[PORT_8].bind(); }
	auto read_port9()  { return m_read_port [PORT_9].bind(); }
	auto write_port9() { return m_write_port[PORT_9].bind(); }
	auto read_porta()  { return m_read_port [PORT_A].bind(); }
	auto write_porta() { return m_write_port[PORT_A].bind(); }
	auto read_portb()  { return m_read_port [PORT_B].bind(); }
	auto write_portb() { return m_write_port[PORT_B].bind(); }
	auto read_portc()  { return m_read_port [PORT_C].bind(); }
	auto write_portc() { return m_write_port[PORT_C].bind(); }

	void set_mode_a20() { m_mode_a20 = true; }
	void set_mode_a24() { m_mode_a20 = false; }

	uint8_t syscr_r();
	void syscr_w(uint8_t data);

	uint8_t rtmcsr_r();
	void rtmcsr_w(uint8_t data);

protected:
	required_device<h8h_intc_device> m_intc;
	required_device<h8_adc_device> m_adc;
	required_device<h8h_dma_device> m_dma;
	required_device<h8h_dma_channel_device> m_dma0;
	required_device<h8h_dma_channel_device> m_dma1;
	required_device<h8h_dma_channel_device> m_dma2;
	required_device<h8h_dma_channel_device> m_dma3;
	required_device<h8_port_device> m_port4;
	required_device<h8_port_device> m_port5;
	required_device<h8_port_device> m_port6;
	required_device<h8_port_device> m_port7;
	required_device<h8_port_device> m_port8;
	required_device<h8_port_device> m_port9;
	required_device<h8_port_device> m_porta;
	required_device<h8_port_device> m_portb;
	required_device<h8_port_device> m_portc;
	required_device<h8_timer16_device> m_timer16;
	required_device<h8h_timer16_channel_device> m_timer16_0;
	required_device<h8h_timer16_channel_device> m_timer16_1;
	required_device<h8h_timer16_channel_device> m_timer16_2;
	required_device<h8h_timer16_channel_device> m_timer16_3;
	required_device<h8h_timer16_channel_device> m_timer16_4;
	required_device<h8_watchdog_device> m_watchdog;

	uint8_t m_syscr;
	uint8_t m_rtmcsr;

	devcb_write_line::array<4> m_tend_cb;

	virtual void update_irq_filter() override;
	virtual void interrupt_taken() override;
	virtual int trapa_setup() override;
	virtual void irq_setup() override;
	virtual void internal_update(uint64_t current_time) override;
	virtual void device_add_mconfig(machine_config &config) override;
	void map(address_map &map);

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void execute_set_input(int inputnum, int state) override;
};

DECLARE_DEVICE_TYPE(H83003, h83003_device)

#endif // MAME_CPU_H8_H83003_H
