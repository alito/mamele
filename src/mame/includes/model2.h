// license:BSD-3-Clause
// copyright-holders:R. Belmont, Olivier Galibert, ElSemi, Angelo Salese
#ifndef MAME_INCLUDES_MODEL2_H
#define MAME_INCLUDES_MODEL2_H

#pragma once

#include "audio/dsbz80.h"
#include "audio/segam1audio.h"
#include "cpu/i960/i960.h"
#include "cpu/mb86233/mb86233.h"
#include "cpu/sharc/sharc.h"
#include "cpu/mb86235/mb86235.h"
#include "machine/315-5881_crypt.h"
#include "machine/315-5838_317-0229_comp.h"
#include "machine/bankdev.h"
#include "machine/eepromser.h"
#include "machine/gen_fifo.h"
#include "machine/i8251.h"
#include "machine/m2comm.h"
#include "machine/segabill.h"
#include "machine/timer.h"
#include "sound/scsp.h"
#include "video/segaic24.h"
#include "video/poly.h"
#include "emupal.h"
#include "screen.h"

class model2_renderer;
struct raster_state;
struct geo_state;
struct triangle;

class model2_state : public driver_device
{
public:
	model2_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_textureram0(*this, "textureram0"),
		m_textureram1(*this, "textureram1"),
		m_workram(*this, "workram"),
		m_bufferram(*this, "bufferram"),
		m_soundram(*this, "soundram"),
		m_maincpu(*this,"maincpu"),
		m_dsbz80(*this, DSBZ80_TAG),
		m_m1audio(*this, M1AUDIO_TAG),
		m_uart(*this, "uart"),
		m_m2comm(*this, "m2comm"),
		m_audiocpu(*this, "audiocpu"),
		m_copro_fifo_in(*this, "copro_fifo_in"),
		m_copro_fifo_out(*this, "copro_fifo_out"),
		m_drivecpu(*this, "drivecpu"),
		m_eeprom(*this, "eeprom"),
		m_tiles(*this, "tile"),
		m_screen(*this, "screen"),
		m_palette(*this, "palette"),
		m_scsp(*this, "scsp"),
		m_timers(*this, "timer%u", 0U),
		m_cryptdevice(*this, "315_5881"),
		m_0229crypt(*this, "317_0229"),
		m_copro_data(*this, "copro_data"),
		m_in0(*this, "IN0"),
		m_gears(*this, "GEARS"),
		m_lightgun_ports(*this, {"P1_Y", "P1_X", "P2_Y", "P2_X"})
	{ }

	/* Public for access by the rendering functions */
	required_shared_ptr<u32> m_textureram0;
	required_shared_ptr<u32> m_textureram1;
	std::unique_ptr<u16[]> m_palram;
	std::unique_ptr<u16[]> m_colorxlat;
	std::unique_ptr<u16[]> m_lumaram;
	u8 m_gamma_table[256];
	std::unique_ptr<model2_renderer> m_poly;

	/* Public for access by the ioports */
	DECLARE_CUSTOM_INPUT_MEMBER(daytona_gearbox_r);

	/* Public for access by MCFG */
	TIMER_DEVICE_CALLBACK_MEMBER(model2_interrupt);
	u16 crypt_read_callback(u32 addr);
	DECLARE_MACHINE_START(model2);


	/* Public for access by GAME() */
	void init_overrev();
	void init_pltkids();
	void init_rchase2();
	void init_manxttdx();
	void init_doa();
	void init_zerogun();
	void init_sgt24h();
	void init_srallyc();
	void init_powsledm();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_shared_ptr<u32> m_workram;
	required_shared_ptr<u32> m_bufferram;
	std::unique_ptr<u16[]> m_fbvramA;
	std::unique_ptr<u16[]> m_fbvramB;
	optional_shared_ptr<u16> m_soundram;

