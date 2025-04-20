// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    h8.h

    H8-300 base cpu emulation


***************************************************************************/

#include "emu.h"
#include "h8.h"
#include "h8_dma.h"
#include "h8_dtc.h"
#include "h8d.h"

h8_device::h8_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, address_map_constructor map_delegate) :
	cpu_device(mconfig, type, tag, owner, clock),
	m_program_config("program", ENDIANNESS_BIG, 16, 16, 0, map_delegate),
	m_read_adc(*this, 0),
	m_read_port(*this, 0),
	m_write_port(*this),
	m_sci(*this, "sci%u", 0),
	m_sci_tx(*this),
	m_sci_clk(*this),
	m_PPC(0), m_NPC(0), m_PC(0), m_PIR(0), m_EXR(0), m_CCR(0), m_MAC(0), m_MACF(0),
	m_TMP1(0), m_TMP2(0), m_TMPR(0), m_inst_state(0), m_inst_substate(0), m_icount(0), m_bcount(0), m_irq_vector(0), m_taken_irq_vector(0), m_irq_level(0), m_taken_irq_level(0), m_irq_required(false), m_irq_nmi(false)
{
	m_supports_advanced = false;
	m_mode_advanced = false;
	m_mode_a20 = false;
	m_has_exr = false;
	m_has_mac = false;
	m_mac_saturating = false;
	m_has_trace = false;
	m_has_hc = true;

	for(unsigned int i=0; i != m_read_adc.size(); i++)
		m_read_adc[i].bind().set([this, i]() { return adc_default(i); });
	for(int i=0; i != PORT_COUNT; i++) {
		m_read_port[i].bind().set([this, i]() { return port_default_r(i); });
		m_write_port[i].bind().set([this, i](u8 data) { port_default_w(i, data); });
	}
}

u16 h8_device::adc_default(int adc)
{
	logerror("read of un-hooked adc %d\n", adc);
	return 0;
}

const char h8_device::port_names[] = "123456789abcdefg";

u8 h8_device::port_default_r(int port)
{
	logerror("read of un-hooked port %c (PC=%X)\n", port_names[port], m_PPC);
	return 0;
}

void h8_device::port_default_w(int port, u8 data)
{
	logerror("write of un-hooked port %c %02x\n", port_names[port], data);
}

void h8_device::device_config_complete()
{
	uint8_t addrbits = m_mode_advanced ? (m_mode_a20 ? 20 : 24) : 16;
	m_program_config.m_addr_width = m_program_config.m_logaddr_width = addrbits;
}

