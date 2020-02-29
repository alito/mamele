// license:BSD-3-Clause
// copyright-holders:Ryan Holtz, David Haywood
/******************************************************************************

    Short Description:

        Systems which run on the SPG243 SoC

        die markings show
        "SunPlus QL8041" ( also known as Sunplus SPG240 & PAC300 )

            All GameKeyReady units
                Disney Princess (GKR)
                Wheel of Fortune (GKR)
                JAKKS WWE (GKR)
                Fantastic 4 (GKR)
                Justice League (GKR)
                Dora the Explorer Nursery Rhyme (GKR)
                Dora the Explorer Play Park (GKR)
                Spiderman 5-in-1 (GKR)
                etc.

            (other non GKR JAKKS games)
            X-Men (Wolverine pad)
            Avatar: The Last Airbender
            Superman in Super Villain Showdown

            (other games)
            Mattel Classic Sports

        "SunPlus QL8041C" ( known as Sunplus SPG2??, seems to be compatible with above, so probably just a chip revision )

            Clickstart ( see clickstart.cpp instead)
            Wheel of Fortune 2nd Edition
            Spider-man - Villain Roundup
            Dream Life Superstar
            Designer's World
            Star Wars TV Touch

        "SunPlus QU7074-P69A"

            The Batman
            Star Wars (non-gamekey, which model? falcon? - check)
            Dream Life

        "SunPlus QL8167b" (is the scrambling built into the CPU, or external?)

            Lexibook Zeus IG900 20-in-1

        "SunPlus QL8139C"

            Radica Cricket
            V Smile Baby (Sweden) - see vsmileb.cpp

        ---

        Very likely the same

        "Sunplus QL8167" (these might have ROM scrambling if that is a 8167 feature)

            Disney Princess Magical Adventure
            Go Diego Go
            Shrek - Over the Hedge (this unit shows a 'GameKey Unlock More Games' on startup, but has no port, not even on the internal PCB)
            Marvel Heroes (Spider-man)
            Spiderman 3 (Movie - black)


        ---

        It is unknown if the following are close to this architecture or not (no dumps yet)

        "SunPlus QU7073-P69A"

            Mortal Kombat

        "Sunplus PU7799-P680?" (difficult to read)

            Mission Paintball

        ---

        These are definitely different but still unSP based

        "SunPlus PA7801" ( known as Sunplus SPG110? )
        - see spg110.cpp instead

        "GCM394" (this is clearly newer, has extra opcodes, different internal map etc. also scaling and higher resolutions based on Spongebob)
        - see sunplus_unsp20soc.cpp instead

    Status:

        Mostly working

    To-Do:

        Proper driver_device inheritance to untangle the mess of members

    Detailed list of bugs:

        All systems:
            Various inaccuracies in samples/envelopes.

        jak_wall, jak_sdoo:
            Game seems unhappy with NVRAM, clears contents on each boot.
        jak_disf:
            Shows corrupt logo on first boot with no valid nvram (possibly hardware does too - verify if possible to invalidate EEPROM on device)
        lexizeus:
            Some corrupt sound effects and a few corrupt ground tiles a few minutes in. (checksum is good, and a video recorded
             from one of these doesn't exhibit these problems, so either emulation issue or alt revision?)
        pvmil:
            Question order depends on SoC RNG, only reads when it wants a new value, so unless RNG runs on a timer question order ends up the same

        vii:
            When loading a cart from file manager, sometimes MAME will crash.
            The "MOTOR" option in the diagnostic menu does nothing when selected.
            The "SPEECH IC" option in the diagnostic menu does nothing when selected.
            On 'vii_vc1' & 'vii_vc2' cart, the left-right keys are transposed with the up-down keys.
            - This is not a bug per se, as the games are played with the controller physically rotated 90 degrees.

    Note:
        Cricket, Skateboarder, Skannerz and Football 2 list a 32-bit checksum at the start of ROM.
        This is the byte sum of the file, excluding the first 16 byte (where the checksum is stored)

        Test Modes:
        Justice League : press UP, DOWN, LEFT, BT3 on the JAKKS logo in that order, quickly, to get test menu
        WWE : press UP, BT1, BT2 together during startup logos

        Disney Friends, MS Pacman, WallE, Batman (and some other HotGen GameKeys) for test mode, hold UP,
        press A, press DOWN during startup

        Capcom test (same access as other Hotgen games) mode looks like this (tested on PAL unit, same ROM as dumped one)

        RAM OK     2800
                111111
                5432109876543210
        IOA    ............111.          (values go from . to 1 when inputs are moved, never 0 as in MAME!, core bug?)
                        GAMEKEY E0
        IOB0
        IOC    XXX.........X...
        SPRITES

        Care Bears : Hold analog stck up, rotate stick 360 degress back to up, press 'A' while still holding up

    TODO:
        Work out how to access the hidden TEST menus for all games (most JAKKS games should have one at least)

*******************************************************************************/

#include "emu.h"
#include "includes/spg2xx.h"

/*************************
*    Machine Hardware    *
*************************/

void spg2xx_game_state::switch_bank(uint32_t bank)
{
	if (m_bank)
	{
		if (bank != m_current_bank)
		{
			m_current_bank = bank;
			m_bank->set_entry(bank);
			m_maincpu->invalidate_cache();
		}
	}
}

WRITE16_MEMBER(spg2xx_game_state::porta_w)
{
	logerror("%s: porta_w %04x\n", machine().describe_context(), data);
}

