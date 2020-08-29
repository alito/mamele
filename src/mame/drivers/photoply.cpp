// license:BSD-3-Clause
// copyright-holders:Angelo Salese
/*******************************************************************************************************

Photo Play (c) 199? Funworld

Preliminary driver by Angelo Salese

TODO:
- BIOS CMOS doesn't save at all (needed for setting up the Hard Disk);
- DISK BOOT FAILURE after EEPROM checking (many unknown IDE cs1 reads/writes);
- Partition boot sector is missing from the CHD dump, protection?
- Detects CPU type as "-S 16 MHz"? Sometimes it detects it as 486SX, unknown repro (after fiddling with CMOS settings anyway)
- VGA BIOS reports being a Cirrus Logic GD5436 / 5446, it is unknown what exactly this game uses.
- PCI hookups (no idea about what this uses), and improve/device-ify SiS85C49x;
- ISA bus cards are completely guessworked;
- EEPROM timings are hacked (writes mostly fail otherwise);
- Eventually needs AudioDrive ES688 / ES1688 / ES1788 & ES1868 devices and serial ports "for linking" before actually booting;


*******************************************************************************************************/

#include "emu.h"
#include "cpu/i386/i386.h"
#include "machine/eepromser.h"
#include "machine/lpci.h"
#include "machine/pcshare.h"
#include "machine/pckeybrd.h"
#include "machine/idectrl.h"
#include "video/clgd542x.h"

class photoply_state : public pcat_base_state
{
public:
	photoply_state(const machine_config &mconfig, device_type type, const char *tag) :
		pcat_base_state(mconfig, type, tag),
		m_eeprom(*this, "eeprom"),
		m_main_bios(*this, "bios"),
		m_video_bios(*this, "video_bios"),
		m_ex_bios(*this, "ex_bios")
	{
	}

	void photoply(machine_config &config);
	void photoply_dx4_100(machine_config &config);

private:
	required_device<eeprom_serial_93cxx_device> m_eeprom;

	required_region_ptr<uint8_t> m_main_bios;
	required_region_ptr<uint8_t> m_video_bios;
	required_region_ptr<uint8_t> m_ex_bios;

	uint8_t *m_shadow_ram;

	uint8_t bios_r(offs_t offset);
	void bios_w(offs_t offset, uint8_t data);
	void eeprom_w(uint8_t data);

	uint16_t m_pci_shadow_reg;

	void photoply_io(address_map &map);
	void photoply_map(address_map &map);

	virtual void machine_start() override;
	virtual void machine_reset() override;

	uint32_t sis_pcm_r(int function, int reg, uint32_t mem_mask);
	void sis_pcm_w(int function, int reg, uint32_t data, uint32_t mem_mask);
};

// regs 0x00-0x3f both devices below
// regs 0x40-0x7f SiS85C496 PCI & CPU Memory Controller (PCM)
// regs 0x80-0xff SiS85C497 AT Bus Controller & Megacell (ATM)
uint32_t photoply_state::sis_pcm_r(int function, int reg, uint32_t mem_mask)
{
	uint32_t r = 0;

	//printf("PCM %02x %08x\n",reg,mem_mask);
	if(reg == 0)
	{
		// Device ID / Vendor ID
		return 0x04961039;
	}

	if(reg == 8)
	{
		// Device Class Code / Device Revision Identification
		return 0x06000002;
	}

	// Device Header Type
	if(reg == 0xc)
		return 0;

	if(reg == 0x44)
	{

		if(ACCESSING_BITS_8_15) // reg 0x45
			r |= (m_pci_shadow_reg & 0xff00);
		if(ACCESSING_BITS_0_7) // reg 0x44
			r |= (m_pci_shadow_reg & 0x00ff);
	}
	return r;
}

