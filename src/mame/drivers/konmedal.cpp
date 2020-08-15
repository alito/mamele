// license:BSD-3-Clause
// copyright-holders:R. Belmont
/***************************************************************************

 konmedal.cpp: Konami Z80 based medal games
 Driver by R. Belmont, MetalliC

Konami PWB 452093A boards

 Mario Roulette
 Shuriken Boy (しゅりけんボーイ)
 Fuusen Pentai (ふうせんペン太)

 Main CPU:  Z80
 Sound: uPD7759C

 Konami Custom chips:
 K052109 (tilemaps)
 K051962 (tilemaps)
 K051649 (sound)

Konami PWB 452574A boards

 Dam Dam Boy (ダムダムボーイ)
 Buttobi Striker (ぶっとびストライカー)

 Main CPU:  Z80
 Sound: OKIM6295

 Konami Custom chips:
 K053252 (timing/interrupt controller)
 K054156 (tilemaps)
 K054157 (tilemaps)
 K051649 (sound)

Konami PWB 402218 boards

 Tsukande Toru Chicchi (つかんでとるちっち)
 Dam Dam Boy (ダムダムボーイ)

 Mostly same as 452574A but sound chips replaced with YMZ280B

 Notes and TODOs:
 - Priorities not understood and wrong in places of GX-based games, apparently controlled by PROM
 - X/Y scroll effects not 100% handled by current K052109(TMNT tilemaps) emulation.
   Mario Roulette issues currently "resolved" using hack.

***************************************************************************/

#include "emu.h"
#include "cpu/z80/z80.h"
#include "machine/nvram.h"
#include "machine/ticket.h"
#include "machine/timer.h"
#include "machine/k053252.h"
#include "sound/ymz280b.h"
#include "sound/okim6295.h"
#include "sound/k051649.h"
#include "sound/upd7759.h"
#include "video/k054156_k054157_k056832.h"
#include "video/k052109.h"
#include "video/konami_helper.h"
#include "emupal.h"
#include "screen.h"
#include "speaker.h"

class konmedal_state : public driver_device
{
public:
	konmedal_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_k056832(*this, "k056832"),
		m_k053252(*this, "k053252"),
		m_k052109(*this, "k052109"),
		m_palette(*this, "palette"),
		m_ymz(*this, "ymz"),
		m_oki(*this, "oki"),
		m_upd7759(*this, "upd"),
		m_nvram(*this, "nvram"),
		m_outport(*this, "OUT"),
		m_lamps(*this, "lamp%u", 0U)
	{ }

	void shuriboy(machine_config &config);
	void ddboy(machine_config &config);
	void tsukande(machine_config &config);
	void fuusenpn(machine_config &config);
	void mariorou(machine_config &config);

	void ddboy_init();
	void tsuka_init();
	void buttobi_init();
	void shuri_init();
	void mario_init();
	void fuusen_init();
private:
	void konmedal_palette(palette_device &palette) const;
	void medal_nvram_init(nvram_device &nvram, void *base, size_t size);
	void shuriboy_nvram_init(nvram_device &nvram, void *base, size_t size);
	void fuusenpn_nvram_init(nvram_device &nvram, void *base, size_t size);
	void mario_nvram_init(nvram_device &nvram, void *base, size_t size);
	DECLARE_MACHINE_START(shuriboy);

	uint8_t vram_r(offs_t offset);
	void vram_w(offs_t offset, uint8_t data);
	void bankswitch_w(uint8_t data);
	void control2_w(uint8_t data);
	void medalcnt_w(uint8_t data);
	void lamps_w(uint8_t data);

