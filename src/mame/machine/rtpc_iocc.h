// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

#ifndef MAME_MACHINE_RTPC_IOCC_H
#define MAME_MACHINE_RTPC_IOCC_H

#pragma once

#include "cpu/romp/rsc.h"

class rtpc_iocc_device
	: public device_t
	, public device_memory_interface
	, public rsc_bus_interface
{
public:
	rtpc_iocc_device(machine_config const &mconfig, char const *tag, device_t *owner, u32 clock);

	auto out_int() { return m_out_int.bind(); }
	auto out_rst() { return m_out_rst.bind(); }

	enum ccr_mask : u8
	{
		CCR_RFE = 0x02, // refresh enable
		CCR_RFT = 0x04, // refresh period (0=14.9µs, 1=6.4µs)
		CCR_MMP = 0x08, // memory map privilege
		CCR_IMP = 0x10, // i/o map privilege
		CCR_DKA = 0x20, // dma keyboard access
		CCR_DSA = 0x40, // dma i/o subsystem access
		CCR_DCA = 0x80, // dma control register access
	};

	enum csr_mask : u32
	{
		CSR_EXR   = 0x80000000, // exception reported
		CSR_INTP  = 0x40000000, // interrupt pending
		CSR_EPOW  = 0x10000000, // early power off warning
		CSR_SRST  = 0x08000000, // soft reset
		CSR_SAT   = 0x04000000, // system attention
		CSR_PER   = 0x01000000, // pio error
		CSR_DE0   = 0x00800000, // dma error channel 0
		CSR_DE1   = 0x00400000, // dma error channel 1
		CSR_DE2   = 0x00200000, // dma error channel 2
		CSR_DE3   = 0x00100000, // dma error channel 3
		CSR_DE5   = 0x00080000, // dma error channel 5
		CSR_DE6   = 0x00040000, // dma error channel 6
		CSR_DE7   = 0x00020000, // dma error channel 7
		CSR_DE8   = 0x00010000, // dma error channel 8
		CSR_PD    = 0x00008000, // pio/dma
		CSR_PVIO  = 0x00004000, // protection violation
		CSR_INVOP = 0x00002000, // invalid operation
		CSR_IOCK  = 0x00001000, // i/o channel check
		CSR_DEXK  = 0x00000800, // dma exception
		CSR_CRC   = 0x00000400, // channel reset captured
		CSR_SBB   = 0x00000200, // system board busy
		CSR_PRP   = 0x00000100, // pio request pending

		CSR_RSV   = 0x220000ff, // reserved
	};

	enum tcw_mask : u16
	{
		TCW_PFX = 0x1fff, // translation prefix
		TCW_IOC = 0x4000, // i/o channel destination
		TCW_VIR = 0x8000, // virtual access
	};

	using rsc_mode = rsc_bus_interface::rsc_mode;

	// rsc_pio_interface overrides
	virtual bool load(u32 address, u8 &data, rsc_mode const mode, bool sp) override { return load<u8>(address, data, mode); }
	virtual bool load(u32 address, u16 &data, rsc_mode const mode, bool sp) override { return load<u16>(address, data, mode); }
	virtual bool load(u32 address, u32 &data, rsc_mode const mode, bool sp) override { return load<u32>(address, data, mode); }
	virtual bool store(u32 address, u8 data, rsc_mode const mode, bool sp) override { return store<u8>(address, data, mode); }
	virtual bool store(u32 address, u16 data, rsc_mode const mode, bool sp) override { return store<u16>(address, data, mode); }
	virtual bool store(u32 address, u32 data, rsc_mode const mode, bool sp) override { return store<u32>(address, data, mode); }
	virtual bool modify(u32 address, std::function<u8(u8)> f, rsc_mode const mode) override { return modify<u8>(address, f, mode); }
	virtual bool modify(u32 address, std::function<u16(u16)> f, rsc_mode const mode) override { return modify<u16>(address, f, mode); }
	virtual bool modify(u32 address, std::function<u32(u32)> f, rsc_mode const mode) override { return modify<u32>(address, f, mode); }

	u8 ccr_r() { return m_ccr; }
	void ccr_w(u8 data) { m_ccr = data; }

	template <unsigned Word> u16 csr_r() { return u16((m_csr | CSR_RSV) >> (16 * Word)); }
	void csr_w(u16 data) { set_int(false); m_csr = 0; }

	u8 dma_b_r(offs_t offset);
	u8 dma_w_r(offs_t offset);
	void dma_b_w(offs_t offset, u8 data);
	void dma_w_w(offs_t offset, u8 data);

	u8 dmr_r() { return m_dmr; }
	u8 dbr_r() { return m_dbr; }
	void dmr_w(u8 data) { m_dmr = data; }
	void dbr_w(u8 data) { m_dbr = data; }

	u16 tcw_r(offs_t offset) { return m_tcw[offset]; }
	void tcw_w(offs_t offset, u16 data, u16 mem_mask) { COMBINE_DATA(&m_tcw[offset]); }

	template <unsigned Channel> void dack_w(int state) { if (!state) m_adc = Channel; }

protected:
	// device_t overrides
	virtual void device_start() override;
	virtual void device_reset() override;

	// device_memory_interface overrides
	virtual space_config_vector memory_space_config() const override;
	//virtual bool memory_translate(int spacenum, int intention, offs_t &address) override;

	template <typename T> bool load(u32 address, T &data, rsc_mode const mode);
	template <typename T> bool store(u32 address, T data, rsc_mode const mode);
	template <typename T> bool modify(u32 address, std::function<T(T)> f, rsc_mode const mode);

	void set_int(bool state)
	{
		if (state != m_out_int_state)
		{
			if (state)
				m_csr |= CSR_INTP;
			m_out_int_state = state;
			m_out_int(!m_out_int_state);
		}
	}

	unsigned target_size(u32 const address) const
	{
		if (address < 0xf000'8000U)
		{
			if (address == 0xf000'0110U || address == 0xf000'0112U)
				return 2;
			else
				// FIXME: get size from isa bus
				return 1;
		}
		else if (address == 0xf000'8400U)
			// exception for kls
			return 2;
		else if (address < 0xf001'0000U)
			return 1;
		else if (address < 0xf001'0800U)
			return 2;
		else
			return 4;
	}

private:
	address_space_config m_mem_config;
	address_space_config m_pio_config;

	devcb_write_line m_out_int;
	devcb_write_line m_out_rst;

	required_device<rsc_bus_interface> m_rsc;

	u32 m_csr;       // channel status register
	u8 m_ccr;        // channel control register

	u8 m_dbr;        // dma buffer register
	u8 m_dmr;        // dma mode register
	u16 m_tcw[1024]; // dma translation control words

	unsigned m_adc;  // active dma channel

	bool m_out_int_state;
};

DECLARE_DEVICE_TYPE(RTPC_IOCC, rtpc_iocc_device)

#endif // MAME_MACHINE_RTPC_IOCC_H