void photoply_state::sis_pcm_w(int function, int reg, uint32_t data, uint32_t mem_mask)
{
	if(reg == 0x44)
	{

		/*
		 * reg 0x45
		 * shadow RAM control
		 * xxxx ---- <reserved>
		 * ---- x--- Internal Cache Cacheable Area Control
		 * ---- -x-- PCI,ISA Master Access Shadow RAM Area Enable
		 * ---- --x- Shadow RAM Read Control (1=Enable)
		 * ---- ---x Shadow RAM Write Control (0=Enable)
		 */
		if(ACCESSING_BITS_8_15)
			m_pci_shadow_reg = (data & 0xff00) | (m_pci_shadow_reg & 0x00ff);

		/*
		 * shadow RAM enable:
		 * bit 7: 0xf8000-0xfffff shadow RAM enable
		 * ...
		 * bit 0: 0xc0000-0xc7fff shadow RAM enable
		 */
		if(ACCESSING_BITS_0_7) // reg 0x44
			m_pci_shadow_reg = (data & 0x00ff) | (m_pci_shadow_reg & 0xff00);

		//printf("%04x\n",m_pci_shadow_reg);
	}
}

uint8_t photoply_state::bios_r(offs_t offset)
{
	uint8_t bit_mask = (offset & 0x38000) >> 15;

	if((m_pci_shadow_reg & 0x200) == 0x200)
	{
		if(m_pci_shadow_reg & (1 << bit_mask))
			return m_shadow_ram[offset];
	}

	// TODO: this mapping is a complete guesswork
	// TODO: worth converting this to bankdev when PCI gets de-legacized
	switch(bit_mask & 7)
	{
		// cirrus logic video bios
		case 0: // c0000-c7fff
			return m_video_bios[offset & 0x7fff];
		// multifunction board bios
		// Note: if filled with a reload on 0x4000-0x7fff it will repeat the "Combo I/O" EEPROM check (which looks unlikely)
		case 1: // c8000-cffff
			if ((offset & 0x7fff) == 0x3fff)
			{
				// other bits are unknown
				return m_eeprom->do_read() << 2;
			}
			return m_ex_bios[offset & 0x7fff];
		case 2: // d0000-dffff
		case 3:
			return 0;
	}

	// e0000-fffff
	return m_main_bios[offset & 0x1ffff];
}

void photoply_state::bios_w(offs_t offset, uint8_t data)
{
//  return m_bios[offset];

	if((m_pci_shadow_reg & 0x100) == 0)
	{
		uint8_t bit_mask = (offset & 0x38000) >> 15;

		if(m_pci_shadow_reg & (1 << bit_mask))
			m_shadow_ram[offset] = data;
	}
}

void photoply_state::eeprom_w(uint8_t data)
{
	//logerror("Writing %X to EEPROM output port\n", data);
	m_eeprom->di_write(BIT(data, 0));
	m_eeprom->clk_write(BIT(data, 1));
	m_eeprom->cs_write(BIT(data, 2));

	// Bits 2-3 also seem to be used to bank something else.
	// Bits 4-7 are set for some writes, but may do nothing?
}

void photoply_state::photoply_map(address_map &map)
{
	map(0x00000000, 0x0009ffff).ram();
	map(0x000a0000, 0x000bffff).rw("vga", FUNC(cirrus_gd5446_device::mem_r), FUNC(cirrus_gd5446_device::mem_w));
//  map(0x000c0000, 0x000c7fff).rom().region("video_bios", 0);
//  map(0x000c8000, 0x000cffff).rom().region("ex_bios", 0);
	map(0x000c0000, 0x000fffff).rw(FUNC(photoply_state::bios_r), FUNC(photoply_state::bios_w));
	map(0x00100000, 0x07ffffff).ram(); // 64MB RAM, guess!
	map(0xfffe0000, 0xffffffff).lr8([this] (offs_t offset) { return m_main_bios[offset]; }, "bios_upper_r");
}

