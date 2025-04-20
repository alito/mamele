// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    osdepend.h

    OS-dependent code interface.

*******************************************************************c********/
#ifndef MAME_OSD_OSDEPEND_H
#define MAME_OSD_OSDEPEND_H

#pragma once


#include "emufwd.h"

#include "bitmap.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>


// forward references
class input_type_entry;
class osd_midi_device;
namespace ui { class menu_item; }


//============================================================
//  TYPE DEFINITIONS
//============================================================

// ======================> osd_font interface

class osd_font
{
public:
	typedef std::unique_ptr<osd_font> ptr;

	virtual ~osd_font() { }

	/** attempt to "open" a handle to the font with the given name */
	virtual bool open(std::string const &font_path, std::string const &name, int &height) = 0;

	/** release resources associated with a given OSD font */
	virtual void close() = 0;

	/*!
	 * allocate and populate a BITMAP_FORMAT_ARGB32 bitmap containing
	 * the pixel values rgb_t(0xff,0xff,0xff,0xff) or
	 * rgb_t(0x00,0xff,0xff,0xff) for each pixel of a black & white font
	 */
	virtual bool get_bitmap(char32_t chnum, bitmap_argb32 &bitmap, std::int32_t &width, std::int32_t &xoffs, std::int32_t &yoffs) = 0;
};

// ======================> osd_interface

// description of the currently-running machine
class osd_interface
{
public:

	// general overridables
	virtual void init(running_machine &machine) = 0;
	virtual void update(bool skip_redraw) = 0;
	virtual void input_update() = 0;
	virtual void set_verbose(bool print_verbose) = 0;

	// debugger overridables
	virtual void init_debugger() = 0;
	virtual void wait_for_debugger(device_t &device, bool firststop) = 0;

	// audio overridables
	virtual void update_audio_stream(const int16_t *buffer, int samples_this_frame) = 0;
	virtual void set_mastervolume(int attenuation) = 0;
	virtual bool no_sound() = 0;

	// input overridables
	virtual void customize_input_type_list(std::vector<input_type_entry> &typelist) = 0;

	// video overridables
	virtual void add_audio_to_recording(const int16_t *buffer, int samples_this_frame) = 0;
	virtual std::vector<ui::menu_item> get_slider_list() = 0;

	// font interface
	virtual osd_font::ptr font_alloc() = 0;
	virtual bool get_font_families(std::string const &font_path, std::vector<std::pair<std::string, std::string> > &result) = 0;

	// command option overrides
	virtual bool execute_command(const char *command) = 0;

	// midi interface
	virtual std::unique_ptr<osd_midi_device> create_midi_device() = 0;

protected:
	virtual ~osd_interface() { }
};


/***************************************************************************
    MIDI I/O INTERFACES
***************************************************************************/

class osd_midi_device
{
public:
	virtual ~osd_midi_device() { }
	virtual bool open_input(const char *devname) = 0;
	virtual bool open_output(const char *devname) = 0;
	virtual void close() = 0;
	virtual bool poll() = 0;
	virtual int read(uint8_t *pOut) = 0;
	virtual void write(uint8_t data) = 0;
};

#endif  // MAME_OSD_OSDEPEND_H