WRITE16_MEMBER(spg2xx_game_state::portb_w)
{
	logerror("%s: portb_w %04x\n", machine().describe_context(), data);
}

WRITE16_MEMBER(spg2xx_game_state::portc_w)
{
	logerror("%s: portc_w %04x\n", machine().describe_context(), data);
}

WRITE8_MEMBER(spg2xx_game_state::i2c_w)
{
	logerror("%s: i2c_w %05x %04x\n", machine().describe_context(), offset, data);
}

READ8_MEMBER(spg2xx_game_state::i2c_r)
{
	logerror("%s: i2c_r %04x\n", machine().describe_context(), offset);
	return 0x0000;
}

READ16_MEMBER(spg2xx_game_state::base_porta_r)
{
	uint16_t data = m_io_p1->read();
	logerror("%s: Port A Read: %04x\n", machine().describe_context(), data);
	return data;
}

READ16_MEMBER(spg2xx_game_state::base_portb_r)
{
	uint16_t data = m_io_p2->read();
	logerror("%s: Port B Read: %04x\n", machine().describe_context(), data);
	return data;
}

READ16_MEMBER(spg2xx_game_state::base_portc_r)
{
	uint16_t data = m_io_p3->read();
	logerror("%s: Port C Read: %04x\n", machine().describe_context(), data);
	return data;
}

READ16_MEMBER(spg2xx_game_state::base_guny_r)
{
	uint16_t data = m_io_guny->read();
	logerror("%s: Gun Y Read: %04x\n", machine().describe_context(), data);
	return data;
}

READ16_MEMBER(spg2xx_game_state::base_gunx_r)
{
	uint16_t data = m_io_gunx->read();
	logerror("%s: Gun X Read: %04x\n", machine().describe_context(), data);
	return data;
}



void spg2xx_game_state::mem_map_4m(address_map &map)
{
	map(0x000000, 0x3fffff).bankr("cartbank");
}

void spg2xx_game_state::mem_map_2m(address_map &map)
{
	map(0x000000, 0x1fffff).mirror(0x200000).bankr("cartbank");
}

void spg2xx_game_state::mem_map_1m(address_map &map)
{
	map(0x000000, 0x0fffff).mirror(0x300000).bankr("cartbank");
}

static INPUT_PORTS_START( rad_skat )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON4 ) PORT_NAME("Full Left")
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON5 ) PORT_NAME("Full Right")
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME("Slight Left") // you have to use this for the menus (eg trick lists)
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME("Slight Right")
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("Front")
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Back")
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_UNUSED )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_UNUSED )
	// there only seem to be 3 buttons on the pad part, so presumably all the above are the skateboard, and below are the pad?
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("M Button")
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_NAME("X Button")
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_BUTTON3 ) PORT_NAME("O Button")
	PORT_BIT( 0xf800, IP_ACTIVE_LOW, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNUSED ) // read but unused?

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_CUSTOM ) // NTSC (1) / PAL (0) flag
INPUT_PORTS_END

static INPUT_PORTS_START( rad_skatp )
	PORT_INCLUDE(rad_skat)

	PORT_MODIFY("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_CUSTOM ) // NTSC (1) / PAL (0) flag
INPUT_PORTS_END

static INPUT_PORTS_START( rad_sktv )
	/* how does the Scanner connect? probably some serial port with comms protocol, not IO ports?
	   internal test mode shows 'uart' ports (which currently fail)

	   To access internal test hold DOWN and BUTTON1 together on startup until a coloured screen appears.
	   To cycle through the tests again hold DOWN and press BUTTON1 */

	PORT_START("P1")
	PORT_DIPNAME( 0x0001, 0x0001, "IN0" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("P2")
	PORT_DIPNAME( 0x0001, 0x0001, "IN1" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( wiwi18 )
	PORT_START("P1")
	PORT_DIPNAME( 0x0001, 0x0001, "IN0" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, "Possible Input" )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("P2")
	PORT_DIPNAME( 0x0001, 0x0001, "IN1" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_CUSTOM ) // NTSC (1) / PAL (0) flag
INPUT_PORTS_END



static INPUT_PORTS_START( tvsprt10 )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0180, IP_ACTIVE_HIGH, IPT_UNKNOWN )
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // Start
	PORT_BIT( 0xfc00, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( decathln )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(1)
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1)
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1)
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(1)
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_PLAYER(2)
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT ) PORT_PLAYER(2)
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2)
	PORT_BIT( 0x0180, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) PORT_PLAYER(2)
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // Start
	PORT_BIT( 0xfc00, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( mattelcs ) // there is a 'secret test mode' that previously got activated before inputs were mapped, might need unused inputs to active?
	PORT_START("P1")
	PORT_BIT( 0x0007, IP_ACTIVE_LOW, IPT_UNUSED ) // must be IP_ACTIVE_LOW or you can't switch to Football properly?
	PORT_DIPNAME( 0x0018, 0x0000, "Game Select Slider" ) // technically not a dipswitch, a 3 position slider, but how best map it?
	PORT_DIPSETTING(      0x0008, "Baseball (Left)" )
	PORT_DIPSETTING(      0x0010, "Basketball (Middle)" )
	PORT_DIPSETTING(      0x0000, "Football (Right)" )
	// no 4th position possible
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Difficulty ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Hard ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( Normal ) )
	PORT_BIT( 0xffa0, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )    PORT_NAME("Joypad Up")
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )  PORT_NAME("Joypad Down")
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )  PORT_NAME("Joypad Left")
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT ) PORT_NAME("Joypad Right")
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 )        PORT_NAME("Sound") // toggles between sound+music, sound only, and no sound
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 )        PORT_NAME("Hike / Pitch")
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON3 )        PORT_NAME("Shoot / Run")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_BUTTON4 )        PORT_NAME("Kick / Hit")
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