void h8_device::device_start()
{
	space(AS_PROGRAM).cache(m_cache);
	space(AS_PROGRAM).specific(m_program);

	uint32_t pcmask = m_mode_advanced ? 0xffffff : 0xffff;
	state_add<uint32_t>(H8_PC, "PC",
		[this]() { return m_NPC; },
		[this](uint32_t pc) { m_PC = m_PPC = m_NPC = pc; m_PIR = read16i(m_PC); m_PC += 2; prefetch_done_noirq_notrace(); }
	).mask(pcmask);
	state_add<uint32_t>(STATE_GENPC, "GENPC",
		[this]() { return m_NPC; },
		[this](uint32_t pc) { m_PC = m_PPC = m_NPC = pc; m_PIR = read16i(m_PC); m_PC += 2; prefetch_done_noirq_notrace(); }
	).mask(pcmask).noshow();
	state_add(STATE_GENPCBASE, "CURPC",     m_PPC).mask(pcmask).noshow();
	state_add(H8_CCR,          "CCR",       m_CCR);
	if(m_has_exr)
		state_add(STATE_GENFLAGS,  "GENFLAGS",  m_CCR).formatstr("%11s").noshow();
	else
		state_add(STATE_GENFLAGS,  "GENFLAGS",  m_CCR).formatstr("%8s").noshow();

	if(m_has_exr)
		state_add(H8_EXR,          "EXR",       m_EXR);
	if(!m_supports_advanced) {
		state_add(H8_R0,           "R0",        m_R[0]);
		state_add(H8_R1,           "R1",        m_R[1]);
		state_add(H8_R2,           "R2",        m_R[2]);
		state_add(H8_R3,           "R3",        m_R[3]);
		state_add(H8_R4,           "R4",        m_R[4]);
		state_add(H8_R5,           "R5",        m_R[5]);
		state_add(H8_R6,           "R6",        m_R[6]);
		state_add(H8_R7,           "R7",        m_R[7]);
	} else {
		state_add(H8_R0,           "R0",        m_R[0]).noshow();
		state_add(H8_R1,           "R1",        m_R[1]).noshow();
		state_add(H8_R2,           "R2",        m_R[2]).noshow();
		state_add(H8_R3,           "R3",        m_R[3]).noshow();
		state_add(H8_R4,           "R4",        m_R[4]).noshow();
		state_add(H8_R5,           "R5",        m_R[5]).noshow();
		state_add(H8_R6,           "R6",        m_R[6]).noshow();
		state_add(H8_R7,           "R7",        m_R[7]).noshow();
		state_add(H8_E0,           "E0",        m_R[8]).noshow();
		state_add(H8_E1,           "E1",        m_R[9]).noshow();
		state_add(H8_E2,           "E2",        m_R[10]).noshow();
		state_add(H8_E3,           "E3",        m_R[11]).noshow();
		state_add(H8_E4,           "E4",        m_R[12]).noshow();
		state_add(H8_E5,           "E5",        m_R[13]).noshow();
		state_add(H8_E6,           "E6",        m_R[14]).noshow();
		state_add(H8_E7,           "E7",        m_R[15]).noshow();
		state_add(H8_R0,           "ER0",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R1,           "ER1",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R2,           "ER2",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R3,           "ER3",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R4,           "ER4",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R5,           "ER5",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R6,           "ER6",       m_TMPR).callimport().callexport().formatstr("%9s");
		state_add(H8_R7,           "ER7",       m_TMPR).callimport().callexport().formatstr("%9s");
	}

	save_item(NAME(m_PPC));
	save_item(NAME(m_NPC));
	save_item(NAME(m_PC));
	save_item(NAME(m_PIR));
	save_item(NAME(m_IR));
	save_item(NAME(m_R));
	save_item(NAME(m_EXR));
	save_item(NAME(m_CCR));
	save_item(NAME(m_TMP1));
	save_item(NAME(m_TMP2));
	save_item(NAME(m_inst_state));
	save_item(NAME(m_inst_substate));
	save_item(NAME(m_irq_vector));
	save_item(NAME(m_taken_irq_vector));
	save_item(NAME(m_irq_level));
	save_item(NAME(m_taken_irq_level));
	save_item(NAME(m_irq_nmi));
	save_item(NAME(m_current_dma));

	set_icountptr(m_icount);

	m_PC = 0;
	m_PPC = 0;
	m_NPC = 0;
	memset(m_IR, 0, sizeof(m_IR));
	memset(m_R, 0, sizeof(m_R));
	m_EXR = 0;
	m_CCR = 0;
	m_MAC = 0;
	m_MACF = 0;
	m_inst_state = STATE_RESET;
	m_inst_substate = 0;
	m_count_before_instruction_step = 0;
	m_requested_state = -1;
	m_dma_device = nullptr;
	m_dtc_device = nullptr;

	memset(m_dma_channel, 0, sizeof(m_dma_channel));
}

void h8_device::device_reset()
{
	m_inst_state = STATE_RESET;
	m_inst_substate = 0;
	m_count_before_instruction_step = 0;
	m_requested_state = -1;

	m_irq_vector = 0;
	m_irq_level = -1;
	m_irq_nmi = false;
	m_taken_irq_vector = 0;
	m_taken_irq_level = -1;
	m_current_dma = -1;
	m_current_dtc = nullptr;
}

