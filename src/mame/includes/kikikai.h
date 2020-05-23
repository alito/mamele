// license:BSD-3-Clause
// copyright-holders:Ernesto Corvi

#include "cpu/m6805/m68705.h"
#include "cpu/m6800/m6801.h"

#include "sound/2203intf.h"
#include "emupal.h"
#include "screen.h"

class kikikai_state : public driver_device
{
public:
	kikikai_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag),
		m_mcu_sharedram(*this, "mcu_sharedram"),
		m_mainram(*this, "mainram"),
		m_maincpu(*this, "maincpu"),
		m_audiocpu(*this, "audiocpu"),
		m_subcpu(*this, "sub"),
		m_mcu(*this, "mcu"),
		m_68705mcu(*this, "68705mcu"),
		m_ymsnd(*this, "ymsnd"),
		m_gfxdecode(*this, "gfxdecode"),
		m_palette(*this, "palette"),
		m_screen(*this, "screen")
	{
	}

	void knightb(machine_config &config);
	void mexico86(machine_config &config);
	void mexico86_68705(machine_config &config);
	void kikikai(machine_config &config);
	void kicknrun(machine_config &config);

private:
	/* memory pointers */
	required_shared_ptr<u8> m_mcu_sharedram;
	required_shared_ptr<u8> m_mainram;

	/* video-related */
	int      m_charbank;

	/* mcu */
	/* mexico86 68705 protection */
	u8       m_port_a_out;
	u8       m_port_b_out;
	int      m_address;
	u8       m_latch;
	/* kikikai mcu simulation */
	int      m_kikikai_simulated_mcu_running;
	int      m_kikikai_simulated_mcu_initialised;
	bool     m_coin_last[2];
	u8       m_coin_fract;

	/* devices */
	required_device<cpu_device>         m_maincpu;
	required_device<cpu_device>         m_audiocpu;
	optional_device<cpu_device>         m_subcpu;
	optional_device<cpu_device>         m_mcu;
	optional_device<m68705p_device>     m_68705mcu;
	required_device<ym2203_device>      m_ymsnd;
	required_device<gfxdecode_device>   m_gfxdecode;
	required_device<palette_device>     m_palette;
	required_device<screen_device>      m_screen;

	/* queue */
	u8 m_queue[64];
	int m_qfront;
	int m_qstate;
	DECLARE_WRITE8_MEMBER(mexico86_sub_output_w);
	DECLARE_WRITE8_MEMBER(mexico86_f008_w);
	DECLARE_WRITE8_MEMBER(mexico86_68705_port_a_w);
	DECLARE_WRITE8_MEMBER(mexico86_68705_port_b_w);
	DECLARE_WRITE8_MEMBER(mexico86_bankswitch_w);
	DECLARE_READ8_MEMBER(kiki_ym2203_r);
	virtual void machine_start() override;
	virtual void machine_reset() override;
	u32 screen_update_mexico86(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	u32 screen_update_kikikai(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	INTERRUPT_GEN_MEMBER(kikikai_interrupt);
	INTERRUPT_GEN_MEMBER(mexico86_m68705_interrupt);
	void mcu_simulate(  );
	bool mcu_coin_counter_w(bool condition);
	void mexico86_map(address_map &map);
	void mexico86_sound_map(address_map &map);
	void mexico86_sub_cpu_map(address_map &map);
	void mcu_map(address_map& map);

	/* Bubble Bobble MCU */
	uint8_t    m_ddr1;
	uint8_t    m_ddr2;
	uint8_t    m_ddr3;
	uint8_t    m_ddr4;
	uint8_t    m_port1_in;
	uint8_t    m_port2_in;
	uint8_t    m_port3_in;
	uint8_t    m_port4_in;
	uint8_t    m_port1_out;
	uint8_t    m_port2_out;
	uint8_t    m_port3_out;
	uint8_t    m_port4_out;

	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr1_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr1_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr2_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr2_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr3_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr3_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_ddr4_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_ddr4_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port1_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port1_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port2_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port2_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port3_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port3_w);
	DECLARE_READ8_MEMBER(bublbobl_mcu_port4_r);
	DECLARE_WRITE8_MEMBER(bublbobl_mcu_port4_w);
};