	uint32_t screen_update_konmedal(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	uint32_t screen_update_shuriboy(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	K056832_CB_MEMBER(tile_callback);
	TIMER_DEVICE_CALLBACK_MEMBER(konmedal_scanline);
	WRITE_LINE_MEMBER(vbl_ack_w) { m_maincpu->set_input_line(0, CLEAR_LINE); }
	WRITE_LINE_MEMBER(nmi_ack_w) { m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE); }
	void ccu_int_time_w(uint8_t data) { m_ccu_int_time = data; }
	void k056832_w(offs_t offset, uint8_t data) { m_k056832->write(offset ^ 1, data); }
	void k056832_b_w(offs_t offset, uint8_t data) { m_k056832->b_w(offset ^ 1, data); }

	K052109_CB_MEMBER(shuriboy_tile_callback);
	TIMER_DEVICE_CALLBACK_MEMBER(shuri_scanline);
	void shuri_bank_w(uint8_t data);
	uint8_t shuri_irq_r();
	void shuri_irq_w(uint8_t data);
	void mario_scrollhack_w(uint8_t data);

	void ddboy_main(address_map &map);
	void medal_main(address_map &map);
	void shuriboy_main(address_map &map);

	virtual void machine_start() override;
	virtual void machine_reset() override;
	virtual void video_start() override;
	void machine_start_common();

	required_device<cpu_device> m_maincpu;
	optional_device<k056832_device> m_k056832;
	optional_device<k053252_device> m_k053252;
	optional_device<k052109_device> m_k052109;
	required_device<palette_device> m_palette;
	optional_device<ymz280b_device> m_ymz;
	optional_device<okim6295_device> m_oki;
	optional_device<upd7759_device> m_upd7759;
	required_device<nvram_device> m_nvram;
	required_ioport m_outport;
	output_finder<8> m_lamps;

	u8 m_control, m_control2, m_shuri_irq;
	int m_ccu_int_time, m_ccu_int_time_count;
	int m_avac;
	int m_layer_colorbase[4];
	int m_layer_order[4];
};

void konmedal_state::control2_w(uint8_t data)
{
/*  CN3
    ---- ---x uPD7759 /ST (TMNT-based boards)
    ---- --x- uPD7759 /RESET (TMNT-based boards)
    ---- -x-- 10Y Counter
    ---- x--- 100Y Counter
    ---x ---- 10Y Lock
    --x- ---- 100Y Lock
    -x-- ---- Hopper On
    x--- ---- K056832 RAM/ROM access switch (GX-based boards)
*/
	m_control2 = data;
	if (m_upd7759)  // note: this is needed because games clear reset line and assert start line at the same time, but MAME's outport can't handle right order
		m_upd7759->reset_w((data & 2) ? 1 : 0);
	m_outport->write(data);
	machine().bookkeeping().coin_counter_w(0, data & 4);
	machine().bookkeeping().coin_counter_w(1, data & 8);
	machine().bookkeeping().coin_lockout_w(0, (data & 0x10) ? 0 : 1);
	machine().bookkeeping().coin_lockout_w(1, (data & 0x20) ? 0 : 1);
}

void konmedal_state::medalcnt_w(uint8_t data)
{
/*  CN5
    ---- ---x Medal counter +1 (medal in)
    ---- --x- Medal counter -1 (hopper out)
    ---- -x-- Medal Lock
*/
	machine().bookkeeping().coin_counter_w(2, data & 2);
	machine().bookkeeping().coin_lockout_w(2, (data & 4) ? 0 : 1);
}

void konmedal_state::lamps_w(uint8_t data)
{
//  CN6
	for (int i = 0; i < 8; i++)
		m_lamps[i] = BIT(data, i);
}

uint8_t konmedal_state::vram_r(offs_t offset)
{
	if (!(m_control2 & 0x80))
	{
		if (offset & 1)
		{
			return m_k056832->ram_code_hi_r(offset>>1);
		}
		else
		{
			return m_k056832->ram_code_lo_r(offset>>1);
		}
	}
	else if (m_control == 0)    // ROM readback
	{
		return m_k056832->konmedal_rom_r(offset);
	}

	return 0;
}

void konmedal_state::vram_w(offs_t offset, uint8_t data)
{
	// there are (very few) writes above F000 in some screens.
	// bug?  debug?  this?  who knows.

	if (offset & 1)
	{
		m_k056832->ram_code_hi_w(offset>>1, data);
		return;
	}

	m_k056832->ram_code_lo_w(offset>>1, data);
}

void konmedal_state::ddboy_init()
{
	m_layer_colorbase[0] = 0;
	m_layer_colorbase[1] = 8;
	m_layer_colorbase[2] = 0;
	m_layer_colorbase[3] = 8;

	// not 100% good, during gameplay should be 2 1 0 3, but this breaks title screen
	m_layer_order[0] = 3;
	m_layer_order[1] = 2;
	m_layer_order[2] = 1;
	m_layer_order[3] = 0;
}
void konmedal_state::buttobi_init()
{
	m_layer_colorbase[0] = 0;
	m_layer_colorbase[1] = 8;
	m_layer_colorbase[2] = 0;
	m_layer_colorbase[3] = 8;

	// not 100% good, in case of loss goalkeeper gfx sometimes became on top of player
	m_layer_order[0] = 1;
	m_layer_order[1] = 0;
	m_layer_order[2] = 3;
	m_layer_order[3] = 2;
}
void konmedal_state::tsuka_init()
{
	m_layer_colorbase[0] = 0;
	m_layer_colorbase[1] = 8;
	m_layer_colorbase[2] = 0;
	m_layer_colorbase[3] = 0;

	m_layer_order[0] = 3;
	m_layer_order[1] = 1;
	m_layer_order[2] = 0;
	m_layer_order[3] = 2;
}

K056832_CB_MEMBER(konmedal_state::tile_callback)
{
	u32 codebits = *code;

	int mode, avac;
	m_k056832->read_avac(&mode, &avac);
	if (mode)
		*code = (((avac >> ((codebits >> 8) & 0xc)) & 0xf) << 10) | (codebits & 0x3ff);
	else
		*code = codebits & 0xfff;

	*code = bitswap<14>(*code, 8, 9, 13, 12, 11, 10, 7, 6, 5, 4, 3, 2, 1, 0);
	*color = m_layer_colorbase[layer] + ((codebits >> 13) & 7);
	*priority = BIT(codebits, 12);
}

void konmedal_state::video_start()
{
}

uint32_t konmedal_state::screen_update_konmedal(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	screen.priority().fill(0, cliprect);

	int mode, data;
	m_k056832->read_avac(&mode, &data);
	data |= mode << 3;
	if (m_avac != data)
	{
		m_avac = data;
		m_k056832->mark_all_tilemaps_dirty();
	}

	for (int p = 0; p < 2; p++)
		for (int l = 0; l < 4; l++)
			m_k056832->tilemap_draw(screen, bitmap, cliprect, m_layer_order[l], TILEMAP_DRAW_CATEGORY(p), 0);
	return 0;
}

uint32_t konmedal_state::screen_update_shuriboy(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect)
{
	bitmap.fill(0, cliprect);
	screen.priority().fill(0, cliprect);
	m_k052109->tilemap_update();
	for (int p = 0; p < 2; p++)
		for (int l = 0; l < 3; l++)
			m_k052109->tilemap_draw(screen, bitmap, cliprect, m_layer_order[l], TILEMAP_DRAW_CATEGORY(p), 0);
	return 0;
}

void konmedal_state::konmedal_palette(palette_device &palette) const
{
	uint8_t const *const PROM = memregion("proms")->base();

	for (int i = 0; i < 256; i++)
	{
		palette.set_pen_color(i,
				(PROM[i]) << 4,
				(PROM[0x100 + i]) << 4,
				(PROM[0x200 + i]) << 4);
	}
}

TIMER_DEVICE_CALLBACK_MEMBER(konmedal_state::konmedal_scanline)
{
	int scanline = param;

	// z80 /IRQ is connected to the IRQ1(vblank) pin of k053252 CCU
	if (scanline == 255)
	{
		m_maincpu->set_input_line(0, ASSERT_LINE);
	}

	// z80 /NMI is connected to the IRQ2 pin of k053252 CCU
	// the following code is emulating INT_TIME of the k053252, this code will go away
	// when the new konami branch is merged.
	m_ccu_int_time_count--;
	if (m_ccu_int_time_count <= 0)
	{
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
		m_ccu_int_time_count = m_ccu_int_time;
	}
}

void konmedal_state::bankswitch_w(uint8_t data)
{
	//printf("ROM bank %x (full %02x)\n", data>>4, data);
	membank("bank1")->set_entry(data>>4);
	m_control = data & 0xf;
}

void konmedal_state::medal_main(address_map &map)
{
	map(0x0000, 0x7fff).rom().region("maincpu", 0);
	map(0x8000, 0x9fff).bankr("bank1");
	map(0xa000, 0xbfff).ram().share("nvram"); // work RAM
	map(0xc000, 0xc03f).w(FUNC(konmedal_state::k056832_w));
	map(0xc100, 0xc100).w(FUNC(konmedal_state::control2_w));
	map(0xc200, 0xc200).w(FUNC(konmedal_state::medalcnt_w));
	map(0xc300, 0xc300).w(FUNC(konmedal_state::lamps_w));
	map(0xc400, 0xc400).w(FUNC(konmedal_state::bankswitch_w));
	map(0xc500, 0xc500).nopr(); // read to reset watchdog
	map(0xc600, 0xc60f).w(FUNC(konmedal_state::k056832_b_w));
	map(0xc700, 0xc700).portr("DSW2");
	map(0xc701, 0xc701).portr("DSW1");
	map(0xc702, 0xc702).portr("IN1");
	map(0xc703, 0xc703).portr("IN2");
	map(0xc800, 0xc80f).rw(m_k053252, FUNC(k053252_device::read), FUNC(k053252_device::write));
	map(0xd000, 0xd001).rw(m_ymz, FUNC(ymz280b_device::read), FUNC(ymz280b_device::write));
	map(0xe000, 0xffff).rw(FUNC(konmedal_state::vram_r), FUNC(konmedal_state::vram_w));
}

void konmedal_state::ddboy_main(address_map &map)
{
	map(0x0000, 0x7fff).rom().region("maincpu", 0);
	map(0x8000, 0x9fff).bankr("bank1");
	map(0xa000, 0xbfff).ram().share("nvram"); // work RAM
	map(0xc000, 0xc03f).w(FUNC(konmedal_state::k056832_w));
	map(0xc100, 0xc100).w(FUNC(konmedal_state::control2_w));
	map(0xc200, 0xc200).w(FUNC(konmedal_state::medalcnt_w));
	map(0xc300, 0xc300).w(FUNC(konmedal_state::lamps_w));
	map(0xc400, 0xc400).w(FUNC(konmedal_state::bankswitch_w));
	map(0xc500, 0xc500).nopr(); // read to reset watchdog
	map(0xc600, 0xc60f).w(FUNC(konmedal_state::k056832_b_w));
	map(0xc700, 0xc700).portr("DSW1");
	map(0xc701, 0xc701).portr("DSW2");
	map(0xc702, 0xc702).portr("IN1");
	map(0xc703, 0xc703).portr("IN2");
	map(0xc800, 0xc80f).rw(m_k053252, FUNC(k053252_device::read), FUNC(k053252_device::write));
	map(0xcc00, 0xcc00).rw(m_oki, FUNC(okim6295_device::read), FUNC(okim6295_device::write));
	map(0xd000, 0xd000).nopw();    // ???  writes 00 and 3f every frame
	map(0xd800, 0xd8ff).m("k051649", FUNC(k051649_device::scc_map));
	map(0xe000, 0xffff).rw(FUNC(konmedal_state::vram_r), FUNC(konmedal_state::vram_w));
}

void konmedal_state::shuriboy_main(address_map &map)
{
	map(0x0000, 0x7fff).rom().region("maincpu", 0);
	map(0x8000, 0x87ff).ram().share("nvram"); // actual RAM chip is 8Kbyte, might be banked somewhow
	map(0x8800, 0x8800).portr("IN2");
	map(0x8801, 0x8801).portr("IN1");
	map(0x8802, 0x8802).portr("DSW1");
	map(0x8803, 0x8803).portr("DSW2");
	map(0x8900, 0x8900).w(FUNC(konmedal_state::control2_w));
	map(0x8a00, 0x8a00).w(FUNC(konmedal_state::medalcnt_w));
	map(0x8b00, 0x8b00).nopw();    // watchdog?
	map(0x8c00, 0x8c00).w(FUNC(konmedal_state::shuri_bank_w));
	map(0x8d00, 0x8d00).w(m_upd7759, FUNC(upd7759_device::port_w));
	map(0x8e00, 0x8e00).w(FUNC(konmedal_state::lamps_w));
	map(0x9000, 0x9000).nopw();     // writes alternating 00 and 3F
	map(0x9800, 0x98ff).m("k051649", FUNC(k051649_device::scc_map));
	map(0xa000, 0xbfff).bankr("bank1");
	map(0xc000, 0xffff).rw(m_k052109, FUNC(k052109_device::read), FUNC(k052109_device::write));
	map(0xdd00, 0xdd00).rw(FUNC(konmedal_state::shuri_irq_r), FUNC(konmedal_state::shuri_irq_w));
	map(0xdc80, 0xdc80).w(FUNC(konmedal_state::mario_scrollhack_w));
}

static INPUT_PORTS_START( konmedal )
	PORT_START("DSW1")
	PORT_DIPNAME( 0x07, 0x07, "Coin Slot 1" )   PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, "5 Coins/2 Credits" )
	PORT_DIPSETTING(    0x01, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 2C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x04, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x78, 0x00, "Coin Slot 2" )   PORT_DIPLOCATION("SW1:4,5,6,7")
	PORT_DIPSETTING(    0x00, "16 Medals" )
	PORT_DIPSETTING(    0x08, "15 Medals" )
	PORT_DIPSETTING(    0x10, "14 Medals" )
	PORT_DIPSETTING(    0x18, "13 Medals" )
	PORT_DIPSETTING(    0x20, "12 Medals" )
	PORT_DIPSETTING(    0x28, "11 Medals" )
	PORT_DIPSETTING(    0x30, "10 Medals" )
	PORT_DIPSETTING(    0x38, "9 Medals" )
	PORT_DIPSETTING(    0x40, "8 Medals" )
	PORT_DIPSETTING(    0x48, "7 Medals" )
	PORT_DIPSETTING(    0x50, "6 Medals" )
	PORT_DIPSETTING(    0x58, "5 Medals" )
	PORT_DIPSETTING(    0x60, "4 Medals" )
	PORT_DIPSETTING(    0x68, "3 Medals" )
	PORT_DIPSETTING(    0x70, "2 Medals" )
	// PORT_DIPSETTING(    0x78, "2 Medals" )
	PORT_DIPNAME( 0x80, 0x80, DEF_STR( Unknown ) )   PORT_DIPLOCATION("SW1:8") // more like debug mode
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )

	PORT_START("DSW2")
	PORT_DIPNAME( 0x0f, 0x00, "Standard of Payout" ) PORT_DIPLOCATION("SW2:1,2,3,4")
	PORT_DIPSETTING(    0x00, "90%" )
	PORT_DIPSETTING(    0x01, "85%" )
	PORT_DIPSETTING(    0x02, "80%" )
	PORT_DIPSETTING(    0x03, "75%" )
	PORT_DIPSETTING(    0x04, "70%" )
	PORT_DIPSETTING(    0x05, "65%" )
	PORT_DIPSETTING(    0x06, "60%" )
	PORT_DIPSETTING(    0x07, "55%" )
	PORT_DIPSETTING(    0x08, "50%" )
	PORT_DIPSETTING(    0x09, "45%" )
	PORT_DIPSETTING(    0x0a, "40%" )
	PORT_DIPSETTING(    0x0b, "35%" )
	PORT_DIPSETTING(    0x0c, "30%" )
	PORT_DIPSETTING(    0x0d, "25%" )
	PORT_DIPSETTING(    0x0e, "20%" )
	PORT_DIPSETTING(    0x0f, "15%" )
	PORT_DIPNAME( 0x30, 0x00, "Play Timer" )         PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x00, "30 sec" )
	PORT_DIPSETTING(    0x10, "24 sec" )
	PORT_DIPSETTING(    0x20, "18 sec" )
	PORT_DIPSETTING(    0x30, "12 sec" )
	PORT_DIPNAME( 0x40, 0x40, "Backup Memory" )      PORT_DIPLOCATION("SW2:7")
	PORT_DIPSETTING(    0x40, "Keep" )
	PORT_DIPSETTING(    0x00, "Clear" )
	PORT_DIPNAME( 0x80, 0x00, "Demo Sound" )         PORT_DIPLOCATION("SW2:8")
	PORT_DIPSETTING(    0x80, DEF_STR( Off ) )
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )

	PORT_START("IN1")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0e, IP_ACTIVE_LOW, IPT_UNKNOWN )    // unused
	PORT_BIT( 0x20, IP_ACTIVE_LOW, IPT_COIN3 ) PORT_NAME("Medal")
	PORT_BIT( 0xd0, IP_ACTIVE_LOW, IPT_UNKNOWN )    // unused

	PORT_START("IN2")
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_SERVICE1 )
	PORT_SERVICE_NO_TOGGLE( 0x02, IP_ACTIVE_LOW )
	PORT_BIT( 0x04, IP_ACTIVE_LOW, IPT_COIN1 ) // 10Y
	PORT_BIT( 0x08, IP_ACTIVE_LOW, IPT_COIN2 ) // 100Y for medals exchange, not game coin in to play
	PORT_BIT( 0x10, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_READ_LINE_DEVICE_MEMBER("hopper", hopper_device, line_r)
	PORT_BIT( 0xe0, IP_ACTIVE_LOW, IPT_UNKNOWN )    // unused

	PORT_START("OUT")
	PORT_BIT( 0x40, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("hopper", hopper_device, motor_w)
INPUT_PORTS_END

static INPUT_PORTS_START( ddboy )
	PORT_INCLUDE( konmedal )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x30, 0x00, "Play Timer" )         PORT_DIPLOCATION("SW2:5,6")
	PORT_DIPSETTING(    0x00, "18 sec" )
	PORT_DIPSETTING(    0x10, "15 sec" )
	PORT_DIPSETTING(    0x20, "12 sec" )
	PORT_DIPSETTING(    0x30, "10 sec" )
INPUT_PORTS_END

static INPUT_PORTS_START( shuriboy )
	PORT_INCLUDE( konmedal )

	PORT_MODIFY("IN1")
	PORT_BIT( 0x80, IP_ACTIVE_HIGH, IPT_CUSTOM ) PORT_READ_LINE_DEVICE_MEMBER("upd", upd7759_device, busy_r)

	PORT_MODIFY("OUT")
	PORT_BIT( 0x02, IP_ACTIVE_HIGH, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("upd", upd7759_device, reset_w) // this should be called 1st, but it's not
	PORT_BIT( 0x01, IP_ACTIVE_LOW, IPT_OUTPUT ) PORT_WRITE_LINE_DEVICE_MEMBER("upd", upd7759_device, start_w)
INPUT_PORTS_END

static INPUT_PORTS_START( fuusenpn )
	PORT_INCLUDE( shuriboy )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x07, 0x07, "Coin Slot 1" )   PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, "5 Coins/0 Credits" )
	PORT_DIPSETTING(    0x01, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x04, "3 Coins/0 Credits" )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