void photoply_state::photoply_io(address_map &map)
{
	pcat32_io_common(map);
	map(0x00e8, 0x00eb).noprw();

	map(0x0170, 0x0177).rw("ide2", FUNC(ide_controller_32_device::cs0_r), FUNC(ide_controller_32_device::cs0_w));
	map(0x01f0, 0x01f7).rw("ide", FUNC(ide_controller_32_device::cs0_r), FUNC(ide_controller_32_device::cs0_w));
	map(0x0202, 0x0202).w(FUNC(photoply_state::eeprom_w));
//  map(0x0278, 0x027f).ram(); //parallel port 2
	map(0x0370, 0x0377).rw("ide2", FUNC(ide_controller_32_device::cs1_r), FUNC(ide_controller_32_device::cs1_w));
//  map(0x0378, 0x037f).ram(); //parallel port
	map(0x03b0, 0x03bf).rw("vga", FUNC(cirrus_gd5446_device::port_03b0_r), FUNC(cirrus_gd5446_device::port_03b0_w));
	map(0x03c0, 0x03cf).rw("vga", FUNC(cirrus_gd5446_device::port_03c0_r), FUNC(cirrus_gd5446_device::port_03c0_w));
	map(0x03d0, 0x03df).rw("vga", FUNC(cirrus_gd5446_device::port_03d0_r), FUNC(cirrus_gd5446_device::port_03d0_w));

	map(0x03f0, 0x03f7).rw("ide", FUNC(ide_controller_32_device::cs1_r), FUNC(ide_controller_32_device::cs1_w));

	map(0x0cf8, 0x0cff).rw("pcibus", FUNC(pci_bus_legacy_device::read), FUNC(pci_bus_legacy_device::write));

}

#define AT_KEYB_HELPER(bit, text, key1) \
	PORT_BIT( bit, IP_ACTIVE_HIGH, IPT_KEYPAD) PORT_NAME(text) PORT_CODE(key1)

static INPUT_PORTS_START( photoply )
	PORT_START("pc_keyboard_0")
	PORT_BIT ( 0x0001, 0x0000, IPT_UNUSED ) // Unused scancode 0
	AT_KEYB_HELPER( 0x0002, "Esc",               KEYCODE_Q     ) // Esc       01  81
	AT_KEYB_HELPER( 0x0004, "1",                 KEYCODE_1     )
	AT_KEYB_HELPER( 0x0008, "2",                 KEYCODE_2     )
	AT_KEYB_HELPER( 0x0010, "3",                 KEYCODE_3     )
	AT_KEYB_HELPER( 0x0020, "4",                 KEYCODE_4     )
	AT_KEYB_HELPER( 0x0040, "5",                 KEYCODE_5     )
	AT_KEYB_HELPER( 0x0080, "6",                 KEYCODE_6     )
	AT_KEYB_HELPER( 0x0100, "7",                 KEYCODE_7     )
	AT_KEYB_HELPER( 0x0200, "8",                 KEYCODE_8     )
	AT_KEYB_HELPER( 0x0400, "9",                 KEYCODE_9     )
	AT_KEYB_HELPER( 0x0800, "0",                 KEYCODE_0     )

	PORT_START("pc_keyboard_1")
	AT_KEYB_HELPER( 0x0020, "Y",                 KEYCODE_Y     ) // Y         15  95
	AT_KEYB_HELPER( 0x1000, "Enter",             KEYCODE_ENTER ) // Enter     1C  9C

	PORT_START("pc_keyboard_2")

	PORT_START("pc_keyboard_3")
	AT_KEYB_HELPER( 0x0002, "N",                 KEYCODE_N     ) // N         31  B1
	AT_KEYB_HELPER( 0x0800, "F1",                KEYCODE_F1    ) // F1        3B  BB
	AT_KEYB_HELPER( 0x1000, "F2",                KEYCODE_F2    )
	AT_KEYB_HELPER( 0x4000, "F4",                KEYCODE_F4    )

	PORT_START("pc_keyboard_4")

	PORT_START("pc_keyboard_5")

	PORT_START("pc_keyboard_6")
	AT_KEYB_HELPER( 0x0040, "(MF2)Cursor Up",    KEYCODE_UP    ) // Up         67  e7
	AT_KEYB_HELPER( 0x0080, "(MF2)Page Up",      KEYCODE_PGUP  ) // Page Up    68  e8
	AT_KEYB_HELPER( 0x0100, "(MF2)Cursor Left",  KEYCODE_LEFT  ) // Left       69  e9
	AT_KEYB_HELPER( 0x0200, "(MF2)Cursor Right", KEYCODE_RIGHT ) // Right      6a  ea
	AT_KEYB_HELPER( 0x0800, "(MF2)Cursor Down",  KEYCODE_DOWN  ) // Down       6c  ec
	AT_KEYB_HELPER( 0x1000, "(MF2)Page Down",    KEYCODE_PGDN  ) // Page Down  6d  ed
	AT_KEYB_HELPER( 0x4000, "Del",               KEYCODE_A     ) // Delete     6f  ef

	PORT_START("pc_keyboard_7")
