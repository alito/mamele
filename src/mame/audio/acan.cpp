// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, superctr
/***************************************************************************

    Super A'Can sound driver

    Currently has a number of unknown registers and functionality.

****************************************************************************/

#include "emu.h"
#include "acan.h"

#define VERBOSE     (1)
#include "logmacro.h"

// device type definition
DEFINE_DEVICE_TYPE(ACANSND, acan_sound_device, "acansound", "Super A'Can Audio")

acan_sound_device::acan_sound_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, ACANSND, tag, owner, clock)
	, device_sound_interface(mconfig, *this)
	, m_stream(nullptr)
	, m_timer(nullptr)
	, m_timer_irq_handler(*this)
	, m_dma_irq_handler(*this)
	, m_ram_read(*this)
	, m_active_channels(0)
	, m_dma_channels(0)
{
}


void acan_sound_device::device_start()
{
	m_stream = stream_alloc(0, 2, clock() / 16 / 5);
	m_mix = std::make_unique<int32_t[]>((clock() / 16 / 5) * 2);
	m_timer = timer_alloc(0);

	m_timer_irq_handler.resolve_safe();
	m_dma_irq_handler.resolve_safe();
	m_ram_read.resolve_safe(0);

	// register for savestates
	save_item(NAME(m_active_channels));
	save_item(NAME(m_dma_channels));
	save_item(STRUCT_MEMBER(m_channels, pitch));
	save_item(STRUCT_MEMBER(m_channels, length));
	save_item(STRUCT_MEMBER(m_channels, start_addr));
	save_item(STRUCT_MEMBER(m_channels, curr_addr));
	save_item(STRUCT_MEMBER(m_channels, end_addr));
	save_item(STRUCT_MEMBER(m_channels, addr_increment));
	save_item(STRUCT_MEMBER(m_channels, frac));
	save_item(STRUCT_MEMBER(m_channels, register9));
	save_item(STRUCT_MEMBER(m_channels, envelope));
	save_item(STRUCT_MEMBER(m_channels, volume));
	save_item(STRUCT_MEMBER(m_channels, volume_l));
	save_item(STRUCT_MEMBER(m_channels, volume_r));
	save_item(STRUCT_MEMBER(m_channels, one_shot));
	save_item(NAME(m_regs));
}

void acan_sound_device::device_reset()
{
	m_active_channels = 0;
	m_dma_channels = 0;
	std::fill(std::begin(m_regs), std::end(m_regs), 0);

	m_timer->reset();
	m_timer_irq_handler(0);
	m_dma_irq_handler(0);
}

void acan_sound_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	if (m_regs[0x14] & 0x40)
	{
		m_timer_irq_handler(1);

		// Update frequency
		uint16_t period = (m_regs[0x12] << 8) + m_regs[0x11];
		m_timer->adjust(clocks_to_attotime(10 * (0x10000 - period)), 0);
	}
}

void acan_sound_device::sound_stream_update(sound_stream &stream, std::vector<read_stream_view> const &inputs, std::vector<write_stream_view> &outputs)
{
	std::fill_n(&m_mix[0], outputs[0].samples() * 2, 0);

	for (int i = 0; i < 16 && m_active_channels != 0; i++)
	{
		if (BIT(m_active_channels, i))
		{
			acan_channel &channel = m_channels[i];
			int32_t *mixp = &m_mix[0];

			for (int s = 0; s < outputs[0].samples(); s++)
			{
				uint8_t data = m_ram_read(channel.curr_addr) + 0x80;
				int16_t sample = (int16_t)(data << 8);

				channel.frac += channel.addr_increment;
				channel.curr_addr += (uint16_t)(channel.frac >> 16);
				channel.frac = (uint16_t)channel.frac;

				*mixp++ += (sample * channel.volume_l) >> 8;
				*mixp++ += (sample * channel.volume_r) >> 8;

				if (channel.curr_addr >= channel.end_addr)
				{
					if (channel.register9)
					{
						m_dma_irq_handler(1);
						keyon_voice(i);
					}
					else if (channel.one_shot)
					{
						m_active_channels &= ~(1 << i);
					}
					else
					{
						channel.curr_addr -= channel.length;
					}
				}
			}
		}
	}

	int32_t *mixp = &m_mix[0];
	for (int i = 0; i < outputs[0].samples(); i++)
	{
		outputs[0].put_int(i, *mixp++, 32768 << 4);
		outputs[1].put_int(i, *mixp++, 32768 << 4);
	}
}

uint8_t acan_sound_device::read(offs_t offset)
{
	if (offset == 0x14)
	{
		// acknowledge timer IRQ?
		m_timer_irq_handler(0);
	}
	else if (offset == 0x16)
	{
		// acknowledge DMA IRQ?
		m_dma_irq_handler(0);
	}
	return m_regs[offset];
}

