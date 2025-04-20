// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#include "emu.h"
#include "h8_timer8.h"

// Verbosity level
// 0 = no messages
// 1 = timer setup
// 2 = everything
static constexpr int V = 1;

DEFINE_DEVICE_TYPE(H8_TIMER8_CHANNEL,  h8_timer8_channel_device,  "h8_timer8_channel",  "H8 8-bit timer channel")
DEFINE_DEVICE_TYPE(H8H_TIMER8_CHANNEL, h8h_timer8_channel_device, "h8h_timer8_channel", "H8H 8-bit timer channel")

h8_timer8_channel_device::h8_timer8_channel_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8_timer8_channel_device(mconfig, H8_TIMER8_CHANNEL, tag, owner, clock)
{
}

h8_timer8_channel_device::h8_timer8_channel_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, type, tag, owner, clock),
	m_cpu(*this, finder_base::DUMMY_TAG),
	m_intc(*this, finder_base::DUMMY_TAG),
	m_chained_timer(*this, finder_base::DUMMY_TAG),
	m_irq_ca(0), m_irq_cb(0), m_irq_v(0), m_chain_type(0), m_tcr(0), m_tcsr(0), m_tcnt(0), m_extra_clock_bit(false),
	m_has_adte(false), m_has_ice(false), m_clock_type(0), m_clock_divider(0), m_clear_type(0), m_counter_cycle(0), m_last_clock_update(0), m_event_time(0)
{
	m_chain_type = STOPPED;
	m_has_adte = false;
	m_has_ice = false;
}

uint8_t h8_timer8_channel_device::tcr_r()
{
	return m_tcr;
}

void h8_timer8_channel_device::tcr_w(uint8_t data)
{
	update_counter();
	m_tcr = data;
	update_tcr();
	recalc_event();
}

void h8_timer8_channel_device::set_extra_clock_bit(bool bit)
{
	update_counter();
	m_extra_clock_bit = bit;
	update_tcr();
	recalc_event();
}

void h8_timer8_channel_device::update_tcr()
{
	std::ostringstream message;
	switch(m_tcr & TCR_CKS) {
	case 0:
		m_clock_type = STOPPED;
		m_clock_divider = 0;
		if(V>=1) message << "clock stopped";
		break;

	case 1: case 2: case 3:
		m_clock_type = DIV;
		m_clock_divider = m_div_tab[((m_tcr & TCR_CKS)-1)*2 + m_extra_clock_bit];
		if(V>=1) util::stream_format(message, "clock %dHz", m_cpu->clock()/m_clock_divider);
		break;

	case 4:
		m_clock_type = m_chain_type;
		m_clock_divider = 0;
		if(V>=1) util::stream_format(message, "clock chained %s", m_clock_type == CHAIN_A ? "tcora" : "overflow");
		break;

	case 5:
		m_clock_type = INPUT_UP;
		m_clock_divider = 0;
		if(V>=1) message << "clock external raising edge";
		break;

	case 6:
		m_clock_type = INPUT_DOWN;
		m_clock_divider = 0;
		if(V>=1) message << "clock external falling edge";
		break;

	case 7:
		m_clock_type = INPUT_UPDOWN;
		m_clock_divider = 0;
		if(V>=1) message << "clock external both edges";
		break;
	}

	switch(m_tcr & TCR_CCLR) {
	case 0x00:
		m_clear_type = CLEAR_NONE;
		if(V>=1) message << ", no clear";
		break;

	case 0x08:
		m_clear_type = CLEAR_A;
		if(V>=1) message << ", clear on tcora";
		break;

	case 0x10:
		m_clear_type = CLEAR_B;
		if(V>=1) message << ", clear on tcorb";
		break;

	case 0x18:
		m_clear_type = CLEAR_EXTERNAL;
		if(V>=1) message << ", clear on external";
		break;
	}

	if(V>=1) util::stream_format(message, ", irq=%c%c%c\n",
						m_tcr & TCR_CMIEB ? 'b' : '-',
						m_tcr & TCR_CMIEA ? 'a' : '-',
						m_tcr & TCR_OVIE  ? 'o' : '-');

	logerror(std::move(message).str());
}

uint8_t h8_timer8_channel_device::tcsr_r()
{
	return m_tcsr;
}

void h8_timer8_channel_device::tcsr_w(uint8_t data)
{
	update_counter();

	uint8_t mask = m_has_adte || m_has_ice ? 0x1f : 0x0f;
	m_tcsr = (m_tcsr & ~mask) | (data & mask);
	m_tcsr &= data | 0x1f;

	if(V>=2) logerror("tcsr_w %02x\n", m_tcsr);

	recalc_event();
}

uint8_t h8_timer8_channel_device::tcor_r(offs_t offset)
{
	return m_tcor[offset];
}

void h8_timer8_channel_device::tcor_w(offs_t offset, uint8_t data)
{
	update_counter();
	m_tcor[offset] = data;
	if(V>=2) logerror("tcor%c_w %02x\n", 'a'+offset, data);
	recalc_event();
}

uint8_t h8_timer8_channel_device::tcnt_r()
{
	update_counter();
	recalc_event();
	return m_tcnt;
}

void h8_timer8_channel_device::tcnt_w(uint8_t data)
{
	update_counter();
	m_tcnt = data;
	if(V>=2) logerror("tcnt_w %02x\n", data);
	recalc_event();
}

void h8_timer8_channel_device::device_start()
{
}

