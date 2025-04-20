// license:BSD-3-Clause
// copyright-holders:Fabio Priuli, Wilbert Pol
/***********************************************************************************************************

 Game Boy carts with MBC (Memory Bank Controller)


 MBC1 Mapper
 ===========

 The MBC1 mapper has two modes: 2MB ROM/8KB RAM or 512KB ROM/32KB RAM.
 Initially, the mapper operates in 2MB ROM/8KB RAM mode.

 0000-1FFF - Writing to this area enables (value 0x0A) or disables (not 0x0A) the
             SRAM.
 2000-3FFF - Writing a value 0bXXXBBBBB into the 2000-3FFF memory area selects the
             lower 5 bits of the ROM bank to select for the 4000-7FFF memory area.
             If a value of 0bXXX00000 is written then this will autmatically be
             changed to 0bXXX00001 by the mbc chip. Initial value 00.
 4000-5FFF - Writing a value 0bXXXXXXBB into the 4000-5FFF memory area either selects
             the RAM bank to use or bits 6 and 7 for the ROM bank to use for the 4000-7FFF
             memory area. This behaviour depends on the memory moddel chosen.
             These address lines are fixed in mode 1 and switch depending on A14 in mode 0.
             In mode 0 these will drive 0 when RB 00 is accessed (A14 low) or the value set
             in 4000-5FFF when RB <> 00 is accessed (A14 high).
             Switching between modes does not clear this register. Initial value 00.
 6000-7FFF - Writing a value 0bXXXXXXXB into the 6000-7FFF memory area switches the mode.
             B=0 - 2MB ROM/8KB RAM mode
             B=1 - 512KB ROM/32KB RAM mode

 Regular ROM aliasing rules apply.


 MBC2 Mapper
 ===========

 The MBC2 mapper includes 512x4bits of builtin RAM.

 0000-3FFF - Writing to this area enables (value 0bXXXX1010) or disables (any
             other value than 0bXXXX1010) the RAM. In order to perform this
             function bit 8 of the address must be reset, so usable areas are
             0000-00FF, 0200-02FF, 0400-04FF, 0600-06FF, ..., 3E00-3EFF,
 0000-3FFF - Writing to this area selects the rom bank to appear at 4000-7FFF.
             Only bits 3-0 are used to select the bank number. If a value of
             0bXXXX0000 is written then this is automatically changed into
             0bXXXX0001 by the mapper.
             In order to perform the rom banking bit 8 of the address must be
             set, so usable areas are 0100-01FF, 0300-03FF, 0500-05FF, 0700-
             07FF,..., 3F00-3FFF,

 Regular ROM aliasing rules apply.


 MBC3 Mapper
 ===========

 The MBC3 mapper cartridges can include a RTC chip.

 0000-1FFF - Writing to this area enables (value 0x0A) or disables (not 0x0A) the
             SRAM and RTC registers.
 2000-3FFF - Writing to this area selects the rom bank to appear at 4000-7FFF.
             Bits 6-0 are used  to select the bank number. If a value of
             0bX0000000 is written then this is autmatically changed into
             0bX0000001 by the mapper.
 4000-5FFF - Writing to this area selects the RAM bank or the RTC register to
             read.
             XXXX00bb - Select RAM bank bb.
             XXXX1rrr - Select RTC register rrr. Accepted values for rrr are:
                        000 - Seconds (0x00-0x3B)
                        001 - Minutes (0x00-0x3B)
                        010 - Hours (0x00-0x17)
                        011 - Bits 7-0 of the day counter
                        100 - bit 0 - Bit 8 of the day counter
                              bit 6 - Halt RTC timer ( 0 = timer active, 1 = halted)
                              bit 7 - Day counter overflow flag
 6000-7FFF - Writing 0x00 followed by 0x01 latches the RTC data. This latching
             method is used for reading the RTC registers.

 Regular ROM aliasing rules apply.


 MBC5 Mapper
 ===========

 0000-1FFF - Writing to this area enables (0x0A) or disables (not 0x0A) the SRAM area.
 2000-2FFF - Writing to this area updates bits 7-0 of the ROM bank number to
             appear at 4000-7FFF.
 3000-3FFF - Writing to this area updates bit 8 of the ROM bank number to appear
             at 4000-7FFF.
 4000-5FFF - Writing to this area select the RAM bank number to use. If the
             cartridge includes a Rumble Pack then bit 3 is used to control
             rumble motor (0 - disable motor, 1 - enable motor).


 MBC7 Mapper (Used by Kirby's Tilt n' Tumble, Command Master)
 ===========

 Status: Partial support (only ROM banking supported at the moment)

 The MBC7 mapper has 0x0200(?) bytes of RAM built in.

 0000-1FFF - Probably enable/disable RAM
             In order to use this area bit 12 of the address be set.
             Values written: 00, 0A
 2000-2FFF - Writing to this area selects the ROM bank to appear at
             4000-7FFF.
             In order to use this area bit 12 of the address be set.
             Values written: 01, 07, 01, 1C
 3000-3FFF - Unknown
             In order to use this area bit 12 of the address be set.
             Values written: 00
 4000-4FFF - Unknown
             In order to use this area bit 12 of the address be set.
             Values written: 00, 40, 3F


 MMM01 mapper
 ============

 Used by: Momotarou Collection 2, Taito Pack

 Status: not supported yet.

 Momotarou Collection 2:

 MOMOTARODENGEKI2, 0x00000, blocks 0x00 - 0x1F
 0x147: 01 04 00 00 33 00
 MOMOTAROU GAIDEN, 0x80000, blocks 0x20 - 0x3F
 0x147: 06 03 00 00 18 00

 When picking top option:
 3FFF <- 20
 5FFF <- 40
 7FFF <- 21
 1FFF <- 3A
 1FFF <- 7A

 When picking bottom option:
 3FFF <- 00
 5FFF <- 01
 7FFF <- 01
 1FFF <- 3A
 1FFF <- 7A

 Taito Pack (MMM01+RAM, 512KB, 64KB RAM):
 1st option (BUBBLE BOBBLE, blocks 0x10 - 0x17, MBC1+RAM, 128KB, 8KB RAM):
   2000 <- 70  01110000  => starting block, 10000
   6000 <- 30  00110000  => 8 blocks
   4000 <- 70  01110000  => ???
   0000 <- 40  01000000  => upper 3 bits determine lower 3 bits of starting block?

 2nd option (ELEVATOR ACTION, blocks 0x18 - 0x1B, MBC1, 64KB, 2KB RAM):
   2000 <- 78  01111000  => starting block, 11000
   6000 <- 38  00111000  => 4 blocks
   4000 <- 70  01110000  => ???
   0000 <- 40  01000000  => upper 3 bits determine lower 3 bits of starting block?

 3rd option (CHASE HQ, blocks 0x08 - 0x0F, MBC1+RAM, 128KB, 8KB RAM):
   2000 <- 68  01101000  => starting block, 01000
   6000 <- 30  00110000  => 8 blocks
   4000 <- 70  01110000  => ???
   0000 <- 40  01000000  => upper 3 bits determine lower 3 bits of starting block?

 4th option (SAGAIA, blocks 0x00 - 0x07, MBC1+RAM, 128KB, 8KB RAM):
   2000 <- 60  01100000  => starting block, 00000
   6000 <- 30  00110000  => 8 blocks
   4000 <- 70  01110000  => ???
   0000 <- 40  01000000  => upper 3 bits determine lower 3 bits of starting block?

 Known:
 The last 2 banks in a MMM01 dump are actually the starting banks for a MMM01 image.

 0000-1FFF => bit6 set => perform mapping

 Possible mapping registers:
 1FFF - Enable RAM ???
 3FFF - xxxbbbbb - Bit0-5 of the rom bank to select at 0x4000-0x7FFF ?


 TODO: RTC runs too fast while in-game, in MBC-3 games... find the problem!

 ***********************************************************************************************************/