INPUT_PORTS_END

static INPUT_PORTS_START( mario )
	PORT_INCLUDE( shuriboy )

	PORT_MODIFY("DSW1")
	PORT_DIPNAME( 0x07, 0x07, "Coin Slot 1" )   PORT_DIPLOCATION("SW1:1,2,3")
	PORT_DIPSETTING(    0x00, "5 Coins/2 Credits" )
	PORT_DIPSETTING(    0x01, DEF_STR( 5C_1C ) )
	PORT_DIPSETTING(    0x02, DEF_STR( 4C_3C ) )
	PORT_DIPSETTING(    0x03, DEF_STR( 4C_1C ) )
	PORT_DIPSETTING(    0x04, "3 Coins/0 Credits" )
	PORT_DIPSETTING(    0x05, DEF_STR( 3C_1C ) )
	PORT_DIPSETTING(    0x06, DEF_STR( 2C_1C ) )
	PORT_DIPSETTING(    0x07, DEF_STR( 1C_1C ) )
	PORT_DIPNAME( 0x38, 0x38, "Coin Slot 2" )   PORT_DIPLOCATION("SW1:4,5,6")
	PORT_DIPSETTING(    0x00, "4 Medals" )
	PORT_DIPSETTING(    0x08, "5 Medals" )
	PORT_DIPSETTING(    0x10, "6 Medals" )
	PORT_DIPSETTING(    0x18, "8 Medals" )
	PORT_DIPSETTING(    0x20, "10 Medals" )
	PORT_DIPSETTING(    0x28, "11 Medals" )
	PORT_DIPSETTING(    0x30, "12 Medals" )
	PORT_DIPSETTING(    0x38, "15 Medals" )
	PORT_DIPNAME( 0x40, 0x40, DEF_STR( Unknown ) )   PORT_DIPLOCATION("SW1:7")
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Off ) )

	PORT_MODIFY("DSW2")
	PORT_DIPNAME( 0x07, 0x00, "Standard of Payout" ) PORT_DIPLOCATION("SW2:1,2,3")
	PORT_DIPSETTING(    0x00, "90%" )
	PORT_DIPSETTING(    0x01, "80%" )
	PORT_DIPSETTING(    0x02, "70%" )
	PORT_DIPSETTING(    0x03, "60%" )
	PORT_DIPSETTING(    0x04, "50%" )
	PORT_DIPSETTING(    0x05, "40%" )
	PORT_DIPSETTING(    0x06, "30%" )
	PORT_DIPSETTING(    0x07, "20%" )
	PORT_DIPNAME( 0x08, 0x08, DEF_STR( Unknown ) )   PORT_DIPLOCATION("SW2:4")
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Off ) )
	PORT_DIPNAME( 0x10, 0x10, DEF_STR( Unknown ) )   PORT_DIPLOCATION("SW2:5")
	PORT_DIPSETTING(    0x00, DEF_STR( On ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Off ) )
	PORT_DIPNAME( 0x20, 0x20, "Play Timer" )         PORT_DIPLOCATION("SW2:6")
	PORT_DIPSETTING(    0x00, "15 sec" )
	PORT_DIPSETTING(    0x20, "20 sec" )
