// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, David Haywood

#include "emu.h"
#include "includes/spg2xx.h"


class spg2xx_pdc100_game_state : public spg2xx_game_state
{
public:
	spg2xx_pdc100_game_state(const machine_config &mconfig, device_type type, const char *tag) :
		spg2xx_game_state(mconfig, type, tag),
		m_numbanks(-1)
	{ }

	void pdc100(machine_config& config);

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

	virtual void porta_w(offs_t offset, uint16_t data, uint16_t mem_mask = ~0) override;

private:
	int m_numbanks;
};


static INPUT_PORTS_START( pdc100 )
	PORT_START("P1")
	PORT_BIT( 0x00ff, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("Left Trigger")
	PORT_BIT( 0x0e00, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x1000, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("Right Trigger")
	PORT_BIT( 0xe000, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("Pause")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END


void spg2xx_pdc100_game_state::machine_start()
{
	spg2xx_game_state::machine_start();
	m_numbanks = memregion("maincpu")->bytes() / 0x800000;
}

void spg2xx_pdc100_game_state::machine_reset()
{
	m_current_bank = -1;
	switch_bank(m_numbanks - 1); // pdc100 must boot from upper bank
	m_maincpu->reset();
}

void spg2xx_pdc100_game_state::porta_w(offs_t offset, uint16_t data, uint16_t mem_mask)
{
	// pdc100 simply writes 0000 at times during bootup while initializing stuff, which causes an invalid bankswitch mid-code execution
	if (data & 0xff00)
		switch_bank(data & (m_numbanks - 1));
}


void spg2xx_pdc100_game_state::pdc100(machine_config &config)
{
	non_spg_base(config);
	m_maincpu->porta_out().set(FUNC(spg2xx_pdc100_game_state::porta_w));
	m_maincpu->porta_in().set_ioport("P1");
	m_maincpu->portb_in().set_ioport("P2");
	m_maincpu->portc_in().set_ioport("P3"); // not used?
}

ROM_START( pdc100 )
	ROM_REGION( 0x4000000, "maincpu", ROMREGION_ERASE00 )
	// only 1st half of this is used "Jumper resistor (0 ohm) that short A25 to ground"
	// 2nd half just contains what seems to be random garbage
	ROM_LOAD16_WORD_SWAP( "pdc100.bin", 0x000000, 0x4000000, CRC(57285b49) SHA1(cfb4be7877ec263d24063a004c56985db5c0f4e2) )
	ROM_IGNORE(0x4000000)
ROM_END

ROM_START( tmntpdc )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "pdc_turtles.bin", 0x000000, 0x800000, CRC(ee9e70a3) SHA1(7620f1b7aeaec8032faa8eb7552f775e8d6d14ba) )
ROM_END


// there were older models eg. PDC30 with fewer games, and some differences (eg "Jo Ma" instead of "Jo Ma 2")
// "Jo Ma 2" shows "Licensed by Mitchell Corporation" (Mitchell made the original Puzzloop on which this style of game is based)  Videos of the original Jo Ma show it lacking this text.

// Other known units
// PDC Dora the Explorer
// PDC 30
// PDC 40
// PDC 200

// This was dumped from an Anncia branded unit, although there's no ingame branding, so ROM is probably the same for all PDC100 units
CONS( 2008, pdc100,  0,        0, pdc100, pdc100,  spg2xx_pdc100_game_state, empty_init, "Conny",            "PDC100 - Pocket Dream Console", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

CONS( 2013, tmntpdc, 0,        0, pdc100, pdc100,  spg2xx_pdc100_game_state, empty_init, "Conny / VideoJet", "Teenage Mutant Ninja Turtles - Pocket Dream Console (France)", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )
