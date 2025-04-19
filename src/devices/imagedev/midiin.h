// license:BSD-3-Clause
// copyright-holders:R. Belmont,Aaron Giles
/*********************************************************************

    midiin.h

    MIDI In image device

*********************************************************************/

#ifndef MAME_IMAGEDEV_MIDIIN_H
#define MAME_IMAGEDEV_MIDIIN_H

#pragma once

#include "diserial.h"


/***************************************************************************
    TYPE DEFINITIONS
***************************************************************************/

class midiin_device :    public device_t,
						public device_image_interface,
						public device_serial_interface
{
public:
	// construction/destruction
	midiin_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	auto input_callback() { return m_input_cb.bind(); }

	// image-level overrides
	virtual image_init_result call_load() override;
	virtual void call_unload() override;

	// image device
	virtual iodevice_t image_type() const noexcept override { return IO_MIDIIN; }
	virtual bool is_readable()  const noexcept override { return true; }
	virtual bool is_writeable() const noexcept override { return false; }
	virtual bool is_creatable() const noexcept override { return false; }
	virtual bool must_be_loaded() const noexcept override { return false; }
	virtual bool is_reset_on_load() const noexcept override { return false; }
	virtual const char *file_extensions() const noexcept override { return "mid"; }
	virtual bool core_opens_image_file() const noexcept override { return false; }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr) override;

	// serial overrides
	virtual void tra_complete() override;    // Tx completed sending byte
	virtual void tra_callback() override;    // Tx send bit

private:
	static const int XMIT_RING_SIZE = (8192*4*4);

	void xmit_char(uint8_t data);

	std::unique_ptr<osd_midi_device> m_midi;
	emu_timer *m_timer;
	devcb_write_line        m_input_cb;
	uint8_t m_xmitring[XMIT_RING_SIZE];
	int m_xmit_read, m_xmit_write;
	bool m_tx_busy;

	// internal helper class for parsing
	class midi_parser
	{
	public:
		// minimal error class
		class error
		{
		public:
			error(char const *description) : m_description(description) { }
			char const *description() const { return m_description; }
		private:
			char const *m_description;
		};

		// construction
		midi_parser(u8 const *data, u32 length, u32 offset);

		// end of buffer?
		bool eob() const { return (m_offset >= m_length); }

		// create a subset
		midi_parser subset(u32 length);

		// change position within buffer
		midi_parser &rewind(u32 count);
		midi_parser &reset() { return rewind(m_offset); }

		// read data of various sizes and endiannesses
		u8 byte() { check_bounds(1); return m_data[m_offset++]; }
		u16 word_be() { u16 result = byte() << 8; return result | byte(); }
		u32 triple_be() { u32 result = word_be() << 8; return result | byte(); }
		u32 dword_be() { u32 result = word_be() << 16; return result | word_be(); }
		u32 dword_le() { return swapendian_int32(dword_be()); }

		// special variable reader for MIDI
		u32 variable();

	private:
		// internal helper
		void check_bounds(u32 length);

		// internal state
		u8 const *m_data;
		u32 m_length;
		u32 m_offset;
	};

	// internal helper class reperesenting an event at a given
	// time containing MIDI data
	class midi_event
	{
	public:
		// constructor
		midi_event(u32 tick) :
			m_tick(tick) { }

		// simple getters
		u32 tick() const { return m_tick; }
		attotime const &time() const { return m_time; }
		std::vector<u8> const &data() const { return m_data; }

		// simple setters
		void set_time(attotime const &time) { m_time = time; }

		// append data to the buffer
		midi_event &append(u8 byte) { m_data.push_back(byte); return *this; }

	private:
		// internal state
		u32 m_tick;
		attotime m_time;
		std::vector<u8> m_data;
	};

	// internal helper class representing a MIDI sequence
	class midi_sequence
	{
	public:
		// constructor
		midi_sequence() : m_iterator(m_list.begin()) { }

		// clear the sequence
		void clear() { m_list.clear(); }

		// parse a new sequence
		bool parse(u8 const *data, u32 length);

		// rewind to the start of time
		void rewind(attotime const &basetime);
		midi_event *current_event() const { return (m_iterator == m_list.end()) ? nullptr : &(*m_iterator); }
		midi_event *advance_event() { ++m_iterator; return current_event(); }
		attotime const &duration() { return m_list.back().time(); }

	private:
		// internal helpers
		midi_event &event_at(u32 tick);
		u32 parse_track_data(midi_parser &buffer, u32 start_tick);
		void parse_midi_data(midi_parser &buffer);

		// internal state
		std::list<midi_event> m_list;
		std::list<midi_event>::iterator m_iterator;
	};

	midi_sequence m_sequence;
	attotime m_sequence_start;
};

// device type definition
DECLARE_DEVICE_TYPE(MIDIIN, midiin_device)

// device iterator
typedef device_type_enumerator<midiin_device> midiin_device_enumerator;

#endif // MAME_IMAGEDEV_MIDIIN_H