/* hold 'Console Down' while powering up to get the test menu, including input tests
   the ball (Wired) and bat (IR) are read some other way as they don't seem to appear in the ports. */
static INPUT_PORTS_START( rad_crik )
	PORT_START("P1")
	PORT_BIT( 0x003f, IP_ACTIVE_LOW, IPT_UNKNOWN )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_NAME("Console Enter") // these are the controls on the base unit
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN ) PORT_NAME("Console Down")
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_NAME("Console Left")
	PORT_BIT( 0x0200, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_NAME("Console Right")
	PORT_BIT( 0x0400, IP_ACTIVE_LOW, IPT_JOYSTICK_UP ) PORT_NAME("Console Up")
	PORT_BIT( 0xf800, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )
INPUT_PORTS_END

static INPUT_PORTS_START( rad_fb2 ) // controls must be multiplexed somehow, as there's no room for P2 controls otherwise (unless P2 controls were never finished and it was only sold in a single mat version, Radica left useless P2 menu options in the mini Genesis consoles)
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_JOYSTICK_LEFT ) PORT_PLAYER(1) // 'left'
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON1 ) PORT_PLAYER(1) // 'up'
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(1) // 'right'
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON2 ) PORT_PLAYER(1) // acts a 'motion ball' in menu (this is an analog input from the ball tho? at least in rad_fb in xavix.cpp so this might just be a debug input here)
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_JOYSTICK_RIGHT ) PORT_PLAYER(2) // 'p2 right'
	// none of the remaining inputs seem to do anything
	PORT_BIT( 0xffe0, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_CUSTOM ) // NTSC (1) / PAL (0) flag
INPUT_PORTS_END

static INPUT_PORTS_START( abltenni )
	PORT_START("P1") // Down and both buttons on startup for Diagnostics Menu
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 )
	// all remaining bits in this port will stop the demo mode, also having them high/low determines if you get 2P demos or Vs. CPU demos, not sure what the real state would be
	PORT_BIT( 0xffc0, IP_ACTIVE_HIGH, IPT_UNKNOWN )

	PORT_START("P2")
	PORT_DIPNAME( 0x0001, 0x0001, "P2" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, "Used" )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )

	PORT_START("P3")
	PORT_DIPNAME( 0x0001, 0x0001, "P3" )
	PORT_DIPSETTING(      0x0001, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0002, 0x0002, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0002, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0004, 0x0004, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0004, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0008, 0x0008, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0008, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0010, 0x0010, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0010, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0020, 0x0020, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0020, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0040, 0x0040, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0040, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0080, 0x0080, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0080, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0100, 0x0100, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0100, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0200, 0x0200, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0200, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0400, 0x0400, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0400, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x0800, 0x0800, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x0800, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x1000, 0x1000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x1000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x2000, 0x2000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x2000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x4000, 0x4000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x4000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
	PORT_DIPNAME( 0x8000, 0x8000, DEF_STR( Unknown ) )
	PORT_DIPSETTING(      0x8000, DEF_STR( Off ) )
	PORT_DIPSETTING(      0x0000, DEF_STR( On ) )
INPUT_PORTS_END

static INPUT_PORTS_START( guitarfv )
	PORT_START("P1")  // Button 1 + 2 and start for service mode
	PORT_BIT( 0x0001, IP_ACTIVE_LOW, IPT_BUTTON1 )
	PORT_BIT( 0x0002, IP_ACTIVE_LOW, IPT_BUTTON2 )
	PORT_BIT( 0x0004, IP_ACTIVE_LOW, IPT_BUTTON3 )
	PORT_BIT( 0x0008, IP_ACTIVE_LOW, IPT_BUTTON4 )
	PORT_BIT( 0x0010, IP_ACTIVE_LOW, IPT_BUTTON5 )
	PORT_BIT( 0x0020, IP_ACTIVE_LOW, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0040, IP_ACTIVE_LOW, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0080, IP_ACTIVE_LOW, IPT_BUTTON6 ) PORT_NAME("Wheel / Whammy") // 'Wheel' in test mode
	PORT_BIT( 0x0100, IP_ACTIVE_LOW, IPT_START1 )
	PORT_BIT( 0xfe00, IP_ACTIVE_LOW, IPT_UNKNOWN ) // unused?

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN ) // unused?

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_LOW, IPT_UNKNOWN ) // unused?
INPUT_PORTS_END

static INPUT_PORTS_START( guitarss )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_BUTTON1 ) PORT_NAME("B1: Blue / Up")
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("B2: Yellow / Down")
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("B3: Purple")
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("B4: Red")
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON5 ) PORT_NAME("B5: Green")
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP ) PORT_NAME("Strum / Select")
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN ) // unused? no Strum down?
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_BUTTON6 ) PORT_NAME("Whammy")
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_BUTTON7 ) PORT_NAME("Start / Select") // pause
	PORT_BIT( 0xfe00, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // unused?

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // unused?

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNKNOWN ) // unused?