void acan_sound_device::keyon_voice(uint8_t voice)
{
	acan_channel &channel = m_channels[voice];
	channel.curr_addr = channel.start_addr << 6;
	channel.end_addr = channel.curr_addr + channel.length;

	m_active_channels |= (1 << voice);

	//printf("Keyon voice %d\n", voice);
}

void acan_sound_device::write(offs_t offset, uint8_t data)
{
	const uint8_t upper = (offset >> 4) & 0x0f;
	const uint8_t lower = offset & 0x0f;

	m_regs[offset] = data;

	switch (upper)
	{
	case 0x1:
		switch (lower)
		{
		case 0x1: // Timer frequency (low byte)
			LOG("%s: Sound timer frequency (low byte) = %02x\n", machine().describe_context(), data);
			break;

		case 0x2: // Timer frequency (high byte)
			LOG("%s: Sound timer frequency (high byte) = %02x\n", machine().describe_context(), data);
			break;

		case 0x4: // Timer control
			// The meaning of the data that is actually written is unknown
			LOG("%s: Sound timer control = %02x\n", machine().describe_context(), data);
			if (BIT(data, 7))
			{
				// Update frequency
				uint16_t period = (m_regs[0x12] << 8) + m_regs[0x11];
				m_timer->adjust(clocks_to_attotime(10 * (0x10000 - period)), 0);
			}
			break;

		case 0x6: // DMA-driven channel flags?
			// The meaning of the data that is actually written is unknown
			m_dma_channels = data << 8;
			LOG("%s: DMA-driven channel flag(?) = %02x\n", machine().describe_context(), data);
			break;

		case 0x7: // Keyon/keyoff
		{
			LOG("%s: Sound key control, voice %02x key%s\n", machine().describe_context(), data & 0xf, (data & 0xf0) ? "on" : "off");
			const uint16_t mask = 1 << (data & 0xf);
			if (data & 0xf0)
			{
				keyon_voice(data & 0xf);
			}
			else
			{
				m_active_channels &= ~mask;
			}
			break;
		}

		default:
			LOG("Unknown sound register: %02x = %02x\n", offset, data);
			break;
		}
		break;

	case 0x2: // Pitch (low byte)
	{
		acan_channel &channel = m_channels[lower];
		channel.pitch &= 0xff00;
		channel.pitch |= data;
		channel.addr_increment = (uint32_t)channel.pitch << 6;
		break;
	}

	case 0x3: // Pitch (high byte)
	{
		acan_channel &channel = m_channels[lower];
		channel.pitch &= 0x00ff;
		channel.pitch |= data << 8;
		channel.addr_increment = (uint32_t)channel.pitch << 6;
		break;
	}

	case 0x5: // Waveform length
	{
		acan_channel &channel = m_channels[lower];
		channel.length = 0x40 << ((data & 0x0e) >> 1);
		channel.one_shot = BIT(data, 0);
		LOG("%s: Waveform length and attributes (voice %02x): %02x\n", machine().describe_context(), lower, data);
		break;
	}

	case 0x6: // Waveform address (divided by 0x40, high byte)
	{
		acan_channel &channel = m_channels[lower];
		channel.start_addr &= 0x00ff;
		channel.start_addr |= data << 8;
		LOG("%s: Waveform address (high) (voice %02x): %02x, will be %04x\n", machine().describe_context(), lower, data, channel.start_addr << 6);
		break;
	}

	case 0x7: // Waveform address (divided by 0x40, low byte)
	{
		acan_channel &channel = m_channels[lower];
		channel.start_addr &= 0xff00;
		channel.start_addr |= data;
		LOG("%s: Waveform address (low) (voice %02x): %02x, will be %04x\n", machine().describe_context(), lower, data, channel.start_addr << 6);
		break;
	}

	case 0x9: // Unknown (set to 0xFF for DMA-driven channels)
	{
		acan_channel &channel = m_channels[lower];
		channel.register9 = data;
		LOG("%s: Unknown voice register 9 (voice %02x): %02x\n", machine().describe_context(), lower, data);
		break;
	}

	case 0xa: // Envelope Parameters? (not yet known)
	case 0xb:
	case 0xc:
	case 0xd:
		m_channels[lower].envelope[upper - 0xa] = data;
		LOG("%s: Envelope parameter %d (voice %02x) = %02x\n", machine().describe_context(), upper - 0xa, lower, data);
		break;

	case 0xe: // Volume
	{
		acan_channel &channel = m_channels[lower];
		channel.volume = data;
		channel.volume_l = (data & 0xf0) | (data >> 4);
		channel.volume_r = (data & 0x0f) | (data << 4);
		LOG("%s: Volume register (voice %02x) = = %02x\n", machine().describe_context(), lower, data);
		break;
	}

	default:
		LOG("Unknown sound register: %02x = %02x\n", offset, data);
		break;
	}
}
