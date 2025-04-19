// license:BSD-3-Clause
// copyright-holders:R. Belmont,Aaron Giles
/*********************************************************************

    midiin.c

    MIDI In image device and serial transmitter

*********************************************************************/

#include "emu.h"
#include "midiin.h"
#include "osdepend.h"

/***************************************************************************
    IMPLEMENTATION
***************************************************************************/

DEFINE_DEVICE_TYPE(MIDIIN, midiin_device, "midiin", "MIDI In image device")

/*-------------------------------------------------
    ctor
-------------------------------------------------*/

midiin_device::midiin_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, MIDIIN, tag, owner, clock),
		device_image_interface(mconfig, *this),
		device_serial_interface(mconfig, *this),
		m_midi(),
		m_timer(nullptr),
		m_input_cb(*this),
		m_xmit_read(0),
		m_xmit_write(0),
		m_tx_busy(false)
{
}

/*-------------------------------------------------
    device_start
-------------------------------------------------*/

void midiin_device::device_start()
{
	m_input_cb.resolve_safe();
	m_timer = timer_alloc(0);
	m_midi.reset();
	m_timer->enable(false);
}

void midiin_device::device_reset()
{
	m_tx_busy = false;
	m_xmit_read = m_xmit_write = 0;

	// we don't Rx, we Tx at 31250 8-N-1
	set_data_frame(1, 8, PARITY_NONE, STOP_BITS_1);
	set_rcv_rate(0);
	set_tra_rate(31250);
}

/*-------------------------------------------------
    device_timer
-------------------------------------------------*/

void midiin_device::device_timer(emu_timer &timer, device_timer_id id, int param, void *ptr)
{
	if (id == 0)
	{
		// if there's a sequence playing, that takes priority
		midi_event *event = m_sequence.current_event();
		if (event != nullptr)
		{
			attotime curtime = timer.expire();
			if (curtime < m_sequence_start)
			{
				// we could get called before we're supposed to start; show a countdown
				attotime delta = m_sequence_start - curtime;
				popmessage("Waiting to start MIDI playback... %d", delta.seconds());
				m_timer->adjust(std::min(delta, attotime(1, 0)));
			}
			else
			{
				// update the playback time
				curtime -= m_sequence_start;
				popmessage("Playing MIDI file: %d:%02d / %d:%02d", curtime.seconds() / 60, curtime.seconds() % 60, m_sequence.duration().seconds() / 60, m_sequence.duration().seconds() % 60);

				// if it's time to process the current event, do it and advance
				if (curtime >= event->time())
				{
					for (auto &curbyte : event->data())
						xmit_char(curbyte);
					event = m_sequence.advance_event();
				}

				// if there are more events, set a timer to trigger them
				// (minimum duration 1 sec so that our playback time doesn't skip)
				if (event != nullptr)
					m_timer->adjust(std::min(event->time() - curtime, attotime(1, 0)));
				else
					popmessage("End of MIDI file");
			}
		}
		else if (m_midi)
		{
			uint8_t buf[8192*4];
			int bytesRead;

			while (m_midi->poll())
			{
				bytesRead = m_midi->read(buf);

				if (bytesRead > 0)
				{
					for (int i = 0; i < bytesRead; i++)
					{
						xmit_char(buf[i]);
					}
				}
			}
		}
	}
}

/*-------------------------------------------------
    call_load
-------------------------------------------------*/

image_init_result midiin_device::call_load()
{
	// attempt to load if it's a real file
	m_err = load_image_by_path(OPEN_FLAG_READ, filename());
	if (m_err == IMAGE_ERROR_SUCCESS)
	{
		// if the parsing succeeds, schedule the start to happen at least
		// 10 seconds after starting to allow the keyboards to initialize
		// TODO: this should perhaps be a driver-configurable parameter?
		if (m_sequence.parse(reinterpret_cast<u8 *>(ptr()), length()))
		{
			m_sequence_start = std::max(machine().time(), attotime(10, 0));
			m_timer->adjust(attotime::zero);
			return image_init_result::PASS;
		}
		return image_init_result::FAIL;
	}
	else
	{
		m_midi = machine().osd().create_midi_device();

		if (!m_midi->open_input(filename()))
		{
			m_midi.reset();
			return image_init_result::FAIL;
		}

		m_timer->adjust(attotime::from_hz(1500), 0, attotime::from_hz(1500));
		return image_init_result::PASS;
	}
}

/*-------------------------------------------------
    call_unload
-------------------------------------------------*/