#include "emu.h"
#include "mbc.h"


namespace {

class gb_rom_mbc_device : public device_t, public device_gb_cart_interface
{
public:
	// reading and writing
	virtual uint8_t read_rom(offs_t offset) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// construction/destruction
	gb_rom_mbc_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override { shared_start(); }
	virtual void device_reset() override { shared_reset(); }

	void shared_start();
	void shared_reset();

	uint8_t m_ram_enable;
};


class gb_rom_mbc1_device : public gb_rom_mbc_device
{
public:

	// construction/destruction
	gb_rom_mbc1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	enum {
		MODE_16M_64k = 0, /// 16Mbit ROM, 64kBit RAM
		MODE_4M_256k = 1  /// 4Mbit ROM, 256kBit RAM
	};

	gb_rom_mbc1_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override { shared_start(); save_item(NAME(m_mode)); }
	virtual void device_reset() override { shared_reset(); m_mode = MODE_16M_64k; }
	virtual void set_additional_wirings(uint8_t mask, int shift) override { m_mask = mask; m_shift = shift; }  // these get set at cart loading

	uint8_t m_mode, m_mask;
	int m_shift;
};


class gb_rom_mbc2_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_mbc2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override { shared_start(); }
	virtual void device_reset() override { shared_reset(); }
};


class gb_rom_mbc3_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_mbc3_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void update_rtc();
	uint8_t m_rtc_regs[5];
	int m_rtc_ready;
};


class gb_rom_mbc5_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_mbc5_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	gb_rom_mbc5_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override { shared_start(); m_rumble.resolve(); }
	virtual void device_reset() override { shared_reset(); }

	output_finder<> m_rumble;
};


class gb_rom_mbc6_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_mbc6_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	uint16_t m_latch1, m_latch2;
	uint8_t m_bank_4000, m_bank_6000;
};


class gb_rom_mbc7_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_mbc7_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override { shared_start(); }
	virtual void device_reset() override { shared_reset(); }
};


class gb_rom_m161_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_m161_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override { return 0xff; }
	virtual void write_ram(offs_t offset, uint8_t data) override { }

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	uint8_t m_base_bank;
	uint8_t m_load_disable;
};


class gb_rom_mmm01_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_mmm01_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// reading and writing
	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	uint16_t m_romb;
	uint8_t  m_romb_nwe;
	uint8_t  m_ramb;
	uint8_t  m_ramb_nwe;
	uint8_t  m_mode;
	uint8_t  m_mode_nwe;
	uint8_t  m_map;
	uint8_t  m_mux;
};


class gb_rom_sachen_mmc1_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_sachen_mmc1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override { return 0xff; }
	virtual void write_ram(offs_t offset, uint8_t data) override { }

protected:
	enum {
		MODE_LOCKED,
		MODE_UNLOCKED
	};

	gb_rom_sachen_mmc1_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	uint8_t m_base_bank, m_mask, m_mode, m_unlock_cnt;
};


class gb_rom_sachen_mmc2_device : public gb_rom_sachen_mmc1_device
{
public:
	// construction/destruction
	gb_rom_sachen_mmc2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	enum {
		MODE_LOCKED_DMG,
		MODE_LOCKED_CGB,
		MODE_UNLOCKED
	};

	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
};


class gb_rom_188in1_device : public gb_rom_mbc1_device
{
public:
	// construction/destruction
	gb_rom_188in1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// reading and writing
	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override { shared_start(); save_item(NAME(m_game_base)); }
	virtual void device_reset() override { shared_reset(); m_game_base = 0; }

private:
	uint32_t m_game_base;
};


class gb_rom_sintax_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_sintax_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// reading and writing
	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;
	void set_xor_for_bank(uint8_t bank);

	uint8_t m_bank_mask, m_bank, m_reg;

	uint8_t m_currentxor, m_xor2, m_xor3, m_xor4, m_xor5, m_sintax_mode;
};


class gb_rom_chongwu_device : public gb_rom_mbc5_device
{
public:
	// construction/destruction
	gb_rom_chongwu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	uint8_t m_protection_checked;
};


class gb_rom_licheng_device : public gb_rom_mbc5_device
{
public:
	// construction/destruction
	gb_rom_licheng_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void write_bank(offs_t offset, uint8_t data) override;
};


class gb_rom_digimon_device : public gb_rom_mbc5_device
{
public:
	// construction/destruction
	gb_rom_digimon_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;

protected:
	// device-level overrides
	virtual void device_start() override { shared_start(); }
	virtual void device_reset() override { shared_reset(); }
	virtual void write_ram(offs_t offset, uint8_t data) override;
};


class gb_rom_rockman8_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_rockman8_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// reading and writing
	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override { shared_start(); }
	virtual void device_reset() override { shared_reset(); }

	uint8_t m_bank_mask, m_bank, m_reg;
};


class gb_rom_sm3sp_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_sm3sp_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	// reading and writing
	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override { shared_start(); }
	virtual void device_reset() override { shared_reset(); }

	uint8_t m_bank_mask, m_bank, m_reg, m_mode;
};


class gb_rom_camera_device : public gb_rom_mbc_device
{
public:
	// construction/destruction
	gb_rom_camera_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual uint8_t read_rom(offs_t offset) override;
	virtual void write_bank(offs_t offset, uint8_t data) override;
	virtual uint8_t read_ram(offs_t offset) override;
	virtual void write_ram(offs_t offset, uint8_t data) override;

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void update_camera();
	uint8_t m_camera_regs[54];
};


//-------------------------------------------------
//  gb_rom_mbc*_device - constructor
//-------------------------------------------------

gb_rom_mbc_device::gb_rom_mbc_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: device_t(mconfig, type, tag, owner, clock)
	, device_gb_cart_interface(mconfig, *this)
	, m_ram_enable(0)
{
}

gb_rom_mbc1_device::gb_rom_mbc1_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, type, tag, owner, clock)
	, m_mode(MODE_16M_64k)
	, m_mask(0x1f)
	, m_shift(0)
{
}

gb_rom_mbc1_device::gb_rom_mbc1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc1_device(mconfig, GB_ROM_MBC1, tag, owner, clock)
{
}

gb_rom_mbc2_device::gb_rom_mbc2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_MBC2, tag, owner, clock)
{
}