INPUT_PORTS_END


void konmedal_state::machine_start_common()
{
	m_lamps.resolve();
	save_item(NAME(m_control));
	save_item(NAME(m_control2));
	save_item(NAME(m_shuri_irq));
	save_item(NAME(m_ccu_int_time));
	save_item(NAME(m_ccu_int_time_count));
}

void konmedal_state::machine_start()
{
	machine_start_common();
	membank("bank1")->configure_entries(0, 0x10, memregion("maincpu")->base(), 0x2000);
	membank("bank1")->set_entry(4);
}

MACHINE_START_MEMBER(konmedal_state, shuriboy)
{
	machine_start_common();
	membank("bank1")->configure_entries(0, 0x8, memregion("maincpu")->base()+0x8000, 0x2000);
	membank("bank1")->set_entry(0);
}

void konmedal_state::machine_reset()
{
	m_control = m_control2 = m_shuri_irq = 0;
	m_ccu_int_time_count = 0;
	m_ccu_int_time = 31;
	m_avac = 0;
}

void konmedal_state::tsukande(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(14'318'181)/2); // z84c0008pec 8mhz part, 14.31818Mhz xtal verified on PCB, divisor unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &konmedal_state::medal_main);
	TIMER(config, "scantimer").configure_scanline(FUNC(konmedal_state::konmedal_scanline), "screen", 0, 1);

	NVRAM(config, m_nvram, nvram_device::DEFAULT_ALL_0);
	m_nvram->set_custom_handler(FUNC(konmedal_state::medal_nvram_init));
	HOPPER(config, "hopper", attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH);

	K053252(config, m_k053252, XTAL(14'318'181) / 2); // not verified
	m_k053252->int1_ack().set(FUNC(konmedal_state::vbl_ack_w));
	m_k053252->int2_ack().set(FUNC(konmedal_state::nmi_ack_w));
	m_k053252->int_time().set(FUNC(konmedal_state::ccu_int_time_w));
	m_k053252->set_offsets(32, 16); // not accurate

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(59.62);  /* verified on pcb */
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(64*8, 32*8);
	screen.set_visarea(80, 400-1, 16, 240-1);
	screen.set_screen_update(FUNC(konmedal_state::screen_update_konmedal));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette, FUNC(konmedal_state::konmedal_palette)).set_format(palette_device::xRGB_444, 256);
	//m_palette->enable_shadows();

	K056832(config, m_k056832, 0);
	m_k056832->set_tile_callback(FUNC(konmedal_state::tile_callback));
	m_k056832->set_config(K056832_BPP_4, 1, 0);
	m_k056832->set_palette(m_palette);

	/* sound hardware */
	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();

	YMZ280B(config, m_ymz, XTAL(16'934'400)); // 16.9344MHz xtal verified on PCB
	m_ymz->add_route(0, "lspeaker", 1.0);
	m_ymz->add_route(1, "rspeaker", 1.0);
}