INPUT_PORTS_END

void photoply_state::machine_start()
{
	m_shadow_ram = auto_alloc_array(machine(), uint8_t, 0x40000);
	save_pointer(NAME(m_shadow_ram),0x40000);
}

void photoply_state::machine_reset()
{
	m_pci_shadow_reg = 0;
}

static const gfx_layout CGA_charlayout =
{
	8,8,
	256,
	1,
	{ 0 },
	{ 0,1,2,3,4,5,6,7 },
	{ 0*8,1*8,2*8,3*8,4*8,5*8,6*8,7*8 },
	8*8
};

static GFXDECODE_START( gfx_photoply )
	GFXDECODE_ENTRY( "video_bios", 0x6000+0xa5*8+7, CGA_charlayout, 0, 256 )
	// There's also a 8x16 entry (just after the 8x8)
GFXDECODE_END

void photoply_state::photoply(machine_config &config)
{
	// Basic machine hardware
	I486DX4(config, m_maincpu, 75000000); // I486DX4, 75 or 100 Mhz
	m_maincpu->set_addrmap(AS_PROGRAM, &photoply_state::photoply_map);
	m_maincpu->set_addrmap(AS_IO, &photoply_state::photoply_io);
	m_maincpu->set_irq_acknowledge_callback("pic8259_1", FUNC(pic8259_device::inta_cb));

	pcat_common(config);

	GFXDECODE(config, "gfxdecode", "vga", gfx_photoply);

	ide_controller_32_device &ide(IDE_CONTROLLER_32(config, "ide").options(ata_devices, "hdd", nullptr, true));
	ide.irq_handler().set("pic8259_2", FUNC(pic8259_device::ir6_w));

	ide_controller_32_device &ide2(IDE_CONTROLLER_32(config, "ide2").options(ata_devices, nullptr, nullptr, true));
	ide2.irq_handler().set("pic8259_2", FUNC(pic8259_device::ir7_w));

	pci_bus_legacy_device &pcibus(PCI_BUS_LEGACY(config, "pcibus", 0, 0));
	pcibus.set_device(5, FUNC(photoply_state::sis_pcm_r), FUNC(photoply_state::sis_pcm_w));

	screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
	screen.set_raw(XTAL(25'174'800),900,0,640,526,0,480);
	screen.set_screen_update("vga", FUNC(cirrus_gd5446_device::screen_update));

	CIRRUS_GD5446(config, "vga", 0).set_screen("screen");

	EEPROM_93C46_16BIT(config, "eeprom")
		.write_time(attotime::from_usec(1))
		.erase_all_time(attotime::from_usec(10));
}

void photoply_state::photoply_dx4_100(machine_config &config)
{
	photoply(config);
	m_maincpu->set_clock(100000000); // 100MHz
}

/* Intel A80486DX4100
   4096KB RAM
   SiS 85C496 + 85C497
   5 x ISSI IS61C256AN-15N Cache RAM
   Winbond W83787IF (near Xtal 24.00 MHz)
   3 x ISA + 2 x PCI */