gb_rom_mbc3_device::gb_rom_mbc3_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_MBC3, tag, owner, clock)
	, m_rtc_ready(0)
{
}

gb_rom_mbc5_device::gb_rom_mbc5_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, type, tag, owner, clock)
	, m_rumble(*this, "Rumble")
{
}

gb_rom_mbc5_device::gb_rom_mbc5_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc5_device(mconfig, GB_ROM_MBC5, tag, owner, clock)
{
}

gb_rom_mbc6_device::gb_rom_mbc6_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_MBC6, tag, owner, clock), m_latch1(0), m_latch2(0), m_bank_4000(0), m_bank_6000(0)
{
}

gb_rom_mbc7_device::gb_rom_mbc7_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_MBC7, tag, owner, clock)
{
}

gb_rom_m161_device::gb_rom_m161_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_M161, tag, owner, clock), m_base_bank(0), m_load_disable(0)
{
}

gb_rom_mmm01_device::gb_rom_mmm01_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_MMM01, tag, owner, clock), m_romb(0), m_romb_nwe(0), m_ramb(0), m_ramb_nwe(0), m_mode(0), m_mode_nwe(0), m_map(0), m_mux(0)
{
}

gb_rom_sachen_mmc1_device::gb_rom_sachen_mmc1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_sachen_mmc1_device(mconfig, GB_ROM_SACHEN1, tag, owner, clock)
{
}

gb_rom_sachen_mmc1_device::gb_rom_sachen_mmc1_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, type, tag, owner, clock), m_base_bank(0), m_mask(0), m_mode(0), m_unlock_cnt(0)
{
}

gb_rom_sachen_mmc2_device::gb_rom_sachen_mmc2_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_sachen_mmc1_device(mconfig, GB_ROM_SACHEN2, tag, owner, clock)
{
}

gb_rom_188in1_device::gb_rom_188in1_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc1_device(mconfig, GB_ROM_188IN1, tag, owner, clock), m_game_base(0)
{
}

gb_rom_sintax_device::gb_rom_sintax_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_SINTAX, tag, owner, clock), m_bank_mask(0), m_bank(0), m_reg(0), m_currentxor(0), m_xor2(0), m_xor3(0), m_xor4(0), m_xor5(0), m_sintax_mode(0)
{
}

gb_rom_chongwu_device::gb_rom_chongwu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc5_device(mconfig, GB_ROM_CHONGWU, tag, owner, clock), m_protection_checked(0)
{
}

gb_rom_licheng_device::gb_rom_licheng_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc5_device(mconfig, GB_ROM_LICHENG, tag, owner, clock)
{
}

gb_rom_digimon_device::gb_rom_digimon_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc5_device(mconfig, GB_ROM_DIGIMON, tag, owner, clock)
{
}

gb_rom_rockman8_device::gb_rom_rockman8_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_ROCKMAN8, tag, owner, clock), m_bank_mask(0), m_bank(0), m_reg(0)
{
}

gb_rom_sm3sp_device::gb_rom_sm3sp_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_SM3SP, tag, owner, clock), m_bank_mask(0), m_bank(0), m_reg(0), m_mode(0)
{
}

gb_rom_camera_device::gb_rom_camera_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock)
	: gb_rom_mbc_device(mconfig, GB_ROM_CAMERA, tag, owner, clock)
{
}


//-------------------------------------------------
//  shared_start
//-------------------------------------------------

void gb_rom_mbc_device::shared_start()
{
	save_item(NAME(m_latch_bank));
	save_item(NAME(m_latch_bank2));
	save_item(NAME(m_ram_bank));
	save_item(NAME(m_ram_enable));
}

//-------------------------------------------------
//  shared_reset
//-------------------------------------------------

void gb_rom_mbc_device::shared_reset()
{
	m_latch_bank = 0;
	m_latch_bank2 = 1;
	m_ram_bank = 0;
	m_ram_enable = 0;
}

//-------------------------------------------------
//  mapper specific start/reset
//-------------------------------------------------

void gb_rom_mbc3_device::device_start()
{
	shared_start();
	save_item(NAME(m_rtc_regs));
	save_item(NAME(m_rtc_ready));
}

void gb_rom_mbc3_device::device_reset()
{
	shared_reset();
	memset(m_rtc_regs, 0, sizeof(m_rtc_regs));
	m_rtc_ready = 0;
}

void gb_rom_mbc6_device::device_start()
{
	save_item(NAME(m_bank_4000));
	save_item(NAME(m_bank_6000));
	save_item(NAME(m_latch1));
	save_item(NAME(m_latch2));
	save_item(NAME(m_latch_bank));
	save_item(NAME(m_latch_bank2));
	save_item(NAME(m_ram_bank));
	save_item(NAME(m_ram_enable));
}

void gb_rom_mbc6_device::device_reset()
{
	m_bank_4000 = 2;    // correct default?
	m_bank_6000 = 3;    // correct default?
	m_latch1 = 0;   // correct default?
	m_latch2 = 0;   // correct default?

	m_latch_bank = 2;   // correct default?
	m_latch_bank2 = 3;  // correct default?
	m_ram_bank = 0;
	m_ram_enable = 0;
}

void gb_rom_m161_device::device_start()
{
	shared_start();
	save_item(NAME(m_base_bank));
	save_item(NAME(m_load_disable));
}

void gb_rom_m161_device::device_reset()
{
	shared_reset();
	m_base_bank = 0;
	m_load_disable = 0;
}

void gb_rom_mmm01_device::device_start()
{
	shared_start();
	save_item(NAME(m_romb));
	save_item(NAME(m_romb_nwe));
	save_item(NAME(m_ramb));
	save_item(NAME(m_ramb_nwe));
	save_item(NAME(m_mode));
	save_item(NAME(m_mode_nwe));
	save_item(NAME(m_map));
	save_item(NAME(m_mux));
}

void gb_rom_mmm01_device::device_reset()
{
	m_romb = 0x000;
	m_romb_nwe = 0x00;
	m_ramb = 0x00;
	m_ramb_nwe = 0x00;
	m_mode = 0x00;
	m_map = 0x00;
	m_mux = 0x00;
}

void gb_rom_sachen_mmc1_device::device_start()
{
	shared_start();
	save_item(NAME(m_base_bank));
	save_item(NAME(m_mask));
	save_item(NAME(m_mode));
	save_item(NAME(m_unlock_cnt));
}

void gb_rom_sachen_mmc1_device::device_reset()
{
	shared_reset();
	m_base_bank = 0x00;
	m_mask = 0x00;
	m_mode = MODE_LOCKED;
	m_unlock_cnt = 0x00;
}

void gb_rom_sachen_mmc2_device::device_start()
{
	shared_start();
	save_item(NAME(m_base_bank));
	save_item(NAME(m_mask));
	save_item(NAME(m_mode));
	save_item(NAME(m_unlock_cnt));
}

void gb_rom_sachen_mmc2_device::device_reset()
{
	shared_reset();
	m_base_bank = 0x00;
	m_mask = 0x00;
	m_mode = MODE_LOCKED_DMG;
	m_unlock_cnt = 0x00;
}