void midiin_device::call_unload()
{
	if (m_midi)
	{
		m_midi->close();
	}
	m_midi.reset();
	m_sequence.clear();
	m_timer->enable(false);
}

void midiin_device::tra_complete()
{
	// is there more waiting to send?
	if (m_xmit_read != m_xmit_write)
	{
//      printf("tx1 %02x\n", m_xmitring[m_xmit_read]);
		transmit_register_setup(m_xmitring[m_xmit_read++]);
		if (m_xmit_read >= XMIT_RING_SIZE)
		{
			m_xmit_read = 0;
		}
	}
	else
	{
		m_tx_busy = false;
	}
}

void midiin_device::tra_callback()
{
	int bit = transmit_register_get_data_bit();
	m_input_cb(bit);
}

void midiin_device::xmit_char(uint8_t data)
{
//  printf("MIDI in: xmit %02x\n", data);

	// if tx is busy it'll pick this up automatically when it completes
	if (!m_tx_busy)
	{
		m_tx_busy = true;
//      printf("tx0 %02x\n", data);
		transmit_register_setup(data);
	}
	else
	{
		// tx is busy, it'll pick this up next time
		m_xmitring[m_xmit_write++] = data;
		if (m_xmit_write >= XMIT_RING_SIZE)
		{
			m_xmit_write = 0;
		}
	}
}


//-------------------------------------------------
//  fourcc_le - helper to compute the little-endian
//  version of a fourcc value from a string
//-------------------------------------------------

static constexpr u32 fourcc_le(char const *string)
{
	return string[0] | (string[1] << 8) | (string[2] << 16) | (string[3] << 24);
}


//-------------------------------------------------
//  midi_parser - constructor
//-------------------------------------------------

midiin_device::midi_parser::midi_parser(u8 const *data, u32 length, u32 offset) :
	m_data(data),
	m_length(length),
	m_offset(offset)
{
}


//-------------------------------------------------
//  subset - construct a midi_parser that
//  represents a subset of the buffer, and advance
//  our offset past it
//-------------------------------------------------

midiin_device::midi_parser midiin_device::midi_parser::subset(u32 length)
{
	check_bounds(length);
	midi_parser result(m_data + m_offset, length, 0);
	m_offset += length;
	return result;
}


//-------------------------------------------------
//  rewind - back up by the given number of bytes
//-------------------------------------------------

midiin_device::midi_parser &midiin_device::midi_parser::rewind(u32 count)
{
	count = std::min(count, m_offset);
	m_offset -= count;
	return *this;
}


//-------------------------------------------------
//  variable - return the MIDI standard "variable"
//  value
//-------------------------------------------------

u32 midiin_device::midi_parser::variable()
{
	u32 result = 0;
	for (int which = 0; which < 4; which++)
	{
		u8 curbyte = byte();
		result = (result << 7) | (curbyte & 0x7f);
		if ((curbyte & 0x80) == 0)
			return result;
	}
	throw error("Invalid variable length field");
}


//-------------------------------------------------
//  check_bounds - check to see if we have at least
//  'length' bytes left to consume; if not,
//  throw an error
//-------------------------------------------------

void midiin_device::midi_parser::check_bounds(u32 length)
{
	if (m_offset + length > m_length)
		throw error("Out of bounds error");
}


//-------------------------------------------------
//  event_at - return a reference to the sequence
//  event at the given tick, or create a new one
//  if one doesn't yet exist
//-------------------------------------------------

midiin_device::midi_event &midiin_device::midi_sequence::event_at(u32 tick)
{
	for (auto it = m_list.begin(); it != m_list.end(); ++it)
	{
		if (it->tick() == tick)
			return *it;
		if (it->tick() > tick)
			return *m_list.emplace(it, tick);
	}
	m_list.emplace_back(tick);
	return m_list.back();
}


//-------------------------------------------------
//  parse - parse a MIDI sequence from a buffer
//-------------------------------------------------

bool midiin_device::midi_sequence::parse(u8 const *data, u32 length)
{
	// start with an empty list of events
	m_list.clear();

	// by default parse the whole data
	midi_parser buffer(data, length, 0);

	// catch errors to make parsing easier
	try
	{
		// if not a RIFF-encoed MIDI, just parse as-is
		if (buffer.dword_le() != fourcc_le("RIFF"))
			parse_midi_data(buffer.reset());
		else
		{
			// check the RIFF type and size
			u32 riffsize = buffer.dword_le();
			u32 rifftype = buffer.dword_le();
			if (rifftype != fourcc_le("RMID"))
				throw midi_parser::error("Input RIFF file is not of type RMID");

			// loop over RIFF chunks
			midi_parser riffdata = buffer.subset(riffsize - 4);
			while (!riffdata.eob())
			{
				u32 chunktype = riffdata.dword_le();
				u32 chunksize = riffdata.dword_le();
				midi_parser chunk = riffdata.subset(chunksize);
				if (chunktype == fourcc_le("data"))
				{
					parse_midi_data(chunk);
					break;
				}
			}
		}
		m_iterator = m_list.begin();
		return true;
	}
	catch (midi_parser::error &)
	{
		m_list.clear();
		m_iterator = m_list.begin();
		return false;
	}
}