bool h8_device::trigger_dma(int vector)
{
	bool dma_triggered = false;
	bool drop_interrupt = false;
	for(int i=0; i != 8; i++)
		if(m_dma_channel[i] && ((m_dma_channel[i]->m_flags & (h8_dma_state::ACTIVE|h8_dma_state::SUSPENDED)) == (h8_dma_state::ACTIVE|h8_dma_state::SUSPENDED)) && m_dma_channel[i]->m_trigger_vector == vector) {
			m_dma_channel[i]->m_flags &= ~h8_dma_state::SUSPENDED;
			dma_triggered = true;
			if(m_dma_channel[i]->m_flags & h8_dma_state::EAT_INTERRUPT)
				drop_interrupt = true;
		}

	// DMA can mask interrupt to the DTC
	if(!drop_interrupt && m_dtc_device && m_dtc_device->trigger_dtc(vector))
		drop_interrupt = true;

	if(dma_triggered)
		update_active_dma_channel();

	return drop_interrupt;
}

void h8_device::set_dma_channel(h8_dma_state *state)
{
	m_dma_channel[state->m_id] = state;
}

void h8_device::update_active_dma_channel()
{
	for(int i=0; i != 8; i++) {
		if(m_dma_channel[i] && ((m_dma_channel[i]->m_flags & (h8_dma_state::ACTIVE|h8_dma_state::SUSPENDED)) == h8_dma_state::ACTIVE)) {
			m_current_dma = i;
			return;
		}
	}
	m_current_dma = -1;
}

void h8_device::set_current_dtc(h8_dtc_state *state)
{
	m_current_dtc = state;
}

void h8_device::request_state(int state)
{
	m_requested_state = state;
}

uint32_t h8_device::execute_min_cycles() const noexcept
{
	return 1;
}

uint32_t h8_device::execute_max_cycles() const noexcept
{
	return 1;
}

uint32_t h8_device::execute_input_lines() const noexcept
{
	return 0;
}

bool h8_device::execute_input_edge_triggered(int inputnum) const noexcept
{
	return inputnum == INPUT_LINE_NMI;
}

void h8_device::recompute_bcount(uint64_t event_time)
{
	if(!event_time || event_time >= total_cycles() + m_icount) {
		m_bcount = 0;
		return;
	}
	m_bcount = total_cycles() + m_icount - event_time;
}

void h8_device::execute_run()
{
	internal_update(total_cycles());

	m_icount -= m_count_before_instruction_step;
	if(m_icount < 0) {
		m_count_before_instruction_step = -m_icount;
		m_icount = 0;
	} else
		m_count_before_instruction_step = 0;

	while(m_bcount && m_icount <= m_bcount)
		internal_update(total_cycles() + m_icount - m_bcount);

	if(m_icount > 0 && m_inst_substate)
		do_exec_partial();

	while(m_icount > 0) {
		while(m_icount > m_bcount) {
			if(m_inst_state < 0x10000) {
				m_PPC = m_NPC;
				if(machine().debug_flags & DEBUG_FLAG_ENABLED)
					debugger_instruction_hook(m_NPC);
			}
			do_exec_full();
		}
		if(m_icount > 0)
			while(m_bcount && m_icount <= m_bcount)
				internal_update(total_cycles() + m_icount - m_bcount);
		if(m_icount > 0 && m_inst_substate)
			do_exec_partial();
	}
	if(m_icount < 0) {
		m_count_before_instruction_step = -m_icount;
		m_icount = 0;
	}
}

void h8_device::add_event(uint64_t &event_time, uint64_t new_event)
{
	if(!new_event)
		return;
	if(!event_time || event_time > new_event)
		event_time = new_event;
}

void h8_device::internal_update()
{
	internal_update(total_cycles());
}

device_memory_interface::space_config_vector h8_device::memory_space_config() const
{
	return space_config_vector {
		std::make_pair(AS_PROGRAM, &m_program_config)
	};
}


void h8_device::state_import(const device_state_entry &entry)
{
	switch(entry.index()) {
	case H8_R0:
	case H8_R1:
	case H8_R2:
	case H8_R3:
	case H8_R4:
	case H8_R5:
	case H8_R6:
	case H8_R7: {
		int r = entry.index() - H8_R0;
		m_R[r + 8] = m_TMPR >> 16;
		m_R[r] = m_TMPR;
		break;
	}
	}
}