INPUT_PORTS_END

static INPUT_PORTS_START( jjstrip )
	PORT_START("P1") // active LOW or HIGH?
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // on pad but not used?
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_SELECT )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_START )
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( pballpup )
	PORT_START("GUNY")
	PORT_BIT(0x0ff, 0x80, IPT_LIGHTGUN_Y) PORT_CROSSHAIR(Y, 256.0f / 240.0f, 0.0, 0) PORT_MINMAX(0x000, 0x0ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("GUNX")
	PORT_BIT(0x1ff, 0x100, IPT_LIGHTGUN_X) PORT_CROSSHAIR(X, 512.0f / 320.0f, -0.03f, 0) PORT_MINMAX(0x000, 0x1ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("P1")
	PORT_BIT( 0x003f, IP_ACTIVE_HIGH, IPT_UNUSED ) // lower bits are seeprom
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0xfc00, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0x000f, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON4 ) // pause
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // trigger
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // hide
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_BUTTON3 ) // reload
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( swclone )
	PORT_START("GUNY")
	PORT_BIT(0x0ff, 0x80, IPT_LIGHTGUN_Y) PORT_CROSSHAIR(Y, 1.0f, 0.0, 0) PORT_MINMAX(0x000, 0x0ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("GUNX")
	PORT_BIT(0x1ff, 0x100, IPT_LIGHTGUN_X) PORT_CROSSHAIR(X, 512.0f / 320.0f, -0.105f, 0) PORT_MINMAX(0x000, 0x1ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_UNUSED ) // i2cmem here
	PORT_BIT( 0x003e, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0200, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0xfc00, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0x000f, IP_ACTIVE_HIGH, IPT_UNUSED )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON4 ) // pause
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // trigger
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON2 ) // grenade
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_BUTTON3 ) // reload (doesn't exist here?)
	PORT_BIT( 0xff00, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( tmntmutm )
	PORT_START("GUNY")
	PORT_BIT(0x0ff, 0x80, IPT_LIGHTGUN_Y) PORT_CROSSHAIR(Y, 256.0f / 240.0f, -0.032f, 0) PORT_MINMAX(0x000, 0x0ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("GUNX")
	PORT_BIT(0x1ff, 0x100, IPT_LIGHTGUN_X) PORT_CROSSHAIR(X, 512.0f / 320.0f, -0.127f, 0) PORT_MINMAX(0x000, 0x1ff) PORT_SENSITIVITY(100) PORT_KEYDELTA(10) PORT_PLAYER(1)

	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_BUTTON1 ) // trigger
	PORT_BIT( 0xfffe, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END

static INPUT_PORTS_START( dreamlss )
	PORT_START("P1")
	PORT_BIT( 0x0001, IP_ACTIVE_HIGH, IPT_JOYSTICK_LEFT )
	PORT_BIT( 0x0002, IP_ACTIVE_HIGH, IPT_JOYSTICK_RIGHT )
	PORT_BIT( 0x0004, IP_ACTIVE_HIGH, IPT_JOYSTICK_UP )
	PORT_BIT( 0x0008, IP_ACTIVE_HIGH, IPT_JOYSTICK_DOWN )
	PORT_BIT( 0x0010, IP_ACTIVE_HIGH, IPT_BUTTON1 )
	PORT_BIT( 0x0020, IP_ACTIVE_HIGH, IPT_BUTTON2 ) PORT_NAME("A")
	PORT_BIT( 0x0040, IP_ACTIVE_HIGH, IPT_BUTTON3 ) PORT_NAME("B")
	PORT_BIT( 0x0080, IP_ACTIVE_HIGH, IPT_BUTTON4 ) PORT_NAME("C")
	PORT_BIT( 0x0100, IP_ACTIVE_HIGH, IPT_SERVICE ) PORT_NAME("Test (Debug)") // not externally connected on unit
	PORT_BIT( 0xfe00, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P2")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )

	PORT_START("P3")
	PORT_BIT( 0xffff, IP_ACTIVE_HIGH, IPT_UNUSED )
INPUT_PORTS_END


void spg2xx_game_state::machine_start()
{
	if (m_bank)
	{
		m_bank->configure_entries(0, (memregion("maincpu")->bytes() + 0x7fffff) / 0x800000, memregion("maincpu")->base(), 0x800000);
		m_bank->set_entry(0);
	}

	save_item(NAME(m_current_bank));
}

void spg2xx_game_state::machine_reset()
{
	m_current_bank = -1;
	switch_bank(0);
	m_maincpu->reset();
}

void spg2xx_game_state::spg2xx_base(machine_config &config)
{
	m_maincpu->porta_out().set(FUNC(spg2xx_game_state::porta_w));
	m_maincpu->portb_out().set(FUNC(spg2xx_game_state::portb_w));
	m_maincpu->portc_out().set(FUNC(spg2xx_game_state::portc_w));

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_size(320, 262);
	m_screen->set_visarea(0, 320-1, 0, 240-1);
	m_screen->set_screen_update("maincpu", FUNC(spg2xx_device::screen_update));
	m_screen->screen_vblank().set(m_maincpu, FUNC(spg2xx_device::vblank));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
	m_maincpu->add_route(ALL_OUTPUTS, "lspeaker", 0.5);
	m_maincpu->add_route(ALL_OUTPUTS, "rspeaker", 0.5);
}

void spg2xx_game_state::non_spg_base(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_4m);

	spg2xx_base(config);
}

void spg2xx_game_state::rad_skat(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set_ioport("P1");
	m_maincpu->portb_in().set_ioport("P2");
	m_maincpu->portc_in().set_ioport("P3");
	m_maincpu->i2c_w().set(FUNC(spg2xx_game_state::i2c_w));
	m_maincpu->i2c_r().set(FUNC(spg2xx_game_state::i2c_r));
}

void spg2xx_game_state::abltenni(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set_ioport("P1");
	m_maincpu->portb_in().set_ioport("P2");
	m_maincpu->portc_in().set_ioport("P3");
}

void spg2xx_game_state::guitarfv(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->set_pal(true);
	m_screen->set_refresh_hz(50);

	m_maincpu->porta_in().set_ioport("P1");
	m_maincpu->portb_in().set_ioport("P2");
	m_maincpu->portc_in().set_ioport("P3");
}


void spg2xx_game_state::tvsprt10(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_2m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(spg2xx_game_state::base_porta_r));
	m_maincpu->portb_in().set(FUNC(spg2xx_game_state::base_portb_r));
	m_maincpu->portc_in().set(FUNC(spg2xx_game_state::base_portc_r));
}

READ16_MEMBER(spg2xx_game_tmntmutm_state::guny_r)
{
	int frame = m_screen->frame_number() & 1; // game will not register shots if the co-ordinates are exactly the same as previous shot
	uint16_t data = m_io_guny->read() ^ frame;
	logerror("%s: Gun Y Read: %04x\n", machine().describe_context(), data);
	return data;
}

READ16_MEMBER(spg2xx_game_tmntmutm_state::gunx_r)
{
	int frame = (m_screen->frame_number() >> 1) & 1;
	uint16_t data = m_io_gunx->read() ^ frame;
	logerror("%s: Gun X Read: %04x\n", machine().describe_context(), data);
	return data;
}



void spg2xx_game_tmntmutm_state::tmntmutm(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_tmntmutm_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(spg2xx_game_tmntmutm_state::base_porta_r));
	m_maincpu->portb_in().set(FUNC(spg2xx_game_tmntmutm_state::base_portb_r));
	m_maincpu->portc_in().set(FUNC(spg2xx_game_tmntmutm_state::base_portc_r));

	m_maincpu->guny_in().set(FUNC(spg2xx_game_tmntmutm_state::guny_r));
	m_maincpu->gunx_in().set(FUNC(spg2xx_game_tmntmutm_state::gunx_r));

	I2C_24C08(config, "i2cmem", 0);
}