ROM_START(photoply99sp)
	ROM_REGION(0x20000, "bios", 0)  // Motherboard BIOS
	ROM_LOAD("funworld_1999_award_v4.51g.bin", 0x000000, 0x20000, CRC(af7ff1d4) SHA1(72eeecf798a03817ce7ba4d65cd4128ed3ef7e68) ) // Award Modular BIOS v4.51G, AT29C010A

	/* Multifunction board with a ESS AudioDrive chip ISA Sound + I/O (PP2000/CI-8000)
	   ESS AudioDrve ES1868F
	   NEC D71055L-10 Parallel Interface Unit
	   Winbond W83877AF
	   Xtal 24.000 MHz (near W83877AF and D71055L-10)
	   Xtal 14.31818 MHz (near ES1868F)
	   Atmel 93C46 Serial EEPROM
	   PALCE16V8H (UNDUMPED)
	   PALCE16V8H (UNDUMPED)
	   8 Dips */
	ROM_REGION(0x8000, "ex_bios", ROMREGION_ERASE00 )
	ROM_LOAD("enhanced_bios_1.06.u13", 0x0000, 0x8000, CRC(d05e9d20) SHA1(854501b7b3bf988b10516109d058f7ca2aa07d3e) ) // Centos Combo I/O ROM BIOS for CI-8000/PP2000 v1.06, W27E257

	/* Cirrus Logic PCI CL-GD5446-HC-A
	   512MB RAM (2 x M5416258B-30J)
	   Xtal 14.31818 MHz */
	ROM_REGION(0x8000, "video_bios", 0 )
	ROM_LOAD("cl-gd5446_pci_vga_bios_version_1.31.u2", 0x0000, 0x8000, CRC(61f8cac7) SHA1(6e54aadfe10dfa5c7e417a054e9a64499a99083c) ) // Cirrus Logic/Quadtel CL-GD5446 PCI VGA BIOS v1.31 , AT27C256R

	/* The PhotoPlay 1999 parallel port dongle contains, under expoxy resin:
	   Atmel AT89C2051 MCU (2KBytes internal ROM, UNDUMPED)
	   Xtal 11.05MHz
	   24C08W6 SEEPROM
	   HC132 */
	ROM_REGION(0xC00, "dongle", 0)
	ROM_SYSTEM_BIOS(0, "pp_99_dongle_r3",  "Parallel port dongle Rev. 3")
	ROMX_LOAD("dongle_photoply_1999_sp_r3_mcu.bin",         0x000, 0x800, NO_DUMP,                                                      ROM_BIOS(0)) // AT89C2051
	ROMX_LOAD("dongle_photoply_1999_sp_r3_seeprom.bin",     0x800, 0x400, CRC(62f68a79) SHA1(72477e07db0982764aede1b7e723aedf58937426), ROM_BIOS(0)) // 24C08W6
	ROM_SYSTEM_BIOS(1, "pp_99_dongle_r3a", "Parallel port dongle Rev. 3 (alt)")
	ROMX_LOAD("dongle_photoply_1999_sp_r3_alt_mcu.bin",     0x000, 0x800, NO_DUMP,                                                      ROM_BIOS(1)) // AT89C2051
	ROMX_LOAD("dongle_photoply_1999_sp_r3_alt_seeprom.bin", 0x800, 0x400, CRC(9442d1d7) SHA1(4426542c4dbb3f1df65e7ba798a7d7e0d8b98838), ROM_BIOS(1)) // 24C08W6
	ROM_SYSTEM_BIOS(2, "pp_99_dongle_r2",  "Parallel port dongle Rev. 2")
	ROMX_LOAD("dongle_photoply_1999_sp_r2_mcu.bin",         0x000, 0x800, NO_DUMP,                                                      ROM_BIOS(2)) // AT89C2051
	ROMX_LOAD("dongle_photoply_1999_sp_r2_seeprom.bin",     0x800, 0x400, CRC(52274688) SHA1(786f7407e510b303401120b8e1b082cdb412e648), ROM_BIOS(2)) // 24C08W6
	ROM_SYSTEM_BIOS(3, "pp_99_dongle_r1",  "Parallel port dongle Rev. 1")
	ROMX_LOAD("dongle_photoply_1999_sp_r1_mcu.bin",         0x000, 0x800, NO_DUMP,                                                      ROM_BIOS(3)) // AT89C2051
	ROMX_LOAD("dongle_photoply_1999_sp_r1_seeprom.bin",     0x800, 0x400, CRC(fe8f14d2) SHA1(1caad3200a22e0d510238ba44e5d96f561045ec1), ROM_BIOS(3)) // 24C08W6

	// Quantum Fireball EX3.2A
	// C/H/S: 3.2 - 6256/16/63
	// PhotoPlay label: 09.02.1999
	DISK_REGION( "ide:0:hdd:image" )
	DISK_IMAGE( "photoplay99sp", 0, BAD_DUMP SHA1(887e5b8c931d6122a1c3a8eda5cb919eb162eced) ) // From an operated HDD. A clean one must be recreated from the CDs