void h8_device::state_export(const device_state_entry &entry)
{
	switch(entry.index()) {
	case H8_R0:
	case H8_R1:
	case H8_R2:
	case H8_R3:
	case H8_R4:
	case H8_R5:
	case H8_R6:
	case H8_R7: {
		int r = entry.index() - H8_R0;
		m_TMPR = (m_R[r + 8] << 16) | m_R[r];
		break;
	}
	}
}

void h8_device::state_string_export(const device_state_entry &entry, std::string &str) const
{
	switch(entry.index()) {
	case STATE_GENFLAGS:
		if(m_has_exr)
			str = string_format("%c%c %c%c%c%c%c%c%c%c",
					(m_EXR & EXR_T) ? 'T' : '-',
					'0' + (m_EXR & EXR_I),
					(m_CCR & F_I)  ? 'I' : '-',
					(m_CCR & F_UI) ? 'u' : '-',
					(m_CCR & F_H)  ? 'H' : '-',
					(m_CCR & F_U)  ? 'U' : '-',
					(m_CCR & F_N)  ? 'N' : '-',
					(m_CCR & F_Z)  ? 'Z' : '-',
					(m_CCR & F_V)  ? 'V' : '-',
					(m_CCR & F_C)  ? 'C' : '-');
		else if(m_has_hc)
			str = string_format("%c%c%c%c%c%c%c%c",
					(m_CCR & F_I)  ? 'I' : '-',
					(m_CCR & F_UI) ? 'u' : '-',
					(m_CCR & F_H)  ? 'H' : '-',
					(m_CCR & F_U)  ? 'U' : '-',
					(m_CCR & F_N)  ? 'N' : '-',
					(m_CCR & F_Z)  ? 'Z' : '-',
					(m_CCR & F_V)  ? 'V' : '-',
					(m_CCR & F_C)  ? 'C' : '-');
		else
			str = string_format("%c%c%c%c%c%c%c%c",
					(m_CCR & F_I)  ? '?' : '-',
					(m_CCR & F_UI) ? 'u' : '-',
					(m_CCR & F_H)  ? 'I' : '-',
					(m_CCR & F_U)  ? 'U' : '-',
					(m_CCR & F_N)  ? 'N' : '-',
					(m_CCR & F_Z)  ? 'Z' : '-',
					(m_CCR & F_V)  ? 'V' : '-',
					(m_CCR & F_C)  ? 'C' : '-');
		break;
	case H8_R0:
	case H8_R1:
	case H8_R2:
	case H8_R3:
	case H8_R4:
	case H8_R5:
	case H8_R6:
	case H8_R7: {
		int r = entry.index() - H8_R0;
		str = string_format("%04X %04X", m_R[r + 8], m_R[r]);
		break;
	}
	}
}

// FIXME: one-state bus cycles are only provided for on-chip ROM & RAM in H8S/2000 and H8S/2600.
// All other accesses take *at least* two states each, and additional wait states are often programmed for external memory!

uint16_t h8_device::read16i(uint32_t adr)
{
	m_icount--;
	return m_cache.read_word(adr & ~1);
}

uint8_t h8_device::read8(uint32_t adr)
{
	m_icount--;
	return m_program.read_byte(adr);
}

void h8_device::write8(uint32_t adr, uint8_t data)
{
	m_icount--;
	m_program.write_byte(adr, data);
}

uint16_t h8_device::read16(uint32_t adr)
{
	m_icount--;
	return m_program.read_word(adr & ~1);
}

void h8_device::write16(uint32_t adr, uint16_t data)
{
	m_icount--;
	m_program.write_word(adr & ~1, data);
}

bool h8_device::exr_in_stack() const
{
	return false;
}

void h8_device::prefetch_done()
{
	if(m_requested_state != -1) {
		m_inst_state = m_requested_state;
		m_requested_state = -1;
	} else if(m_current_dma != -1)
		m_inst_state = STATE_DMA;
	else if(m_current_dtc)
		m_inst_state = STATE_DTC;
	else if(m_irq_vector) {
		m_inst_state = STATE_IRQ;
		m_taken_irq_vector = m_irq_vector;
		m_taken_irq_level = m_irq_level;
	} else if(m_has_trace && (m_EXR & EXR_T) && exr_in_stack())
		m_inst_state = STATE_TRACE;
	else
		m_inst_state = m_IR[0] = m_PIR;
}