void konmedal_state::ddboy(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(14'318'181)/2); // z84c0008pec 8mhz part, 14.31818Mhz xtal verified on PCB, divisor unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &konmedal_state::ddboy_main);
	TIMER(config, "scantimer").configure_scanline(FUNC(konmedal_state::konmedal_scanline), "screen", 0, 1);

	NVRAM(config, m_nvram, nvram_device::DEFAULT_ALL_0);
	m_nvram->set_custom_handler(FUNC(konmedal_state::medal_nvram_init));
	HOPPER(config, "hopper", attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH);

	K053252(config, m_k053252, XTAL(14'318'181) / 2); // not verified
	m_k053252->int1_ack().set(FUNC(konmedal_state::vbl_ack_w));
	m_k053252->int2_ack().set(FUNC(konmedal_state::nmi_ack_w));
	m_k053252->int_time().set(FUNC(konmedal_state::ccu_int_time_w));
	m_k053252->set_offsets(32, 16); // not accurate

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_refresh_hz(59.62);  /* verified on pcb */
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(0));
	screen.set_size(64*8, 32*8);
	screen.set_visarea(80, 400-1, 16, 240-1);
	screen.set_screen_update(FUNC(konmedal_state::screen_update_konmedal));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette, FUNC(konmedal_state::konmedal_palette)).set_format(palette_device::xRGB_444, 256);
	//m_palette->enable_shadows();

	K056832(config, m_k056832, 0);
	m_k056832->set_tile_callback(FUNC(konmedal_state::tile_callback));
	m_k056832->set_config(K056832_BPP_4, 1, 0);
	m_k056832->set_palette(m_palette);

	/* sound hardware */
	SPEAKER(config, "mono").front_center();
	OKIM6295(config, m_oki, XTAL(14'318'181)/14, okim6295_device::PIN7_HIGH);
	m_oki->add_route(ALL_OUTPUTS, "mono", 1.0);

	K051649(config, "k051649", XTAL(14'318'181)/8).add_route(ALL_OUTPUTS, "mono", 0.45);
}

/*
Shuriken Boy
While being a Z80 medal game, it runs on fairly different hardware. It might merit a new driver when emulation will be fleshed out.

PCB: Konami PWB 452039A
Main CPU: Z80B
OSC: 24.000 MHz
Custom video chips: 051962 + 052109
Sound chips: NEC UPD7759C + 051649
Other custom chip: 051550
Dips: 2 x 8 dips bank
*/

void konmedal_state::shuri_init()
{
	m_layer_colorbase[0] = 0;
	m_layer_colorbase[1] = 8;
	m_layer_colorbase[2] = 8;

	m_layer_order[0] = 2;
	m_layer_order[1] = 1;
	m_layer_order[2] = 0;
}
void konmedal_state::fuusen_init()
{
	m_layer_colorbase[0] = 0;
	m_layer_colorbase[1] = 8;
	m_layer_colorbase[2] = 8;

	m_layer_order[0] = 0;
	m_layer_order[1] = 2;
	m_layer_order[2] = 1;
}
void konmedal_state::mario_init()
{
	m_layer_colorbase[0] = 0;
	m_layer_colorbase[1] = 8;
	m_layer_colorbase[2] = 8;

	m_layer_order[0] = 0;
	m_layer_order[1] = 1;
	m_layer_order[2] = 2;
}

K052109_CB_MEMBER(konmedal_state::shuriboy_tile_callback)
{
	*code |= ((*color & 0xc) << 6) | (bank << 10);
	if (*color & 0x2) *code |= 0x1000;
	*flags = (*color & 0x1) ? TILE_FLIPX : 0;
	*priority = BIT(*color, 4);
	*color = m_layer_colorbase[layer] + ((*color >> 5) & 7);
}

void konmedal_state::shuri_bank_w(uint8_t data)
{
	m_k052109->set_rmrd_line((data & 0x40) ? ASSERT_LINE : CLEAR_LINE);
	membank("bank1")->set_entry(data&0x3);
}

uint8_t konmedal_state::shuri_irq_r()
{
	return m_shuri_irq;
}

void konmedal_state::shuri_irq_w(uint8_t data)
{
	if ((m_shuri_irq & 0x4) && !(data & 0x4))
	{
		m_maincpu->set_input_line(0, CLEAR_LINE);
	}
	else if ((m_shuri_irq & 0x1) && !(data & 0x1))
	{
		m_maincpu->set_input_line(INPUT_LINE_NMI, CLEAR_LINE);
	}

	m_shuri_irq = data;
}

TIMER_DEVICE_CALLBACK_MEMBER(konmedal_state::shuri_scanline)
{
	int scanline = param;

	if ((scanline == 240) && (m_shuri_irq & 0x4))
	{
		m_maincpu->set_input_line(0, ASSERT_LINE);

	}

	if ((scanline == 255) && (m_shuri_irq & 0x1))
	{
		m_maincpu->set_input_line(INPUT_LINE_NMI, ASSERT_LINE);
	}
}

void konmedal_state::mario_scrollhack_w(uint8_t data)
{
	// Mario Roulette enable X and Y scroll in the same time for both layers, which is currently not supported by emulated K052109.
	// here we hacky disable Y scroll for layer A and X scroll for layer B.
	if (data == 0x36)
		data = 0x22;
	m_k052109->write(0x1c80, data);
}