void gb_rom_sintax_device::device_start()
{
	shared_start();
	save_item(NAME(m_sintax_mode));
	save_item(NAME(m_currentxor));
	save_item(NAME(m_xor2));
	save_item(NAME(m_xor3));
	save_item(NAME(m_xor4));
	save_item(NAME(m_xor5));
}

void gb_rom_sintax_device::device_reset()
{
	shared_reset();
	m_sintax_mode = 0;
	m_currentxor = 0;
	m_xor2 = 0;
	m_xor3 = 0;
	m_xor4 = 0;
	m_xor5 = 0;
}

void gb_rom_chongwu_device::device_start()
{
	shared_start();
	save_item(NAME(m_protection_checked));
}

void gb_rom_chongwu_device::device_reset()
{
	shared_reset();
	m_protection_checked = 0;
}

void gb_rom_camera_device::device_start()
{
	shared_start();
	save_item(NAME(m_camera_regs));
}

void gb_rom_camera_device::device_reset()
{
	shared_reset();
	memset(m_camera_regs, 0, sizeof(m_camera_regs));
}


/*-------------------------------------------------
 mapper specific handlers
 -------------------------------------------------*/

uint8_t gb_rom_mbc_device::read_rom(offs_t offset)
{
	return m_rom[rom_bank_map[m_latch_bank] + offset];
}

uint8_t gb_rom_mbc_device::read_ram(offs_t offset)
{
	if (!m_ram.empty())
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + offset];
	else
		return 0xff;
}

void gb_rom_mbc_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty())
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + offset] = data;
}


// MBC1

uint8_t gb_rom_mbc1_device::read_rom(offs_t offset)
{
	if (offset & 0x4000) /* RB1 */
		return m_rom[rom_bank_map[(m_ram_bank << (5 + m_shift)) | m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
	else
	{                    /* RB0 */
		int bank = (m_mode == MODE_4M_256k) ? (m_ram_bank << (5 + m_shift)) : 0;
		return m_rom[rom_bank_map[bank] * 0x4000 + (offset & 0x3fff)];
	}
}

void gb_rom_mbc1_device::write_bank(offs_t offset, uint8_t data)
{
	// the mapper only uses inputs A15..A13
	switch (offset & 0xe000)
	{
		case 0x0000:    // RAM Enable Register
			m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
			break;
		case 0x2000:    // ROM Bank Register
			data &= 0x1f;
			m_latch_bank2 = data ? data : 1;
			m_latch_bank2 &= m_mask;
			break;
		case 0x4000:    // RAM Bank Register
			m_ram_bank = data & 0x3;
			break;
		case 0x6000:    // MBC1 Mode Register
			m_mode = (data & 0x1) ? MODE_4M_256k : MODE_16M_64k;
			break;
	}
}

uint8_t gb_rom_mbc1_device::read_ram(offs_t offset)
{
	if (!m_ram.empty() && m_ram_enable)
	{
		int bank = (m_mode == MODE_4M_256k) ? m_ram_bank : 0;
		return m_ram[ram_bank_map[bank] * 0x2000 + offset];
	}
	else
		return 0xff;
}

void gb_rom_mbc1_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty() && m_ram_enable)
	{
		int bank = (m_mode == MODE_4M_256k) ? m_ram_bank : 0;
		m_ram[ram_bank_map[bank] * 0x2000 + offset] = data;
	}
}


// MBC2

uint8_t gb_rom_mbc2_device::read_rom(offs_t offset)
{
	if (offset & 0x4000) /* RB1 */
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
	else                 /* RB0 */
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_mbc2_device::write_bank(offs_t offset, uint8_t data)
{
	// the mapper only has data lines D3..D0
	data &= 0x0f;

	// the mapper only uses inputs A15..A14, A8 for register accesses
	switch (offset & 0xc100)
	{
		case 0x0000:    // RAM Enable Register
			m_ram_enable = (data == 0x0a) ? 1 : 0;
			break;
		case 0x0100:    // ROM Bank Register
			m_latch_bank2 = (data == 0x00) ? 0x01 : data;
			break;
	}
}

uint8_t gb_rom_mbc2_device::read_ram(offs_t offset)
{
	if (!m_ram.empty() && m_ram_enable)
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x01ff)] | 0xf0;
	else
		return 0xff;
}

void gb_rom_mbc2_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty() && m_ram_enable)
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x01ff)] = data & 0x0f;
}


// MBC3

void gb_rom_mbc3_device::update_rtc()
{
	system_time curtime;
	machine().current_datetime(curtime);

	m_rtc_regs[0] = curtime.local_time.second;
	m_rtc_regs[1] = curtime.local_time.minute;
	m_rtc_regs[2] = curtime.local_time.hour;
	m_rtc_regs[3] = curtime.local_time.day & 0xff;
	m_rtc_regs[4] = (m_rtc_regs[4] & 0xf0) | (curtime.local_time.day >> 8);
}

uint8_t gb_rom_mbc3_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_mbc3_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
		m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
	else if (offset < 0x4000)
	{
		// 7bits
		data &= 0x7f;
		/* Selecting bank 0 == selecting bank 1 */
		if (data == 0)
			data = 1;

		m_latch_bank2 = data;
	}
	else if (offset < 0x6000)
	{
		m_ram_bank = data;
	}
	else if (has_timer)
	{
		if (m_rtc_ready == 1 && data == 0)
			m_rtc_ready = 0;
		if (m_rtc_ready == 0 && data == 1)
		{
			m_rtc_ready = 1;
			update_rtc();
		}
	}
}

uint8_t gb_rom_mbc3_device::read_ram(offs_t offset)
{
	if (m_ram_bank < 4 && m_ram_enable)
	{
		// RAM
		if (!m_ram.empty())
			return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	}
	if (m_ram_bank >= 0x8 && m_ram_bank <= 0xc)
	{
		// RTC registers
		if (has_timer)
			return m_rtc_regs[m_ram_bank - 8];
	}
	return 0xff;
}

void gb_rom_mbc3_device::write_ram(offs_t offset, uint8_t data)
{
	if (m_ram_bank < 4 && m_ram_enable)
	{
		// RAM
		if (!m_ram.empty())
			m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
	}
	if (m_ram_bank >= 0x8 && m_ram_bank <= 0xc && m_ram_enable)
	{
		// RTC registers are writeable too
		if (has_timer)
			m_rtc_regs[m_ram_bank - 8] = data;
	}
}

// MBC5

uint8_t gb_rom_mbc5_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_mbc5_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
		m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
	else if (offset < 0x3000)
	{
		// MBC5 has a 9 bit bank select
		// Writing into 2000-2fff sets the lower 8 bits
		m_latch_bank2 = (m_latch_bank2 & 0x100) | data;
	}
	else if (offset < 0x4000)
	{
		// MBC5 has a 9 bit bank select
		// Writing into 3000-3fff sets the 9th bit
		m_latch_bank2 = (m_latch_bank2 & 0xff) | ((data & 0x01) << 8);
	}
	else if (offset < 0x6000)
	{
		data &= 0x0f;
		if (has_rumble)
		{
			m_rumble = BIT(data, 3);
			data &= 0x7;
		}
		m_ram_bank = data;
	}
}

