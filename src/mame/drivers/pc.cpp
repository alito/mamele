// license:BSD-3-Clause
// copyright-holders:Wilbert Pol, Miodrag Milanovic
/*****************************************************************************

    drivers/pc.c

Driver file for IBM PC, IBM PC XT, and related machines.

    PC-XT memory map

    00000-9FFFF   RAM
    A0000-AFFFF   NOP       or videoram EGA/VGA
    B0000-B7FFF   videoram  MDA, page #0
    B8000-BFFFF   videoram  CGA and/or MDA page #1, T1T mapped RAM
    C0000-C7FFF   NOP       or ROM EGA/VGA
    C8000-C9FFF   ROM       XT HDC #1
    CA000-CBFFF   ROM       XT HDC #2
    D0000-EFFFF   NOP       or 'adapter RAM'
    F0000-FDFFF   NOP       or ROM Basic + other Extensions
    FE000-FFFFF   ROM

******************************************************************************/

#include "emu.h"
#include "machine/genpc.h"
#include "machine/i8251.h"
#include "cpu/i86/i86.h"
#include "cpu/nec/nec.h"
#include "bus/isa/isa.h"
#include "bus/isa/isa_cards.h"
#include "bus/pc_kbd/keyboards.h"
#include "softlist.h"

/******************************************************* Generic PC with CGA ***/


class pc_state : public driver_device
{
public:
	pc_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu")
	{ }

	void ataripc1(machine_config &config);
	void coppc400(machine_config &config);
	void ncrpc4i(machine_config &config);
	void kaypro16(machine_config &config);
	void kaypropc(machine_config &config);
	void m15(machine_config &config);
	void bondwell(machine_config &config);
	void siemens(machine_config &config);
	void iskr3104(machine_config &config);
	void poisk2(machine_config &config);
	void dgone(machine_config &config);
	void pccga(machine_config &config);
	void mk88(machine_config &config);
	void eppc(machine_config &config);
	void olystar20f(machine_config &config);
	void olytext30(machine_config &config);
	void zenith(machine_config &config);
	void eagle1600(machine_config &config);
	void laser_turbo_xt(machine_config &config);
	void ibm5550(machine_config &config);
	void comport(machine_config &config);
	void mpc1600(machine_config &config);
	void ittxtra(machine_config &config);
	void cadd810(machine_config &config);
	void juko16(machine_config &config);

	void init_bondwell();

	DECLARE_INPUT_CHANGED_MEMBER(pc_turbo_callback);

private:
	required_device<cpu_device> m_maincpu;

	DECLARE_READ8_MEMBER(unk_r);

	double m_turbo_off_speed;

	static void cfg_dual_720K(device_t *device);
	static void cfg_single_360K(device_t *device);
	static void cfg_single_720K(device_t *device);

	void ibm5550_io(address_map &map);
	void pc16_io(address_map &map);
	void pc16_map(address_map &map);
	void pc8_io(address_map &map);
	void pc8_map(address_map &map);
	void zenith_map(address_map &map);
};

void pc_state::pc8_map(address_map &map)
{
	map.unmap_value_high();
	map(0xf0000, 0xfffff).rom().region("bios", 0);
}

void pc_state::pc16_map(address_map &map)
{
	map.unmap_value_high();
	map(0xf0000, 0xfffff).rom().region("bios", 0);
}

void pc_state::pc8_io(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x00ff).m("mb", FUNC(ibm5160_mb_device::map));
}

void pc_state::pc16_io(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x00ff).m("mb", FUNC(ibm5160_mb_device::map));
	map(0x0070, 0x007f).ram(); // needed for Poisk-2
}

READ8_MEMBER(pc_state::unk_r)
{
	return 0;
}

INPUT_CHANGED_MEMBER(pc_state::pc_turbo_callback)
{
	m_maincpu->set_clock_scale((newval & 2) ? 1 : m_turbo_off_speed);
}



static INPUT_PORTS_START( pccga )
	PORT_START("DSW1") /* IN2 */
	PORT_DIPNAME( 0x80, 0x80, "COM1: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x80, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x40, 0x40, "COM2: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x40, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x20, 0x00, "COM3: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x20, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x10, 0x00, "COM4: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x10, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x08, 0x08, "LPT1: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x04, 0x00, "LPT2: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x02, 0x00, "LPT3: enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x02, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x01, 0x00, "Game port enable")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x01, DEF_STR( Yes ) )

	PORT_START("DSW2") /* IN3 */
	PORT_DIPNAME( 0x08, 0x08, "HDC1 (C800:0 port 320-323)")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x08, DEF_STR( Yes ) )
	PORT_DIPNAME( 0x04, 0x04, "HDC2 (CA00:0 port 324-327)")
	PORT_DIPSETTING(    0x00, DEF_STR( No ) )
	PORT_DIPSETTING(    0x04, DEF_STR( Yes ) )
	PORT_BIT( 0x02, 0x02,   IPT_UNUSED ) /* no turbo switch */
	PORT_BIT( 0x01, 0x01,   IPT_UNUSED )
INPUT_PORTS_END


static DEVICE_INPUT_DEFAULTS_START( pccga )
	DEVICE_INPUT_DEFAULTS("DSW0", 0x30, 0x20)
DEVICE_INPUT_DEFAULTS_END


// Floppy configurations
void pc_state::cfg_dual_720K(device_t *device)
{
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:0")).set_default_option("35dd");
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:1")).set_default_option("35dd");
}

void pc_state::cfg_single_360K(device_t *device)
{
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:0")).set_default_option("525dd");
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:0")).set_fixed(true);
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:1")).set_default_option(nullptr);
}

void pc_state::cfg_single_720K(device_t *device)
{
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:0")).set_default_option("35dd");
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:0")).set_fixed(true);
	dynamic_cast<device_slot_interface &>(*device->subdevice("fdc:1")).set_default_option(nullptr);
}

void pc_state::pccga(machine_config &config)
{
	/* basic machine hardware */
	i8088_cpu_device &maincpu(I8088(config, "maincpu", XTAL(14'318'181)/3)); /* 4.77 MHz */
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc8_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc8_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5160_mb_device &mb(IBM5160_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(pccga));

	// FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "cga", false);
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);
	ISA8_SLOT(config, "isa5", 0, "mb:isa", pc_isa8_cards, nullptr, false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");

	/* software lists */
	SOFTWARE_LIST(config, "disk_list").set_original("ibm5150");
}


/**************************************************************** Atari PC1 ***

Links:  http://www.ataripc.net/pc1-8088/ ; http://krap.pl/mirrorz/atari/www.atari-computermuseum.de/pc.htm ;
        http://www.atari-computermuseum.de/pc1.htm
Form Factor: Desktop
CPU: 8088 @ 4.77 MHz / 8 MHz
RAM: 512K / 640KB
Bus: ISA signals available on board, no slot
Video: Hercules/CGA/EGA
Mass storage: 1 5.25" 360K floppy
On board ports: floppy, graphics, parallel, serial, mouse, external floppy
Options: 8087 FPU
Expansion: Up to two external floppy drives: PCF554, SF314 or SF354

***************************************************************** Atari PC2 ***

Links:  http://www.binarydinosaurs.co.uk/Museum/atari/pc2.php ; http://www.ataripc.net/pc2-8088/ ;
        http://www.ataripc.net/components/
Info: The Atari PC2 mainboard has only one ISA slot, but is expanded via a four slot riser card. BIOS is identical to later PC1 and PC3
CPU: 8088 @ 4.77 MHz / 8 MHz
RAM: 512K / 640KB
Bus: 4x ISA
Video: Hercules/CGA/EGA
Mass storage: 1 5.25" 360K floppy and 1 5.25" 360K floppy or 20MB hard drive
On board ports: floppy, external floppy (Atari ST style), graphics, parallel, serial, mouse
Expansion: 8087 FPU

DIP switches:                      Sw.1  Sw.2  Sw.3  Sw.4
                EGA Monitor         OFF    ON   OFF   OFF
                Color Monitor        ON   OFF   OFF    ON
                Monochrome Monitor   ON   OFF    ON    ON

EGA.COM, CGA.COM, HGC.COM, MDA.COM, PALETTE.COM, HCOLOR.COM and CURSOR are utilities to change
the behavior of the integrated graphics card.

Turbo option: From DOS, commands "TURBO ON" and "TURRBO OFF or key combos [Ctrl][Alt][1] or
[Ctrl][Alt][+] for Turbo on, [Ctrl][Alt][2] or [Ctrl][Alt][-] for Turbo off
Keyboard click: From DOS, "CLICK ON" and "CLICK OFF" or key combos [Ctrl][Alt][<]
for click on, [Ctrl][Alt][>] for click off

******************************************************************************/

void pc_state::ataripc1(machine_config &config)
{
	pccga(config);
	subdevice<isa8_slot_device>("isa1")->set_default_option("ega");
	subdevice<isa8_slot_device>("isa2")->set_option_machine_config("fdc_xt", cfg_single_360K);
}