void konmedal_state::shuriboy(machine_config &config)
{
	/* basic machine hardware */
	Z80(config, m_maincpu, XTAL(24'000'000) / 3); // divisor unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &konmedal_state::shuriboy_main);
	TIMER(config, "scantimer").configure_scanline(FUNC(konmedal_state::shuri_scanline), "screen", 0, 1);

	NVRAM(config, m_nvram, nvram_device::DEFAULT_ALL_0);
	m_nvram->set_custom_handler(FUNC(konmedal_state::shuriboy_nvram_init));
	HOPPER(config, "hopper", attotime::from_msec(100), TICKET_MOTOR_ACTIVE_HIGH, TICKET_STATUS_ACTIVE_HIGH);

	/* video hardware */
	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER)); // everything not verified, just a placeholder
	screen.set_refresh_hz(60);
	screen.set_vblank_time(ATTOSECONDS_IN_USEC(30));
	screen.set_size(64*8, 32*8);
	screen.set_visarea(112, 400-1, 16, 240-1);
	screen.set_screen_update(FUNC(konmedal_state::screen_update_shuriboy));
	screen.set_palette(m_palette);

	PALETTE(config, m_palette, FUNC(konmedal_state::konmedal_palette)).set_format(palette_device::xRGB_444, 256); // not verified
//  m_palette->enable_shadows();
//  m_palette->enable_hilights();

	K052109(config, m_k052109, 0);
	m_k052109->set_palette(m_palette);
	m_k052109->set_screen(nullptr);
	m_k052109->set_tile_callback(FUNC(konmedal_state::shuriboy_tile_callback));

	MCFG_MACHINE_START_OVERRIDE(konmedal_state, shuriboy)

	/* sound hardware */
	SPEAKER(config, "mono").front_center();

	K051649(config, "k051649", XTAL(24'000'000) / 12).add_route(ALL_OUTPUTS, "mono", 0.45); // divisor unknown

	UPD7759(config, m_upd7759, XTAL(640'000)).add_route(ALL_OUTPUTS, "mono", 0.60);
}

void konmedal_state::fuusenpn(machine_config &config)
{
	shuriboy(config);
	m_nvram->set_custom_handler(FUNC(konmedal_state::fuusenpn_nvram_init));
}

void konmedal_state::mariorou(machine_config &config)
{
	shuriboy(config);
	m_nvram->set_custom_handler(FUNC(konmedal_state::mario_nvram_init));
}

void konmedal_state::medal_nvram_init(nvram_device &nvram, void *base, size_t size)
{
	memset(base, 0x00, size);
	u8 *ram = (u8*)base;
	ram[0x303] = 0xff;
	ram[0x309] = 0xaa;
}

void konmedal_state::shuriboy_nvram_init(nvram_device &nvram, void *base, size_t size)
{
	memset(base, 0x00, size);
	u8 *ram = (u8*)base;
	ram[0x08] = 0x01;
	ram[0x09] = 0x02;
	ram[0x0e] = 0x04;
	ram[0x17] = 0x08;
	ram[0x18] = 0x10;
	ram[0x21] = 0x20;
	ram[0x50] = 0x40;
	ram[0x52] = 0x80;
	ram[0x53] = 0x55;
	ram[0x54] = 0xaa;
	ram[0x5d] = 0x33;
}

void konmedal_state::fuusenpn_nvram_init(nvram_device &nvram, void *base, size_t size)
{
	memset(base, 0x00, size);
	u8 *ram = (u8*)base;
	ram[0x203] = 0xff;
	ram[0x209] = 0xaa;
}

void konmedal_state::mario_nvram_init(nvram_device &nvram, void *base, size_t size)
{
	memset(base, 0x00, size);
	u8 *ram = (u8*)base;
	ram[0x502] = 0xff;
	ram[0x506] = 0xaa;
	ram[0x508] = 0x55;
}

ROM_START( tsukande )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "441-d02.4g",   0x000000, 0x020000, CRC(6ed17227) SHA1(4e3f5219cbf6f42c60df38a99f3009fe49f78fc1) )

	ROM_REGION( 0x80000, "k056832", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "441-a03.4l",   0x000002, 0x020000, CRC(8adf3304) SHA1(1c8312c76cd626978ff5b3896fb5a5b34be72988) )
	ROM_LOAD32_BYTE( "441-a04.4m",   0x000003, 0x020000, CRC(038e0c67) SHA1(2b8640bfad7026a2d86fb6498aff4d7a9cb0b700) )
	ROM_LOAD32_BYTE( "441-a05.4p",   0x000000, 0x020000, CRC(937c4740) SHA1(155c869b9321d62df115435d7c855f9be4278e45) )
	ROM_LOAD32_BYTE( "441-a06.4p",   0x000001, 0x020000, CRC(947a8c45) SHA1(16e3dceb304266bbd2bddc2cec832ebff04e4c71) )

	ROM_REGION( 0x400, "proms", 0 )
	// R
	ROM_LOAD( "441a07.20k",   0x000000, 0x000100, CRC(7d0c53c2) SHA1(f357e0cb3d53374208ad1670e70be03b399a4c02) )
	// G
	ROM_LOAD( "441a08.21k",   0x000100, 0x000100, CRC(e2c3e853) SHA1(36a3008dde714ade53b9a01ac9d94c6cc655c293) )
	// B
	ROM_LOAD( "441a09.23k",   0x000200, 0x000100, CRC(3daca33a) SHA1(38644f574beaa593f3348b49eabea9e03d722013) )
	// P(riority?)
	ROM_LOAD( "441a10.21m",   0x000300, 0x000100, CRC(063722ff) SHA1(7ba43acfdccb02e7913dc000c4f9c57c54b1315f) )

	ROM_REGION( 0x100000, "ymz", 0 )
	ROM_LOAD( "441a11.10d",   0x000000, 0x080000, CRC(e60a7495) SHA1(76963324e818974bc5209e7122282ba4d73fda93) )
	ROM_LOAD( "441a12.10e",   0x080000, 0x080000, CRC(dc2dd5bc) SHA1(28ef6c96c360d706a4296a686f3f2a54fce61bfb) )
ROM_END

ROM_START( ddboy )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "342_c02.27c010.4d", 0x000000, 0x020000, CRC(dc33af9f) SHA1(db22f3b28e3aba69f70fd2581c77755373b582d0) )

	ROM_REGION( 0x80000, "k056832", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "342_a03.27c010.4f", 0x000002, 0x020000, CRC(424f80dd) SHA1(fb7648960ce0951aebcf5cf4465a9acb3ab49cd8) )
	ROM_LOAD32_BYTE( "342_a04.27c010.4g", 0x000003, 0x020000, CRC(a4d4e15e) SHA1(809afab3f2adc58ca5d18e2413b40a6f33bd0cfa) )
	ROM_LOAD32_BYTE( "342_a05.27c010.4h", 0x000000, 0x020000, CRC(e7e50901) SHA1(5e01377a3ad8ccb2a2b56610e8225b9b6bf15122) )
	ROM_LOAD32_BYTE( "342_a06.27c010.4j", 0x000001, 0x020000, CRC(49f35d66) SHA1(3d5cf3b6eb6a3497609117acd002169a31130418) )

	ROM_REGION( 0x100000, "oki", 0 )
	ROM_LOAD( "342_a01.27c010.8b", 0x000000, 0x020000, CRC(e9ce569c) SHA1(ce9b3e60eac3543aca9e82a9ccf77c53a6aff504) )

	ROM_REGION( 0x400, "proms", 0 )
	// R
	ROM_LOAD( "342_a07.82s129.13f", 0x000000, 0x000100, CRC(f8c11f4d) SHA1(95061d0af7c8bac702aa48e16c0711719250653f) )
	// G
	ROM_LOAD( "342_a08.82s129.14f", 0x000100, 0x000100, CRC(1814db4b) SHA1(08b25f96dc3af15b3fa3c88b2884845abd3ff620) )
	// B
	ROM_LOAD( "342_a09.82s129.15f", 0x000200, 0x000100, CRC(21e2dd13) SHA1(721c7fa1a01c810a7ce35b4331d280704b4e04fd) )
	// P(riority?)
	ROM_LOAD( "342_a10.82s129.14g", 0x000300, 0x000100, CRC(1fa443f9) SHA1(84b0a36a4e49bf75bda1871bf52090ee5a75cd03) )