uint8_t gb_rom_mbc5_device::read_ram(offs_t offset)
{
	if (!m_ram.empty() && m_ram_enable)
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	else
		return 0xff;
}

void gb_rom_mbc5_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty() && m_ram_enable)
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
}

// MBC6

uint8_t gb_rom_mbc6_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else if (offset < 0x6000)
		return m_rom[rom_bank_map[m_bank_4000 >> 1] * 0x4000 + (m_bank_4000 & 0x01) * 0x2000 + (offset & 0x1fff)];
	else
		return m_rom[rom_bank_map[m_bank_6000 >> 1] * 0x4000 + (m_bank_6000 & 0x01) * 0x2000 + (offset & 0x1fff)];
}

void gb_rom_mbc6_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
	{
		logerror( "%s write to mbc6 ram enable area: %04X <- 0x%02X\n", machine().describe_context(), offset, data );
	}
	else if (offset < 0x3000)
	{
		if (!(offset & 0x0800))
			m_latch1 = data;
		else if (data == 0x00)
			m_bank_4000 = m_latch1;
	}
	else if (offset < 0x4000)
	{
		if (!(offset & 0x0800))
			m_latch2 = data;
		else if (data == 0x00)
			m_bank_6000 = m_latch2;
	}
}

uint8_t gb_rom_mbc6_device::read_ram(offs_t offset)
{
	if (!m_ram.empty())
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	else
		return 0xff;
}

void gb_rom_mbc6_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty())
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
}

// MBC7

uint8_t gb_rom_mbc7_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_mbc7_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
	{
		// FIXME: Add RAM enable support
		logerror("%s Write to ram enable register 0x%04X <- 0x%02X\n", machine().describe_context(), offset, data);
	}
	else if (offset < 0x3000)
	{
		logerror( "%s write to mbc7 rom select register: 0x%04X <- 0x%02X\n", machine().describe_context(), 0x2000 + offset, data );
		/* Bit 12 must be set for writing to the mbc register */
		if (offset & 0x0100)
			m_latch_bank2 = data;
	}
	else
	{
		logerror( "%s write to mbc7 rom area: 0x%04X <- 0x%02X\n", machine().describe_context(), 0x3000 + offset, data );
		/* Bit 12 must be set for writing to the mbc register */
		if (offset & 0x0100)
		{
			switch (offset & 0x7000)
			{
				case 0x3000:    /* 0x3000-0x3fff */
				case 0x4000:    /* 0x4000-0x4fff */
				case 0x5000:    /* 0x5000-0x5fff */
				case 0x6000:    /* 0x6000-0x6fff */
				case 0x7000:    /* 0x7000-0x7fff */
					break;
			}
		}
	}
}

uint8_t gb_rom_mbc7_device::read_ram(offs_t offset)
{
	if (!m_ram.empty())
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	else
		return 0xff;
}

void gb_rom_mbc7_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty())
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
}


// M161

uint8_t gb_rom_m161_device::read_rom(offs_t offset)
{
	return m_rom[rom_bank_map[m_base_bank] * 0x4000 + (offset & 0x7fff)];
}

void gb_rom_m161_device::write_bank(offs_t offset, uint8_t data)
{
	// the mapper (74HC161A) only has data lines D2..D0
	data &= 0x07;

	// A15 is connected to #LOAD and overwritten by QD (m_load_disable)
	switch (offset & 0x8000)
	{
		case 0x0000:    // Base Bank Register
			if (!m_load_disable)
				m_base_bank = data << 1;
			m_load_disable = 0x01;
			break;
		default:
			break;
	}
}


// MMM01