ROM_END

ROM_START(photoply)
	ROM_REGION(0x20000, "bios", 0)  // Motherboard BIOS
	ROM_LOAD("award bootblock bios v1.0.bin", 0x000000, 0x20000, CRC(e96d1bbc) SHA1(64d0726c4e9ecee8fddf4cc39d92aecaa8184d5c) ) // Award Modular BIOS v4.51G

	ROM_REGION(0x8000, "ex_bios", ROMREGION_ERASE00 ) // Multifunction board with a ESS AudioDrive chip
	ROM_LOAD("enhanced bios.bin", 0x000000, 0x4000, CRC(a216404e) SHA1(c9067cf87d5c8106de00866bb211eae3a6c02c65) ) // Centos Combo I/O ROM BIOS for CI-8000/PP2000 v1.06, M27128A
//  ROM_RELOAD(                   0x004000, 0x4000 )
//  ROM_RELOAD(                   0x008000, 0x4000 )
//  ROM_RELOAD(                   0x00c000, 0x4000 )

	ROM_REGION(0x8000, "video_bios", 0 )
	ROM_LOAD("vga.bin", 0x000000, 0x8000, CRC(7a859659) SHA1(ff667218261969c48082ec12aa91088a01b0cb2a) ) // Cirrus Logic/Quadtel CL-GD5436/46 PCI VGA BIOS v1.25

	DISK_REGION( "ide:0:hdd:image" )
	DISK_IMAGE( "pp201", 0, SHA1(23e1940d485d19401e7d0ad912ddad2cf2ea10b4) )
ROM_END

// BIOS not provided, might be different
ROM_START(photoply2k4)
	ROM_REGION(0x20000, "bios", 0)  // Motherboard BIOS
	ROM_LOAD("award bootblock bios v1.0.bin", 0x000000, 0x20000, BAD_DUMP CRC(e96d1bbc) SHA1(64d0726c4e9ecee8fddf4cc39d92aecaa8184d5c) )

	ROM_REGION(0x8000, "ex_bios", ROMREGION_ERASE00 ) // Multifunction board with a ESS AudioDrive chip,  M27128A
	ROM_LOAD("enhanced bios.bin", 0x000000, 0x4000, BAD_DUMP CRC(a216404e) SHA1(c9067cf87d5c8106de00866bb211eae3a6c02c65) )
//  ROM_RELOAD(                   0x004000, 0x4000 )
//  ROM_RELOAD(                   0x008000, 0x4000 )
//  ROM_RELOAD(                   0x00c000, 0x4000 )

	ROM_REGION(0x8000, "video_bios", 0 )
	ROM_LOAD("vga.bin", 0x000000, 0x8000, CRC(7a859659) BAD_DUMP SHA1(ff667218261969c48082ec12aa91088a01b0cb2a) )

	DISK_REGION( "ide:0:hdd:image" )
//  CYLS:1023,HEADS:64,SECS:63,BPS:512.
	DISK_IMAGE( "pp2004", 0, SHA1(a3f8861cf91cf7e7446ec931f812e774ada20802) )
ROM_END


GAME( 1999, photoply99sp, 0,  photoply_dx4_100, photoply, photoply_state, empty_init, ROT0, "Funworld", "Photo Play 1999 (Spanish)", MACHINE_NOT_WORKING|MACHINE_NO_SOUND|MACHINE_UNEMULATED_PROTECTION )
GAME( 199?, photoply,     0,  photoply,         photoply, photoply_state, empty_init, ROT0, "Funworld", "Photo Play 2000 (v2.01)",   MACHINE_NOT_WORKING|MACHINE_NO_SOUND|MACHINE_UNEMULATED_PROTECTION )
GAME( 2004, photoply2k4,  0,  photoply,         photoply, photoply_state, empty_init, ROT0, "Funworld", "Photo Play 2004",           MACHINE_NOT_WORKING|MACHINE_NO_SOUND|MACHINE_UNEMULATED_PROTECTION )