	required_device<i960_cpu_device> m_maincpu;
	optional_device<dsbz80_device> m_dsbz80;    // Z80-based MPEG Digital Sound Board
	optional_device<segam1audio_device> m_m1audio;  // Model 1 standard sound board
	required_device<i8251_device> m_uart;
	optional_device<m2comm_device> m_m2comm;        // Model 2 communication board
	optional_device<cpu_device> m_audiocpu;
	required_device<generic_fifo_u32_device> m_copro_fifo_in;
	required_device<generic_fifo_u32_device> m_copro_fifo_out;
	optional_device<cpu_device> m_drivecpu;
	optional_device<eeprom_serial_93cxx_device> m_eeprom;
	required_device<segas24_tile_device> m_tiles;
	required_device<screen_device> m_screen;
	required_device<palette_device> m_palette;
	optional_device<scsp_device> m_scsp;
	required_device_array<timer_device, 4> m_timers;
	optional_device<sega_315_5881_crypt_device> m_cryptdevice;
	optional_device<sega_315_5838_comp_device> m_0229crypt;
	optional_memory_region m_copro_data;

	required_ioport m_in0;
	optional_ioport m_gears;
	optional_ioport_array<4> m_lightgun_ports;

	u32 m_timervals[4];
	u32 m_timerorig[4];
	int m_timerrun[4];
	int m_ctrlmode;
	u16 m_cmd_data;
	u8 m_driveio_comm_data;
	int m_iop_write_num;
	u32 m_iop_data;
	int m_geo_iop_write_num;
	u32 m_geo_iop_data;

	u32 m_geo_read_start_address;
	u32 m_geo_write_start_address;
	std::unique_ptr<raster_state> m_raster;
	std::unique_ptr<geo_state> m_geo;
	bitmap_rgb32 m_sys24_bitmap;
//  u32 m_soundack;
	void model2_check_irq_state();
	void model2_check_irqack_state(u32 data);
	u8 m_gearsel;
	u8 m_lightgun_mux;

	// Coprocessor communications
	u32 copro_prg_r();
	u32 copro_ctl1_r();
	void copro_ctl1_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 copro_status_r();

	// Geometrizer communications
	void geo_ctl1_w(u32 data);
	u32 geo_prg_r(offs_t offset);
	void geo_prg_w(u32 data);
	u32 geo_r(offs_t offset);
	void geo_w(offs_t offset, u32 data);

	// Everything else
	u32 timers_r(offs_t offset);
	void timers_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u16 palette_r(offs_t offset);
	void palette_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u16 colorxlat_r(offs_t offset);
	void colorxlat_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void eeprom_w(u8 data);
	u8 in0_r();
	u32 fifo_control_2a_r();
	u32 videoctl_r();
	void videoctl_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u8 rchase2_drive_board_r();
	void rchase2_drive_board_w(u8 data);
	void drive_board_w(u8 data);
	u8 lightgun_data_r(offs_t offset);
	u8 lightgun_mux_r();
	void lightgun_mux_w(u8 data);
	u8 lightgun_offscreen_r(offs_t offset);
	u32 irq_request_r();
	void irq_ack_w(u32 data);
	u32 irq_enable_r();
	void irq_enable_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 model2_serial_r(offs_t offset, u32 mem_mask = ~0);
	void model2_serial_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	void horizontal_sync_w(u16 data);
	void vertical_sync_w(u16 data);
	u32 doa_prot_r(offs_t offset, u32 mem_mask = ~0);
	u32 doa_unk_r();
	void sega_0229_map(address_map &map);
	int m_prot_a;

	void raster_init(memory_region *texture_rom);
	void geo_init(memory_region *polygon_rom);
	u32 render_mode_r();
	void render_mode_w(u32 data);
	u16 lumaram_r(offs_t offset);
	void lumaram_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u16 fbvram_bankA_r(offs_t offset);
	void fbvram_bankA_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u16 fbvram_bankB_r(offs_t offset);
	void fbvram_bankB_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	void model2_3d_zclip_w(u32 data);
	void model2snd_ctrl(u16 data);
	u8 tgpid_r(offs_t offset);
	u32 polygon_count_r();

	u8 driveio_portg_r();
	u8 driveio_porth_r();
	void driveio_port_w(u8 data);
	void push_geo_data(u32 data);
	DECLARE_VIDEO_START(model2);
	void reset_model2_scsp();
	u32 screen_update_model2(screen_device &screen, bitmap_rgb32 &bitmap, const rectangle &cliprect);
//  DECLARE_WRITE_LINE_MEMBER(screen_vblank_model2);
//  DECLARE_WRITE_LINE_MEMBER(sound_ready_w);
	TIMER_DEVICE_CALLBACK_MEMBER(model2_timer_cb);
	void scsp_irq(offs_t offset, u8 data);