uint8_t gb_rom_mmm01_device::read_rom(offs_t offset)
{
	uint16_t romb = m_romb & ~(0x1e0 | m_romb_nwe);
	uint16_t romb_base = m_romb & (0x1e0 | m_romb_nwe);
	uint8_t ramb_masked = ((offset & 0x4000) | m_mode ? m_ramb : m_ramb & ~0x03);

	// zero-adjust RA18..RA14
	romb = (romb ? romb : 0x01);
	// if unmapped, force
	romb = (m_map ? romb : 0x01);

	// RB 0 logic
	if (!(offset & 0x4000))
		romb = 0x00;

	// combine with base
	romb |= romb_base;

	// multiplex with AA14..AA13
	if (m_mux)
		romb = (romb & ~0x60) | ((ramb_masked & 0x03) << 5);

	// if unmapped, force
	if (!m_map)
		romb |= 0x1fe;

	return m_rom[rom_bank_map[romb] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_mmm01_device::write_bank(offs_t offset, uint8_t data)
{
	// the mapper only has data lines D6..D0
	data &= 0x7f;

	// the mapper only uses inputs A15..A13
	switch (offset & 0xe000)
	{
		case 0x0000: // Map Latch, AA #WE, RAM Enable
			if (!m_map) {
				m_ramb_nwe = (data & (0x30)) >> 4;
				m_map = (data & 0x40);
			}
			m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
			break;
		case 0x2000: // RA20..RA19 RA18..RA14
			if (!m_map)
				m_romb = (m_romb & ~0x60) | (data & 0x60);

			m_romb = (m_romb & (~0x1f | m_romb_nwe)) | (data & (0x1f & ~m_romb_nwe));
			break;
		case 0x4000: // Mode #WE, RA22..RA21, AA16..AA15, AA14..AA13
			if (!m_map) {
				m_mode_nwe = data & 0x40;
				m_romb = (m_romb & ~0x180) | ((data & 0x30) << 3);
				m_ramb = (m_ramb & ~0x0c) | (data & 0x0c);
			}

			m_ramb = (m_ramb & (~0x03 | m_ramb_nwe)) | (data & (0x03 & ~m_ramb_nwe));
			break;
		case 0x6000: // Mux, RA18..RA15 #WE/Mask, ???, MBC1 Mode
			if (!m_map) {
				m_mux = data & 0x40;
				// m_romb_nwe is aligned to RA14, hence >> 1 instead of >> 2
				m_romb_nwe = (data & 0x3c) >> 1;
			}

			if (!m_mode_nwe)
				m_mode = data & 0x01;
			break;
		default:
			break;
	}
}

uint8_t gb_rom_mmm01_device::read_ram(offs_t offset)
{
	uint8_t ramb_masked = ((offset & 0x4000) | m_mode ? m_ramb : m_ramb & ~0x03);
	uint8_t ramb = ramb_masked;

	// multiplex with RA20..RA19
	if (m_mux)
		ramb = (ramb & ~0x03) | ((m_romb & 0x60) >> 5);

	if (!m_ram.empty() && m_ram_enable)
	{
		return m_ram[ram_bank_map[ramb] * 0x2000 + (offset & 0x1fff)];
	}
	else
		return 0xff;
}

void gb_rom_mmm01_device::write_ram(offs_t offset, uint8_t data)
{
	uint8_t ramb_masked = ((offset & 0x4000) | m_mode ? m_ramb : m_ramb & ~0x03);
	uint8_t ramb = ramb_masked;

	// multiplex with RA20..RA19
	if (m_mux)
		ramb = (ramb & ~0x03) | ((m_romb & 0x60) >> 5);

	if (!m_ram.empty() && m_ram_enable)
	{
		m_ram[ram_bank_map[ramb] * 0x2000 + (offset & 0x1fff)] = data;
	}
}

// Sachen MMC1

uint8_t gb_rom_sachen_mmc1_device::read_rom(offs_t offset)
{
	uint16_t off_edit = offset;

	/* Wait for 0x31 transitions of A15 (hi -> lo), i.e. ROM accesses; A15 = HI while in bootstrap */
	/* This is 0x31 transitions, because we increment counter _after_ checking it */
	if (m_unlock_cnt == 0x30)
		m_mode = MODE_UNLOCKED;
	else
		m_unlock_cnt++;

	/* Logo Switch */
	if (m_mode == MODE_LOCKED)
		off_edit |= 0x80;

	/* Header Un-Scramble */
	if ((off_edit & 0xff00) == 0x0100) {
		off_edit &= 0xffac;
		off_edit |= ((offset >> 6) & 0x01) << 0;
		off_edit |= ((offset >> 4) & 0x01) << 1;
		off_edit |= ((offset >> 1) & 0x01) << 4;
		off_edit |= ((offset >> 0) & 0x01) << 6;
	}
	//logerror("read from %04X (%04X)\n", offset, off_edit);

	if (offset & 0x4000) /* RB1 */
		return m_rom[rom_bank_map[(m_base_bank & m_mask) | (m_latch_bank2 & ~m_mask)] * 0x4000 + (offset & 0x3fff)];
	else                 /* RB0 */
		return m_rom[rom_bank_map[(m_base_bank & m_mask) | (m_latch_bank & ~m_mask)] * 0x4000 + (off_edit & 0x3fff)];
}

void gb_rom_sachen_mmc1_device::write_bank(offs_t offset, uint8_t data)
{
	/* Only A15..A6, A4, A1..A0 are connected */
	/* We only decode upper three bits */
	switch ((offset & 0xffd3) & 0xe000)
	{
		case 0x0000: /* Base ROM Bank Register */

			if ((m_latch_bank2 & 0x30) == 0x30)
				m_base_bank = data;
			//logerror("write to base bank %X - %X\n", data, (m_base_bank & m_mask) | (m_latch_bank2 & ~m_mask));
			break;

		case 0x2000: /* ROM Bank Register */

			m_latch_bank2 = data ? data : 0x01;
			//logerror("write to latch %X - %X\n", data, (m_base_bank & m_mask) | (m_latch_bank2 & ~m_mask));
			break;

		case 0x4000: /* ROM Bank Mask Register */

			if ((m_latch_bank2 & 0x30) == 0x30)
				m_mask = data;
			//logerror("write to mask %X - %X\n", data, (m_base_bank & m_mask) | (m_latch_bank2 & ~m_mask));
			break;

		case 0x6000:

			/* nothing happens when writing to 0x6000-0x7fff, as verified by Tauwasser */
			break;

		default:

			//logerror("write to unknown/unmapped area %04X <= %02X\n", offset, data);
			/* did not extensively test other unlikely ranges */
			break;
	}
}

// Sachen MMC2

uint8_t gb_rom_sachen_mmc2_device::read_rom(offs_t offset)
{
	uint16_t off_edit = offset;

	/* Wait for 0x30 transitions of A15 (lo -> hi), i.e. ROM accesses; A15 = HI while in bootstrap */
	/* This is 0x30 transitions, because we increment counter _after_ checking it, but A15 lo -> hi*/
	/* transition means first read (hi -> lo transition) must not count */

	if (m_unlock_cnt == 0x30 && m_mode == MODE_LOCKED_DMG) {
		m_mode = MODE_LOCKED_CGB;
		m_unlock_cnt = 0x00;
	} else if (m_unlock_cnt == 0x30 && m_mode == MODE_LOCKED_CGB) {
		m_mode = MODE_UNLOCKED;
	}

	if (m_unlock_cnt != 0x30)
		m_unlock_cnt++;

	/* Logo Switch */
	if (m_mode == MODE_LOCKED_CGB)
		off_edit |= 0x80;

	/* Header Un-Scramble */
	if ((off_edit & 0xff00) == 0x0100) {
		off_edit &= 0xffac;
		off_edit |= ((offset >> 6) & 0x01) << 0;
		off_edit |= ((offset >> 4) & 0x01) << 1;
		off_edit |= ((offset >> 1) & 0x01) << 4;
		off_edit |= ((offset >> 0) & 0x01) << 6;
	}
	//logerror("read from %04X (%04X) cnt: %02X\n", offset, off_edit, m_unlock_cnt);

	if (offset & 0x4000) /* RB1 */
		return m_rom[rom_bank_map[(m_base_bank & m_mask) | (m_latch_bank2 & ~m_mask)] * 0x4000 + (offset & 0x3fff)];
	else                 /* RB0 */
		return m_rom[rom_bank_map[(m_base_bank & m_mask) | (m_latch_bank & ~m_mask)] * 0x4000 + (off_edit & 0x3fff)];
}

uint8_t gb_rom_sachen_mmc2_device::read_ram(offs_t offset)
{
	if (m_mode == MODE_LOCKED_DMG) {
		m_unlock_cnt = 0x00;
		m_mode = MODE_LOCKED_CGB;
	}
	return 0xff;

}

void gb_rom_sachen_mmc2_device::write_ram(offs_t offset, uint8_t data)
{
	if (m_mode == MODE_LOCKED_DMG) {
		m_unlock_cnt = 0x00;
		m_mode = MODE_LOCKED_CGB;
	}

}


// 188 in 1 pirate (only preliminary)

uint8_t gb_rom_188in1_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[m_game_base + rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[m_game_base + rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_188in1_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset == 0x7b00)
	{
		if (data < 0x80)
			logerror("write to 0x%X data 0x%X\n", offset, data);
		else
		{
			data -= 0x80;
			m_game_base = 0x400000 + (data * 0x8000);
			//logerror("offset 0x%X\n", m_game_base);
		}
	}
	else if (offset == 0x7b01 || offset == 0x7b02)
	{
		// what do these writes do?
		printf("write to 0x%X data 0x%X\n", offset, data);
	}
	else
		gb_rom_mbc1_device::write_bank(offset, data);
}


// MBC5 variant used by Li Cheng / Niutoude games

void gb_rom_licheng_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset > 0x2100 && offset < 0x3000)
		return;

	gb_rom_mbc5_device::write_bank(offset, data);
}

// MBC5 variant used by Chong Wu Xiao Jing Ling (this appears to be a re-release of a Li Cheng / Niutoude game,
// given that it contains the Niutoude logo, with most protection checks patched out)