void h8_device::prefetch_done_noirq()
{
	if(m_has_trace && (m_EXR & EXR_T) && exr_in_stack())
		m_inst_state = STATE_TRACE;
	else
		m_inst_state = m_IR[0] = m_PIR;
}

void h8_device::prefetch_done_noirq_notrace()
{
	m_inst_state = m_IR[0] = m_PIR;
}

void h8_device::set_irq(int irq_vector, int irq_level, bool irq_nmi)
{
	m_irq_vector = irq_vector;
	m_irq_level = irq_level;
	m_irq_nmi = irq_nmi;
}

void h8_device::internal(int cycles)
{
	m_icount -= cycles;

	// All internal operations take an even number of states (at least 2 each) on H8/300L and H8/300H
	if(!m_has_exr)
		m_icount--;
}

void h8_device::illegal()
{
	logerror("Illegal instruction at address %x\n", m_PPC);
	m_icount = -10000000;
}

int h8_device::trace_setup()
{
	throw emu_fatalerror("%s: Trace setup called but unimplemented.\n", tag());
}

int h8_device::trapa_setup()
{
	throw emu_fatalerror("%s: Trapa setup called but unimplemented.\n", tag());
}

uint8_t h8_device::do_addx8(uint8_t v1, uint8_t v2)
{
	uint16_t res = v1 + v2 + (m_CCR & F_C ? 1 : 0);
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xf) + (v2 & 0xf) + (m_CCR & F_C ? 1 : 0)) & 0x10)
			m_CCR |= F_H;
	}
	if(!uint8_t(res))
		m_CCR |= F_Z;
	else if(int8_t(res) < 0)
		m_CCR |= F_N;
	if(~(v1^v2) & (v1^res) & 0x80)
		m_CCR |= F_V;
	if(res & 0x100)
		m_CCR |= F_C;
	return res;

}

uint8_t h8_device::do_subx8(uint8_t v1, uint8_t v2)
{
	uint16_t res = v1 - v2 - (m_CCR & F_C ? 1 : 0);
	m_CCR &= ~(F_N|F_V|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xf) - (v2 & 0xf) - (m_CCR & F_C ? 1 : 0)) & 0x10)
			m_CCR |= F_H;
	}
	if(uint8_t(res))
		m_CCR &= ~F_Z;
	if(int8_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80)
		m_CCR |= F_V;
	if(res & 0x100)
		m_CCR |= F_C;
	return res;

}

uint8_t h8_device::do_inc8(uint8_t v1, uint8_t v2)
{
	uint8_t res = v1 + v2;
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!res)
		m_CCR |= F_Z;
	else if(int8_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80)
		m_CCR |= F_V;
	return res;
}

uint16_t h8_device::do_inc16(uint16_t v1, uint16_t v2)
{
	uint16_t res = v1 + v2;
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!res)
		m_CCR |= F_Z;
	else if(int16_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x8000)
		m_CCR |= F_V;
	return res;
}

uint32_t h8_device::do_inc32(uint32_t v1, uint32_t v2)
{
	uint32_t res = v1 + v2;
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!res)
		m_CCR |= F_Z;
	else if(int32_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80000000)
		m_CCR |= F_V;
	return res;
}

uint8_t h8_device::do_add8(uint8_t v1, uint8_t v2)
{
	uint16_t res = v1 + v2;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xf) + (v2 & 0xf)) & 0x10)
			m_CCR |= F_H;
	}
	if(!uint8_t(res))
		m_CCR |= F_Z;
	else if(int8_t(res) < 0)
		m_CCR |= F_N;
	if(~(v1^v2) & (v1^res) & 0x80)
		m_CCR |= F_V;
	if(res & 0x100)
		m_CCR |= F_C;
	return res;

}