	void model2_3d_frame_start( void );
	void geo_parse( void );
	void model2_3d_frame_end( bitmap_rgb32 &bitmap, const rectangle &cliprect );
	void draw_framebuffer(bitmap_rgb32 &bitmap, const rectangle &cliprect );

	void model2_timers(machine_config &config);
	void model2_screen(machine_config &config);
	void model2_scsp(machine_config &config);

	void sj25_0207_01(machine_config &config);

	void drive_io_map(address_map &map);
	void drive_map(address_map &map);
	void geo_sharc_map(address_map &map);
	void model2_base_mem(address_map &map);
	void model2_5881_mem(address_map &map);
	void model2_0229_mem(address_map &map);
	void model2_snd(address_map &map);
	void scsp_map(address_map &map);

	void debug_init();
	void debug_commands( int ref, const std::vector<std::string> &params );
	void debug_geo_dasm_command(int ref, const std::vector<std::string> &params);
	void debug_tri_dump_command(int ref, const std::vector<std::string> &params);
	void debug_help_command(int ref, const std::vector<std::string> &params);

	virtual void video_start() override;

	u32 m_intreq;
	u32 m_intena;
	u32 m_coproctl;
	u32 m_coprocnt;

	virtual void copro_halt() = 0;
	virtual void copro_boot() = 0;

private:
	void tri_list_dump(FILE *dst);

	u32 m_geoctl;
	u32 m_geocnt;
	u32 m_videocontrol;

	bool m_render_unk;
	bool m_render_mode;
	bool m_render_test_mode;
	int16 m_crtc_xoffset, m_crtc_yoffset;