void h8_timer8_channel_device::device_reset()
{
	m_tcr = 0x00;
	m_tcsr = m_has_adte || m_has_ice ? 0x00 : 0x10;
	m_tcor[0] = 0xff;
	m_tcor[1] = 0xff;
	m_tcnt = 0x00;
	m_counter_cycle = 0x100;
	m_clock_type = STOPPED;
	m_clock_divider = 0;
	m_clear_type = CLEAR_NONE;
	m_last_clock_update = 0;
	m_event_time = 0;
	m_extra_clock_bit = false;
}

uint64_t h8_timer8_channel_device::internal_update(uint64_t current_time)
{
	if(m_event_time && current_time >= m_event_time) {
		update_counter(current_time);
		recalc_event(current_time);
	}

	return m_event_time;
}

void h8_timer8_channel_device::update_counter(uint64_t cur_time)
{
	if(m_clock_type != DIV)
		return;

	if(!cur_time)
		cur_time = m_cpu->total_cycles();

	uint64_t base_time = (m_last_clock_update + m_clock_divider/2) / m_clock_divider;
	uint64_t new_time = (cur_time + m_clock_divider/2) / m_clock_divider;

	int tt = m_tcnt + new_time - base_time;
	m_tcnt = tt % m_counter_cycle;

	if(tt == m_tcor[0] || m_tcnt == m_tcor[0]) {
		if(m_chained_timer)
			m_chained_timer->chained_timer_tcora();

		if(!(m_tcsr & TCSR_CMFA)) {
			m_tcsr |= TCSR_CMFA;
			if(m_tcr & TCR_CMIEA)
				m_intc->internal_interrupt(m_irq_ca);
		}
	}

	if(!(m_tcsr & TCSR_CMFB) && (tt == m_tcor[1] || m_tcnt == m_tcor[1])) {
		m_tcsr |= TCSR_CMFB;
		if(m_tcr & TCR_CMIEB)
			m_intc->internal_interrupt(m_irq_cb);
	}

	if(tt >= 0x100) {
		if(m_chained_timer)
			m_chained_timer->chained_timer_overflow();
		if(!(m_tcsr & TCSR_OVF)) {
			m_tcsr |= TCSR_OVF;
			if(m_tcr & TCR_OVIE)
				m_intc->internal_interrupt(m_irq_v);
		}
	}
	m_last_clock_update = cur_time;
}

void h8_timer8_channel_device::recalc_event(uint64_t cur_time)
{
	bool update_cpu = cur_time == 0;
	uint64_t old_event_time = m_event_time;

	if(m_clock_type != DIV) {
		m_event_time = 0;
		if(old_event_time && update_cpu)
			m_cpu->internal_update();
		return;
	}

	if(!cur_time)
		cur_time = m_cpu->total_cycles();

	uint32_t event_delay = 0xffffffff;
	if(m_clear_type == CLEAR_A || m_clear_type == CLEAR_B)
		m_counter_cycle = m_tcor[m_clear_type - CLEAR_A];
	else {
		m_counter_cycle = 0x100;
		event_delay = m_counter_cycle - m_tcnt;
		if(!event_delay)
			event_delay = m_counter_cycle;
	}

	for(auto &elem : m_tcor) {
		uint32_t new_delay = 0xffffffff;
		if(elem > m_tcnt) {
			if(m_tcnt >= m_counter_cycle || elem <= m_counter_cycle)
				new_delay = elem - m_tcnt;
		} else if(elem <= m_counter_cycle) {
			if(m_tcnt < m_counter_cycle)
				new_delay = (m_counter_cycle - m_tcnt) + elem;
			else
				new_delay = (0x100 - m_tcnt) + elem;
		}
		if(event_delay > new_delay)
			event_delay = new_delay;
	}

	if(event_delay != 0xffffffff)
		m_event_time = ((((cur_time + m_clock_divider) / m_clock_divider) + event_delay - 1) * m_clock_divider) + m_clock_divider/2;
	else
		m_event_time = 0;

	if(old_event_time != m_event_time && update_cpu)
		m_cpu->internal_update();
}

void h8_timer8_channel_device::chained_timer_overflow()
{
	if(m_clock_type == CHAIN_OVERFLOW)
		timer_tick();
}

void h8_timer8_channel_device::chained_timer_tcora()
{
	if(m_clock_type == CHAIN_A)
		timer_tick();
}

void h8_timer8_channel_device::timer_tick()
{
	m_tcnt++;

	if(m_tcnt == m_tcor[0]) {
		if(m_chained_timer)
			m_chained_timer->chained_timer_tcora();

		if(!(m_tcsr & TCSR_CMFA)) {
			m_tcsr |= TCSR_CMFA;
			if(m_tcr & TCR_CMIEA)
				m_intc->internal_interrupt(m_irq_ca);
		}
	}

	if(!(m_tcsr & TCSR_CMFB) && m_tcnt == m_tcor[1]) {
		m_tcsr |= TCSR_CMFB;
		if(m_tcr & TCR_CMIEB)
			m_intc->internal_interrupt(m_irq_cb);
	}

	if(m_tcnt == 0x00) {
		if(m_chained_timer)
			m_chained_timer->chained_timer_overflow();
		if(!(m_tcsr & TCSR_OVF)) {
			m_tcsr |= TCSR_OVF;
			if(m_tcr & TCR_OVIE)
				m_intc->internal_interrupt(m_irq_v);
		}
	}
}

h8h_timer8_channel_device::h8h_timer8_channel_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8_timer8_channel_device(mconfig, H8H_TIMER8_CHANNEL, tag, owner, clock)
{
	// The extra clock bit is not used for h8h+
	m_div_tab[0] = 8;
	m_div_tab[1] = 8;
	m_div_tab[2] = 64;
	m_div_tab[3] = 64;
	m_div_tab[4] = 8192;
	m_div_tab[5] = 8192;
}