ROM_END

// this is a slightly different version on the same PCB as tsukande
ROM_START( ddboya )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "342-f02-4g-=p=.bin", 0x000000, 0x020000, CRC(563dfd4f) SHA1(a50544735a9d6f448b969b9fd84e6cdca303d7a0) )

	ROM_REGION( 0x80000, "k056832", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "342_a03.27c010.4f", 0x000002, 0x020000, CRC(424f80dd) SHA1(fb7648960ce0951aebcf5cf4465a9acb3ab49cd8) )
	ROM_LOAD32_BYTE( "342_a04.27c010.4g", 0x000003, 0x020000, CRC(a4d4e15e) SHA1(809afab3f2adc58ca5d18e2413b40a6f33bd0cfa) )
	ROM_LOAD32_BYTE( "342_a05.27c010.4h", 0x000000, 0x020000, CRC(e7e50901) SHA1(5e01377a3ad8ccb2a2b56610e8225b9b6bf15122) )
	ROM_LOAD32_BYTE( "342_a06.27c010.4j", 0x000001, 0x020000, CRC(49f35d66) SHA1(3d5cf3b6eb6a3497609117acd002169a31130418) )

	ROM_REGION( 0x100000, "ymz", 0 )
	ROM_LOAD( "342-a11-10d-=s1=.bin", 0x000000, 0x080000, CRC(b523bced) SHA1(87a814035af4dcf24454667d4346d301303d697e) )
	ROM_LOAD( "342-a12-10e-=s2=.bin", 0x080000, 0x080000, CRC(6febafe7) SHA1(69e550dd067f326b4d20a859345f193b43a5af99) )

	ROM_REGION( 0x400, "proms", 0 )
	// R
	ROM_LOAD( "342_a07.82s129.13f", 0x000000, 0x000100, CRC(f8c11f4d) SHA1(95061d0af7c8bac702aa48e16c0711719250653f) )
	// G
	ROM_LOAD( "342_a08.82s129.14f", 0x000100, 0x000100, CRC(1814db4b) SHA1(08b25f96dc3af15b3fa3c88b2884845abd3ff620) )
	// B
	ROM_LOAD( "342_a09.82s129.15f", 0x000200, 0x000100, CRC(21e2dd13) SHA1(721c7fa1a01c810a7ce35b4331d280704b4e04fd) )
	// P(riority?)
	ROM_LOAD( "342_a10.82s129.14g", 0x000300, 0x000100, CRC(1fa443f9) SHA1(84b0a36a4e49bf75bda1871bf52090ee5a75cd03) )
ROM_END

ROM_START( buttobi )
	ROM_REGION( 0x20000, "maincpu", 0 ) /* main program */

	ROM_LOAD( "440-b02-4d.bin", 0x000000, 0x020000, CRC(428fc60e) SHA1(b980ee0bce0cd30b4c070b7c32ba37ac1b92fa9f) )
	ROM_REGION( 0x80000, "k056832", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "440-a03-4f.bin", 0x000002, 0x020000, CRC(04917807) SHA1(e10796e4d2dcf126db6ac03749fcd2532a4ef846) )
	ROM_LOAD32_BYTE( "440-a04-4g.bin", 0x000003, 0x020000, CRC(0396c6d9) SHA1(1d6b8f997b69828d09930a45724c5f0799ac988a) )
	ROM_LOAD32_BYTE( "440-a05-4h.bin", 0x000000, 0x020000, CRC(11d3ec99) SHA1(4bc267fa58ba1dd83e1d43b0e981b9e65d688042) )
	ROM_LOAD32_BYTE( "440-a06-4j.bin", 0x000001, 0x020000, CRC(f915c517) SHA1(26128fcdf9947efd7959deddb52575088fa492d4) )

	ROM_REGION( 0x400, "proms", 0 )
	// R (440a07.13f)
	ROM_LOAD( "440a07.13f",   0x000000, 0x000100, CRC(6ebce405) SHA1(778be32bf3cbc34014122d60c640979947d380ae) )
	// G (440a08.14f)
	ROM_LOAD( "440a08.14f",   0x000100, 0x000100, CRC(7b36a53c) SHA1(021ff79289848e094dc3b23b7f0b1cc0e9fe55a6) )
	// B (440a09.15f)
	ROM_LOAD( "440a09.15f",   0x000200, 0x000100, CRC(2e4c9051) SHA1(ba5d3d96749ec8ae65595dba46dad6f2b1f8953e) )
	// P(riority?) (440a10.14g)
	ROM_LOAD( "440a10.14g",   0x000300, 0x000100, CRC(c2739e08) SHA1(c409ddc94f80a185e9e382762126d3ae551bf414) )

	ROM_REGION( 0x40000, "oki", 0 )
	ROM_LOAD( "440-a01-8b.bin", 0x000000, 0x040000, CRC(955b1bd5) SHA1(1e8130a3634972d742ba4ad103e1738e44a4e28c) )
ROM_END


ROM_START( shuriboy )
	ROM_REGION( 0x10000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "gs-341-b01.13g", 0x000000, 0x010000, CRC(3c0f36b6) SHA1(1d3838f45969228a8b2054cd5baf8892db68b644) )

	ROM_REGION( 0x40000, "k052109", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "341-a03.2h", 0x000000, 0x010000, CRC(8e9e9835) SHA1(f8dc4579f238d91c0aef59167be7e5de87dc4ba7) )
	ROM_LOAD32_BYTE( "341-a04.4h", 0x000001, 0x010000, CRC(ac82d67b) SHA1(65869adfbb67cf10c92e50239fd747fc5ad4714d) )
	ROM_LOAD32_BYTE( "341-a05.5h", 0x000002, 0x010000, CRC(31403832) SHA1(d13c54d3768a0c2d60a3751db8980199f60db243) )
	ROM_LOAD32_BYTE( "341-a06.7h", 0x000003, 0x010000, CRC(361e26eb) SHA1(7b5ad6a6067afb3350d85a3f2026e4d685429e20) )

	ROM_REGION( 0x200000, "upd", 0 )
	ROM_LOAD( "341-a02.13c", 0x000000, 0x020000, CRC(e1f5c8f1) SHA1(323a078720e09a7326e82cb623b6c90e2674e800) )

	ROM_REGION( 0x400, "proms", 0 ) // am27s21apc
	ROM_LOAD( "342_a07.2d", 0x000000, 0x000100, CRC(1260128d) SHA1(c49ee917aa38d87edaccbed7acf6e1076f23a0fd) )
	ROM_LOAD( "342_a08.3d", 0x000100, 0x000100, CRC(a5a504b5) SHA1(e4da0bc4c4b44dc0e3355497d99d80219b9178c0) )
	ROM_LOAD( "342_a09.4d", 0x000200, 0x000100, CRC(09141cc7) SHA1(2b32af236caa159fe6e9c0021bfc31b8cdfdbe70) )
	ROM_LOAD( "341_a10.3e", 0x000300, 0x000100, CRC(01335046) SHA1(63a2826c3883cde8e23f78e27f8d766f15799d1a) )