uint16_t h8_device::do_add16(uint16_t v1, uint16_t v2)
{
	uint32_t res = v1 + v2;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xfff) + (v2 & 0xffff)) & 0x1000)
			m_CCR |= F_H;
	}
	if(!uint16_t(res))
		m_CCR |= F_Z;
	else if(int16_t(res) < 0)
		m_CCR |= F_N;
	if(~(v1^v2) & (v1^res) & 0x8000)
		m_CCR |= F_V;
	if(res & 0x10000)
		m_CCR |= F_C;
	return res;

}

uint32_t h8_device::do_add32(uint32_t v1, uint32_t v2)
{
	uint64_t res = uint64_t(v1) + uint64_t(v2);
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xfffffff) + (v2 & 0xfffffff)) & 0x10000000)
			m_CCR |= F_H;
	}
	if(!uint32_t(res))
		m_CCR |= F_Z;
	else if(int32_t(res) < 0)
		m_CCR |= F_N;
	if(~(v1^v2) & (v1^res) & 0x80000000)
		m_CCR |= F_V;
	if(res & 0x100000000U)
		m_CCR |= F_C;
	return res;
}

uint8_t h8_device::do_dec8(uint8_t v1, uint8_t v2)
{
	uint8_t res = v1 - v2;
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!res)
		m_CCR |= F_Z;
	else if(int8_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80)
		m_CCR |= F_V;
	return res;
}

uint16_t h8_device::do_dec16(uint16_t v1, uint16_t v2)
{
	uint16_t res = v1 - v2;
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!res)
		m_CCR |= F_Z;
	else if(int16_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x8000)
		m_CCR |= F_V;
	return res;
}

uint32_t h8_device::do_dec32(uint32_t v1, uint32_t v2)
{
	uint32_t res = v1 - v2;
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!res)
		m_CCR |= F_Z;
	else if(int32_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80000000)
		m_CCR |= F_V;
	return res;
}

uint8_t h8_device::do_sub8(uint8_t v1, uint8_t v2)
{
	uint16_t res = v1 - v2;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xf) - (v2 & 0xf)) & 0x10)
			m_CCR |= F_H;
	}
	if(!uint8_t(res))
		m_CCR |= F_Z;
	else if(int8_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80)
		m_CCR |= F_V;
	if(res & 0x100)
		m_CCR |= F_C;
	return res;

}

uint16_t h8_device::do_sub16(uint16_t v1, uint16_t v2)
{
	uint32_t res = v1 - v2;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xfff) - (v2 & 0xffff)) & 0x1000)
			m_CCR |= F_H;
	}
	if(!uint16_t(res))
		m_CCR |= F_Z;
	else if(int16_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x8000)
		m_CCR |= F_V;
	if(res & 0x10000)
		m_CCR |= F_C;
	return res;

}

uint32_t h8_device::do_sub32(uint32_t v1, uint32_t v2)
{
	uint64_t res = uint64_t(v1) - uint64_t(v2);
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(m_has_hc)
	{
		m_CCR &= ~F_H;
		if(((v1 & 0xfffffff) - (v2 & 0xfffffff)) & 0x10000000)
			m_CCR |= F_H;
	}
	if(!uint32_t(res))
		m_CCR |= F_Z;
	else if(int32_t(res) < 0)
		m_CCR |= F_N;
	if((v1^v2) & (v1^res) & 0x80000000)
		m_CCR |= F_V;
	if(res & 0x100000000U)
		m_CCR |= F_C;
	return res;
}

uint8_t h8_device::do_shal8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80)
		m_CCR |= F_C;
	if((v & 0xc0) == 0x40 || (v & 0xc0) == 0x80)
		m_CCR |= F_V;
	v <<= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_shal16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x8000)
		m_CCR |= F_C;
	if((v & 0xc000) == 0x4000 || (v & 0xc000) == 0x8000)
		m_CCR |= F_V;
	v <<= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_shal32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80000000)
		m_CCR |= F_C;
	if((v & 0xc0000000) == 0x40000000 || (v & 0xc0000000) == 0x80000000)
		m_CCR |= F_V;
	v <<= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_shar8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 1)
		m_CCR |= F_C;
	v >>= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(v & 0x40) {
		v |= 0x80;
		m_CCR |= F_N;
	}
	return v;
}

