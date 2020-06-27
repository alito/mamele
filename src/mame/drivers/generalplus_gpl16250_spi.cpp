// license:BSD-3-Clause
// copyright-holders:David Haywood
/*
    GPL16250 / GPAC800 / GMC384 / GCM420 related support

    GPL16250 is the GeneralPlus / SunPlus part number
    GPAC800 is the JAKKS Pacific codename
    GMC384 / GCM420 is what is printed on the die

    ----

    GPL16250 games using SPI Flash + RAM configuration
*/

#include "emu.h"
#include "includes/generalplus_gpl16250.h"


class generalplus_gpspispi_game_state : public gcm394_game_state
{
public:
	generalplus_gpspispi_game_state(const machine_config& mconfig, device_type type, const char* tag) :
		gcm394_game_state(mconfig, type, tag)
	{
	}

	void generalplus_gpspispi(machine_config &config);

	void init_spi();

protected:
	virtual void machine_start() override;
	virtual void machine_reset() override;

private:
};



class generalplus_gpspispi_bkrankp_game_state : public generalplus_gpspispi_game_state
{
public:
	generalplus_gpspispi_bkrankp_game_state(const machine_config& mconfig, device_type type, const char* tag) :
		generalplus_gpspispi_game_state(mconfig, type, tag),
		m_cart(*this, "cartslot")
	{
	}

	void generalplus_gpspispi_bkrankp(machine_config &config);

protected:
	required_device<generic_slot_device> m_cart;
	DECLARE_DEVICE_IMAGE_LOAD_MEMBER(cart_load);

private:
};


void generalplus_gpspispi_game_state::machine_start()
{
}

void generalplus_gpspispi_game_state::machine_reset()
{
	m_maincpu->reset(); // reset CPU so vector gets read etc.

	//m_maincpu->set_paldisplaybank_high_hack(0);
	m_maincpu->set_alt_tile_addressing_hack(1);
}

static INPUT_PORTS_START( gcm394 )
	PORT_START("IN0")
	PORT_START("IN1")
	PORT_START("IN2")
INPUT_PORTS_END


void generalplus_gpspispi_game_state::generalplus_gpspispi(machine_config &config)
{
	GP_SPISPI(config, m_maincpu, 96000000/2, m_screen);
	m_maincpu->porta_in().set(FUNC(generalplus_gpspispi_game_state::porta_r));
	m_maincpu->portb_in().set(FUNC(generalplus_gpspispi_game_state::portb_r));
	m_maincpu->portc_in().set(FUNC(generalplus_gpspispi_game_state::portc_r));
	m_maincpu->porta_out().set(FUNC(generalplus_gpspispi_game_state::porta_w));
	m_maincpu->space_read_callback().set(FUNC(generalplus_gpspispi_game_state::read_external_space));
	m_maincpu->space_write_callback().set(FUNC(generalplus_gpspispi_game_state::write_external_space));
	m_maincpu->set_irq_acknowledge_callback(m_maincpu, FUNC(sunplus_gcm394_base_device::irq_vector_cb));
	m_maincpu->add_route(ALL_OUTPUTS, "lspeaker", 0.5);
	m_maincpu->add_route(ALL_OUTPUTS, "rspeaker", 0.5);
	m_maincpu->set_bootmode(0); // boot from internal ROM (SPI bootstrap)
	m_maincpu->set_cs_config_callback(FUNC(gcm394_game_state::cs_callback));

	FULL_MEMORY(config, m_memory).set_map(&generalplus_gpspispi_game_state::cs_map_base);

	SCREEN(config, m_screen, SCREEN_TYPE_RASTER);
	m_screen->set_refresh_hz(60);
	m_screen->set_size(320*2, 262*2);
	m_screen->set_visarea(0, (320*2)-1, 0, (240*2)-1);
	m_screen->set_screen_update("maincpu", FUNC(sunplus_gcm394_device::screen_update));
	m_screen->screen_vblank().set(m_maincpu, FUNC(sunplus_gcm394_device::vblank));

	SPEAKER(config, "lspeaker").front_left();
	SPEAKER(config, "rspeaker").front_right();
}

