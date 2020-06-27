// license:BSD-3-Clause
// copyright-holders:Pierpaolo Prazzoli

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
		, m_work_ram(*this, "work_ram")
		, m_work_ram_1(*this, "work_ram_1")
		, m_gfx1_rom(*this, "gfx1")
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
	void init_MMC3();
	void init_vsfdf();
	void init_tkoboxng();
	void init_vsgun();
	void init_supxevs();
	void init_vsgshoe();
	void init_vsnormal();
	void init_platoon();
	void init_rbibb();
	void init_vsdual();

private:
	required_device<cpu_device> m_maincpu;
	optional_device<cpu_device> m_subcpu;
	required_device<ppu2c0x_device> m_ppu1;
	optional_device<ppu2c0x_device> m_ppu2;
	optional_device<sn76489_device> m_sn1;
	optional_device<sn76489_device> m_sn2;

	required_shared_ptr<uint8_t> m_work_ram;
	optional_shared_ptr<uint8_t> m_work_ram_1;

	optional_memory_region m_gfx1_rom;

	void sprite_dma_0_w(address_space &space, uint8_t data);
	void sprite_dma_1_w(address_space &space, uint8_t data);
	void vsnes_coin_counter_w(uint8_t data);
	uint8_t vsnes_coin_counter_r();
	void vsnes_coin_counter_1_w(uint8_t data);
	void vsnes_in0_w(uint8_t data);
	uint8_t vsnes_in0_r();
	uint8_t vsnes_in1_r();
	void vsnes_in0_1_w(uint8_t data);
	uint8_t vsnes_in0_1_r();
	uint8_t vsnes_in1_1_r();
	uint8_t gun_in0_r();
	void vsnes_nt0_w(offs_t offset, uint8_t data);
	void vsnes_nt1_w(offs_t offset, uint8_t data);
	uint8_t vsnes_nt0_r(offs_t offset);
	uint8_t vsnes_nt1_r(offs_t offset);
	void vsnormal_vrom_banking(uint8_t data);
	void gun_in0_w(uint8_t data);
	void vskonami_rom_banking(offs_t offset, uint8_t data);
	void vsgshoe_gun_in0_w(uint8_t data);
	void drmario_rom_banking(offs_t offset, uint8_t data);
	void vsvram_rom_banking(uint8_t data);
	void mapper4_w(offs_t offset, uint8_t data);
	uint8_t rbi_hack_r(offs_t offset);
	uint8_t supxevs_read_prot_1_r();
	uint8_t supxevs_read_prot_2_r();
	uint8_t supxevs_read_prot_3_r();
	uint8_t supxevs_read_prot_4_r();
	uint8_t tko_security_r(offs_t offset);
	void mapper68_rom_banking(offs_t offset, uint8_t data);
	void set_bnglngby_irq_w(uint8_t data);
	uint8_t set_bnglngby_irq_r();
	void vsdual_vrom_banking_main(uint8_t data);
	void vsdual_vrom_banking_sub(uint8_t data);
	void vssmbbl_sn_w(offs_t offset, uint8_t data);
	void v_set_mirroring(int ppu, int mirroring);

	DECLARE_MACHINE_START(vsnes);
	DECLARE_MACHINE_RESET(vsnes);
	DECLARE_MACHINE_START(vsdual);
	DECLARE_MACHINE_RESET(vsdual);
	void v_set_videorom_bank(  int start, int count, int vrom_start_bank );
	void mapper4_set_prg(  );
	void mapper4_set_chr(  );
	void mapper4_irq( int scanline, int vblank, int blanked );

	uint8_t vsnes_bootleg_z80_latch_r();
	void bootleg_sound_write(offs_t offset, uint8_t data);
	uint8_t vsnes_bootleg_z80_data_r();
	uint8_t vsnes_bootleg_z80_address_r();

	void vsnes_bootleg_z80_map(address_map &map);
	void vsnes_cpu1_bootleg_map(address_map &map);
	void vsnes_cpu1_map(address_map &map);
	void vsnes_cpu2_map(address_map &map);

	int m_coin;
	int m_do_vrom_bank;
	int m_input_latch[4];
	int m_sound_fix;
	uint8_t m_last_bank;
	std::unique_ptr<uint8_t[]> m_vram;
	uint8_t* m_vrom[2];
	std::unique_ptr<uint8_t[]> m_nt_ram[2];
	uint8_t* m_nt_page[2][4];
	uint32_t m_vrom_size[2];
	int m_vrom_banks;
	int m_zapstore;
	int m_old_bank;
	int m_drmario_shiftreg;
	int m_drmario_shiftcount;
	int m_size16k;
	int m_switchlow;
	int m_vrom4k;
	int m_MMC3_cmd;
	int m_MMC3_prg_bank[4];
	int m_MMC3_chr_bank[6];
	int m_MMC3_prg_mask;
	int m_IRQ_enable;
	int m_IRQ_count;
	int m_IRQ_count_latch;
	int m_VSindex;
	int m_supxevs_prot_index;
	int m_security_counter;
	int m_ret;

	uint8_t m_bootleg_sound_offset;
	uint8_t m_bootleg_sound_data;
};
