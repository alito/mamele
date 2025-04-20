// license:BSD-3-Clause
// copyright-holders:Pierpaolo Prazzoli

#include "bus/nes_ctrl/zapper_sensor.h"
#include "machine/nvram.h"
#include "sound/sn76496.h"
#include "video/ppu2c0x.h"

class vsnes_state : public driver_device
{
public:
	vsnes_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
		, m_subcpu(*this, "sub")
		, m_ppu1(*this, "ppu1")
		, m_ppu2(*this, "ppu2")
		, m_sn1(*this, "sn1")
		, m_sn2(*this, "sn2")
		, m_sensor(*this, "sensor")
		, m_nvram(*this, "nvram")
		, m_gfx1_rom(*this, "gfx1")
		, m_in(*this, "IN%u", 0U)
		, m_coins(*this, "COINS%u", 1U)
		, m_dsw(*this, "DSW%u", 0U)
		, m_gunx(*this, "GUNX")
		, m_guny(*this, "GUNY")
		, m_prg_banks(*this, "prg%u", 0U)
		, m_prg_view(*this, "prg_view")
		, m_chr_banks(*this, "chr%u", 0U)
		, m_chr_view(*this, "chr_view")
	{
	}

	void vsdual(machine_config &config);
	void vsgshoe(machine_config &config);
	void vsnes(machine_config &config);
	void vsdual_pi(machine_config &config);
	void topgun(machine_config &config);
	void mightybj(machine_config &config);
	void vsnes_bootleg(machine_config &config);
	void jajamaru(machine_config &config);

	void init_vskonami();
	void init_vsvram();
	void init_bnglngby();
	void init_drmario();
	void init_vs108();
	void init_vsfdf();
	void init_tkoboxng();
	void init_vsgun();
	void init_supxevs();
	void init_vsgshoe();
	void init_vsnormal();
	void init_platoon();
	void init_rbibb();
	void init_vsdual();
	void init_bootleg();

protected:
	virtual void machine_reset() override;

private:
	enum { MAIN = 0, SUB = 1 };

	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_subcpu;
	required_device<ppu2c0x_device> m_ppu1;
	optional_device<ppu2c0x_device> m_ppu2;
	optional_device<sn76489_device> m_sn1;
	optional_device<sn76489_device> m_sn2;

	optional_device<nes_zapper_sensor_device> m_sensor;
	optional_device<nvram_device> m_nvram;

	optional_memory_region m_gfx1_rom;

	optional_ioport_array<4> m_in;
	optional_ioport_array<2> m_coins;
	optional_ioport_array<2> m_dsw;
	optional_ioport m_gunx;
	optional_ioport m_guny;

	template <u8 Side> void sprite_dma_w(address_space &space, u8 data);
	template <u8 Side> void vsnes_coin_counter_w(offs_t offset, u8 data);
	template <u8 Side> u8 vsnes_coin_counter_r(offs_t offset);
	template <u8 Side> void vsnes_in0_w(u8 data);
	template <u8 Side> u8 vsnes_in0_r();
	template <u8 Side> u8 vsnes_in1_r();
	void vsnormal_vrom_banking(uint8_t data);
	void vskonami_rom_banking(offs_t offset, uint8_t data);
	void vsgshoe_gun_in0_w(uint8_t data);
	void drmario_rom_banking(offs_t offset, uint8_t data);
	void vsvram_rom_banking(uint8_t data);
	void vs108_rom_banking(offs_t offset, uint8_t data);
	u8 rbibb_prot_r(offs_t offset);
	u8 supxevs_prot_1_r();
	u8 supxevs_prot_2_r();
	u8 supxevs_prot_3_r();
	u8 supxevs_prot_4_r();
	u8 tkoboxng_prot_r(offs_t offset);
	void sunsoft3_rom_banking(offs_t offset, uint8_t data);
	void set_bnglngby_irq_w(uint8_t data);
	uint8_t set_bnglngby_irq_r();
	void vsdual_vrom_banking_main(uint8_t data);
	void vsdual_vrom_banking_sub(uint8_t data);
	void vssmbbl_sn_w(offs_t offset, uint8_t data);

	DECLARE_MACHINE_START(vsnes);
	DECLARE_MACHINE_START(vsdual);
	DECLARE_MACHINE_START(bootleg);
	void v_set_videorom_bank(int start, int count, int vrom_start_bank);

	void bootleg_sound_write(offs_t offset, uint8_t data);
	uint8_t vsnes_bootleg_z80_data_r();
	uint8_t vsnes_bootleg_z80_address_r(offs_t offset);
	void vsnes_bootleg_scanline(int scanline, bool vblank, bool blanked);
	uint8_t vsnes_bootleg_ppudata();

	void vsnes_bootleg_z80_map(address_map &map);
	void vsnes_cpu1_bootleg_map(address_map &map);
	void vsnes_ppu_bootleg_map(address_map &map);
	void vsnes_cpu1_map(address_map &map);
	void vsnes_cpu2_map(address_map &map);
	void vsnes_ppu1_map(address_map &map);
	void vsnes_ppu2_map(address_map &map);

	void init_prg_banking();
	void prg32(int bank);
	void prg16(int slot, int bank);
	void prg8(int slot, int bank);

	memory_bank_array_creator<4> m_prg_banks;
	memory_view m_prg_view;
	int m_prg_chunks = 0;

	memory_bank_array_creator<8> m_chr_banks;
	memory_view m_chr_view;
	int m_chr_chunks = 0;

	bool m_has_gun = false;
	int m_coin = 0;
	int m_input_latch[4]{};
	int m_input_strobe[2]{};
	int m_mmc1_shiftreg = 0;
	int m_mmc1_shiftcount = 0;
	int m_mmc1_prg16k = 0;
	int m_mmc1_switchlow = 0;
	int m_mmc1_chr4k = 0;
	int m_108_reg = 0;
	int m_prot_index = 0;
	int m_ret = 0;

	u8 m_bootleg_sound_offset = 0U;
	u8 m_bootleg_sound_data = 0U;
	int m_bootleg_latched_scanline = 0;
};
