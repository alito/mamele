// license:BSD-3-Clause
// copyright-holders:Aaron Giles
/*************************************************************************

    Atari System 2 hardware

*************************************************************************/

#include "cpu/m6502/m6502.h"
#include "cpu/t11/t11.h"
#include "machine/bankdev.h"
#include "machine/gen_latch.h"
#include "machine/timer.h"
#include "machine/watchdog.h"
#include "sound/ym2151.h"
#include "sound/pokey.h"
#include "sound/tms5220.h"
#include "video/atarimo.h"
#include "emupal.h"
#include "screen.h"
#include "slapstic.h"
#include "tilemap.h"

class atarisy2_state : public driver_device
{
public:
	atarisy2_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_audiocpu(*this, "audiocpu")
		, m_gfxdecode(*this, "gfxdecode")
		, m_screen(*this, "screen")
		, m_mob(*this, "mob")
		, m_slapstic_base(*this, "slapstic_base")
		, m_vrambank(*this, "vrambank")
		, m_playfield_tilemap(*this, "playfield")
		, m_alpha_tilemap(*this, "alpha")
		, m_xscroll(*this, "xscroll")
		, m_yscroll(*this, "yscroll")
		, m_soundlatch(*this, "soundlatch")
		, m_mainlatch(*this, "mainlatch")
		, m_ym2151(*this, "ymsnd")
		, m_pokey(*this, "pokey%u", 1U)
		, m_tms5220(*this, "tms")
		, m_rombank(*this, "rombank%u", 1U)
		, m_slapstic(*this, "slapstic")
		, m_leds(*this, "led%u", 0U)
	{ }

	void init_ssprint();
	void init_apb();
	void init_csprint();
	void init_paperboy();
	void init_720();

	void atarisy2(machine_config &config);
	void apb(machine_config &config);
	void paperboy(machine_config &config);
	void ssprint(machine_config &config);
	void _720(machine_config &config);
	void csprint(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void device_post_load() override;
	virtual void video_start() override;

private:
	void update_interrupts();

	required_device<t11_device> m_maincpu;
	required_device<m6502_device> m_audiocpu;
	required_device<gfxdecode_device> m_gfxdecode;
	required_device<screen_device> m_screen;
	required_device<atari_motion_objects_device> m_mob;
	required_shared_ptr<uint16_t> m_slapstic_base;
	required_device<address_map_bank_device> m_vrambank;

	uint8_t           m_interrupt_enable;

	required_device<tilemap_device> m_playfield_tilemap;
	required_device<tilemap_device> m_alpha_tilemap;
	required_shared_ptr<uint16_t> m_xscroll;
	required_shared_ptr<uint16_t> m_yscroll;

	int8_t            m_pedal_count;

	required_device<generic_latch_8_device> m_soundlatch;
	required_device<generic_latch_8_device> m_mainlatch;
	required_device<ym2151_device> m_ym2151;
	required_device_array<pokey_device, 2> m_pokey;
	optional_device<tms5220_device> m_tms5220;

	bool            m_scanline_int_state;
	bool            m_video_int_state;
	bool            m_p2portwr_state;
	bool            m_p2portrd_state;

	required_memory_bank_array<2> m_rombank;
	required_device<atari_slapstic_device> m_slapstic;

	uint8_t           m_sound_reset_state;

	emu_timer *     m_yscroll_reset_timer;
	uint32_t          m_playfield_tile_bank[2];

	// 720 fake joystick
	double          m_joy_last_angle;
	int             m_joy_rotations;

	// 720 fake spinner
	uint32_t          m_spin_last_rotate_count;
	int32_t           m_spin_pos;                 /* track fake position of spinner */
	uint32_t          m_spin_center_count;

	output_finder<2> m_leds;

	void scanline_int_ack_w(uint8_t data);
	void video_int_ack_w(uint8_t data);
	void int0_ack_w(uint8_t data);
	void sound_reset_w(uint8_t data);
	void int_enable_w(uint8_t data);
	INTERRUPT_GEN_MEMBER(sound_irq_gen);
	void sound_irq_ack_w(uint8_t data);
	DECLARE_WRITE_LINE_MEMBER(boost_interleave_hack);
	void bankselect_w(offs_t offset, uint16_t data);
	uint16_t switch_r();
	uint8_t switch_6502_r();
	void switch_6502_w(uint8_t data);
	uint8_t leta_r(offs_t offset);
	void mixer_w(uint8_t data);
	void sndrst_6502_w(uint8_t data);
	uint16_t sound_r();
	void sound_6502_w(uint8_t data);
	uint8_t sound_6502_r();
	void tms5220_w(uint8_t data);
	void tms5220_strobe_w(offs_t offset, uint8_t data);
	void coincount_w(uint8_t data);

	TIMER_DEVICE_CALLBACK_MEMBER(scanline_update);

	TILE_GET_INFO_MEMBER(get_alpha_tile_info);
	TILE_GET_INFO_MEMBER(get_playfield_tile_info);
	uint32_t screen_update_atarisy2(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	DECLARE_WRITE_LINE_MEMBER(vblank_int);
	TIMER_CALLBACK_MEMBER(delayed_int_enable_w);
	TIMER_CALLBACK_MEMBER(reset_yscroll_callback);
	DECLARE_READ16_MEMBER(slapstic_r);
	DECLARE_WRITE16_MEMBER(slapstic_w);
	DECLARE_WRITE16_MEMBER(yscroll_w);
	DECLARE_WRITE16_MEMBER(xscroll_w);
	DECLARE_WRITE16_MEMBER(spriteram_w);
	static rgb_t RRRRGGGGBBBBIIII(uint32_t raw);

	static const atari_motion_objects_config s_mob_config;
	void main_map(address_map &map);
	void sound_map(address_map &map);
	void vrambank_map(address_map &map);
};