READ16_MEMBER(spg2xx_game_pballpup_state::porta_r)
{
	uint16_t ret = m_io_p1->read() & 0xfff7;
//  logerror("%s: spg2xx_game_pballpup_state::porta_r\n", machine().describe_context());
	ret |= m_eeprom->do_read() ? 0x8 : 0x0;
	return ret;
}

WRITE16_MEMBER(spg2xx_game_pballpup_state::porta_w)
{
	logerror("%s: spg2xx_game_pballpup_state::porta_w (%04x)\n", machine().describe_context(), data);
	m_eeprom->di_write(BIT(data, 2));
	m_eeprom->cs_write(BIT(data, 0) ? ASSERT_LINE : CLEAR_LINE);
	m_eeprom->clk_write(BIT(data, 1) ? ASSERT_LINE : CLEAR_LINE);

	// this can actually change bank from running code, because the code part in each bank is almost identical, just the data changes
	switch_bank((data & 0x1000) ? 1 : 0);
}


void spg2xx_game_pballpup_state::pballpup(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_pballpup_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(spg2xx_game_pballpup_state::porta_r));
	m_maincpu->portb_in().set(FUNC(spg2xx_game_pballpup_state::base_portb_r));
	m_maincpu->portc_in().set(FUNC(spg2xx_game_pballpup_state::base_portc_r));

	m_maincpu->porta_out().set(FUNC(spg2xx_game_pballpup_state::porta_w));
	m_maincpu->portb_out().set(FUNC(spg2xx_game_pballpup_state::portb_w));
	m_maincpu->portc_out().set(FUNC(spg2xx_game_pballpup_state::portc_w));

	m_maincpu->guny_in().set(FUNC(spg2xx_game_pballpup_state::base_guny_r));
	m_maincpu->gunx_in().set(FUNC(spg2xx_game_pballpup_state::base_gunx_r));

	EEPROM_93C66_16BIT(config, m_eeprom); // type?
}

READ16_MEMBER(spg2xx_game_swclone_state::porta_r)
{
	uint16_t ret = m_io_p1->read() & 0xfffe;
	ret |= m_i2cmem->read_sda() ? 0x1: 0x0;

	//logerror("%s: spg2xx_game_swclone_state::porta_r (%04x)\n", machine().describe_context(), ret);
	return ret;
}

WRITE16_MEMBER(spg2xx_game_swclone_state::porta_w)
{
	//logerror("%s: spg2xx_game_swclone_state::porta_w (%04x & %04x)\n", machine().describe_context(), data, mem_mask);

	if (BIT(mem_mask, 1))
		m_i2cmem->write_scl(BIT(data, 1));
	if (BIT(mem_mask, 0))
		m_i2cmem->write_sda(BIT(data, 0));

	m_porta_data = data;
}