uint8_t gb_rom_chongwu_device::read_rom(offs_t offset)
{
	// protection check at the first read here...
	if (offset == 0x41c3 && !m_protection_checked)
	{
		m_protection_checked = 1;
		return 0x5d;
	}

	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

// MBC5 variant used by Sintax games

void gb_rom_sintax_device::set_xor_for_bank(uint8_t bank)
{
	switch (bank & 0x0f)
	{
		case 0x00: case 0x04: case 0x08: case 0x0c:
			m_currentxor = m_xor2;
			break;
		case 0x01: case 0x05: case 0x09: case 0x0d:
			m_currentxor = m_xor3;
			break;
		case 0x02: case 0x06: case 0x0a: case 0x0e:
			m_currentxor = m_xor4;
			break;
		case 0x03: case 0x07: case 0x0b: case 0x0f:
			m_currentxor = m_xor5;
			break;
	}
}

uint8_t gb_rom_sintax_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)] ^ m_currentxor;
}

void gb_rom_sintax_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
		m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
	else if (offset < 0x3000)
	{
		set_xor_for_bank(data);

		switch (m_sintax_mode & 0x0f)
		{
			case 0x0d:
				data = bitswap<8>(data, 1,0,7,6,5,4,3,2);
				break;
			case 0x09:
				//data = bitswap<8>(data, 3,2,5,4,0,1,6,7); // Monkey..no
				data = bitswap<8>(data, 4,5,2,3,0,1,6,7);
				break;
			case 0x00: // 0x10=lion 0x00 hmmmmm // 1 and 0 unconfirmed
				data = bitswap<8>(data, 7,0,5,6,3,4,1,2);
				break;
			case 0x01:
				data = bitswap<8>(data, 0,1,6,7,4,5,2,3);
				break;
			case 0x05:
				data = bitswap<8>(data, 7,6,1,0,3,2,5,4); // Not 100% on this one
				break;
			case 0x07:
				data = bitswap<8>(data, 2,0,3,1,5,4,7,6); // 5 and 7 unconfirmed
				break;
			case 0x0b:
				data = bitswap<8>(data, 2,3,0,1,6,7,4,5); // 5 and 6 unconfirmed
				break;
		}
		m_latch_bank2 = (m_latch_bank2 & 0x100) | data;
	}
	else if (offset < 0x4000)
	{
		m_latch_bank2 = (m_latch_bank2 & 0xff) | ((data & 0x01) << 8);
	}
	else if (offset < 0x5000)
	{
		data &= 0x0f;
		m_ram_bank = data;
	}
	else if (offset < 0x6000)
	{
		if (!m_sintax_mode)
		{
			m_sintax_mode = data;
			write_bank(0x2000, 1);   //force a fake bank switch
		}
//      printf("sintax mode %x\n", m_sintax_mode & 0xf);
	}
	else if (offset >= 0x7000)
	{
		switch ((offset & 0x00f0) >> 4)
		{
			case 2:
				m_xor2 = data;
				break;
			case 3:
				m_xor3 = data;
				break;
			case 4:
				m_xor4 = data;
				break;
			case 5:
				m_xor5 = data;
				break;
		}

		if (m_currentxor == 0)
			set_xor_for_bank(4);
	}

}

uint8_t gb_rom_sintax_device::read_ram(offs_t offset)
{
	if (!m_ram.empty() && m_ram_enable)
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	else
		return 0xff;
}

void gb_rom_sintax_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty() && m_ram_enable)
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
}

/*

 Further MBC5 variants to emulate:

 Digimon 2 & Digimon 4 (Yong Yong)

 Digimon 2 writes at $2000 to select latch2 (data must be divided by 2, and 0 becomes 1),
 then writes to $2400 a series of values that the patched version does not write...
 Digimon 4 seems to share part of the $2000 behavior, but does not write to $2400...

 */

// MBC5 variant used by Digimon 2 (and maybe 4?)

uint8_t gb_rom_digimon_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_digimon_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
		m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
	else if (offset == 0x2000)
	{
//      printf("written $02 %X at %X\n", data, offset);
		if (!data)
			data++;
		m_latch_bank2 = data/2;
	}
	else if (offset < 0x3000)
	{
//      printf("written $03 %X at %X\n", data, offset);
	}
	else if (offset < 0x4000)
	{
//      printf("written $04 %X at %X\n", data, offset);
	}
	else if (offset < 0x6000)
	{
//      printf("written $05-$06 %X at %X\n", data, offset);
		data &= 0x0f;
		m_ram_bank = data;
	}
//  else
//      printf("written $07 %X at %X\n", data, offset);
}

uint8_t gb_rom_digimon_device::read_ram(offs_t offset)
{
	if (!m_ram.empty() && m_ram_enable)
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	else
		return 0xff;
}

void gb_rom_digimon_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty() && m_ram_enable)
		m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
}


// MBC1 variant used by Yong Yong for Rockman 8

uint8_t gb_rom_rockman8_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[m_latch_bank * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[m_latch_bank2 * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_rockman8_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
		return;
	else if (offset < 0x4000)
	{
		// 5bits only
		data &= 0x1f;
		if (data == 0)
			data = 1;
		if (data > 0xf)
			data -= 8;

		m_latch_bank2 = data;
	}
}

uint8_t gb_rom_rockman8_device::read_ram(offs_t offset)
{
	if (!m_ram.empty())
		return m_ram[offset];
	else
		return 0xff;
}

void gb_rom_rockman8_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty())
		m_ram[offset] = data;
}

// MBC1 variant used by Yong Yong for Super Mario 3 Special

// Mario special seems to be 512k image (mirrored up to 1m or 2m [redump needed to establish this])
// it consists of 13 unique 16k chunks layed out as follows
// unique chunk --> bank in bin
// 1st to 7th   --> 0x00 to 0x06
// 8th          --> 0x08
// 9th          --> 0x0b
// 10th         --> 0x0c
// 11th         --> 0x0d
// 12th         --> 0x0f
// 13th         --> 0x13

// writing data to 0x2000-0x2fff switches bank according to the table below
// (the value values corresponding to table[0x0f] is not confirmed, choices
// 0,1,2,3,8,c,f freeze the game, while 4,5,6,7,b,d,0x13 work with glitches)
static uint8_t smb3_table1[0x20] =
{
	0x00,0x04,0x01,0x05, 0x02,0x06,0x03,0x05, 0x08,0x0c,0x03,0x0d, 0x03,0x0b,0x0b,0x08 /* original doc here put 0x0f (i.e. 11th unique bank) */,
	0x05,0x06,0x0b,0x0d, 0x08,0x06,0x13,0x0b, 0x08,0x05,0x05,0x08, 0x0b,0x0d,0x06,0x05
};

// according to old doc from Brian Provinciano, writing bit5 in 0x5000-0x5fff should
// change the bank layout, in the sense that writing to bankswitch acts like if
// the original rom has a different layout (as if unique chunks were under permutations
// (24), (365) and (8a9) with 0,1,7,b,c fixed) and the same table above is used
// however, no such a write ever happen (only bit4 is written, but changing mode with
// bit4 breaks the gfx...)