ROM_START ( ataripc1 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_SYSTEM_BIOS( 0, "v3.06", "v3.06" )
	ROMX_LOAD("award_atari_pc_bios_3.06.bin", 0x8000, 0x8000, CRC(256427ce) SHA1(999f6af64b79f88c1d3492f386d9bee08efb50e7), ROM_BIOS(0))
	ROM_SYSTEM_BIOS( 1, "v3.07", "v3.07" )
	ROMX_LOAD("award_atari_pc_bios_3.07.bin", 0x8000, 0x8000, CRC(a73b80e6) SHA1(03af5902cdfd1cde217022b823162f24aba435ab), ROM_BIOS(1))
	ROM_SYSTEM_BIOS( 2, "v3.08", "v3.08" )
	ROMX_LOAD("award_atari_pc_bios_3.08.bin", 0x8000, 0x8000, CRC(929a2443) SHA1(8e98f3c9180c55b1f5521727779c016083d27960), ROM_BIOS(2)) //same as on Atari PC3, also used on Atari PC2
ROM_END


/**************************************************************** Atari PC3 ***

Links:  http://www.atari-computermuseum.de/pc1.htm , http://trelohra.blogspot.de/2015/06/atari-pc3.html ,
        http://www.ataripc.net/pc3-8088/
Form Factor: Desktop
CPU: 8088 @ 4.77 MHz or 8 MHz
RAM: 640K
Bus: 5x ISA:    1) Adaptec ACB-2072 RLL Controller Card
Video: On-board MDA/CGA/Hercules/EGA
Mass storage: 1x 5.25" 360K floppy + 30MB RLL harddisk
On board ports: floppy, parallel, serial, mouse, speaker
Options: 8087 FPU
DIP switches:                    4   3   2   1
                EGA (smart on)  OFF OFF  ON OFF
                EGA (smart OFF) OFF  ON  ON OFF
                Color monitor    ON OFF OFF  ON
                monochrome       ON  ON OFF  ON

If you add a monochrome adapter board, set the switches to "Smart OFF", the HGC, MDA and HCOLOR
options are meaningless, then.
EGA.COM, CGA.COM, HGC.COM, MDA.COM, PALETTE.COM, HCOLOR.COM and CURSOR are utilities to change
the behavior of the integrated graphics card.

Turbo option: From DOS, commands "TURBO ON" and "TURRBO OFF or key combos [Ctrl][Alt][1] or
[Ctrl][Alt][+] for Turbo on, [Ctrl][Alt][2] or [Ctrl][Alt][-] for Turbo off
Keyboard click: From DOS, "CLICK ON" and "CLICK OFF" or key combos [Ctrl][Alt][<]
for click on, [Ctrl][Alt][>] for click off

******************************************************************************/

ROM_START( ataripc3 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("c101701-004 308.u61",0x8000, 0x8000, CRC(929a2443) SHA1(8e98f3c9180c55b1f5521727779c016083d27960))

	ROM_REGION(0x8000,"gfx1", 0)
	ROM_LOAD("5788005.u33", 0x00000, 0x2000, BAD_DUMP CRC(0bf56d70) SHA1(c2a8b10808bf51a3c123ba3eb1e9dd608231916f)) // not the real character ROM

	ROM_REGION(0x8000,"plds", 0)
	ROM_LOAD("c101681 6ffb.u60",0x000, 0x100, NO_DUMP ) // PAL20L10NC
ROM_END


/******************************************** Bondwell BW230 (Pro28 series) ***

Links:  http://gallery.fdd5-25.net/details.php?image_id=3463&sessionid=1eaeb42abdf2758a020b16204a2a8e5a ;
        http://www.zonadepruebas.com/viewtopic.php?t=3696 ;
        ftp://ftp.whtech.com/emulators/mess/old/Complete%20MESS%20Geneve%20emulation/mess/sysinfo/bondwell.htm
Info:   Info is hard to come by. A BW230 is nowhere to be found, the links about the Pro28 series suggest an XT
        compatible built around a passive backplane and a slot CPU. This is confirmed by the old MESS info.
Form Factor: Desktop
CPU: 8088 @ 4.77 MHz (MESS sysinfo: 3.75)/ 8 MHz
RAM: 512K / 640K
Bus: at least 2x ISA:   1)  CPU, RAM, Floppy controller
                        2)  Graphics, Game, Parallel
Video: Hercules/CGA
Mass storage: 1x 5.25" 360K and 20/30MB Harddisk.

******************************************************************************/

void pc_state::init_bondwell()
{
	m_turbo_off_speed = 4.77/12;
}

static INPUT_PORTS_START( bondwell )
	PORT_INCLUDE(pccga)

	PORT_MODIFY("DSW2") /* IN3 */
	PORT_DIPNAME( 0x02, 0x02, "Turbo Switch" ) PORT_CHANGED_MEMBER(DEVICE_SELF, pc_state, pc_turbo_callback, 0)
	PORT_DIPSETTING(    0x00, "Off (4.77 MHz)" )
	PORT_DIPSETTING(    0x02, "On (12 MHz)" )
INPUT_PORTS_END

void pc_state::bondwell(machine_config &config)
{
	pccga(config);

	i8088_cpu_device &maincpu(I8088(config.replace(), "maincpu", 4772720)); /* turbo? */
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc8_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc8_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));
}

ROM_START( bw230 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("bondwell.bin", 0xe000, 0x2000, CRC(d435a405) SHA1(a57c705d1144c7b61940b6f5c05d785c272fc9bb))
ROM_END


/****************************************** Columbia Data Products MPC 1600 ***

Links:  https://www.old-computers.com/museum/computer.asp?st=1&c=633, https://winworldpc.com/download/6f07e280-9d12-7ae2-80a6-11c3a6e28094,
        http://www.minuszerodegrees.net/rom/rom.htm
Form Factor: Desktop
CPU: 8088 @ 4.77 MHz
RAM: 128K, up to 1MB
Bus: 8x ISA
Video: CGA
Mass storage: 2x 5.25" 320K
On board ports: Floppy, serial, console, Centronics, hard disk ("The Winchester disk interface is an 8 bit parallel data port with 4 control lines
    for byte and unit synchronization. This interface connects the MPC system board to the CDP cache buffered Winchester controller.")
Options: 5MB harddisk, light pen
ToDo: The ROM for the CGA is available (see ROM section)

If all of the testing is accomplished, the system will respond with a single 1/2 second tone and continue according to the input output media attached.
If a dumb terminal is used, another tone will sound and the system will produce another 1/2 second tone then wait until an ASCII period(.)is typed at the
terminal. The system uses the ASCII character to determine the baud-rate of the terminal device. If an ASCII(.)is not received in 5 seconds, the system
will default to 19200 baud. If a keyboard and monitor is attached, then no baud rate determination is required.
Note:Type[ESC] here to activate the ROM monitor for system testing. If a response is not made within five (5) seconds, the MPC will automatically
enter the system bootstrap sequence detailed below.

******************************************************************************/

void pc_state::mpc1600(machine_config &config)
{
	pccga(config);
	ISA8_SLOT(config, "isa6", 0, "mb:isa", pc_isa8_cards, nullptr, false);
	ISA8_SLOT(config, "isa7", 0, "mb:isa", pc_isa8_cards, nullptr, false);
	ISA8_SLOT(config, "isa8", 0, "mb:isa", pc_isa8_cards, nullptr, false);
	subdevice<ram_device>(RAM_TAG)->set_default_size("128K").set_extra_options("256K, 512K, 640K");
}

ROM_START( mpc1600 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("mpc4.34_u45.bin",  0xf000, 0x01000, CRC(ed9a11b3) SHA1(ca819579e6c2a06cddacf893e1f57c5b37723d90))
	ROM_LOAD("mpc4.34_u46.bin",  0xe000, 0x01000, CRC(33a87335) SHA1(a8ee188cbb93fe32c6cde881bdf3b9c783a59a5b))
	ROM_LOAD("mpc4.34_u47.bin",  0xd000, 0x01000, CRC(cc2e4c28) SHA1(3b02be4bebe2b57098102eca04f738df50a734a4))

	ROM_REGION(0x8000,"gfx1", 0)
	ROM_LOAD("mpc_vid-1.0.bin", 0x00000, 0x0800, CRC(a362ffe6) SHA1(1fddd01dcc0fa8c002ced3a1a94873dccdc88424)) // CGA Card
ROM_END


/********************************************************** Compaq Portable ***

Links:  https://en.wikipedia.org/wiki/Compaq_Portable , http://oldcomputers.net/compaqi.html ,
        http://www.digibarn.com/collections/systems/compaq/index.html ,
        http://www.old-computers.com/museum/computer.asp?c=547 , https://www.seasip.info/VintagePC/compaq.html
Form Factor: Luggable
CPU: 8088 @ 4.77 MHz
RAM: 128K, up to 640KB
Bus: 5x ISA
Video: CGA/MDA capable card, both fonts available, Ctrl+Alt+> switches between internal and external monitor
Mass storage: 1/2x 5.25" double sided/double density (320K/360K), Plus: 10-21MB harddisk

SW1: 1   2   3   4   5   6   7   8  Descr.
    OFF                             Not used/always OFF (def.)
     ON                             Coprocessor/always ON (def.)
            OFF OFF                 Processor board memory/always OFF (def.)
                     ON OFF         Compaq video display Controller board (def.)
                    OFF OFF         Optional monochrome video board
                             ON  ON 1 Diskette drive (def.)
                            OFF  ON 2
                             ON OFF 3
                            OFF OFF

SW2: 1   2   3   4   5   6   7   8  Descr.
     ON OFF  ON  ON OFF OFF OFF OFF 128 Kbyte total memory
     ON  ON OFF  ON OFF OFF OFF OFF 192 Kbyte total memory
     ON OFF OFF  ON OFF OFF OFF OFF 156 Kbyte total memory
     ON  ON  ON OFF OFF OFF OFF OFF 320 Kbyte total memory
     ON OFF  ON OFF OFF OFF OFF OFF 384 Kbyte total memory
     ON  ON OFF OFF OFF OFF OFF OFF 448 Kbyte total memory
     ON OFF OFF OFF OFF OFF OFF OFF 512 Kbyte total memory
    OFF OFF OFF OFF OFF OFF OFF OFF 544 Kbyte total memory

If the ROMs installed in socket(s) U40 (and U47 if available) are Revision C or above,
SW2 is ignored.  Therefore, on system boards ofRevision J or above, SW2 has been removed.
If Revision C ROMs or above are installed, 256K x 1 RAM chips may be usedinstead of 64K x 1 bit
RAM chips in banks 2 and 3 of the system board.  To dothis, however, a new decoder PROM must
be used in socket U35:o PN 101257-001 (No longer available) if banks 2 and 3 are filled with
256K x 1 RAM chips for a total of 640 Kbytes.o  PN 101256-001 if only bank 3 is filled with
256K x 1 RAM chips for a total of 448 Kbytes.

******************************************************************************/

void pc_state::comport(machine_config &config)
{
	pccga(config);
	subdevice<isa8_slot_device>("isa1")->set_default_option("ega");
	subdevice<ram_device>(RAM_TAG)->set_default_size("128K").set_extra_options("256K, 512K, 640K");
}

ROM_START( comport )
	ROM_REGION(0x10000, "bios", 0)
	ROM_DEFAULT_BIOS("rev.c") // set to use EGA until the proper graphics card is emulated, the earlier rev.b doesn't like that
	ROM_SYSTEM_BIOS( 0, "rev.b", "rev.b" )
	ROMX_LOAD("award_atari_pc_bios_3.06.bin", 0xe000, 0x2000, CRC(e9a055b2) SHA1(faa31687ef3d967c5e46d6b2546a28efb79a2097), ROM_BIOS(0))
	ROM_SYSTEM_BIOS( 1, "rev.c", "rev.c" )
	ROMX_LOAD("compaq_portable_rev_c.bin", 0xe000, 0x2000, CRC(1d1f7c38) SHA1(d9782eb46cd1a7d40f3e8b37eb48db04ac588acb), ROM_BIOS(1))

	ROM_REGION(0x8000,"gfx1", 0)
	ROM_LOAD("compaq_portable_video_cpqvid.bin", 0x0000, 0x1ffe, BAD_DUMP CRC(3ae64565) SHA1(6eeb06620e588a2f7bfab72eb4fadbd70503ea94))
ROM_END


/************************************************** Data General One / DG-1 ***

Links: http://www.1000bit.it/ad/bro/datageneral/DG-ONE-PersonalSystem.pdf , http://www.1000bit.it/ad/bro/datageneral/DG-ONE-Interduction-PR.pdf ,
       http://www.oldcomputers.net/data-general-one.html , http://forums.bannister.org/ubbthreads.php?ubb=showflat&Number=30897&page=all
Info: According to the discussion in the thread, the ROM we have is from the original version. Specs for later permutations can be found on oldcomputers.net
Form Factor: Laptop
CPU: 80C88 @ 4 MHz
RAM: 128K - 256K - 384K - 512K internally
Bus: no internal slots
Video: On board, Text mode 80x25 with 8x8 or 8x10 characters, CGA
Display: non-backlit LCD 640x256 pixels
Mass storage: 1/2x Floppy 3.5" 720K
On board Ports: Floppy, RTC, 1x RS232C + 1x RS232C/RS422 via 8251, speaker
Options: ext. 5.25" Floppy, int. Bell 103A 300 Baud Modem, 8087 FPU
Expansion: Expansion box, with 5 ISA slots and space for a 5.25" drive and a harddisk; specifically mentioned are the 5.25" drive, color graphics and memory expansion via ISA cards

******************************************************************************/

void pc_state::dgone(machine_config &config)
{
	pccga(config);
	subdevice<isa8_slot_device>("isa2")->set_option_machine_config("fdc_xt", cfg_dual_720K);
}

ROM_START( dgone )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD( "dgone.bin",  0x8000, 0x08000, CRC(2c38c86e) SHA1(c0f85a000d1d13cd354965689e925d677822549e))
ROM_END


/*************************************************************** Eagle 1600 ***

Links: https://archive.org/details/bitsavers_eagleCompu00Brochure_9975235 , http://www.vcfed.org/forum/showthread.php?49510-Eagle-Computer-model-list ,
       http://bitsavers.trailing-edge.com/pdf/eagleComputer/1600/1600_Series_Training_Notes.pdf
Info:   Eagle 1620 - 8086/128K, 2 Quad density floppy drives, 4 Expansion slots available, ~1983, Eagle 1630 - 8086/128K,
        1 Quad density floppy drive, 10MB HD, 3 Expansion Slots available (Same as 1620 with hard drive), ~1983
        Eagle 1640 - 8086/512K, 1 Quad density floppy drive, 32MB HD, 3 Expansion Slots available, ~1984
        The native floppy format is 780K, 2 sides, 80 tracks/side, 1024 bytes/sector, 5 sectors per track. Standard 360K disks can be read
        Holding "T" and resetting starts a system diagnostics test
Form Factor: Desktop
CPU: 8086 @ 8 MHz
RAM: 128K / 512K
Bus: 8xISA:     1) SASI board, connects to a XEBEC Sl410 SASI => MFM bridge board
                2) Floppy controller
                3) empty
                4) Video/graphics controller board
                5) empty
                6) empty
                7) Serial board: 2x serial, one sync/async, one async only
                8) Parallel board
Video: 80x25 text mode, 720x352 pixel graphics mode
Mass storage: 1x 5.25" QD 780K floppy and 1x 5.25" QD 820K floppy or 10/30MB MFM harddisk
Options: 8087 FPU, EagleNet File server, EightPort serial card, High Resolution color board and video, Video Cassette Adapter board for 80MB backup on video cassette

******************************************************************************/

void pc_state::eagle1600(machine_config &config)
{
	pccga(config);

	i8086_cpu_device &maincpu(I8086(config.replace(), "maincpu", 8000000));
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc16_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc16_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));
}