//-------------------------------------------------
//  parse_midi_data - parse the core MIDI format
//  into tracks
//-------------------------------------------------

void midiin_device::midi_sequence::parse_midi_data(midi_parser &buffer)
{
	// scan for syntactic correctness, and to find global state
	u32 headertype = buffer.dword_le();
	if (headertype != fourcc_le("MThd"))
		throw midi_parser::error("Input file is not a MIDI file.");
	if (buffer.dword_be() != 0x00000006)
		throw midi_parser::error("Invalid MIDI file header.");

	// parse format info
	int format = buffer.word_be();
	if (format > 2)
		throw midi_parser::error("Invalid MIDI file header.");

	// parse track count
	u16 tracks = buffer.word_be();
	if (format == 0 && tracks != 1)
		throw midi_parser::error("MIDI format 0 expects exactly one track.");

	// parse time divisor
	u16 timediv = buffer.word_be();
	if ((timediv & 0x8000) != 0)
		throw midi_parser::error("SMPTE timecode time division not supported.");
	if (timediv == 0)
		throw midi_parser::error("Invalid time divisor of 0.");

	// iterate over tracks
	u32 curtick = 0;
	for (u16 index = 0; index < tracks; index++)
	{
		// verify header
		if (buffer.dword_le() != fourcc_le("MTrk"))
			throw midi_parser::error("Invalid MIDI track header.");
		u32 chunksize = buffer.dword_be();

		// parse the track data
		midi_parser trackdata = buffer.subset(chunksize);
		u32 numticks = parse_track_data(trackdata, curtick);
		if (format == 2)
			curtick += numticks;
	}

	// now go through the event list and compute times
	u32 lasttick = 0;
	attotime ticktime = attotime::from_usec(1000000) / timediv;
	attotime curtime;
	for (auto &event : m_list)
	{
		// update the elapsed time
		u32 curtick = event.tick();
		curtime += ticktime * (curtick - lasttick);
		lasttick = curtick;

		// determine if we have a new tempo here before replacing the time
		if (!event.time().is_zero())
			ticktime = event.time() / timediv;
		event.set_time(curtime);
	}
}


//-------------------------------------------------
//  parse_track_data - parse data from a track and
//  add it to the buffer
//-------------------------------------------------

u32 midiin_device::midi_sequence::parse_track_data(midi_parser &buffer, u32 start_tick)
{
	u32 curtick = start_tick;
	u8 last_type = 0;
	while (!buffer.eob())
	{
		// parse the time delta
		curtick += buffer.variable();
		midi_event &event = event_at(curtick);

		// handle running status
		u8 type = buffer.byte();
		if (BIT(type, 7) != 0)
			last_type = type;
		else
		{
			type = last_type;
			buffer.rewind(1);
		}

		// determine the event class
		uint8_t eclass = type >> 4;
		if (eclass != 15)
		{
			// simple events: all but program change and aftertouch have a second parameter
			// TODO: should we respect the channel for these? or maybe the drivers should
			// configure us with the number of channels they support?
			event.append(type & 0xf0);
			event.append(buffer.byte());
			if (eclass != 12 && eclass != 13)
				event.append(buffer.byte());
		}
		else if (type != 0xff)
		{
			// handle non-meta events
			midi_parser eventdata = buffer.subset(buffer.variable());
			while (!eventdata.eob())
				event.append(eventdata.byte());
		}
		else
		{
			// handle meta-events
			u8 type = buffer.byte();
			midi_parser eventdata = buffer.subset(buffer.variable());

			// end of data?
			if (type == 0x2f)
				break;

			// only care about tempo events; set the "time" to the new tick
			// value; we will sweep this later and compute actual times
			if (type == 0x51)
			{
				u32 usec_per_quarter = eventdata.triple_be();
				if (usec_per_quarter != 0)
					event.set_time(attotime::from_usec(usec_per_quarter));
			}
		}
	}
	return curtick;
}