uint8_t gb_rom_sm3sp_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[0] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[m_latch_bank2 * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_sm3sp_device::write_bank(offs_t offset, uint8_t data)
{
//  printf("write 0x%x at %x\n", data, offset);
	if (offset < 0x2000)
		return;
	else if (offset < 0x3000)
	{
		// Table 1 confirmed...
		// 0->0, 4->2, 6->3
		// 1e -> 6 (level 1 bg gfx)
		// 19 -> 5 (level 2 bg gfx)
		// 1b -> 8 (level 3 bg gfx)
		// 1d -> D (level 4 bg gfx)
		// 1c -> B (bonus house bg gfx)
		// 1 (9 maybe, or 3)? f (5 maybe)? 2->1?
		// 16 -> 4-8? b?

		// 5bits only
		data &= 0x1f;

		m_latch_bank2 = smb3_table1[data];
		if (m_mode)
		{
			switch (m_latch_bank2)
			{
				case 0x02:  m_latch_bank2 = 4;  break;
				case 0x03:  m_latch_bank2 = 6;  break;
				case 0x04:  m_latch_bank2 = 2;  break;
				case 0x05:  m_latch_bank2 = 3;  break;
				case 0x06:  m_latch_bank2 = 5;  break;
				case 0x0b:  m_latch_bank2 = 0xd;    break;
				case 0x0c:  m_latch_bank2 = 0xb;    break;
				case 0x0d:  m_latch_bank2 = 0xc;    break;

				case 0x00:
				case 0x01:
				case 0x08:
				case 0x0f:
				case 0x13:
				default:
					break;
			}
		}
	}
	else if (offset < 0x5000)
	{
//      printf("write $5 %X at %X\n", data, offset);
		//maybe rumble??
	}
	else if (offset < 0x6000)
	{
//      printf("write mode %x\n", data);
		m_mode = BIT(data, 5);
//      write_bank(0x2000, 1);
	}
}

uint8_t gb_rom_sm3sp_device::read_ram(offs_t offset)
{
	if (!m_ram.empty())
		return m_ram[offset];
	else
		return 0xff;
}

void gb_rom_sm3sp_device::write_ram(offs_t offset, uint8_t data)
{
	if (!m_ram.empty())
		m_ram[offset] = data;
}

void gb_rom_camera_device::update_camera()
{
	m_camera_regs[0] &= ~0x1;
}

uint8_t gb_rom_camera_device::read_rom(offs_t offset)
{
	if (offset < 0x4000)
		return m_rom[rom_bank_map[m_latch_bank] * 0x4000 + (offset & 0x3fff)];
	else
		return m_rom[rom_bank_map[m_latch_bank2] * 0x4000 + (offset & 0x3fff)];
}

void gb_rom_camera_device::write_bank(offs_t offset, uint8_t data)
{
	if (offset < 0x2000)
		m_ram_enable = ((data & 0x0f) == 0x0a) ? 1 : 0;
	else if (offset < 0x4000)
	{
		// 7bits
		data &= 0x7f;
		/* Selecting bank 0 == selecting bank 1 */
		if (data == 0)
			data = 1;

		m_latch_bank2 = data;
	}
	else if (offset < 0x6000)
	{
		m_ram_bank = data & 0x1f;
	}
}

uint8_t gb_rom_camera_device::read_ram(offs_t offset)
{
	if ((m_ram_bank & 0x10) != 0)
		return (offset == 0) ? (m_camera_regs[0] & 0x7) : 0;
	if (!m_ram.empty() && (m_camera_regs[0] & 0x1) == 0)
	{
		/* Use first saved image as the snapshot. Top and bottom of snapshot are not saved. */
		if (m_ram_bank == 0 && offset >= 0x100 && offset < 0xf00)
			return m_ram[0x1f00 + offset];
		return m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)];
	}
	return 0;
}

void gb_rom_camera_device::write_ram(offs_t offset, uint8_t data)
{
	if ((m_ram_bank & 0x10) != 0)
	{
		if (offset == 0)
		{
			m_camera_regs[0] = data & 0x7;
			if (data & 0x1) update_camera();
		}
		else if (offset < 54)
		{
			m_camera_regs[offset] = data;
		}
	}
	else if (m_ram_enable && (m_camera_regs[0] & 0x1) == 0)
	{
		// RAM
		if (!m_ram.empty())
			m_ram[ram_bank_map[m_ram_bank] * 0x2000 + (offset & 0x1fff)] = data;
	}
}

} // anonymous namespace


// device type definition
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MBC1,     device_gb_cart_interface, gb_rom_mbc1_device,        "gb_rom_mbc1",     "GB MBC1 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MBC2,     device_gb_cart_interface, gb_rom_mbc2_device,        "gb_rom_mbc2",     "GB MBC2 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MBC3,     device_gb_cart_interface, gb_rom_mbc3_device,        "gb_rom_mbc3",     "GB MBC3 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MBC5,     device_gb_cart_interface, gb_rom_mbc5_device,        "gb_rom_mbc5",     "GB MBC5 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MBC6,     device_gb_cart_interface, gb_rom_mbc6_device,        "gb_rom_mbc6",     "GB MBC6 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MBC7,     device_gb_cart_interface, gb_rom_mbc7_device,        "gb_rom_mbc7",     "GB MBC7 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_M161,     device_gb_cart_interface, gb_rom_m161_device,        "gb_rom_m161",     "GB M161 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_MMM01,    device_gb_cart_interface, gb_rom_mmm01_device,       "gb_rom_mmm01",    "GB MMM01 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_SACHEN1,  device_gb_cart_interface, gb_rom_sachen_mmc1_device, "gb_rom_sachen1",  "GB Sachen MMC1 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_SACHEN2,  device_gb_cart_interface, gb_rom_sachen_mmc2_device, "gb_rom_sachen2",  "GB Sachen MMC2 Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_188IN1,   device_gb_cart_interface, gb_rom_188in1_device,      "gb_rom_188in1",   "GB 188in1")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_SINTAX,   device_gb_cart_interface, gb_rom_sintax_device,      "gb_rom_sintax",   "GB MBC5 Sintax Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_CHONGWU,  device_gb_cart_interface, gb_rom_chongwu_device,     "gb_rom_chongwu",  "GB Chong Wu Xiao Jing Ling")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_LICHENG,  device_gb_cart_interface, gb_rom_licheng_device,     "gb_rom_licheng",  "GB MBC5 Li Cheng Carts")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_DIGIMON,  device_gb_cart_interface, gb_rom_digimon_device,     "gb_rom_digimon",  "GB Digimon")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_ROCKMAN8, device_gb_cart_interface, gb_rom_rockman8_device,    "gb_rom_rockman8", "GB MBC1 Rockman 8")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_SM3SP,    device_gb_cart_interface, gb_rom_sm3sp_device,       "gb_sm3sp",        "GB MBC1 Super Mario 3 Special")
DEFINE_DEVICE_TYPE_PRIVATE(GB_ROM_CAMERA,   device_gb_cart_interface, gb_rom_camera_device,      "gb_rom_camera",   "GB Camera")
