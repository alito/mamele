// license:BSD-3-Clause
// copyright-holders:Olivier Galibert

// Samsung Semiconductor KS0164 Wavetable Synthesizer

#include "emu.h"
#include "ks0164.h"


DEFINE_DEVICE_TYPE(KS0164, ks0164_device, "ks0164", "Samsung KS0164 Wavetable Synthesizer")

ks0164_device::ks0164_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, KS0164, tag, owner, clock),
	  device_sound_interface(mconfig, *this),
	  device_memory_interface(mconfig, *this),
	  m_mem_region(*this, DEVICE_SELF),
	  m_cpu(*this, "cpu"),
	  m_mem_config("mem", ENDIANNESS_BIG, 16, 23)
{
}

device_memory_interface::space_config_vector ks0164_device::memory_space_config() const
{
	return space_config_vector {
		std::make_pair(0, &m_mem_config)
	};
}

void ks0164_device::device_add_mconfig(machine_config &config)
{
	KS0164CPU(config, m_cpu, DERIVED_CLOCK(1, 6));
	m_cpu->set_addrmap(AS_PROGRAM, &ks0164_device::cpu_map);
}

void ks0164_device::device_start()
{
	if(!has_configured_map(0) && m_mem_region) {
		u32 size = m_mem_region->bytes();
		u32 rend = size-1;

		// Round up to the nearest power-of-two-minus-one
		u32 rmask = rend;
		rmask |= rmask >> 1;
		rmask |= rmask >> 2;
		rmask |= rmask >> 4;
		rmask |= rmask >> 8;
		rmask |= rmask >> 16;
		// Mirror over the high bits.  rmask is a
		// power-of-two-minus-one, so the xor works
		space().install_rom(0, rend, ((1 << 23) - 1) ^ rmask, m_mem_region->base());
	}

	m_stream = stream_alloc(0, 2, 44100);
	m_mem_cache = space().cache<1, 0, ENDIANNESS_BIG>();
	m_timer = timer_alloc(0);

	save_item(NAME(m_bank1_base));
	save_item(NAME(m_bank1_select));
	save_item(NAME(m_bank2_base));
	save_item(NAME(m_bank2_select));
	save_item(NAME(m_sregs));
	save_item(NAME(m_mpu_in));
	save_item(NAME(m_mpu_out));
	save_item(NAME(m_mpu_status));
	save_item(NAME(m_unk60));
	save_item(NAME(m_voice_select));
	save_item(NAME(m_irqen_76));
	save_item(NAME(m_irqen_77));
	save_item(NAME(m_timer_interrupt));
}

void ks0164_device::device_reset()
{
	m_bank1_select = 0;
	m_bank1_base = 0;
	m_bank2_select = 0;
	m_bank2_base = 0;
	memset(m_sregs, 0, sizeof(m_sregs));
	m_unk60 = 0;
	m_voice_select = 0;
	m_irqen_76 = 0;
	m_irqen_77 = 0;
	m_timer_interrupt = false;

	m_mpu_in = 0x00;
	m_mpu_out = 0x00;
	m_mpu_status = 0x00;

	m_timer->adjust(attotime::from_msec(1), 0, attotime::from_msec(1));
}

void ks0164_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	m_timer_interrupt = true;
	if(m_irqen_76 & 0x40)
		m_cpu->set_input_line(14, ASSERT_LINE);
}

void ks0164_device::mpuin_set(bool control, u8 data)
{
	//  logerror("mpu push %s %02x\n", control ? "ctrl" : "data", data);
	m_mpu_in = data;
	if(control)
		m_mpu_status |= MPUS_RX_CTRL;
	else
		m_mpu_status &= ~MPUS_RX_CTRL;
	m_mpu_status |= MPUS_RX_FULL;

	if(m_mpu_status & MPUS_RX_INT)
		m_cpu->set_input_line(11, ASSERT_LINE);
}