ROM_START( eagle1600 )
	ROM_REGION16_LE(0x10000, "bios", 0)
	ROMX_LOAD("eagle 1600 62-2732-001 rev e u403.bin",0xe000, 0x1000, CRC(3da1e96a) SHA1(77861ba5ebd056da1daf048f5abd459e0528666d), ROM_SKIP(1))
	ROMX_LOAD("eagle 1600 62-2732-002 rev e u404.bin",0xe001, 0x1000, CRC(be6492d4) SHA1(ef25faf33e8336121d030e38e177be39be8afb7a), ROM_SKIP(1))

	ROM_REGION(0x8000,"gfx1", 0)
	ROM_LOAD("eagle 1600 video char gen u301.bin", 0x00000, 0x1000, CRC(1a7e552f) SHA1(749058783eec9d96a70dc5fdbfccb56196f889dc))
ROM_END

/*************************************************************** Eagle PC-2 ***

Links: http://www.digibarn.com/collections/systems/eagle-pc/index.html , https://www.atarimagazines.com/creative/v10n2/28_Eagle_PC2.php http://www.old-computers.com/museum/computer.asp?st=1&c=529
Form Factor: Desktop

Error message: Cannot read boot sector

******************************************************************************/

ROM_START( eaglepc2 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("eagle_pc-2_bios_2.812_1986_u1101.bin", 0xe000, 0x2000, CRC(cd0fc034) SHA1(883cb4808c565f2582873a51cc637ab25b457f88))

	ROM_REGION(0x8000,"gfx1", 0)
	ROM_LOAD("eagle_pc-2_cga_char_rom_u401.bin", 0x00000, 0x1000, CRC(e85da08d) SHA1(176a7027bd14cc7efbb5cec5c2ac89ba002912d0))

ROM_END

/********************************************************** Eagle PC Spirit ***

Links: http://www.old-computers.com/museum/computer.asp?st=1&c=530 , https://archive.org/details/eagle_pc_spirit_users_guide_nov83
Form Factor: Luggable
CPU: 8088 @ 4.77 MHz
RAM: 128K, up to 640K
Video: CGA
Mass storage: 1/2x 5.25" 360KB floppy or 1x 360KB floppy and 10MB harddisk (XL model)

Pressing "T" after a hard reset brings up a ROM based test suite.

DIP switches:
SW801: Sw.1  Sw.2  Sw.3  Sw.4 Max.RAM     J13  Sw.5  Sw.6  Floppy  Sw.7  Sw.8  Display@
                              on mainbd.                   drives              powerup
         ON    ON    ON    ON    64K      OUT
        OFF    ON    ON    ON   128K      OUT
         ON   OFF    ON    ON   192K      OUT
        OFF   OFF    ON    ON   256K      OUT
        OFF    ON   OFF    ON   384K       IN
        OFF    ON    ON   OFF   640K       IN
                                                  ON    ON    1
                                                  ON   OFF    2
                                                 OFF    ON    3
                                                 OFF   OFF    4
                                                                      ON    ON    No Display
                                                                      ON   OFF    Color 40x35
                                                                     OFF    ON    Color 80x25
                                                                      OFF   OFF   Monochrome

******************************************************************************/

ROM_START( eaglespirit )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("u1101.bin", 0xe000, 0x2000, CRC(3fef0b0b) SHA1(fa75e90c5595b72ef33d178f1f86511cbe08191d))
	ROM_LOAD("u1103.bin", 0xc000, 0x2000, CRC(efa2b0d9) SHA1(1fcd01dd2676539a0f6498ef866fb450caab1ac4))
ROM_END


/****************************************************** Elektronika MC-1702 ***

******************************************************************************/

ROM_START( mc1702 )
	ROM_REGION16_LE(0x10000,"bios", 0)
	ROM_LOAD16_BYTE("2764_2,573rf4.rom", 0xc000,  0x2000, CRC(34a0c8fb) SHA1(88dc247f2e417c2848a2fd3e9b52258ad22a2c07))
	ROM_LOAD16_BYTE("2764_3,573rf4.rom", 0xc001, 0x2000, CRC(68ab212b) SHA1(f3313f77392877d28ce290ffa3432f0a32fc4619))
	ROM_LOAD("ba1m,573rf5.rom", 0x0000, 0x0800, CRC(08d938e8) SHA1(957b6c691dbef75c1c735e8e4e81669d056971e4))
ROM_END


/************************************************ Ericsson Portable PC - EPPC ***

Links: https://youtu.be/Qmke4L4Jls8 , https://youtu.be/yXK01gBQE6Q
Form Factor: Laptop
CPU: 8088 @ 4.77MHz
RAM: 256K
Bus: No internal slots
Video: Monochrome 80x25 character mode. 320x200 and 640x400 (CGA?) grahics modes
Display: Orange Gas Plasma (GP) display
Mass storage: half height 5.25" 360K
On board ports: Beeper,
Ports: serial, parallel, ext. floppy
Internal Options: 256K RAM, thermal printer
External Options: A disk cabinet with networking, 1200/300 accoustic modem, 256K Ergo disk electronic disk drive
Misc: No battery due to the power hungry GP display. 10-15.000 units sold

******************************************************************************/

ROM_START( eppc )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD( "eppcbios60605.bin",  0xc000, 0x04000, CRC(fe82e11b) SHA1(97ed48dc30f1ed0acce0a14b8085f13b84d4444b))
ROM_END


/***************************************************************** IBM 5550 ***

Information can be found at http://homepage3.nifty.com/ibm5550/index-e.html
It's a heavily modified IBM PC-XT machine, with a completely different
video HW too.

******************************************************************************/

void pc_state::ibm5550_io(address_map &map)
{
	map.unmap_value_high();
	map(0x0000, 0x00ff).m("mb", FUNC(ibm5160_mb_device::map));
	map(0x00a0, 0x00a0).r(FUNC(pc_state::unk_r));
}

void pc_state::ibm5550(machine_config &config)
{
	/* basic machine hardware */
	i8086_cpu_device &maincpu(I8086(config, "maincpu", 8000000));
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc16_map);
	maincpu.set_addrmap(AS_IO, &pc_state::ibm5550_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5160_mb_device &mb(IBM5160_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(pccga));

	// FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "cga", false);
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");
}

ROM_START( ibm5550 )
	ROM_REGION16_LE(0x10000,"bios", 0)
	ROM_LOAD("ipl5550.rom", 0xc000, 0x4000, CRC(40cf34c9) SHA1(d41f77fdfa787b0e97ed311e1c084b8699a5b197))
ROM_END

/***************************************************************** ITT XTRA ***

Links:  https://www.atarimagazines.com/creative/v10n12/71_ITT_Xtra_an_IBM_PC_compa.php
Info:   Model I: 128K RAM, 14" mono (green or amber) or 12" colour screen; Model II adds another floppy drive;
        Model III: 256K RAM, 1 floppy, 10MB harddisk
Form Factor: Desktop
CPU: 8088
RAM: 128K or 256K on board, expandable to 512K
Bus: 5xISA
Mass storage: 1/2x 5.25" floppy drives
Options: 8087 FPU, 10MB harddisk, combo board: "The optional Combo board plugs into one of the 62-pin motherboard expansion slots
    and provides an additional 128KB of memory, a battery backup real-time clock, and an 8-bit general purpose port which can be
    used for a parallel printer. Two "baby" add-on memory cards of 128KB each can also be plugged into the Combo board raising the
    available memory on the board to 384K.
On board connectors: Floppy, keyboard, serial, parallel
ToDo: find dump of original graphics card ROM

DIP switches:
SW1: 1   2   3   4   5   6   7   8   effect
        OFF                          FPU installed
         ON                          no FPU
            OFF  ON                  128K mainboard memory
             ON OFF                  192K
            OFF OFF                  256K
                     ON  ON          80x25 color monitor
                    OFF  ON          40x25 color monitor
                     ON OFF          color monitor in the 80x25 mode
                    OFF OFF          monochrome or both mono and color monitors
                              ON  ON 1 floppy drive
                             OFF  ON 2
                              ON OFF 3
                             OFF OFF 4

SW2: 1   2   3   4   5   6   7   8   effect
                     ON  ON          Screen Time out ON
                    OFF  ON          Screen Time out OFF
                            OFF      Power up Self Test ON
                             ON      Power up Self Test OFF
                                 OFF Normal Operation
                                  ON Factory Testing

The ROM contains a monitor program that can be activated by pressing "ESC" at the "Insert Diskette"  prompt
or by pressing [Ctrl]-[Alt]-[Esc]

******************************************************************************/

ROM_START( ittxtra )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("bios_itt_xtra_5c00_v200_u92.bin", 0xc000, 0x4000, CRC(77056e54) SHA1(6a2d28340cb6be09c9f59bf6971e5d7fa693e66b))
	ROM_LOAD("bios_itt_xtra_bf00_v200_u93.bin", 0x8000, 0x4000, CRC(c5191343) SHA1(01f9feaf2adf118703479ead224271da55373a62))
ROM_END

/**************************************************************** Kaypro 16 ***

Links:  http://www.mofeel.net/679-comp-sys-ibm-pc-classic/309.aspx, https://groups.google.com/forum/#!topic/comp.os.cpm/HYQnpUOyQXg,
        https://amaus.org/static/S100/kaypro/systems/kaypro%2016/Kaypro%2016.pdf , http://ajordan.dpease.com/kaypro16/index.htm
Form Factor: Luggable
CPU: 8088 @ 4.77MHz
RAM: 256K, expandable to 512K and 640K
Mainboard with 4 ISA slots, video decoder circuitry to show 16 levels of grayscale on the internal monitor, interface to WD1002-HD0 harddisk controller
Bus: 4x ISA:    1) 8088 slot CPU, keyboard connector, reset switch,
                2) Floppy disk controller, serial, parallel, RAM expansion
                3) Kaypro CGA card with composite and colour TTL outputs, ROM 81-820 needs to be dumped
                4) empty
Video: CGA
Mass storage: 1x 5.25" 360K, 10MB harddisk (Seagate ST212)
Options: 8087 FPU
Misc: A Kaypro 16/2 is a configuration without harddisk but with two floppy disk drives (interface ics on mainboard were not populated)

DIP switches:
SW1 on the PROCESSOR CARD: Position 1 is used to specify the numeric processor option. Positions 2 and 3 are used to specify the size and type
of display interface. Positions 4 and 5 are used to specify the number of disk drives.
(1: on, 2: off, 3: on, 4: on, 5: on); Kapro 16/2: (1: on, 2: off, 3: on, 4: on, 5: off)
SW1 on the FLOPPY-RAM-I/O CARD: Positions 1,2,3 and 4 are used to specify the starting address for the RAM on the FLOPPY-RAM-I/O card (the
memory expansion). Positions 5 and 6 indicate the number of RAM banks on the FLOPPY-RAM-I/O card. Position 7 is used to specify whether those
banks contain 64K or 256K. Position 8 is used to enable or disable parity checking.
(1: off, 2: on, 3: on, 5: on, 5: on, 6: off, 7: on).
SW2 on the FLOPPY-RAM-I/O card: Positions 1 and 2 are used to select the serial port. Positions 3 and 4 are used to select the parallel port.
(1: on, 2: on, 3: on, 4: off).

******************************************************************************/

void pc_state::kaypro16(machine_config &config)
{
	pccga(config);
	subdevice<isa8_slot_device>("isa1")->set_fixed(true);
	subdevice<isa8_slot_device>("isa2")->set_fixed(true);
	subdevice<isa8_slot_device>("isa3")->set_fixed(true);
	subdevice<isa8_slot_device>("isa4")->set_fixed(true);
	subdevice<isa8_slot_device>("isa5")->set_default_option(nullptr);
	subdevice<ram_device>(RAM_TAG)->set_default_size("256K").set_extra_options("512K, 640K");
}

ROM_START( kaypro16 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("pc102782.bin", 0xe000, 0x2000, CRC(ade4ed14) SHA1(de6d87ae83a71728d60df6a5964e680487ea8400))
ROM_END

/**************************************************************** Kaypro PC ***

Links:  https://www.youtube.com/watch?v=2YAEOhYEZbc ,

DIP switches: 2 blocks of 8 switches on the FLOPPY-RAM-I/O board, 1 block of 5 switches on the CPU board

******************************************************************************/

ROM_START( kaypropc )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("kpb203n.rom", 0xe000, 0x2000, CRC(49ea41e9) SHA1(14db6b8f302833f64f6e740a293d12f76e71f78f))
ROM_END

/******************************************************************** MK-88 ***

******************************************************************************/
// MK-88
void pc_state::mk88(machine_config &config)
{
	poisk2(config);
	subdevice<isa8_slot_device>("isa1")->set_default_option("cga_ec1841");
}

// MK-88
ROM_START( mk88 )
	ROM_REGION16_LE(0x10000,"bios", 0)
	ROM_DEFAULT_BIOS("v392")
	ROM_SYSTEM_BIOS(0, "v290", "v2.90")
	ROMX_LOAD("mk88m.bin", 0xc000, 0x2000, CRC(09c9da3b) SHA1(d1e7ad23b5f5b3576ad128c1198294129754f39f), ROM_BIOS(0))
	ROMX_LOAD("mk88b.bin", 0xe000, 0x2000, CRC(8a922476) SHA1(c19c3644ab92fd12e13f32b410cd26e3c844a03b), ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "v391", "v3.91")
	ROMX_LOAD("mkm.bin", 0xc000, 0x2000, CRC(65f979e8) SHA1(13e85be9bc8ceb5ab9e559e7d0089e26fbbb84fc), ROM_BIOS(1))
	ROMX_LOAD("mkb.bin", 0xe000, 0x2000, CRC(830a0447) SHA1(11bc200fdbcfbbe335f4c282020750c0b5ca4167), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "v392", "v3.92")
	ROMX_LOAD("m88.bin", 0xc000, 0x2000, CRC(fe1b4e36) SHA1(fcb420af0ff09a7d43fcb9b7d0b0233a2071c159), ROM_BIOS(2))
	ROMX_LOAD("b88.bin", 0xe000, 0x2000, CRC(58a418df) SHA1(216398d4e4302ee7efcc2c8f9ff9d8a1161229ea), ROM_BIOS(2))
ROM_END


/***************************************************************** NCR PC4i ***

Links: http://www.minuszerodegrees.net/manuals/NCR/NCR%20PC4i%20-%20Technical%20Reference%20Manual%20-%20January%201986.pdf
Info:   The earlier PC4 is not quite IBM compatible, the "i" in PC4i indicates full IBM compatibility.
        The NCR Graphics card supports a special 640x400 video mode
Form Factor: All-in-one desktop
CPU: 8088 @ 4.77 MHz
RAM: 256K, expandable to 640K
Bus: 7x ISA:    1)  (optional) RAM expansion board
                2)  empty
                3)  32K Video/Graphics board (64K option)
                4)  (optional) Alpha board
                5)  empty
                6)  (optional) MFM harddisk controller
                7)  empty
Video: K510: 4KB Alpha for internal monitor; K511: 32KB Graphics for internal monitor; K512: 32KB upgrade for K512; K140: 16KB Graphics for external monitor; K141: 4KB Alpha for external monitor
Display: Mono or color CRT 640x400 pixel
Mass storage: 1x 5.25" 360K floppy and 1x 5.25" 360K floppy or 10 MB harddisk
On board ports: parallel, serial, speaker, floppy
Options: 8087 FPU, K101 memory upgrade in 64K steps, 1.2MB floppy and controller board

Regular motherboard, an alternate board using more integrated components exists.
Jumpers: JP1 closed: enable flex. disk drives, JP2 closed: enable standad serial I/O
JP3 closed: enable parallel interface OR just JP5: closed, enable standard serial I/O

DIP settings:  Sw.1  Sw.2  Sw.3  Sw.4  Sw.5  Sw.6  Sw.7  Sw.8  effect
                OFF                                            normal operation
                       ON                                      FPU not installed
                             OFF  OFF                          256KB RAM
                                        OFF   OFF              Alpha Controller
                                        OFF    ON              40x25 Graphics Controller
                                         ON   OFF              80x25 Graphics Controller
                                                     ON    ON  1 Flexible Disk Drive
                                                    OFF    ON  2 Flexible Disk Drives

******************************************************************************/

void pc_state::ncrpc4i(machine_config & config)
{
	pccga(config);
	ISA8_SLOT(config, "isa6", 0, "mb:isa", pc_isa8_cards, nullptr, false); // FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa7", 0, "mb:isa", pc_isa8_cards, nullptr, false);

	subdevice<ram_device>(RAM_TAG)->set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");
}

ROM_START( ncrpc4i )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("ncr_pc4i_biosrom_1985.bin",0xc000, 0x4000, CRC(b9732648) SHA1(0d5d96fbc36089ca4d893b0db84faffa8043a5e4))
ROM_END


/************************************************************* Olivetti M15 ***

Links:  http://www.1000bit.it/ad/bro/olivetti/olivettiM15.pdf , http://electrickery.xs4all.nl/comp/m15/ ,
        http://electrickery.xs4all.nl/comp/m15/doc/M15_InstallationAndOperationsGuide.pdf
        http://www.museotecnologicamente.it/olivetti-m-15-1987/ , http://www.museotecnologicamente.it/wp-content/uploads/M15_Depliant_inglese.pdf
Info: The info brochure has a picture of a working M15. This shows the LCD display with a green background and blue text/graphics.
Form Factor: Laptop
CPU: 80C88 @ 4.77 MHz
RAM: 256K / 512K
Bus: no internal slots
Video: 80x25 text mode, CGA
Display: LCD
Mass storage: 2x 3.5" 720K drives
Ports: serial, parallel, ext. floppy, RTC
Expansion: External 5.25" 360K floppy drive

******************************************************************************/

static DEVICE_INPUT_DEFAULTS_START( m15 )
	DEVICE_INPUT_DEFAULTS("DSW0", 0x30, 0x20) // TODO: document correct dip settings
	DEVICE_INPUT_DEFAULTS("DSW0", 0x01, 0x00)
DEVICE_INPUT_DEFAULTS_END

void pc_state::m15(machine_config &config)
{
	pccga(config);
	subdevice<ibm5160_mb_device>("mb")->set_input_default(DEVICE_INPUT_DEFAULTS_NAME(m15));
	subdevice<isa8_slot_device>("isa2")->set_option_machine_config("fdc_xt", cfg_dual_720K);

	subdevice<ram_device>(RAM_TAG)->set_default_size("448K").set_extra_options("16K, 160K, 304K");
}

ROM_START( olivm15 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("oliv_m15.bin",0xc000, 0x04000, CRC(bf2ef795) SHA1(02d497131f5ca2c78f2accd38ab0eab6813e3ebf))
ROM_END


/*************************************************** AEG Olympia Olytext 30 ***

Form Factor: Desktop
CPU: NEC V20 @ 4.77MHz
RAM: 768K, not sure how to address the area above 640K
Bus: 8x ISA:    1) NEC V20 Slot CPU with 786K RAM, TI TACT80181FT chip
                2) Z180 CP/M emulation card, needed to run the proprietary Olytext 30 word processor)
                3) Monochrome graphics/color graphics card (possibly EGA capable) ICs: Chips P82C441 and P82A442A
                4) MFM hard disk controller HDC-770, ICs: HDC9224, HDC92C26, HDC9223,
                5) Floppy, serial and RTC DIO-770, ICs: 2x UM8250B, UM8272A, OKI M5832
Video: MDA/Hercules/CGA, possibly EGA
Mass storage: 1x 3.5" 720K, 20MB Miniscribe harddisk
On board ports: speaker
Options: 8087 FPU

DIP switches: block with six switches on the CPU board
******************************************************************************/

void pc_state::olytext30(machine_config &config)
{
	pccga(config);

	v20_device &maincpu(V20(config.replace(), "maincpu", XTAL(25'000'000)/3)); /* 8.33 MHz */ // determine divider, it's a 25MHz crystal and a 10MHz V20
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc8_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc8_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	subdevice<isa8_slot_device>("isa2")->set_option_machine_config("fdc_xt", cfg_single_720K);
	subdevice<isa8_slot_device>("isa3")->set_default_option(nullptr);
	subdevice<isa8_slot_device>("isa5")->set_default_option("hdc");
	subdevice<ram_device>(RAM_TAG)->set_default_size("768K");
}

ROM_START( olytext30 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("o45995.bin", 0xe000, 0x2000, CRC(fdc05b4f) SHA1(abb94e75e7394be1e85ff706d4d8f3b9cdfea09f))
ROM_END


/****************************************************************** Poisk-2 ***

******************************************************************************/

void pc_state::poisk2(machine_config &config)
{
	/* basic machine hardware */
	i8086_cpu_device &maincpu(I8086(config, "maincpu", 4772720));
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc16_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc16_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5160_mb_device &mb(IBM5160_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(pccga));

	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "cga_poisk2", false); // FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");
}

ROM_START( poisk2 )
	ROM_REGION16_LE(0x10000,"bios", 0)
	ROM_SYSTEM_BIOS(0, "v20", "v2.0")
	ROMX_LOAD("b_p2_20h.rf4", 0xc001, 0x2000, CRC(d53189b7) SHA1(ace40f1a40642b51fe5d2874acef81e48768b23b), ROM_SKIP(1) | ROM_BIOS(0))
	ROMX_LOAD("b_p2_20l.rf4", 0xc000, 0x2000, CRC(2d61fcc9) SHA1(11873c8741ba37d6c2fe1f482296aece514b7618), ROM_SKIP(1) | ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "v21", "v2.1")
	ROMX_LOAD("b_p2_21h.rf4", 0xc001, 0x2000, CRC(22197297) SHA1(506c7e63027f734d62ef537f484024548546011f), ROM_SKIP(1) | ROM_BIOS(1))
	ROMX_LOAD("b_p2_21l.rf4", 0xc000, 0x2000, CRC(0eb2ea7f) SHA1(67bb5fec53ebfa2a5cad2a3d3d595678d6023024), ROM_SKIP(1) | ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "v24", "v2.4")
	ROMX_LOAD("b_p2_24h.rf4", 0xc001, 0x2000, CRC(ea842c9e) SHA1(dcdbf27374149dae0ef76d410cc6c615d9b99372), ROM_SKIP(1) | ROM_BIOS(2))
	ROMX_LOAD("b_p2_24l.rf4", 0xc000, 0x2000, CRC(02f21250) SHA1(f0b133fb4470bddf2f7bf59688cf68198ed8ce55), ROM_SKIP(1) | ROM_BIOS(2))
	ROM_SYSTEM_BIOS(3, "v21d", "v2.1d")
	ROMX_LOAD("opp2_1h.rf4", 0xc001, 0x2000, CRC(b7cd7f4f) SHA1(ac473822fb44d7b898d628732cf0a27fcb4d26d6), ROM_SKIP(1) | ROM_BIOS(3))
	ROMX_LOAD("opp2_1l.rf4", 0xc000, 0x2000, CRC(1971dca3) SHA1(ecd61cc7952af834d8abc11db372c3e70775489d), ROM_SKIP(1) | ROM_BIOS(3))
	ROM_SYSTEM_BIOS(4, "v22d", "v2.2d")
	ROMX_LOAD("opp2_2h.rf4", 0xc001, 0x2000, CRC(b9e3a5cc) SHA1(0a28afbff612471ee81d69a98789e75253c57a30), ROM_SKIP(1) | ROM_BIOS(4))
	ROMX_LOAD("opp2_2l.rf4", 0xc000, 0x2000, CRC(6877aad6) SHA1(1d0031d044beb4f9f321e3c8fdedf57467958900), ROM_SKIP(1) | ROM_BIOS(4))
	ROM_SYSTEM_BIOS(5, "v23d", "v2.3d")
	ROMX_LOAD("opp2_3h.rf4", 0xc001, 0x2000, CRC(ac7d4f06) SHA1(858d6e084a38814280b3e29fb54971f4f532e484), ROM_SKIP(1) | ROM_BIOS(5))
	ROMX_LOAD("opp2_3l.rf4", 0xc000, 0x2000, CRC(3c877ea1) SHA1(0753168659653538311c0ad1df851cbbdba426f4), ROM_SKIP(1) | ROM_BIOS(5))
ROM_END


/****************************************************** Samsung Samtron 88S ***

******************************************************************************/
ROM_START( ssam88s )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("samsung_samtron_88s_vers_2.0a.bin",  0x8000, 0x08000, CRC(d1252a91) SHA1(469d15b6ecd7b70234975dc12c6bda4212a66652))
ROM_END


/************************************************************* Sanyo MBC-16 ***

Links:
Info: In the MBC-16 I had, the graphics card had a Sanyo sticker on it, so I assume that was the original graphics card for the machine.
Form Factor: Desktop
CPU: 8088 @ 8MHz
RAM: 640KB
Bus: 3x ISA:    1)  ATI Graphics Solution SR https://sites.google.com/site/atiwonderseriesdatabase/
Video: MDA/CGA/Plantronics
Mass storage: 1 or 2 5.25" 360K floppies, MFM harddisk on hardcard or via seperate controller
On board ports: serial, parallel, floppy

******************************************************************************/

ROM_START( mbc16 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("mbc16.bin", 0xc000, 0x4000, CRC(f3e0934a) SHA1(e4b91c3d395be0414e20f23ad4919b8ac52639b2))
	ROM_REGION(0x2000,"gfx1", 0)
	//ATI Graphics Solution SR (graphics card, need to make it ISA card)
	ROM_LOAD("atigssr.bin", 0x0000, 0x2000, CRC(aca81498) SHA1(0d84c89487ee7a6ac4c9e73fdb30c5fd8aa595f8))
ROM_END

/************************************************************ Sanyo SX-16 **

******************************************************************************/

ROM_START( sx16 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("tmm27128ad.bin",0xc000, 0x4000, CRC(f8543362) SHA1(fef625e260ca89ba02174584bdc12db609f0780e))
ROM_END


/***************************************************** Schetmash Iskra-3104 ***

******************************************************************************/

static DEVICE_INPUT_DEFAULTS_START( iskr3104 )
	DEVICE_INPUT_DEFAULTS("DSW0", 0x30, 0x00)
DEVICE_INPUT_DEFAULTS_END

void pc_state::iskr3104(machine_config &config)
{
	/* basic machine hardware */
	i8086_cpu_device &maincpu(I8086(config, "maincpu", 4772720));
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc16_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc16_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5160_mb_device &mb(IBM5160_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(iskr3104));

	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "ega", false).set_option_default_bios("ega", "iskr3104"); // FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");
}

ROM_START( iskr3104 )
	ROM_REGION16_LE(0x10000,"bios", 0)
	ROMX_LOAD( "198.bin", 0xc000, 0x2000, CRC(bcfd8e41) SHA1(e21ddf78839aa51fa5feb23f511ff5e2da31b433),ROM_SKIP(1))
	ROMX_LOAD( "199.bin", 0xc001, 0x2000, CRC(2da5fe79) SHA1(14d5dccc141a0b3367f7f8a7188306fdf03c2b6c),ROM_SKIP(1))
	// EGA card from Iskra-3104
	//ROMX_LOAD( "143-03.bin", 0xc0001, 0x2000, CRC(d0706345) SHA1(e04bb40d944426a4ae2e3a614d3f4953d7132ede),ROM_SKIP(1))
	//ROMX_LOAD( "143-02.bin", 0xc0000, 0x2000, CRC(c8c18ebb) SHA1(fd6dac76d43ab8b582e70f1d5cc931d679036fb9),ROM_SKIP(1))
ROM_END


/************************************************************ Sharp PC-7000 ***

Links:  http://oldcomputers.net/sharp-pc7000.html , http://curtamania.com/curta/database/brand/sharp/Sharp%20PC-7000/index.html ,
        http://pcmuseum.de/pc7000.html
Form Factor: Luggable
CPU: 8086 @ 4.77 MHz or 7.37 MHz
RAM: 320K / 704K
Bus: no internal slots
Video: 80x24 text, 600x200 pixel graphics
Display: electroluminescent mono backlit (blue) LCD
Mass storage: 2x 5.25" 360K floppies
On board ports: serial, parallel
Options: Modem, color video output

******************************************************************************/

ROM_START( pc7000 )
	ROM_REGION16_LE(0x10000,"bios", 0)
	ROMX_LOAD("mitsubishi-m5l27128k-1.bin", 0x8000, 0x4000, CRC(9683957f) SHA1(4569eab6d88eb1bba0d553d1358e593c326978aa), ROM_SKIP(1))
	ROMX_LOAD("mitsubishi-m5l27128k-2.bin", 0x8001, 0x4000, CRC(99b229a4) SHA1(5800c8bafed26873d8cfcc79a05f93a780a31c91), ROM_SKIP(1))
ROM_END


/*************************************************** Siemens Sicomp PC16-05 ***

Links:  http://www.computerwoche.de/a/siemens-erweitert-pc-16-programm,1169752 ,
        http://www.phantom.sannata.ru/museum/siemens_pc_16_05.shtml
Info: Multitech PC/700 mainboard
Form Factor: Desktop
CPU: 8088 @ 4.77MHz / 8 MHz
RAM: 640KB
Bus: 6x ISA:    1) MDA/Hercules/CGA and parallel port
                2) Floppy, RTC and serial port
                3) (optional) MFM harddisk controller
Video: MDA/Hercules, exchangable via ISA-slot
Mass storage: 1x 5.25" 360K floppy and 1x 5.25" 360K floppy or MFM hard drive (10MB or 20MB)
On board ports: parallel, serial, beeper
Options: 8087 FPU
OSC: 24MHz, 1843.200KHz

Two blocks of dip switches, 8 switches each
******************************************************************************/

static DEVICE_INPUT_DEFAULTS_START( siemens )
	DEVICE_INPUT_DEFAULTS("DSW0", 0x30, 0x30)
DEVICE_INPUT_DEFAULTS_END

void pc_state::siemens(machine_config &config)
{
	/* basic machine hardware */
	i8088_cpu_device &maincpu(I8088(config, "maincpu", XTAL(24'000'000)/3)); /* 8.00 MHz */ // Turbo, can be changed to 4.77MHz
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc8_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc8_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5150_mb_device &mb(IBM5150_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(siemens));

	// FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "hercules", false);
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);
	ISA8_SLOT(config, "isa5", 0, "mb:isa", pc_isa8_cards, "hdc", false);
	ISA8_SLOT(config, "isa6", 0, "mb:isa", pc_isa8_cards, nullptr, false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");
}

ROM_START( sicpc1605 )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("multitech pc-700 3.1.bin", 0xe000, 0x2000, CRC(0ac7a2e1) SHA1(b9c8504e21213d81a068dde9f51f9c973d726e7b))

	// ROM for INS8039N-11 keyboard MCU in Siemens KB-097B/SI keyboard
	ROM_REGION(0x8000,"kbd", 0)
	ROM_LOAD("kb097b-st_v1.0.bin", 0x0000, 0x2000, CRC(5fc5903f) SHA1(5fc14d12800e22bb354e4b329c6ffc25efa0397c))
ROM_END


/***************************************************** VTech Laser Turbo XT ***

Links: http://minuszerodegrees.net/manuals.htm#VTech , http://minuszerodegrees.net/manuals/VTech/VTech%20-%20Laser%20Turbo%20XT%20-%20Brochure.pdf
Form Factor: Desktop
CPU: 8088 @ 4.77 MHz or 10 MHz
RAM: 512K / 640K, additionally 512K or 1M EMS on board
Bus: 8xISA:     1) Monochrome graphics/color graphics card
                2) Multi I/O Card (Floppy, 2x serial, parallel, game, RTC)
                3) (optional) hard disk controller
Video: MDA/CGA/Hercules
Mass storage: 2x 5.25" 360K floppies and 1 or 2 harddisks (20MB / 30MB / 40MB)
On board ports: speaker
Options: 8087 FPU

DIP settings:
SW1: 1    2    3    4    5    6    7    8    effect
     OFF                                     normal operation
      ON                                     Factory Testing only
           ON                                FPU absent
          OFF                                FPU present
               OFF  ON                       512K conventional memory
               OFF OFF                       640K conventional memory
                          ON   ON            normally on
                                    ON   ON  1 floppy drive
                                   OFF   ON  2
                                    ON  OFF  3
                                   OFF  OFF  4

Switch SW2 is used for the starting address for the expanded memory in the Turbo XT. If you have up to
one megabyte of expanded memory, the settings are easy: 512K: 01111111 1MB 01110111
Expanded memory is broken into two bundles as you install it. For example, with 1MB you have 512K in both
bundle 1 and bundle 2.
The positions of switches 1, 2 and 3 determine the address of the first bundle of expanded memory. Likewise,
switches 4, 5 and 6 determine the second bundle's address. Each 512K must have a unique starting address

SW2: 1/4  2/5  3/6  effect
     OFF   ON   ON  208h I/O port expanded memory
      ON  OFF   ON  218h
     OFF  OFF   ON  258h
      ON   ON  OFF  268h
     OFF   ON  OFF  2A8h
      ON  OFF  OFF  2B8h
     OFF  OFF  OFF  2E8h
      ON   ON   ON  bundle disabled

At 4.77MHz, memory accesses take four clock cycles (840ns), while I/O accesses take five clock
cycles (1050ns). At 10MHz, the internal RAM accesses take four cycles (400ns) while all other
memory accesses take 5 cycles (500ns). I/O accesses still take 5 cycles. However, the clock is
slowed down to 4.77MHz for all I/O accesses. The same is true for DMA cycles. This ensures the
Turbo XT is compatible with most expansion cards even when running at 10MHz.

******************************************************************************/

ROM_START( laser_turbo_xt )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("laser_turbo_xt.bin", 0x0e000, 0x02000, CRC(0a6121d3) SHA1(59b1f8dd6fe981ef9a7700adebf6e1adda7cee90)) // version 1.11 - 27c64d
ROM_END


/********************************************************* VTech Laser XT/3 ***

Links: http://minuszerodegrees.net/manuals.htm#VTech , http://th99.classic-computing.de/src/v/U-Z/52547.htm
Form Factor: Desktop
CPU: 8088 @ 4.77MHz or 10 MHz
RAM: 512K / 640K, additionally 512K or 1M EMS on board
Bus: 8x ISA:    1) Monochrome graphics/color graphics card http://th99.classic-computing.de/src/v/U-Z/52547.htm , alternatively an EGA card
                2) Multi I/O Card (Floppy, 2x serial, 1x parallel, game, RTC) http://th99.classic-computing.de/src/i/U-Z/52519.htm
                3) (optional) hard disk controller
Video: MDA/Hercules/CGA
Mass storage: 2x 5.25" 360K or 1x 5.25" 360K and 1x 3.5" 720K, additional harddisk optional
On board ports: speaker
Options: 8087 FPU

DIP settings:
SW1: 1    2    3    4    5    6    7    8    effect
     OFF                                     normal operation
      ON                                     Factory Testing only
           ON                                FPU absent
          OFF                                FPU present
                ON                           512K conventional memory
               OFF                           640K conventional memory
                    --                       not used
                          ON   ON            EGA or VGA
                         OFF   ON            CGA 40x25 mode
                          ON  OFF            CGA 80x25 mode
                         OFF  OFF            MDA or Hercules
                                    ON   ON  1 floppy drive
                                   OFF   ON  2
                                    ON  OFF  3
                                   OFF  OFF  4

SW2: 1    2    3    effect
     OFF   ON   ON  208h I/O port expanded memory
      ON  OFF   ON  218h
     OFF  OFF   ON  258h
      ON   ON  OFF  268h
     OFF   ON  OFF  2A8h
      ON  OFF  OFF  2B8h
     OFF  OFF  OFF  2E8h
      ON   ON   ON  expanded memory disabled

******************************************************************************/

ROM_START( laser_xt3 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("laser_xt3.bin", 0x0e000, 0x02000, CRC(b45a7dd3) SHA1(62f17c408be0036d00a182e94c5c88b83d46b625)) // version 1.26 - 27c64
ROM_END


/******************************************************** Zenith SuperSport ***

Links:  http://www.focushacks.com/zenith/myzenith.html , http://retro-computing.blogspot.de/2006/08/zds-supersport-laptop.html ,
        http://www.minuszerodegrees.net/manuals/Zenith%20Data%20Systems/ZDS%20SupersPort%20-%20Service%20Manual.pdf
        http://www.minuszerodegrees.net/manuals/Zenith%20Data%20Systems/ZDS%20SupersPort%20-%20User%20and%20Technical%20Manual.pdf
Info: ZWL-184 to distinguish it from the later 80286 based models
Form Factor: Laptop
CPU: 80C88 @ 4.77 MHz or 8 MHz
RAM: 640 KB
Bus: no internal slots
Video: CGA
Display: The second link has a picture of a working SuperSport. This shows the LCD display with a green background and blue text/graphics.
Mass storage: 1x 3.5" 720K floppy and 1x720K floppy or 20MB harddisk
On board ports: serial, parallel, ext. keyboard, ext. CGA video, ext. floppy
Options: 2400 Baud Modem, 8087 FPU
******************************************************************************/

void pc_state::zenith_map(address_map &map)
{
	map.unmap_value_high();
	map(0xf0000, 0xf7fff).ram();
	map(0xf8000, 0xfffff).rom().region("bios", 0x8000);
}

void pc_state::zenith(machine_config &config)
{
	/* basic machine hardware */
	i8088_cpu_device &maincpu(I8088(config, "maincpu", XTAL(14'318'181)/3)); /* 4.77 MHz */
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::zenith_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc8_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5150_mb_device &mb(IBM5150_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(pccga));

	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "cga", false); // FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false).set_option_machine_config("fdc_xt", cfg_dual_720K);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("128K, 256K, 512K");

	/* software lists */
	SOFTWARE_LIST(config, "disk_list").set_original("ibm5150");
}

ROM_START( zdsupers )
	ROM_REGION(0x10000,"bios", 0)
	ROM_SYSTEM_BIOS( 0, "v31d", "v3.1d" )
	ROMX_LOAD("z184m v3.1d.10d", 0x8000, 0x8000, CRC(44012c3b) SHA1(f2f28979798874386ca8ba3dd3ead24ae7c2aeb4), ROM_BIOS(0))
	ROM_SYSTEM_BIOS( 1, "v29e", "v2.9e" )
	ROMX_LOAD("z184m v2.9e.10d", 0x8000, 0x8000, CRC(de2f200b) SHA1(ad5ce601669a82351e412fc6c1c70c47779a1e55), ROM_BIOS(1))
ROM_END

/****************************************************** Zenith Z-150 series ***

Form factor: Desktop
Bus: 8 slot passive backplane: 1) CPU/memory card
                               2) Floppy/Video card (color and monochrome)
CPU: Intel 8088 4.77MHz/8MHz, FPU socket present
RAM: up to 640KB
Mass storage: 2xDSDD 5.25" floppy disks / DSDD 5.25" floppy disk + winchester
on board: speaker


DIP settings:
SW202: 1    2    3    4    effect
       ON                  60Hz display frequency
      OFF                  50Hz display frequency
            ON             autoboot floppy drive
           OFF             autoboot winchester
                 ON        floppy controller not installed
                OFF        floppy controller installed
                      ON   color video adapter
                     OFF   monochrome video adapter

Pres "ESC" during powerup to enter the ROM monitor

******************************************************************************/

ROM_START( zdz150 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("444-260-18.bin", 0x8000, 0x4000, CRC(685208fe) SHA1(a1384627e8ecfd93842f6eabda4a417dd92be6df))
	ROM_LOAD("444-229-18.bin", 0xc000, 0x4000, CRC(a6078b8a) SHA1(9a970013f5109a5003365eb2923cc26f08516dcb))
ROM_END

/****************************************************** Zenith Z-160 series ***

Form factor: (Trans-)Portable

******************************************************************************/

ROM_START( zdz160 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("f800ffff.rom", 0x8000, 0x8000, CRC(46dd9695) SHA1(beaf6b45cecdadf630a94902fa84006bf00e2b3d))
ROM_END

/************************************************************** CompuAdd 810 **

http://mkgraham.dx.am/810.html
https://smg.photobucket.com/user/zzm113/library?page=1

CPU: NEC V20@4.77MHZ/5.15MHZ/9.54 MHz / FPU socket provided
on board: dual disk drive controller, dual IDE hard drive interface, 2xser, game
Bus: 5 ISA slots on a riser
RAM: 640KB
mass storage: 5.25" DSDD floppy drive
options: 20MB/40MB hard drive, RTC

System has an AT style enhanced keyboard, despite providing that, the emulated 810
emits a steady beep and waits for F1 to be pressed.
SW1 and SW2 DIP switch blocks

******************************************************************************/

void pc_state::cadd810(machine_config &config)
{
	pccga(config);
	config.device_remove("kbd");
	PC_KBDC_SLOT(config, "kbd", pc_at_keyboards, STR_KBD_IBM_PC_AT_101).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));
}

ROM_START( cadd810 )
	ROM_REGION(0x10000,"bios", 0) // continuous beep, complains about missing keyboard
	ROM_LOAD("compuadd810.bin",0xc000, 0x4000, CRC(39dc8f28) SHA1(c0d50186db30c924fad7d42d4aefb7ae8dd32c7d))
	ROM_REGION(0x2000,"ide", 0)
	ROM_LOAD("wd_ide_bios_rev_2.0.bin",0x0000,0x2000, NO_DUMP) //missing: dump of hd controller
ROM_END

/**************************************** JUKO NEST N3 true 16 bit variants ***

https://www.vogons.org/viewtopic.php?f=46&t=60077
https://sites.google.com/site/misterzeropage/
http://www.vcfed.org/forum/showthread.php?67127-Juko-nest-n3

CPU: 8086 and V30, 4.77MHz/7.16MHz/10MHz hardware or software selectable
Bus: 8 ISA slots, dynamic bus speed control
RAM: 512K/640K/1MB on board, EMS 4.0 support (384K on board can be configured either as
RAMDISK in extended memory or EMS in expanded memory

key commands: [Ctrl]-[Alt]-[1]/[2]/[3] to select CPU speed after running CONTROL.COM

DIP switches: (SW3 to SW8 are autodetected by the NEST BIOS, they need to be set if another BIOS is used).
SWA: SW1  SW2  SW3  SW4  SW5  SW6  SW7  SW8  effect
     ---                                     reserved, ON/OFF don't matter
           ON                                no 8087
          OFF                                8087 present
                ON   ON                      0KB memory size
               OFF   ON                      512KB
                ON  OFF                      640KB
               OFF  OFF                      1MB
                          ON   ON            EGA
                         OFF   ON            CGA 40x25
                          ON  OFF            CGA 80x25
                         OFF  OFF            MDA
                                    ON   ON  1 diskette drive
                                   OFF   ON  2
                                    ON  OFF  3
                                   OFF  OFF  4
******************************************************************************/

void pc_state::juko16(machine_config &config)
{
	/* basic machine hardware */
	v30_device &maincpu(V30(config, "maincpu", 4772720));
	maincpu.set_addrmap(AS_PROGRAM, &pc_state::pc16_map);
	maincpu.set_addrmap(AS_IO, &pc_state::pc16_io);
	maincpu.set_irq_acknowledge_callback("mb:pic8259", FUNC(pic8259_device::inta_cb));

	ibm5160_mb_device &mb(IBM5160_MOTHERBOARD(config, "mb", 0));
	mb.set_cputag(m_maincpu);
	mb.int_callback().set_inputline(m_maincpu, 0);
	mb.nmi_callback().set_inputline(m_maincpu, INPUT_LINE_NMI);
	mb.set_input_default(DEVICE_INPUT_DEFAULTS_NAME(pccga));

	ISA8_SLOT(config, "isa1", 0, "mb:isa", pc_isa8_cards, "cga", false); // FIXME: determine ISA bus clock
	ISA8_SLOT(config, "isa2", 0, "mb:isa", pc_isa8_cards, "fdc_xt", false);
	ISA8_SLOT(config, "isa3", 0, "mb:isa", pc_isa8_cards, "lpt", false);
	ISA8_SLOT(config, "isa4", 0, "mb:isa", pc_isa8_cards, "com", false);

	/* keyboard */
	PC_KBDC_SLOT(config, "kbd", pc_xt_keyboards, STR_KBD_IBM_PC_XT_83).set_pc_kbdc_slot(subdevice("mb:pc_kbdc"));

	/* internal ram */
	RAM(config, RAM_TAG).set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K");
}

ROM_START( juko16 )
	ROM_REGION16_LE(0x10000, "bios", 0)
	ROM_SYSTEM_BIOS(0, "v107", "v1.07")
	ROMX_LOAD("c22.bin", 0xc000, 0x2000, BAD_DUMP CRC(e947237b) SHA1(65e84675752a4deb0d0712e2aba8c0735959b43a),ROM_BIOS(0))
	ROMX_LOAD("c24.bin", 0xe000, 0x2000, BAD_DUMP CRC(1d3246e4) SHA1(4ff875d15b1231a2464dfe08e480c637fa0c4613),ROM_BIOS(0))
	ROM_SYSTEM_BIOS(1, "v201", "v2.01")
	ROMX_LOAD("juko_nest_odd.bin", 0xc000, 0x2000, CRC(2bfa545f) SHA1(1cdaf90323cbed3224b4b8863bf27e709be6a73a),ROM_BIOS(1))
	ROMX_LOAD("juko_nest_even.bin", 0xe000, 0x2000, CRC(2bfa545f) SHA1(1cdaf90323cbed3224b4b8863bf27e709be6a73a),ROM_BIOS(1))
ROM_END


/****************************************************** Hyosung Topstar 88T ***

http://minuszerodegrees.net/xt_clone_bios/xt_clone_bios.htm

CPU: 8088, FPU socket provided
RAM: 27xKM41256AP-15 (768KB)
Bus: 5xISA
on board:  'Paradise' CGA (ROM not dumped), floppy controller  (connector labelled DISC)  (supporting 4 drives on the one connector), RTC
    par(connector labelled PR), 2xser(connectors labelled ASYNC1 and ASYNC2), Light pen connector
OSC: 22.440000MHz, 14.31818, 16MHz, 1.8432MHz

DIP switches: one block of 8 DIP switches

******************************************************************************/

ROM_START( hyo88t )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD( "hyosung_topstar_88t_v3.0.bin", 0xc000, 0x4000, CRC(2429046b) SHA1(e2a8e1ffdd4c6ff84791f486df3204811fa5f589))
ROM_END

/*************************************************************** Kyocera XT ***

http://www.hampa.ch/pce/download.html

******************************************************************************/

ROM_START( kyoxt )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD( "kyocera.rom", 0xc000, 0x4000, CRC(cd732ac6) SHA1(7258fc18565135870e31962e4bd528a06d1ee0e0))
ROM_END

/*********************Panasonic Sr. Partner / *** Nixdorf 8810/25 CPC - PC01 ***

Luggable machine with a 9" monochrome enhanced CGA display and an electrostatic printer
ROM is identical between the Nixdorf and the Panasonic
Displays "PIT1 ERROR"

CPU: Intel 8088 @ 4.77MHz
RAM: 256KB
Monitor: 9" amber
Bus: 2xISA
mass storage: 2xDSDD 5.25"
integrated thermal printer, 80/132 characters per line, Epson MX 80 compatible
on board: parallel port, serial port, RGB port for color monitor

The version 8810/25 CPC has 256KB RAM on the mainboard, a harddisk and RAM can be expanded on the harddisk controller by 320/512KB
in addition to the DIP switches on the mainboard, DIP switches on the HD controller have to be set.

DIP switches: 1    2    3    4    5    6    7    8    effect
              ON  OFF   ON   ON   ON                  128 KB RAM
              ON  OFF  OFF   ON   ON                  256
              ON   ON   ON  OFF   ON                  320
              ON  OFF   ON  OFF   ON                  384
              ON   ON  OFF  OFF   ON                  448
              ON  OFF  OFF  OFF   ON                  512
              ON   ON   ON   ON  OFF                  576
              ON  OFF   ON   ON  OFF                  640
                                       OFF            8087 present
                                        ON            8087 absent
                                             OFF      80 char/line
                                              ON      40 char/line
                                                  OFF 1 FDD
                                                   ON 2 FDD

******************************************************************************/

ROM_START( nixpc01 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD( "nx01.bin", 0xc000, 0x4000, CRC(b0a75d1f) SHA1(7c2890eced917969968fc2e7491cda90a9734e03))
ROM_END

/***************************************************** Leading Edge Model D ***

Those use an Intel Wildcard 88, a XT computer sans slots and DRAM on a SIMM like module
Chipset: Faraday FE2010A

0300-031F Clock port

******************************************************************************/

ROM_START( ledgmodd )
	ROM_REGION(0x10000, "bios", 0)
	// 0: blank display
	ROM_SYSTEM_BIOS(0, "le", "Leading Edge")
	ROMX_LOAD( "wildcard_88-the_leading_edge-model_d-le_303-27.bin", 0xc000, 0x4000, CRC(cc05347d) SHA1(c44f3ce56472e0894ab955a14f6a91a3fb876baf), ROM_BIOS(0) )
	// 1: blank display
	ROM_SYSTEM_BIOS(1, "daewoo", "Daewoo")
	ROMX_LOAD( "wildcard_88-the_leading_edge-model_d-daewoo-pn_23096023.bin", 0xc000, 0x4000, CRC(34f5fa32) SHA1(73c0489532a1f9a0b23bdd1865cd8b0c6f131ad9), ROM_BIOS(1) )
	// 2: Phoenix 8088 ROM BIOS Version 2.52 / P E Nelson - No scancode from keyboard
	ROM_SYSTEM_BIOS(2, "wildcard", "Wildcard")
	ROMX_LOAD( "wildcard7354-1001rev2.52.05.bin", 0x8000, 0x8000, CRC(ea0c4c2f) SHA1(d817f57dd5332a943b33826dbe67b23e4c94a6ca), ROM_BIOS(2) )
ROM_END

/******************************************************Leading Edge Model M ***

aka the Sperry PC, the "Sperry HT - 4.71 Bios" that can be found online is identical to the v.4.71 below
E-TD10 - TOD Error
acording to http://www.o3one.org/hwdocs/bios_doc/dosref22.html this machine had AT-like RTC services
The "M" stood for a Mitsubishi made machine, the "Leading Edge Model D" was made by Daewoo
Works with the "siemens" config, so instead of duplicating it until more is known we'll use that.

Interrupt 1Ah  Time of Day, Function 02h, 03h, 04h, 05h are valid on the Model M

******************************************************************************/

ROM_START( ledgmodm )
	ROM_REGION(0x10000, "bios", 0)
	ROM_SYSTEM_BIOS(0, "v330", "Version 3.30")
	ROMX_LOAD( "leading_edge-model_m-version_3.30.bin", 0xc000, 0x4000, CRC(386dd187) SHA1(848ccdc8209c24478a4f75dd941760c43d3bc732), ROM_BIOS(0) )
	ROM_SYSTEM_BIOS(1, "v471", "Version 4.71")
	ROMX_LOAD( "leading_edge-model_m-version_4.71.bin", 0xc000, 0x4000, CRC(0d5d8bee) SHA1(6c35adf6a8da149e420b5aa8dd0e18e02488cfa0), ROM_BIOS(1) )
ROM_END

/************************************** CCI Micromint MPX-16 PC Motherboard ***

Circuit Cellar Project
The ROMs are marked "Micromint MPX16 5/8 PC/Term 3/1/84"
hangs on boot, maybe they are waiting for a serial connection

One block of eight DIP switches

******************************************************************************/

ROM_START( mpx16 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("mpx16u84.bin", 0xe000, 0x1000, CRC(8a557a25) SHA1(90f8112c094cc0ac44c2d5d43fbb577333dfc165))
	ROM_LOAD("mpx16u85.bin", 0xf000, 0x1000, CRC(42097571) SHA1(2acaca033242e35e512b30b2233da02bde561cc3))
ROM_END

/*************************************************** Vendex HeadStart Plus ***

Samsung manufactured - Chipset: Faraday FE2010A - "Keyboard Error or no keyboard present"
On-board: FDC

******************************************************************************/
ROM_START( hstrtpls )
	ROM_REGION(0x10000,"bios", 0)
	ROM_LOAD("bios.bin",  0xc000, 0x04000, CRC(19d705f8) SHA1(5e607fec6b533bc59d8d804e399bb9d438d6999d))
ROM_END

/************************************************* Philips NMS 9100 series ***
Desktop

*****************************************************************************/

ROM_START( nms9100 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_SYSTEM_BIOS(0, "pcrom12", "PC ROM 1.2") // there is also a 1.5 yet undumped
	ROMX_LOAD("philipsnms9100.bin", 0xc000, 0x4000, CRC(3c1cfa16) SHA1(d060501588b451b0f4a816bede65eafb514b9603), ROM_BIOS(0)) // Philips PC ROM 1.2
	ROM_SYSTEM_BIOS(1, "v313", "Philips ROM BIOS Version 3.13") // from a P3120, use Hercules
	ROMX_LOAD("philips_p3120.bin", 0x8000, 0x8000, CRC(0370e9e6) SHA1(61017e36b9f34f163970cdd2bb3ffd9f66e57382), ROM_BIOS(1))
	ROM_SYSTEM_BIOS(2, "5017", "5017") // no display
	ROMX_LOAD("philipsxt.bin", 0x8000, 0x8000, CRC(2f3135e7) SHA1(d2fc4c06cf09e2c5a62017f0977b084be8bf9bbd), ROM_BIOS(2))
ROM_END

/************************************************* AEG Olympia Olystar 20F ***
Form Factor: Desktop
uses an Acer 710IIN motherboard, BIOS-Version 4.06
CPU: AMD P8088-1, FPU socket available
Chips: Acer M1101, 2201A, UM8250B, WD37C65B-PL , Paradise PVC4
OSC: 14.31818, 30.000000MHz, 16.000, 1.832
RAM: 640K (256K, 512K, 768K, 1024K)
Bus: two ISA8 slots on a riser card
Video: Hercules/CGA compatible, on board
Mass storage: Floppy 720KB, HD 20MB on WD MFM-controller
On board ports: parallel, serial, Video, keyboard (Mini-DIN)

*****************************************************************************/

void pc_state::olystar20f(machine_config &config)
{
	pccga(config);

	subdevice<isa8_slot_device>("isa2")->set_option_machine_config("fdc_xt", cfg_single_720K);
	subdevice<isa8_slot_device>("isa3")->set_default_option(nullptr);
	subdevice<isa8_slot_device>("isa5")->set_default_option("hdc");
	subdevice<ram_device>(RAM_TAG)->set_default_size("640K").set_extra_options("64K, 128K, 256K, 512K, 768K, 1024K"); // the BIOS detects 2432KB extension RAM in the 640K setting ...
}

ROM_START( olystar20f )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("20f_ebios_u43_v4.06.bin", 0x8000, 0x8000, CRC(0dddb623) SHA1(d821f48ddc7c77868b3f5952fa12f41911bea406))

	ROM_REGION(0x2000,"gfx1", 0) // on board PVC4 based graphics card (similar to Commodore PC AGA and Schneider EuroPC)
	ROM_LOAD("20f_u11_v1.3.bin", 0x0000, 0x2000, CRC(d252ee8d) SHA1(035385521abc3d1b79967b5302a87d08f9383215))
ROM_END

/********************************************************* Cordata PPC-400 ***
Form factor: Luggable
Links: https://www.system-cfg.com/detailcollection.php?ident=243
CPU: 8088/4.77MHz
RAM: 256K or 512K
Mass storage: 1/2 floppy disks 5.25" DD, 10MB or 20MB harddisk
On board: serial, parallel, video (CGA, Hercules, 640x400 mode)
Monitor: 9" monochrome

*****************************************************************************/

void pc_state::coppc400(machine_config &config)
{
	pccga(config);

	subdevice<ram_device>(RAM_TAG)->set_default_size("512K").set_extra_options("256K");
	// the top 16K of the 512K are used for graphics even if a RAM expansion card is used
}

ROM_START( coppc400 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("f800ffff.rom", 0x8000, 0x8000, CRC(3d9b6594) SHA1(41f85e692e2020326fd580f7c436c23c76840119))
ROM_END

/************************************************************ Thomson TO16 ***
Form factor: Desktop
CPU: 8088 9.54MHz / 4.77MHz, FPU socket provided
RAM: 512KB-768KB
ROM: 32KB ROM BIOS, 16KB character generator (not dumped)
On board video: Plantronics Colorplus (MDA/Hercules/CGA/Plantronics), EGA card (GB100) on XPHD
On board: RS232C, parallel
Mass storage: 1x5.25" DS/DD (TO16PCDD / TO16 XP), additional 20MB HDD (TO16 XP HD)
Options: Modem (TO16 PCM, ISA card)
ISA8: 2 (PC, PCM), 4 (XPDD, XPHD)
RTC: on XPDD and XPHD

*****************************************************************************/

ROM_START( to16 )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("to16_103.bin", 0x8000, 0x8000, CRC(a2d55e16) SHA1(fcc61bbfe49164c4b79c368fb782d1ecc17e0a42))
ROM_END

/********************************************************** Sanyo SPC-400D ***
ROM BIOS Version 1.18
*****************************************************************************/

ROM_START( spc400d )
	ROM_REGION(0x10000, "bios", 0)
	ROM_LOAD("fb896.u6", 0xc000, 0x4000, CRC(a6f3ad8c) SHA1(1ee012f9a1757eb68150fedc9db16ff356722f72))
ROM_END

/***************************************************************************

  Game driver(s)

***************************************************************************/

//    YEAR  NAME            PARENT   COMPAT  MACHINE         INPUT     CLASS     INIT           COMPANY                            FULLNAME                 FLAGS
COMP( 1989, mk88,           ibm5150, 0,      mk88,           pccga,    pc_state, empty_init,    "<unknown>",                       "MK-88",                 MACHINE_NOT_WORKING )
COMP( 1991, poisk2,         ibm5150, 0,      poisk2,         pccga,    pc_state, empty_init,    "<unknown>",                       "Poisk-2",               MACHINE_NOT_WORKING )
COMP( 1990, mc1702,         ibm5150, 0,      eagle1600,      pccga,    pc_state, empty_init,    "<unknown>",                       "Elektronika MC-1702",   MACHINE_NOT_WORKING )
COMP( 198?, olystar20f,     ibm5150, 0,      olystar20f,     pccga,    pc_state, empty_init,    "AEG Olympia",                     "Olystar 20F",           MACHINE_NOT_WORKING )
COMP( 198?, olytext30,      ibm5150, 0,      olytext30,      pccga,    pc_state, empty_init,    "AEG Olympia",                     "Olytext 30",            MACHINE_NOT_WORKING )
COMP( 1987, ataripc1,       ibm5150, 0,      ataripc1,       pccga,    pc_state, empty_init,    "Atari",                           "PC1",                   0 )
COMP( 1988, ataripc3,       ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Atari",                           "PC3",                   0 )
COMP( 1985, bw230,          ibm5150, 0,      bondwell,       bondwell, pc_state, init_bondwell, "Bondwell Holding",                "BW230 (PRO28 Series)",  0 )
COMP( 1982, mpc1600,        ibm5150, 0,      mpc1600,        pccga,    pc_state, empty_init,    "Columbia Data Products",          "MPC 1600",              0 )
COMP( 198?, coppc400,       ibm5150, 0,      coppc400,       pccga,    pc_state, empty_init,    "Corona Data Systems, Inc.",       "Cordata PPC-400",       MACHINE_NOT_WORKING )
COMP( 1983, comport,        ibm5150, 0,      comport,        pccga,    pc_state, empty_init,    "Compaq",                          "Compaq Portable",       MACHINE_NOT_WORKING )
COMP( 198?, cadd810,        ibm5150, 0,      cadd810,        pccga,    pc_state, empty_init,    "CompuAdd",                        "810",                   MACHINE_NOT_WORKING )
COMP( 1984, dgone,          ibm5150, 0,      dgone,          pccga,    pc_state, empty_init,    "Data General",                    "Data General/One" ,     MACHINE_NOT_WORKING )
COMP( 1983, eagle1600,      ibm5150, 0,      eagle1600,      pccga,    pc_state, empty_init,    "Eagle",                           "Eagle 1600" ,           MACHINE_NOT_WORKING )
COMP( 1983, eaglespirit,    ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Eagle",                           "Eagle PC Spirit",       MACHINE_NOT_WORKING )
COMP( 198?, eaglepc2,       ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Eagle",                           "PC-2",                  MACHINE_NOT_WORKING )
COMP( 1985, eppc,           ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Ericsson Information System",     "Ericsson Portable PC",  MACHINE_NOT_WORKING )
COMP( 198?, hyo88t,         ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Hyosung",                         "Topstar 88T",           MACHINE_NOT_WORKING )
COMP( 1983, ibm5550,        ibm5150, 0,      ibm5550,        pccga,    pc_state, empty_init,    "International Business Machines", "5550",                  MACHINE_NOT_WORKING )
COMP( 1984, ittxtra,        ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "ITT Information Systems",         "ITT XTRA",              MACHINE_NOT_WORKING )
COMP( 198?, juko16,         ibm5150, 0,      juko16,         pccga,    pc_state, empty_init,    "JUKO",                            "NEST 8086 and V30",     MACHINE_NOT_WORKING )
COMP( 1985, kaypro16,       ibm5150, 0,      kaypro16,       pccga,    pc_state, empty_init,    "Kaypro Corporation",              "Kaypro 16",             0 )
COMP( 198?, kaypropc,       ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Kaypro Corporation",              "PC",                    MACHINE_NOT_WORKING )
COMP( 198?, kyoxt,          ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Kyocera",                         "XT",                    MACHINE_NOT_WORKING )
COMP( 198?, ledgmodd,       ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Leading Edge Hardware Products, Inc.", "Model D",          MACHINE_NOT_WORKING )
COMP( 198?, ledgmodm,       ibm5150, 0,      siemens,        pccga,    pc_state, empty_init,    "Leading Edge Hardware Products, Inc.", "Model M",          MACHINE_NOT_WORKING )
COMP( 198?, mpx16,          ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Micromint",                       "MPX-16",                MACHINE_NOT_WORKING )
COMP( 1985, ncrpc4i,        ibm5150, 0,      ncrpc4i,        pccga,    pc_state, empty_init,    "NCR",                             "PC4i",                  MACHINE_NOT_WORKING )
COMP( 198?, nixpc01,        ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Nixdorf Computer AG",             "8810/25 CPC - PC01",    MACHINE_NOT_WORKING )
COMP( 198?, olivm15,        ibm5150, 0,      m15,            pccga,    pc_state, empty_init,    "Olivetti",                        "M15",                   0 )
COMP( 198?, nms9100,        ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Philips",                         "NMS 9100",              MACHINE_NOT_WORKING )
COMP( 1989, ssam88s,        ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Samsung",                         "Samtron 88S",           MACHINE_NOT_WORKING )
COMP( 1988, sx16,           ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Sanyo",                           "SX-16",                 MACHINE_NOT_WORKING )
COMP( 198?, mbc16,          ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Sanyo",                           "MBC-16",                MACHINE_NOT_WORKING )
COMP( 198?, spc400d,        ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Sanyo",                           "SPC-400D",              MACHINE_NOT_WORKING )
COMP( 1992, iskr3104,       ibm5150, 0,      iskr3104,       pccga,    pc_state, empty_init,    "Schetmash",                       "Iskra 3104",            MACHINE_NOT_WORKING )
COMP( 1985, sicpc1605,      ibm5150, 0,      siemens,        pccga,    pc_state, empty_init,    "Siemens",                         "Sicomp PC16-05",        MACHINE_NOT_WORKING )
COMP( 1985, pc7000,         ibm5150, 0,      eagle1600,      pccga,    pc_state, empty_init,    "Sharp",                           "PC-7000",               MACHINE_NOT_WORKING )
COMP( 1987, to16,           ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Thomson SIMIV",                   "TO16",                  MACHINE_NOT_WORKING )
COMP( 198?, hstrtpls,       ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "Vendex",                          "HeadStart Plus",        MACHINE_NOT_WORKING )
COMP( 1988, laser_turbo_xt, ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "VTech",                           "Laser Turbo XT",        0 )
COMP( 1989, laser_xt3,      ibm5150, 0,      pccga,          pccga,    pc_state, empty_init,    "VTech",                           "Laser XT/3",            0 )
COMP( 1987, zdsupers,       ibm5150, 0,      zenith,         pccga,    pc_state, empty_init,    "Zenith Data Systems",             "SuperSport",            0 )
COMP( 198?, zdz150,         ibm5150, 0,      zenith,         pccga,    pc_state, empty_init,    "Zenith Data Systems",             "Z-150 series",          0 )
COMP( 198?, zdz160,         ibm5150, 0,      zenith,         pccga,    pc_state, empty_init,    "Zenith Data Systems",             "Z-160 series",          0 )
