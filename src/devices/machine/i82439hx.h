// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
// Intel i82439hx northbridge (440hx)

#ifndef MAME_MACHINE_I82439HX_H
#define MAME_MACHINE_I82439HX_H

#pragma once

#include "pci.h"

class i82439hx_host_device : public pci_host_device {
public:
	template <typename T>
	i82439hx_host_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&cpu_tag, int ram_size)
		: i82439hx_host_device(mconfig, tag, owner, clock)
	{
		set_ids_host(0x80861250, 0x03, 0x00000000);
		set_cpu_tag(std::forward<T>(cpu_tag));
		set_ram_size(ram_size);
	}
	i82439hx_host_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <typename T> void set_cpu_tag(T &&tag) { cpu.set_tag(std::forward<T>(tag)); }
	void set_ram_size(int ram_size);

	DECLARE_WRITE_LINE_MEMBER(smi_act_w);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

	virtual void reset_all_mappings() override;

	virtual void map_extra(uint64_t memory_window_start, uint64_t memory_window_end, uint64_t memory_offset, address_space *memory_space,
						   uint64_t io_window_start, uint64_t io_window_end, uint64_t io_offset, address_space *io_space) override;

	virtual void config_map(address_map &map) override;

private:
	int ram_size;
	required_device<device_memory_interface> cpu;
	std::vector<uint32_t> ram;

	uint8_t pcon, cc, dramec, dramc, dramt;
	uint8_t pam[7], drb[8];
	uint8_t drt, drat, smram, errcmd, errsts, errsyn;
	int smiact_n;

	virtual uint8_t header_type_r() override;
	uint8_t pcon_r();
	void pcon_w(uint8_t data);
	uint8_t cc_r();
	void cc_w(uint8_t data);
	uint8_t dramec_r();
	void dramec_w(uint8_t data);
	uint8_t dramc_r();
	void dramc_w(uint8_t data);
	uint8_t dramt_r();
	void dramt_w(uint8_t data);
	uint8_t pam_r(offs_t offset);
	void pam_w(offs_t offset, uint8_t data);
	uint8_t drb_r(offs_t offset);
	void drb_w(offs_t offset, uint8_t data);
	uint8_t drt_r();
	void drt_w(uint8_t data);
	uint8_t drat_r();
	void drat_w(uint8_t data);
	uint8_t smram_r();
	void smram_w(uint8_t data);
	uint8_t errcmd_r();
	void errcmd_w(uint8_t data);
	uint8_t errsts_r();
	void errsts_w(uint8_t data);
	uint8_t errsyn_r();
};

DECLARE_DEVICE_TYPE(I82439HX, i82439hx_host_device)

#endif // MAME_MACHINE_I82439HX_H