void ks0164_device::mpu401_data_w(u8 data)
{
	mpuin_set(false, data);
}

void ks0164_device::mpu401_ctrl_w(u8 data)
{
	mpuin_set(true, data);
}

u8 ks0164_device::mpu401_data_r()
{
	//  logerror("mpu pop %02x\n", m_mpu_out);
	return m_mpu_out;
}

u8 ks0164_device::mpu401_status_r()
{
	u8 res = 0x3f;
	if(!(m_mpu_status & MPUS_TX_FULL))
		res |= 0x80;
	if(m_mpu_status & MPUS_RX_FULL)
		res |= 0x40;

	static std::string pc;
	static u8 pr;

	std::string cc = machine().describe_context();
	if(pc != cc || pr != res) {
		//      logerror("status read %02x (%s)\n", res, cc);
		pc = cc;
		pr = res;
	}
	return res;
}

u8 ks0164_device::mpu401_istatus_r()
{
	//  logerror("mpu istatus read %02x (%04x)\n", m_mpu_status, m_cpu->pc());
	return m_mpu_status;
}

void ks0164_device::mpu401_istatus_w(u8 data)
{
	m_mpu_status = (m_mpu_status & ~(MPUS_RX_INT|MPUS_TX_INT)) | (data & (MPUS_RX_INT|MPUS_TX_INT));
	m_cpu->set_input_line(11, (m_mpu_status & (MPUS_RX_INT|MPUS_RX_FULL)) == (MPUS_RX_INT|MPUS_RX_FULL) ? ASSERT_LINE : CLEAR_LINE);
	//  logerror("mpu status write %02x (%04x)\n", m_mpu_status, m_cpu->pc());
}

u8 ks0164_device::mpu401_r()
{
	m_mpu_status &= ~MPUS_RX_FULL;
	m_cpu->set_input_line(11, CLEAR_LINE);
	//  logerror("mpu_r %02x (%04x)\n", m_mpu_in, m_cpu->pc());
	return m_mpu_in;
}

void ks0164_device::mpu401_w(u8 data)
{
	m_mpu_out = data;
	m_mpu_status |= MPUS_TX_FULL;
	//  logerror("mpu_w %02x (%04x)\n", m_mpu_out, m_cpu->pc());
}

u16 ks0164_device::vec_r(offs_t offset, u16 mem_mask)
{
	return m_mem_cache->read_word(offset << 1, mem_mask);
}

u16 ks0164_device::rom_r(offs_t offset, u16 mem_mask)
{
	return m_mem_cache->read_word((offset << 1) + 0x80, mem_mask);
}

u16 ks0164_device::bank1_r(offs_t offset, u16 mem_mask)
{
	return m_mem_cache->read_word(((offset << 1) & 0x3fff) | m_bank1_base, mem_mask);
}

void ks0164_device::bank1_w(offs_t offset, u16 data, u16 mem_mask)
{
	m_mem_cache->write_word(((offset << 1) & 0x3fff) | m_bank1_base, data, mem_mask);
}

u16 ks0164_device::bank2_r(offs_t offset, u16 mem_mask)
{
	return m_mem_cache->read_word(((offset << 1) & 0x3fff) | m_bank2_base, mem_mask);
}

void ks0164_device::bank2_w(offs_t offset, u16 data, u16 mem_mask)
{
	m_mem_cache->write_word(((offset << 1) & 0x3fff) | m_bank2_base, data, mem_mask);
}

u16 ks0164_device::bank1_select_r()
{
	return m_bank1_select;
}

void ks0164_device::bank1_select_w(offs_t, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_bank1_select);
	m_bank1_base = m_bank1_select << 14;
}

u16 ks0164_device::bank2_select_r()
{
	return m_bank2_select;
}

void ks0164_device::bank2_select_w(offs_t, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_bank2_select);
	m_bank2_base = m_bank2_select << 14;
}

u16 ks0164_device::voice_r(offs_t offset)
{
	return m_sregs[m_voice_select & 0x1f][offset];
}