uint16_t h8_device::do_shar16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 1)
		m_CCR |= F_C;
	v >>= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(v & 0x4000) {
		v |= 0x8000;
		m_CCR |= F_N;
	}
	return v;
}

uint32_t h8_device::do_shar32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 1)
		m_CCR |= F_C;
	v >>= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(v & 0x40000000) {
		v |= 0x80000000;
		m_CCR |= F_N;
	}
	return v;
}

uint8_t h8_device::do_shll8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80)
		m_CCR |= F_C;
	v <<= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_shll16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x8000)
		m_CCR |= F_C;
	v <<= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_shll32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80000000)
		m_CCR |= F_C;
	v <<= 1;
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_shlr8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 1)
		m_CCR |= F_C;
	v >>= 1;
	if(!v)
		m_CCR |= F_Z;
	return v;
}

uint16_t h8_device::do_shlr16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 1)
		m_CCR |= F_C;
	v >>= 1;
	if(!v)
		m_CCR |= F_Z;
	return v;
}

uint32_t h8_device::do_shlr32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 1)
		m_CCR |= F_C;
	v >>= 1;
	if(!v)
		m_CCR |= F_Z;
	return v;
}

uint8_t h8_device::do_shal2_8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40)
		m_CCR |= F_C;
	if((v & 0xc0) == 0x40 || (v & 0xc0) == 0x80 ||
		(v & 0x60) == 0x20 || (v & 0x60) == 0x40)
		m_CCR |= F_V;
	v <<= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_shal2_16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x4000)
		m_CCR |= F_C;
	if((v & 0xc000) == 0x4000 || (v & 0xc000) == 0x8000 ||
		(v & 0x6000) == 0x2000 || (v & 0x6000) == 0x4000)
		m_CCR |= F_V;
	v <<= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_shal2_32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40000000)
		m_CCR |= F_C;
	if((v & 0xc0000000) == 0x40000000 || (v & 0xc0000000) == 0x80000000 ||
		(v & 0x60000000) == 0x20000000 || (v & 0x60000000) == 0x40000000)
		m_CCR |= F_V;
	v <<= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_shar2_8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 2)
		m_CCR |= F_C;
	v >>= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(v & 0x20) {
		v |= 0xc0;
		m_CCR |= F_N;
	}
	return v;
}

uint16_t h8_device::do_shar2_16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 2)
		m_CCR |= F_C;
	v >>= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(v & 0x2000) {
		v |= 0xc000;
		m_CCR |= F_N;
	}
	return v;
}

uint32_t h8_device::do_shar2_32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 2)
		m_CCR |= F_C;
	v >>= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(v & 0x20000000) {
		v |= 0xc0000000;
		m_CCR |= F_N;
	}
	return v;
}

uint8_t h8_device::do_shll2_8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40)
		m_CCR |= F_C;
	v <<= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_shll2_16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x4000)
		m_CCR |= F_C;
	v <<= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_shll2_32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40000000)
		m_CCR |= F_C;
	v <<= 2;
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_shlr2_8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 2)
		m_CCR |= F_C;
	v >>= 2;
	if(!v)
		m_CCR |= F_Z;
	return v;
}

uint16_t h8_device::do_shlr2_16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 2)
		m_CCR |= F_C;
	v >>= 2;
	if(!v)
		m_CCR |= F_Z;
	return v;
}

uint32_t h8_device::do_shlr2_32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 2)
		m_CCR |= F_C;
	v >>= 2;
	if(!v)
		m_CCR |= F_Z;
	return v;
}

