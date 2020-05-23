// license:BSD-3-Clause
// copyright-holders:Tomasz Slanina, David Haywood
/* ST0016 - CPU (z80) + Sound + Video */

#ifndef MAME_MACHINE_ST0016_H
#define MAME_MACHINE_ST0016_H

#pragma once

#include "cpu/z80/z80.h"
#include "sound/st0016.h"
#include "screen.h"


class st0016_cpu_device : public z80_device, public device_gfx_interface
{
public:
	typedef device_delegate<uint8_t ()> dma_offs_delegate;

	st0016_cpu_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t);

	template <typename... T> void set_dma_offs_callback(T &&... args) { m_dma_offs_cb.set(std::forward<T>(args)...); }

	void set_game_flag(uint32_t flag) { m_game_flag = flag; }

	void draw_sprites(bitmap_ind16 &bitmap, const rectangle &cliprect);
	void save_init();
	void draw_bgmap(bitmap_ind16 &bitmap,const rectangle &cliprect, int priority);

	void startup();
	uint32_t update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	void draw_screen(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);

	std::unique_ptr<uint8_t[]> spriteram;
	std::unique_ptr<uint8_t[]> paletteram;


	int32_t spr_bank,spr2_bank,pal_bank,char_bank;
	int spr_dx,spr_dy;

	uint8_t vregs[0xc0];
	int ramgfx;
	std::unique_ptr<uint8_t[]> m_charram;

	void cpu_internal_io_map(address_map &map);
	void cpu_internal_map(address_map &map);
protected:
	bool ismacs() const { return m_game_flag & 0x80; }
	bool ismacs1() const { return (m_game_flag & 0x180) == 0x180; }
	bool ismacs2() const { return (m_game_flag & 0x180) == 0x080; }


	static constexpr unsigned MAX_SPR_BANK   = 0x10;
	static constexpr unsigned MAX_CHAR_BANK  = 0x10000;
	static constexpr unsigned MAX_PAL_BANK   = 4;

	static constexpr unsigned SPR_BANK_SIZE  = 0x1000;
	static constexpr unsigned CHAR_BANK_SIZE = 0x20;
	static constexpr unsigned PAL_BANK_SIZE  = 0x200;

	static constexpr unsigned UNUSED_PEN = 1024;

	static constexpr unsigned SPR_BANK_MASK  = MAX_SPR_BANK - 1;
	static constexpr unsigned CHAR_BANK_MASK = MAX_CHAR_BANK - 1;
	static constexpr unsigned PAL_BANK_MASK  = MAX_PAL_BANK - 1;


	// device-level overrides
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
	virtual void device_reset() override;

	const address_space_config m_io_space_config;
	const address_space_config m_space_config;


	virtual space_config_vector memory_space_config() const override;

	required_device<screen_device> m_screen;
	required_memory_region m_rom;

private:
	uint8_t m_dma_offset;
	dma_offs_delegate m_dma_offs_cb;
	uint32_t m_game_flag;

	uint8_t soundram_read(offs_t offset);
	void sprite_bank_w(uint8_t data);
	void palette_bank_w(uint8_t data);
	void character_bank_w(offs_t offset, uint8_t data);
	uint8_t sprite_ram_r(offs_t offset);
	void sprite_ram_w(offs_t offset, uint8_t data);
	uint8_t sprite2_ram_r(offs_t offset);
	void sprite2_ram_w(offs_t offset, uint8_t data);
	uint8_t palette_ram_r(offs_t offset);
	void palette_ram_w(offs_t offset, uint8_t data);
	uint8_t character_ram_r(offs_t offset);
	void character_ram_w(offs_t offset, uint8_t data);
	uint8_t vregs_r(offs_t offset);
	uint8_t dma_r();
	void vregs_w(offs_t offset, uint8_t data);
};


// device type definition
DECLARE_DEVICE_TYPE(ST0016_CPU, st0016_cpu_device)


#endif // MAME_MACHINE_ST0016_H