ROM_END

ROM_START( fuusenpn )
	ROM_REGION( 0x10000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "241-d01-13g.bin", 0x000000, 0x010000, CRC(e9fee0f8) SHA1(2619b94284649243a84e84b166815ba1c7658814) )

	ROM_REGION( 0x40000, "k052109", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "241-a03-2h.bin", 0x000000, 0x010000, CRC(b8bd7bfa) SHA1(883f3591d87275416f917f9c302b807aac5845a4) )
	ROM_LOAD32_BYTE( "241-a04-4h.bin", 0x000001, 0x010000, CRC(04ffa2a3) SHA1(a1b0615dc8326c296fadb5c45f94f2ea3d670556) )
	ROM_LOAD32_BYTE( "241-a05-5h.bin", 0x000002, 0x010000, CRC(8c4ad5fa) SHA1(987f24d0566d6b815070b74dada331a4f739f601) )
	ROM_LOAD32_BYTE( "241-a06-7h.bin", 0x000003, 0x010000, CRC(e650e4c4) SHA1(ac1f03b89f4a17b2583e3a81bd474eda01d41be0) )

	ROM_REGION( 0x200000, "upd", 0 )
	ROM_LOAD( "241-a02-13c.bin", 0x000000, 0x020000, CRC(f2c39c7b) SHA1(ec420a1fbd6e83fe1ff5c9c8f7169b755d0cc494) )

	ROM_REGION( 0x400, "proms", ROMREGION_ERASE00 ) // am27s21apc
	ROM_LOAD( "241a07.bin",   0x000000, 0x000100, CRC(b246f88e) SHA1(e16aae373b41bc11d4828e1cc2cc267552b0397c) )
	ROM_LOAD( "241a08.bin",   0x000100, 0x000100, CRC(e84cbf2a) SHA1(a0e99df97ca268c16625a02b6e6427aadcca1b5b) )
	ROM_LOAD( "241a09.bin",   0x000200, 0x000100, CRC(79bd3e49) SHA1(ff94856d11acfba364f2d05ca955c10fbc02e265) )
	ROM_LOAD( "241a10.bin",   0x000300, 0x000100, CRC(f7e3d8ee) SHA1(89c505873c884f9e1ec0cb113a3557d3f67943b9) )
ROM_END

ROM_START( mariorou )
	ROM_REGION( 0x10000, "maincpu", 0 ) /* main program */
	ROM_LOAD( "111_d01.3g.27c512", 0x000000, 0x010000, CRC(96e075c1) SHA1(ca8170e1756c7c4610680f5c5a95020ea34fd1f5) )

	ROM_REGION( 0x40000, "k052109", 0 )   /* tilemaps */
	ROM_LOAD32_BYTE( "111_c03.2h.27c256", 0x000000, 0x008000, CRC(b4e769ba) SHA1(5af1aa7768daa7cc844d64cfd13c8d81d801fc70) )
	ROM_LOAD32_BYTE( "111_c04.4h.27c256", 0x000001, 0x008000, CRC(c6081712) SHA1(308df4824e884cb7d1febd34d85b1c1b9f494166) )
	ROM_LOAD32_BYTE( "111_c05.5h.27c256", 0x000002, 0x008000, CRC(83eec257) SHA1(fcd0ff5050ef13f3a6d3a1d12c24f16ef6b4d25d) )
	ROM_LOAD32_BYTE( "111_c06.7h.27c256", 0x000003, 0x008000, CRC(d71554e9) SHA1(e388e705f2fd84d98f84d7b843ded5c509f05796) )

	ROM_REGION( 0x200000, "upd", 0 )
	ROM_LOAD( "111_c02.13c.27c010", 0x000000, 0x020000, CRC(e42ba2d7) SHA1(4520353aa60c9ced87d94ef338903e08dff9f4cc) )

	ROM_REGION( 0x400, "proms", ROMREGION_ERASE00 ) // am27s21apc
	ROM_LOAD( "111_c07.2d.82s129", 0x000000, 0x000100, CRC(89bc3e83) SHA1(4089430cd48009ab1711a00ace81a5823cfafba6) )
	ROM_LOAD( "111_c08.3d.82s129", 0x000100, 0x000100, CRC(f9f7dfa8) SHA1(e502c7c554e3add5d27645ea3bddbe2339c93f7a) )
	ROM_LOAD( "111_c09.4d.82s129", 0x000200, 0x000100, CRC(879b796b) SHA1(f41d48013f0d55e7de4e40dc00dd30482946c14c) )
	ROM_LOAD( "111_a10.3e.82s129", 0x000300, 0x000100, CRC(07ffc2ed) SHA1(37955d1788a86b90439233bb098c59b191056f68) )
ROM_END

// Konami PWB 452093A boards (TMNT tilemaps)
GAME( 1991, mariorou, 0,     mariorou, mario,    konmedal_state, mario_init,   ROT0, "Konami", "Mario Roulette", MACHINE_SUPPORTS_SAVE)
GAME( 1993, shuriboy, 0,     shuriboy, shuriboy, konmedal_state, shuri_init,   ROT0, "Konami", "Shuriken Boy", MACHINE_SUPPORTS_SAVE)
GAME( 1993, fuusenpn, 0,     fuusenpn, fuusenpn, konmedal_state, fuusen_init,  ROT0, "Konami", "Fuusen Pentai", MACHINE_SUPPORTS_SAVE)

// Konami PWB 452574A boards (GX tilemaps)
GAME( 1994, buttobi,  0,     ddboy,    ddboy,    konmedal_state, buttobi_init, ROT0, "Konami", "Buttobi Striker", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE)
GAME( 1994, ddboy,    0,     ddboy,    ddboy,    konmedal_state, ddboy_init,   ROT0, "Konami", "Dam Dam Boy (on dedicated PCB)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE)

// Konami PWB 402218 boards (GX tilemaps)
GAME( 1994, ddboya,   ddboy, tsukande, ddboy,    konmedal_state, ddboy_init,   ROT0, "Konami", "Dam Dam Boy (on Tsukande Toru Chicchi PCB)", MACHINE_IMPERFECT_GRAPHICS | MACHINE_SUPPORTS_SAVE)
GAME( 1995, tsukande, 0,     tsukande, konmedal, konmedal_state, tsuka_init,   ROT0, "Konami", "Tsukande Toru Chicchi", MACHINE_SUPPORTS_SAVE)