	u32 *geo_process_command( geo_state *geo, u32 opcode, u32 *input, bool *end_code );
	// geo commands
	u32 *geo_nop( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_object_data( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_direct_data( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_window_data( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_texture_data( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_polygon_data( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_texture_parameters( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_mode( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_zsort_mode( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_focal_distance( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_light_source( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_matrix_write( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_translate_write( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_data_mem_push( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_test( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_end( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_dummy( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_log_data( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_lod( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_code_upload( geo_state *geo, u32 opcode, u32 *input );
	u32 *geo_code_jump( geo_state *geo, u32 opcode, u32 *input );
	// geo code drawing paths
	void geo_parse_np_ns( geo_state *geo, u32 *input, u32 count );
	void geo_parse_np_s( geo_state *geo, u32 *input, u32 count );
	void geo_parse_nn_ns( geo_state *geo, u32 *input, u32 count );
	void geo_parse_nn_s( geo_state *geo, u32 *input, u32 count );

	// raster functions
	// main data input port
	void model2_3d_push( raster_state *raster, u32 input );
	// quad & triangle push paths
	void model2_3d_process_quad( raster_state *raster, u32 attr );
	void model2_3d_process_triangle( raster_state *raster, u32 attr );

	// inliners
	inline void model2_3d_project( triangle *tri );
	inline u16 float_to_zval( float floatval );
	inline bool check_culling( raster_state *raster, u32 attr, float min_z, float max_z );
};

/*****************************
 *
 * Model 2/2A TGP support
 *
 *****************************/

class model2_tgp_state : public model2_state
{
public:
	model2_tgp_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2_state(mconfig, type, tag),
		  m_copro_tgp(*this, "copro_tgp"),
		  m_copro_tgp_program(*this, "copro_tgp_program"),
		  m_copro_tgp_tables(*this, "copro_tgp_tables"),
		  m_copro_tgp_bank(*this, "copro_tgp_bank")
	{}

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_device<mb86234_device> m_copro_tgp;
	required_shared_ptr<u32> m_copro_tgp_program;
	required_region_ptr<u32> m_copro_tgp_tables;
	required_device<address_map_bank_device> m_copro_tgp_bank;

	u32 m_copro_tgp_bank_reg;
	u32 m_copro_sincos_base;
	u32 m_copro_inv_base;
	u32 m_copro_isqrt_base;
	u32 m_copro_atan_base[4];

	void copro_function_port_w(offs_t offset, u32 data);
	u32 copro_fifo_r();
	void copro_fifo_w(u32 data);
	void tex0_w(offs_t offset, u32 data);
	void tex1_w(offs_t offset, u32 data);

	void copro_tgp_bank_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 copro_tgp_memory_r(offs_t offset);
	void copro_tgp_memory_w(offs_t offset, u32 data, u32 mem_mask = ~0);

	void copro_sincos_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 copro_sincos_r(offs_t offset);
	void copro_inv_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 copro_inv_r(offs_t offset);
	void copro_isqrt_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 copro_isqrt_r(offs_t offset);
	void copro_atan_w(offs_t offset, u32 data, u32 mem_mask = ~0);
	u32 copro_atan_r();

	void model2_tgp_mem(address_map &map);

	void copro_tgp_prog_map(address_map &map);
	void copro_tgp_data_map(address_map &map);
	void copro_tgp_bank_map(address_map &map);
	void copro_tgp_io_map(address_map &map);
	void copro_tgp_rf_map(address_map &map);

	virtual void copro_halt() override;
	virtual void copro_boot() override;
};

/*****************************
 *
 * Model 2 support
 *
 *****************************/

class model2o_state : public model2_tgp_state
{
public:
	model2o_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2_tgp_state(mconfig, type, tag)
	{}

	void model2o(machine_config &config);
	void daytona(machine_config &config);
	void desert(machine_config &config);
	void vcop(machine_config &config);

protected:
	u32 fifo_control_2o_r();
	void daytona_output_w(u8 data);
	void desert_output_w(u8 data);
	void vcop_output_w(u8 data);

	void model2o_mem(address_map &map);
};

/*****************************
 *
 * Daytona To The Maxx
 *
 *****************************/

class model2o_maxx_state : public model2o_state
{
public:
	model2o_maxx_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2o_state(mconfig, type, tag)
	{}

	u32 maxx_r(offs_t offset, u32 mem_mask = ~0);
	void daytona_maxx(machine_config &config);
	void model2o_maxx_mem(address_map &map);

private:
	int m_maxxstate;
};

/*****************************
 *
 * Daytona GTX 2004 Edition
 *
 *****************************/

class model2o_gtx_state : public model2o_state
{
public:
	model2o_gtx_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2o_state(mconfig, type, tag)
	{}

	u8 gtx_r(offs_t offset);
	void daytona_gtx(machine_config &config);
	void model2o_gtx_mem(address_map &map);

private:
	int m_gtx_state;
};

/*****************************
 *
 * Model 2A
 *
 *****************************/

class model2a_state : public model2_tgp_state
{
public:
	model2a_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2_tgp_state(mconfig, type, tag),
		  m_billboard(*this, "billboard")
	{}

	void manxtt(machine_config &config);
	void manxttdx(machine_config &config);
	void model2a(machine_config &config);
	void model2a_0229(machine_config &config);
	void model2a_5881(machine_config &config);
	void srallyc(machine_config &config);
	void vcop2(machine_config &config);
	void skytargt(machine_config &config);
	void zeroguna(machine_config &config);

protected:
	virtual void machine_reset() override;

	void model2a_crx_mem(address_map &map);
	void model2a_5881_mem(address_map &map);
	void model2a_0229_mem(address_map &map);

private:
	required_device<sega_billboard_device> m_billboard;
};

/*****************************
 *
 * Model 2B
 *
 *****************************/

class model2b_state : public model2_state
{
public:
	model2b_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2_state(mconfig, type, tag),
		  m_copro_adsp(*this, "copro_adsp"),
		  m_billboard(*this, "billboard")
	{}

	void model2b(machine_config &config);
	void model2b_0229(machine_config &config);
	void model2b_5881(machine_config &config);
	void indy500(machine_config &config);
	void overrev2b(machine_config &config);
	void powsled(machine_config &config);
	void rchase2(machine_config &config);
	void gunblade(machine_config &config);
	void dynabb(machine_config &config);
	void zerogun(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_device<adsp21062_device> m_copro_adsp;

	void copro_function_port_w(offs_t offset, u32 data);
	u32 copro_fifo_r();
	void copro_fifo_w(u32 data);
	void copro_sharc_iop_w(offs_t offset, u32 data);
	u32 copro_sharc_buffer_r(offs_t offset);
	void copro_sharc_buffer_w(offs_t offset, u32 data);

	void model2b_crx_mem(address_map &map);
	void model2b_5881_mem(address_map &map);
	void model2b_0229_mem(address_map &map);
	// TODO: split into own class
	void rchase2_iocpu_map(address_map &map);
	void rchase2_ioport_map(address_map &map);

	void copro_sharc_map(address_map &map);

	virtual void copro_halt() override;
	virtual void copro_boot() override;

private:
	required_device<sega_billboard_device> m_billboard;
};

/*****************************
 *
 * Model 2C
 *
 *****************************/

class model2c_state : public model2_state
{
public:
	model2c_state(const machine_config &mconfig, device_type type, const char *tag)
		: model2_state(mconfig, type, tag),
		  m_copro_tgpx4(*this, "copro_tgpx4"),
		  m_copro_tgpx4_program(*this, "copro_tgpx4_program")
	{}

	void model2c(machine_config &config);
	void model2c_5881(machine_config &config);
	void skisuprg(machine_config &config);
	void stcc(machine_config &config);
	void waverunr(machine_config &config);
	void bel(machine_config &config);
	void hotd(machine_config &config);
	void overrev2c(machine_config &config);
	void segawski(machine_config &config);
	void topskatr(machine_config &config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	required_device<mb86235_device> m_copro_tgpx4;
	required_shared_ptr<u64> m_copro_tgpx4_program;

	void copro_function_port_w(offs_t offset, u32 data);
	u32 copro_fifo_r();
	void copro_fifo_w(u32 data);

	TIMER_DEVICE_CALLBACK_MEMBER(model2c_interrupt);

	void model2c_crx_mem(address_map &map);
	void model2c_5881_mem(address_map &map);
	void copro_tgpx4_map(address_map &map);
	void copro_tgpx4_data_map(address_map &map);

	virtual void copro_halt() override;
	virtual void copro_boot() override;
};

/*****************************
 *
 * Modern polygon renderer
 *
 *****************************/

struct m2_poly_extra_data
{
	model2_state *  state;
	u32      lumabase;
	u32      colorbase;
	u32 *    texsheet;
	u32      texwidth;
	u32      texheight;
	u32      texx, texy;
	u8       texmirrorx;
	u8       texmirrory;
};


static inline u16 get_texel( u32 base_x, u32 base_y, int x, int y, u32 *sheet )
{
	u32  baseoffs = ((base_y/2)*512)+(base_x/2);
	u32  texeloffs = ((y/2)*512)+(x/2);
	u32  offset = baseoffs + texeloffs;
	u32  texel = sheet[offset>>1];

	if ( offset & 1 )
		texel >>= 16;

	if ( (y & 1) == 0 )
		texel >>= 8;

	if ( (x & 1) == 0 )
		texel >>= 4;

	return (texel & 0x0f);
}

// 0x10000 = size of the tri_sorted_list array
class model2_renderer : public poly_manager<float, m2_poly_extra_data, 4, 0x10000>
{
public:
	typedef void (model2_renderer::*scanline_render_func)(int32_t scanline, const extent_t& extent, const m2_poly_extra_data& object, int threadid);

public:
	model2_renderer(model2_state& state)
		: poly_manager<float, m2_poly_extra_data, 4, 0x10000>(state.machine())
		, m_state(state)
		, m_destmap(512, 512)
	{
		m_renderfuncs[0] = &model2_renderer::model2_3d_render_0;
		m_renderfuncs[1] = &model2_renderer::model2_3d_render_1;
		m_renderfuncs[2] = &model2_renderer::model2_3d_render_2;
		m_renderfuncs[3] = &model2_renderer::model2_3d_render_3;
		m_renderfuncs[4] = &model2_renderer::model2_3d_render_4;
		m_renderfuncs[5] = &model2_renderer::model2_3d_render_5;
		m_renderfuncs[6] = &model2_renderer::model2_3d_render_6;
		m_renderfuncs[7] = &model2_renderer::model2_3d_render_7;
		m_xoffs = 90;
		m_yoffs = -8;
	}

	bitmap_rgb32& destmap() { return m_destmap; }

	void model2_3d_render(triangle *tri, const rectangle &cliprect);
	void set_xoffset(int16 xoffs) { m_xoffs = xoffs; }
	void set_yoffset(int16 yoffs) { m_yoffs = yoffs; }

	/* checker = 0, textured = 0, transparent = 0 */
	#define MODEL2_FUNC 0
	#define MODEL2_FUNC_NAME    model2_3d_render_0
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 0, textured = 0, translucent = 1 */
	#define MODEL2_FUNC 1
	#define MODEL2_FUNC_NAME    model2_3d_render_1
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 0, textured = 1, translucent = 0 */
	#define MODEL2_FUNC 2
	#define MODEL2_FUNC_NAME    model2_3d_render_2
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 0, textured = 1, translucent = 1 */
	#define MODEL2_FUNC 3
	#define MODEL2_FUNC_NAME    model2_3d_render_3
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 1, textured = 0, translucent = 0 */
	#define MODEL2_FUNC 4
	#define MODEL2_FUNC_NAME    model2_3d_render_4
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 1, textured = 0, translucent = 1 */
	#define MODEL2_FUNC 5
	#define MODEL2_FUNC_NAME    model2_3d_render_5
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 1, textured = 1, translucent = 0 */
	#define MODEL2_FUNC 6
	#define MODEL2_FUNC_NAME    model2_3d_render_6
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	/* checker = 1, textured = 1, translucent = 1 */
	#define MODEL2_FUNC 7
	#define MODEL2_FUNC_NAME    model2_3d_render_7
	#include "video/model2rd.hxx"
	#undef MODEL2_FUNC
	#undef MODEL2_FUNC_NAME

	scanline_render_func m_renderfuncs[8];

private:
	model2_state& m_state;
	bitmap_rgb32 m_destmap;
	int16_t m_xoffs,m_yoffs;
};

typedef model2_renderer::vertex_t poly_vertex;


/*******************************************
 *
 *  Basic Data Types
 *
 *******************************************/

struct plane
{
	poly_vertex normal;
	float       distance;
};

struct texture_parameter
{
	float   diffuse;
	float   ambient;
	u32  specular_control;
	float   specular_scale;
};

struct triangle
{
	void *              next;
	poly_vertex         v[3];
	u16              z;
	u16              texheader[4];
	u8               luma;
	int16_t               viewport[4];
	int16_t               center[2];
};

struct quad_m2
{
	poly_vertex         v[4];
	u16              z;
	u16              texheader[4];
	u8               luma;
};

/*******************************************
 *
 *  Hardware 3D Rasterizer Internal State
 *
 *******************************************/

#define MAX_TRIANGLES       32768

struct raster_state
{
//  u32 mode;                      /* bit 0 = Test Mode, bit 2 = Switch 60Hz(1)/30Hz(0) operation */
	u16 *texture_rom;              /* Texture ROM pointer */
	u32 texture_rom_mask;          /* Texture ROM mask */
	int16_t viewport[4];                /* View port (startx,starty,endx,endy) */
	int16_t center[4][2];               /* Centers (eye 0[x,y],1[x,y],2[x,y],3[x,y]) */
	u16 center_sel;                /* Selected center */
	u32 reverse;                   /* Left/Right Reverse */
	float z_adjust;                     /* ZSort Mode */
	float triangle_z;                   /* Current Triangle z value */
	u8 master_z_clip;              /* Master Z-Clip value */
	u32 cur_command;               /* Current command */
	u32 command_buffer[32];        /* Command buffer */
	u32 command_index;             /* Command buffer index */
	triangle tri_list[MAX_TRIANGLES];   /* Triangle list */
	u32 tri_list_index;            /* Triangle list index */
	triangle *tri_sorted_list[0x10000]; /* Sorted Triangle list */
	u16 min_z;                     /* Minimum sortable Z value */
	u16 max_z;                     /* Maximum sortable Z value */
	u16 texture_ram[0x10000];      /* Texture RAM pointer */
	u8 log_ram[0x40000];           /* Log RAM pointer */
};

/*******************************************
 *
 *  Geometry Engine Internal State
 *
 *******************************************/

struct geo_state
{
	raster_state *          raster;
	u32              mode;                   /* bit 0 = Enable Specular, bit 1 = Calculate Normals */
	u32 *          polygon_rom;            /* Polygon ROM pointer */
	u32            polygon_rom_mask;       /* Polygon ROM mask */
	float               matrix[12];             /* Current Transformation Matrix */
	poly_vertex         focus;                  /* Focus (x,y) */
	poly_vertex         light;                  /* Light Vector */
	float               lod;                    /* LOD */
	float               coef_table[32];         /* Distane Coefficient table */
	texture_parameter   texture_parameters[32]; /* Texture parameters */
	u32          polygon_ram0[0x8000];           /* Fast Polygon RAM pointer */
	u32          polygon_ram1[0x8000];           /* Slow Polygon RAM pointer */
	model2_state    *state;
};

#endif // MAME_INCLUDES_MODEL2_H