void spg2xx_game_swclone_state::swclone(machine_config &config)
{
	SPG2XX_128(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_swclone_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(spg2xx_game_swclone_state::porta_r));
	m_maincpu->portb_in().set(FUNC(spg2xx_game_swclone_state::base_portb_r));
	m_maincpu->portc_in().set(FUNC(spg2xx_game_swclone_state::base_portc_r));

	m_maincpu->porta_out().set(FUNC(spg2xx_game_swclone_state::porta_w));
	m_maincpu->portb_out().set(FUNC(spg2xx_game_swclone_state::portb_w));
	m_maincpu->portc_out().set(FUNC(spg2xx_game_swclone_state::portc_w));

	m_maincpu->guny_in().set(FUNC(spg2xx_game_swclone_state::base_guny_r));
	m_maincpu->gunx_in().set(FUNC(spg2xx_game_swclone_state::base_gunx_r));

	I2C_24C08(config, "i2cmem", 0);
}



READ16_MEMBER(spg2xx_game_dreamlss_state::porta_r)
{
	uint16_t ret = m_io_p1->read()&0xefff;
	ret |= m_porta_data & 0x1000; // needs to be able to read back current bank
	logerror("%s: spg2xx_game_dreamlss_state::porta_r\n", machine().describe_context());
	return ret;
}

WRITE16_MEMBER(spg2xx_game_dreamlss_state::porta_w)
{
	logerror("%s: spg2xx_game_dreamlss_state::porta_w (%04x)\n", machine().describe_context(), data);

	m_porta_data = data;

	switch_bank((data & 0x1000) ? 1 : 0);
}


// TODO: how does the SEEPROM hook up? (will need hack removing in init_dreamlss )
READ16_MEMBER(spg2xx_game_dreamlss_state::portb_r)
{
	uint16_t ret = m_portb_data & 0xfffe;
	ret |= m_i2cmem->read_sda() ? 0x1: 0x0;

	//logerror("%s: spg2xx_game_dreamlss_state::portb_r (%04x)\n", machine().describe_context(), ret);
	return ret;
}

WRITE16_MEMBER(spg2xx_game_dreamlss_state::portb_w)
{
	//logerror("%s: spg2xx_game_dreamlss_state::portb_w (%04x & %04x)\n", machine().describe_context(), data, mem_mask);

	if (BIT(mem_mask, 1))
		m_i2cmem->write_scl(BIT(data, 1));
	if (BIT(mem_mask, 0))
		m_i2cmem->write_sda(BIT(data, 0));

	m_portb_data = data;
}


void spg2xx_game_dreamlss_state::dreamlss(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_dreamlss_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(spg2xx_game_dreamlss_state::porta_r));
	m_maincpu->portb_in().set(FUNC(spg2xx_game_dreamlss_state::portb_r));
	m_maincpu->portc_in().set(FUNC(spg2xx_game_dreamlss_state::base_portc_r));

	m_maincpu->porta_out().set(FUNC(spg2xx_game_dreamlss_state::porta_w));
	m_maincpu->portb_out().set(FUNC(spg2xx_game_dreamlss_state::portb_w));
	m_maincpu->portc_out().set(FUNC(spg2xx_game_dreamlss_state::portc_w));

	I2C_24C08(config, "i2cmem", 0);
}



void spg2xx_game_state::rad_skatp(machine_config &config)
{
	rad_skat(config);
	m_maincpu->set_pal(true);
	m_screen->set_refresh_hz(50);
//  m_screen->set_size(320, 312);
}

void spg2xx_game_state::rad_sktv(machine_config &config)
{
	SPG24X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set(FUNC(spg2xx_game_state::base_porta_r));
	m_maincpu->portb_in().set(FUNC(spg2xx_game_state::base_portb_r));
	m_maincpu->portc_in().set(FUNC(spg2xx_game_state::base_portc_r));
	m_maincpu->i2c_w().set(FUNC(spg2xx_game_state::i2c_w));
	m_maincpu->i2c_r().set(FUNC(spg2xx_game_state::i2c_r));
}

void spg2xx_game_state::rad_crik(machine_config &config)
{
	SPG28X(config, m_maincpu, XTAL(27'000'000), m_screen);
	m_maincpu->set_addrmap(AS_PROGRAM, &spg2xx_game_state::mem_map_4m);

	spg2xx_base(config);

	m_maincpu->porta_in().set_ioport("P1");
	m_maincpu->portb_in().set_ioport("P2");
	m_maincpu->portc_in().set_ioport("P3");
	m_maincpu->i2c_w().set(FUNC(spg2xx_game_state::i2c_w));
	m_maincpu->i2c_r().set(FUNC(spg2xx_game_state::i2c_r));
}

ROM_START( rad_skat )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "skateboarder.bin", 0x000000, 0x400000, CRC(08b9ab91) SHA1(6665edc4740804956136c68065890925a144626b) )
ROM_END

ROM_START( rad_skatp ) // rom was dumped from the NTSC version, but region comes from an io port, so ROM is probably the same
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "skateboarder.bin", 0x000000, 0x400000, CRC(08b9ab91) SHA1(6665edc4740804956136c68065890925a144626b) )
ROM_END

ROM_START( rad_sktv )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "skannerztv.bin", 0x000000, 0x200000, CRC(e92278e3) SHA1(eb6bee5e661128d83784960dfff50379c36bfaeb) )

	/* The external scanner MCU is a Winbond from 2000: SA5641
	   the scanner plays sound effects when scanning, without being connected to the main unit, so a way to dump / emulate
	   this MCU is also needed for complete emulation

	   TODO: find details on MCU so that we know capacity etc. */