uint8_t h8_device::do_rotl8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80)
		m_CCR |= F_C;
	v = (v << 1) | (v >> 7);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotl16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x8000)
		m_CCR |= F_C;
	v = (v << 1) | (v >> 15);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotl32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80000000)
		m_CCR |= F_C;
	v = (v << 1) | (v >> 31);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotr8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x01)
		m_CCR |= F_C;
	v = (v << 7) | (v >> 1);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotr16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x0001)
		m_CCR |= F_C;
	v = (v << 15) | (v >> 1);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotr32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x00000001)
		m_CCR |= F_C;
	v = (v << 31) | (v >> 1);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotxl8(uint8_t v)
{
	uint8_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80)
		m_CCR |= F_C;
	v = (v << 1) | c;
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotxl16(uint16_t v)
{
	uint16_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x8000)
		m_CCR |= F_C;
	v = (v << 1) | c;
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotxl32(uint32_t v)
{
	uint32_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x80000000)
		m_CCR |= F_C;
	v = (v << 1) | c;
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotxr8(uint8_t v)
{
	uint8_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x01)
		m_CCR |= F_C;
	v = (v >> 1) | (c << 7);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotxr16(uint16_t v)
{
	uint8_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x0001)
		m_CCR |= F_C;
	v = (v >> 1) | (c << 15);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotxr32(uint32_t v)
{
	uint8_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x00000001)
		m_CCR |= F_C;
	v = (v >> 1) | (c << 31);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotl2_8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40)
		m_CCR |= F_C;
	v = (v << 2) | (v >> 6);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotl2_16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x4000)
		m_CCR |= F_C;
	v = (v << 2) | (v >> 14);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotl2_32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40000000)
		m_CCR |= F_C;
	v = (v << 2) | (v >> 30);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotr2_8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x02)
		m_CCR |= F_C;
	v = (v << 6) | (v >> 2);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotr2_16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x0002)
		m_CCR |= F_C;
	v = (v << 14) | (v >> 2);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotr2_32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x00000002)
		m_CCR |= F_C;
	v = (v << 30) | (v >> 2);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotxl2_8(uint8_t v)
{
	uint8_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40)
		m_CCR |= F_C;
	v = (v << 2) | (c << 1) | ((v >> 6) & 0x01);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotxl2_16(uint16_t v)
{
	uint16_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x4000)
		m_CCR |= F_C;
	v = (v << 2) | (c << 1) | ((v >> 14) & 0x0001);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotxl2_32(uint32_t v)
{
	uint32_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x40000000)
		m_CCR |= F_C;
	v = (v << 2) | (c << 1) | ((v >> 30) & 0x00000001);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint8_t h8_device::do_rotxr2_8(uint8_t v)
{
	uint8_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x02)
		m_CCR |= F_C;
	v = (v >> 2) | (c << 6) | (v << 7);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint16_t h8_device::do_rotxr2_16(uint16_t v)
{
	uint16_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x0002)
		m_CCR |= F_C;
	v = (v >> 2) | (c << 14) | (v << 15);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

uint32_t h8_device::do_rotxr2_32(uint32_t v)
{
	uint32_t c = m_CCR & F_C ? 1 : 0;
	m_CCR &= ~(F_N|F_V|F_Z|F_C);
	if(v & 0x00000002)
		m_CCR |= F_C;
	v = (v >> 2) | (c << 30) | (v << 31);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
	return v;
}

void h8_device::set_nzv8(uint8_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!v)
		m_CCR |= F_Z;
	else if(int8_t(v) < 0)
		m_CCR |= F_N;
}

void h8_device::set_nzv16(uint16_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
}

void h8_device::set_nzv32(uint32_t v)
{
	m_CCR &= ~(F_N|F_V|F_Z);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
}

void h8_device::set_nz16(uint16_t v)
{
	m_CCR &= ~(F_N|F_Z);
	if(!v)
		m_CCR |= F_Z;
	else if(int16_t(v) < 0)
		m_CCR |= F_N;
}

void h8_device::set_nz32(uint32_t v)
{
	m_CCR &= ~(F_N|F_Z);
	if(!v)
		m_CCR |= F_Z;
	else if(int32_t(v) < 0)
		m_CCR |= F_N;
}

std::unique_ptr<util::disasm_interface> h8_device::create_disassembler()
{
	return std::make_unique<h8_disassembler>();
}

#include "cpu/h8/h8.hxx"