DEVICE_IMAGE_LOAD_MEMBER(generalplus_gpspispi_bkrankp_game_state::cart_load)
{
	uint32_t size = m_cart->common_get_size("rom");

	m_cart->rom_alloc(size, GENERIC_ROM16_WIDTH, ENDIANNESS_LITTLE);
	m_cart->common_load_rom(m_cart->get_rom_base(), size, "rom");

	return image_init_result::PASS;
}

void generalplus_gpspispi_bkrankp_game_state::generalplus_gpspispi_bkrankp(machine_config &config)
{
	generalplus_gpspispi_game_state::generalplus_gpspispi(config);

	GENERIC_CARTSLOT(config, m_cart, generic_plain_slot, "bkrankp_cart");
	m_cart->set_width(GENERIC_ROM16_WIDTH);
	m_cart->set_device_load(FUNC(generalplus_gpspispi_bkrankp_game_state::cart_load));

	SOFTWARE_LIST(config, "cart_list").set_original("bkrankp_cart");
}



ROM_START( bkrankp )
	ROM_REGION16_BE( 0x40000, "maincpu:internal", ROMREGION_ERASE00 )
	//ROM_LOAD16_WORD_SWAP( "internal.rom", 0x00000, 0x40000, NO_DUMP ) // used as bootstrap only

	ROM_REGION(0x400000, "maincpu", ROMREGION_ERASE00)
	ROM_LOAD16_WORD_SWAP( "unit_mx25l3206e_c22016.bin", 0x0000, 0x400000, CRC(7efad116) SHA1(427d707e97586ae6ab5fe08f29ca450ddc7ad36e) )
ROM_END


void generalplus_gpspispi_game_state::init_spi()
{
	int vectorbase = 0x2fe0;
	uint8_t* spirom = memregion("maincpu")->base();

	address_space& mem = m_maincpu->space(AS_PROGRAM);

	/*  Offset(h) 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F

	    00000000  50 47 70 73 73 69 69 70 70 73 44 00 44 3F 44 00  PGpssiipps
	    00000010  -- -- -- -- -- bb -- -- -- -- -- -- -- -- -- --
	                             ^^ copy dest, just like with nand type

	    bb = where to copy first block

	    The header is GPspispi (byteswapped) then some params
	    one of the params appears to be for the initial code copy operation done
	    by the bootstrap
	*/

	// probably more bytes are used
	int dest = spirom[0x15] << 8;

	// copy a block of code from the NAND to RAM
	for (int i = 0; i < 0x2000; i++)
	{
		uint16_t word = spirom[(i * 2) + 0] | (spirom[(i * 2) + 1] << 8);

		mem.write_word(dest + i, word);
	}

	// these vectors must either directly point to RAM, or at least redirect there after some code
	uint16_t* internal = (uint16_t*)memregion("maincpu:internal")->base();
	internal[0x7ff5] = vectorbase + 0x0a;
	internal[0x7ff6] = vectorbase + 0x0c;
	internal[0x7ff7] = dest + 0x20; // point boot vector at code in RAM (probably in reality points to internal code that copies the first block)
	internal[0x7ff8] = vectorbase + 0x10;
	internal[0x7ff9] = vectorbase + 0x12;
	internal[0x7ffa] = vectorbase + 0x14;
	internal[0x7ffb] = vectorbase + 0x16;
	internal[0x7ffc] = vectorbase + 0x18;
	internal[0x7ffd] = vectorbase + 0x1a;
	internal[0x7ffe] = vectorbase + 0x1c;
	internal[0x7fff] = vectorbase + 0x1e;
}


CONS(200?, bkrankp, 0, 0, generalplus_gpspispi_bkrankp, gcm394, generalplus_gpspispi_bkrankp_game_state , init_spi, "Bandai", "Karaoke Ranking Party (Japan)", MACHINE_NOT_WORKING | MACHINE_IMPERFECT_SOUND)