ROM_END

ROM_START( rad_fb2 )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "football2.bin", 0x000000, 0x400000, CRC(96b4f0d2) SHA1(e91f2ac679fb0c026ffe216eb4ab58802f361a17) )
ROM_END

ROM_START( rad_crik ) // only released in EU?
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "cricket.bin", 0x000000, 0x200000, CRC(6fa0aaa9) SHA1(210d2d4f542181f59127ce2f516d0408dc6de7a8) )
ROM_END

ROM_START( mattelcs )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mattelclassicsports.bin", 0x000000, 0x100000, CRC(e633e7ad) SHA1(bf3e325a930cf645a7e32195939f3c79c6d35dac) )
ROM_END

ROM_START( abltenni )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "ablpnpwirelesstennis.bin", 0x000000, 0x400000, CRC(66bd8ef1) SHA1(a83640d5d9e84e10d29a065a61e0d7bbec16c6e4) )
ROM_END

ROM_START( tvsprt10 )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "tvsports10in1.bin", 0x000000, 0x400000, CRC(98b79889) SHA1(b0ba534d59b794bb38c071c70ab5bcf711364e06) )
ROM_END

ROM_START( decathln )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "decathlon.bin", 0x000000, 0x400000, CRC(63c8e6b6) SHA1(6a25b68b45336e04a2bfd75b43a494349024d714) )
ROM_END


ROM_START( wiwi18 )
	ROM_REGION( 0x1000000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "26gl128.bin", 0x000000, 0x1000000, CRC(0b103ac9) SHA1(14434908f429942096fb8db5b5630603fd54fb2c) )
ROM_END

ROM_START( guitarfv )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "mx26l64.bin", 0x000000, 0x800000, CRC(eaadd2c2) SHA1(0c3fe004dbaa52a335c6ddcecb9e9f5582d7ef35) )
ROM_END

ROM_START( guitarss )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "guitar_superstar_stratocaster.bin", 0x000000, 0x800000, CRC(63950016) SHA1(28b9613571f47c49995aa35c4d4a2d6f68389813) )
ROM_END


ROM_START( jjstrip )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "strippoker.bin", 0x000000, 0x200000, CRC(7a70e6c8) SHA1(3d5da4774b00977939f309f3e71473dde9b70435) )
ROM_END

ROM_START( tmntmutm )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "tmntmutantsandmonsters_sst39vf3201_00bf235b.bin", 0x000000, 0x400000, CRC(93ab5ff7) SHA1(e78a5d380663d351ad9be5087ec8434d9be16ba7) )
ROM_END

ROM_START( pballpup )
	ROM_REGION( 0x1000000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "paintballpoweredup.bin", 0x000000, 0x1000000, CRC(57dbdfd1) SHA1(d98cb7321cc7af092f6f4f83e85fabbdbc1bbd95) )

	ROM_REGION16_BE( 0x200, "eeprom", ROMREGION_ERASE00 )
	// ensure eeprom defaults to 00 or there are unwanted invalid entries already saved
ROM_END

ROM_START( dreamlss )
	ROM_REGION( 0x1000000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "dreamlifesuperstar.bin", 0x000000, 0x1000000, CRC(aefad2c3) SHA1(cf962082f09e27d7d24cfc722ae978d9aa735a57) )
ROM_END

ROM_START( swclone )
	ROM_REGION( 0x800000, "maincpu", ROMREGION_ERASE00 )
	ROM_LOAD16_WORD_SWAP( "swclone.bin", 0x000000, 0x800000, CRC(2c983509) SHA1(6138f21fe0b82a7121c4639b6833d4014d5aeb74) )

	ROM_REGION( 0x400, "i2cmem", ROMREGION_ERASE00 )
	// ensure eeprom defaults to 00 or there are unwanted invalid entries already saved
ROM_END


void spg2xx_game_state::init_crc()
{
	// several games have a byte sum checksum listed at the start of ROM, this little helper function logs what it should match.
	const int length = memregion("maincpu")->bytes();
	const uint8_t* rom = memregion("maincpu")->base();

	uint32_t checksum = 0x00000000;
	// the first 0x10 bytes are where the "chksum:xxxxxxxx " string is listed, so skip over them
	for (int i = 0x10; i < length; i++)
	{
		checksum += rom[i];
	}

	logerror("Calculated Byte Sum of bytes from 0x10 to 0x%08x is %08x)\n", length - 1, checksum);
}

void spg2xx_game_state::init_wiwi18()
{
	uint16_t* rom = (uint16_t*)memregion("maincpu")->base();
	rom[0x1ca259] = 0xf165;
}

void spg2xx_game_state::init_tvsprt10()
{
	uint16_t* rom = (uint16_t*)memregion("maincpu")->base();
	// hack 'MASK' check (some kind of EEPROM maybe?)
	// this hack means the ROM CRC fails, but without it the CRC is OK, so dump is good.
	/*
	port b 0010 = chip select? / always set when accessing?
	       0008 = write enable (set when writing, not when reading)
	       0004 = chip select? / always set when accessing?
	       0002 = clock? (toggles)
	       0001 = data in / out
	*/
	if (rom[0x18c55d] == 0x9240) rom[0x18c55d] ^= 0x0001; // tvsprt10
	if (rom[0x179911] == 0x9240) rom[0x179911] ^= 0x0001; // decathln
}

