// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/***************************************************************************

    speaker.h

    Speaker output sound device.

    Speakers have (x, y, z) coordinates in 3D space:
    * Observer is at position (0, 0, 0)
    * Positive x is to the right of the observer
    * Negative x is to the left of the observer
    * Positive y is above the observer
    * Negative y is below the observer
    * Positive z is in front of the observer
    * Negative z is behind the observer

    Currently, MAME only considers the sign of the x coordinate (not its
    magnitude), and completely ignores the y and z coordinates.

***************************************************************************/

#ifndef MAME_EMU_SPEAKER_H
#define MAME_EMU_SPEAKER_H

#pragma once


//**************************************************************************
//  GLOBAL VARIABLES
//**************************************************************************

// device type definition
DECLARE_DEVICE_TYPE(SPEAKER, speaker_device)



//**************************************************************************
//  TYPE DEFINITIONS
//**************************************************************************

// ======================> speaker_device

class speaker_device : public device_t, public device_mixer_interface
{
public:
	// construction/destruction
	speaker_device(const machine_config &mconfig, const char *tag, device_t *owner, double x, double y, double z)
		: speaker_device(mconfig, tag, owner, 0)
	{
		set_position(x, y, z);
	}
	speaker_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock = 0);
	virtual ~speaker_device();

	// inline configuration helpers
	speaker_device &set_position(double x, double y, double z) { m_x = x; m_y = y; m_z = z; return *this; }
	speaker_device &front_center()      { set_position( 0.0,  0.0,  1.0); return *this; }
	speaker_device &front_left()        { set_position(-0.2,  0.0,  1.0); return *this; }
	speaker_device &front_floor()       { set_position( 0.0, -0.5,  1.0); return *this; }
	speaker_device &front_right()       { set_position( 0.2,  0.0,  1.0); return *this; }
	speaker_device &rear_center()       { set_position( 0.0,  0.0, -0.5); return *this; }
	speaker_device &rear_left()         { set_position(-0.2,  0.0, -0.5); return *this; }
	speaker_device &rear_right()        { set_position( 0.2,  0.0, -0.5); return *this; }
	speaker_device &headrest_center()   { set_position( 0.0,  0.0, -0.1); return *this; }
	speaker_device &headrest_left()     { set_position(-0.1,  0.0, -0.1); return *this; }
	speaker_device &headrest_right()    { set_position( 0.1,  0.0, -0.1); return *this; }
	speaker_device &seat()              { set_position( 0.0, -0.5,  0.0); return *this; }
	speaker_device &backrest()          { set_position( 0.0, -0.2,  0.1); return *this; }

	// internally for use by the sound system
	void mix(s32 *leftmix, s32 *rightmix, int &samples_this_update, bool suppress);

protected:
	// device-level overrides
	virtual void device_start() override ATTR_COLD;
	virtual void device_stop() override ATTR_COLD;

	// inline configuration state
	double              m_x;
	double              m_y;
	double              m_z;

	// internal state
	static constexpr int BUCKETS_PER_SECOND = 10;
	std::vector<s32>    m_max_sample;
	s32                 m_current_max;
	u32                 m_samples_this_bucket;
};


// speaker device iterator
typedef device_type_iterator<speaker_device> speaker_device_iterator;


#endif // MAME_EMU_SPEAKER_H