void ks0164_device::voice_w(offs_t offset, u16 data, u16 mem_mask)
{
	COMBINE_DATA(&m_sregs[m_voice_select & 0x1f][offset]);
	if(1 || m_cpu->pc() < 0x5f94 || m_cpu->pc() > 0x5fc0)
		logerror("voice %02x.%02x = %04x (%04x)\n", m_voice_select & 0x1f, offset, m_sregs[m_voice_select & 0x1f][offset], m_cpu->pc());
}

u8 ks0164_device::irqen_76_r()
{
	return m_irqen_76;
}

void ks0164_device::irqen_76_w(u8 data)
{
	m_irqen_76 = data;
	if(m_irqen_76 & 0x40)
		m_cpu->set_input_line(14, m_timer_interrupt ? ASSERT_LINE : CLEAR_LINE);

	else {
		m_timer_interrupt = false;
		m_cpu->set_input_line(14, CLEAR_LINE);
	}

	//  logerror("irqen_76 = %02x (%04x)\n", m_irqen_76, m_cpu->pc());
}

u8 ks0164_device::irqen_77_r()
{
	return m_irqen_77;
}

void ks0164_device::irqen_77_w(u8 data)
{
	m_irqen_77 = data;
	logerror("irqen_77 = %02x (%04x)\n", m_irqen_77, m_cpu->pc());
}

u8 ks0164_device::unk60_r()
{
	return m_unk60;
}

void ks0164_device::unk60_w(u8 data)
{
	m_unk60 = data;
	logerror("unk60 = %02x (%04x)\n", m_unk60, m_cpu->pc());
}

u8 ks0164_device::voice_select_r()
{
	return m_voice_select;
}

void ks0164_device::voice_select_w(u8 data)
{
	m_voice_select = data;
	logerror("voice_select = %02x (%04x)\n", m_voice_select, m_cpu->pc());
}

void ks0164_device::cpu_map(address_map &map)
{
	map(0x0000, 0x001f).r(FUNC(ks0164_device::vec_r));

	map(0x0020, 0x005f).rw(FUNC(ks0164_device::voice_r), FUNC(ks0164_device::voice_w));
	map(0x0060, 0x0060).rw(FUNC(ks0164_device::unk60_r), FUNC(ks0164_device::unk60_w));
	map(0x0061, 0x0061).rw(FUNC(ks0164_device::voice_select_r), FUNC(ks0164_device::voice_select_w));
	map(0x0062, 0x0063).rw(FUNC(ks0164_device::bank1_select_r), FUNC(ks0164_device::bank1_select_w));
	map(0x0064, 0x0065).rw(FUNC(ks0164_device::bank2_select_r), FUNC(ks0164_device::bank2_select_w));

	map(0x0068, 0x0068).rw(FUNC(ks0164_device::mpu401_r), FUNC(ks0164_device::mpu401_w));
	map(0x0069, 0x0069).rw(FUNC(ks0164_device::mpu401_istatus_r), FUNC(ks0164_device::mpu401_istatus_w));

	map(0x0076, 0x0076).rw(FUNC(ks0164_device::irqen_76_r), FUNC(ks0164_device::irqen_76_w));
	map(0x0077, 0x0077).rw(FUNC(ks0164_device::irqen_77_r), FUNC(ks0164_device::irqen_77_w));

	map(0x0080, 0x3fff).r(FUNC(ks0164_device::rom_r));
	map(0x4000, 0x7fff).rw(FUNC(ks0164_device::bank1_r), FUNC(ks0164_device::bank1_w));
	map(0x8000, 0xbfff).rw(FUNC(ks0164_device::bank2_r), FUNC(ks0164_device::bank2_w));
	map(0xe000, 0xffff).ram();
}

void ks0164_device::sound_stream_update(sound_stream &stream, stream_sample_t **inputs, stream_sample_t **outputs, int samples)
{
}