void spg2xx_game_swclone_state::init_swclone()
{
	uint16_t* rom = (uint16_t*)memregion("maincpu")->base();
	rom[0x2649d1] = 0x0000; // don't write 0x1234 to the start of the RAM that is copied to spriteram on startup (this is an explicit write, probably actually a failure condition for something?)
}


// year, name, parent, compat, machine, input, class, init, company, fullname, flags

// Radica TV games
CONS( 2006, rad_skat,  0,        0, rad_skat, rad_skat,   spg2xx_game_state, init_crc, "Radica", "Play TV Skateboarder (NTSC)", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )
CONS( 2006, rad_skatp, rad_skat, 0, rad_skatp,rad_skatp,  spg2xx_game_state, init_crc, "Radica", "Connectv Skateboarder (PAL)", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )
CONS( 2006, rad_crik,  0,        0, rad_crik, rad_crik,   spg2xx_game_state, init_crc, "Radica", "Connectv Cricket (PAL)",      MACHINE_IMPERFECT_SOUND | MACHINE_NOT_WORKING ) // Version 3.00 20/03/06 is listed in INTERNAL TEST
CONS( 2007, rad_sktv,  0,        0, rad_sktv, rad_sktv,   spg2xx_game_state, init_crc, "Radica", "Skannerz TV",                 MACHINE_IMPERFECT_SOUND | MACHINE_NOT_WORKING )
CONS( 2007, rad_fb2,   0,        0, rad_skat, rad_fb2,    spg2xx_game_state, init_crc, "Radica", "Play TV Football 2",          MACHINE_IMPERFECT_SOUND | MACHINE_NOT_WORKING ) // offers a 2 player option in menus, but seems to have only been programmed for, and released as, a single player unit, P2 controls appear unfinished.

// ABL TV Games
CONS( 2006, abltenni,    0,     0,        abltenni,       abltenni,    spg2xx_game_state, empty_init, "Advance Bright Ltd / V-Tac Technology Co Ltd.", "Wireless Tennis (WT2000, ABL TV Game)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

// same as Excalibur Decathlon? not the same as the ABL game
CONS( 2006, tvsprt10,    0,     0,        tvsprt10,       tvsprt10,    spg2xx_game_state, init_tvsprt10, "Simba / V-Tac Technology Co Ltd.",              "TV Sports 10-in-1 / Decathlon Athletic Sport Games", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )
CONS( 200?, decathln,    0,     0,        tvsprt10,       decathln,    spg2xx_game_state, init_tvsprt10, "Advance Bright Ltd / V-Tac Technology Co Ltd.", "Decathlon", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS ) // unit found in Spain

CONS( 2007, guitarfv,    0,     0,        guitarfv,       guitarfv,    spg2xx_game_state, empty_init, "Advance Bright Ltd", "Guitar Fever (2007.07.03 Ver 2.7)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

// Another version of this has a red "Gibson Flying V" style controller.  The difference is reflected on the title screen, songs are the same.
CONS( 200?, guitarss,    0,     0,        abltenni,       guitarss,    spg2xx_game_state, empty_init, "Senario", "Guitar Super Star (Fender Stratocaster style)", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

CONS( 200?, jjstrip,    0,     0,        tvsprt10,       jjstrip,    spg2xx_game_state, empty_init, "Shiggles Inc.", "Club Jenna Presents: Jenna Jameson's Strip Poker", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

CONS( 2005, tmntmutm,   0,     0,        tmntmutm,       tmntmutm,    spg2xx_game_tmntmutm_state, empty_init, "Tech2Go / WayForward", "Teenage Mutant Ninja Turtles: Mutant and Monster Mayhem", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

CONS( 2006, pballpup,   0,     0,        pballpup,       pballpup,    spg2xx_game_pballpup_state, empty_init, "Hasbro / Tiger Electronics", "Mission: Paintball Powered Up", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

CONS( 2007, dreamlss,   0,     0,        dreamlss,       dreamlss,    spg2xx_game_dreamlss_state, empty_init, "Hasbro / Tiger Electronics", "Dream Life Superstar (Version 0.3, Mar 16 2007)", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )


// meeds a hack to not show garbage sprite on startup, seeprom hookup (24LC08), gun fine-tuning etc.
CONS( 2008, swclone,   0,     0,        swclone,       swclone,    spg2xx_game_swclone_state, init_swclone, "Hasbro / Tiger Electronics", "Star Wars - The Clone Wars", MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )

// Mattel games
CONS( 2005, mattelcs,  0,        0, rad_skat, mattelcs,   spg2xx_game_state, empty_init, "Mattel", "Mattel Classic Sports",     MACHINE_IMPERFECT_SOUND )

// box marked 'Wireless game console' 'Drahtlose Spielekonsole' 87 Sports games included : 18 hyper sports games, 69 arcade games.
// Unit marked 'Hamy System' 'WiWi'
// actually a cartridge, but all hardware is in the cart, overriding any internal hardware entirely.  see nes_vt.cp 'mc_sp69' for the '69 arcade game' part
CONS( 200?, wiwi18,  0,        0, rad_skat, wiwi18,  spg2xx_game_state, init_wiwi18, "Hamy System",      "WiWi 18-in-1 Sports Game", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND | MACHINE_IMPERFECT_GRAPHICS )
