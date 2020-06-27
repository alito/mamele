// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

#ifndef MAME_MACHINE_INTERPRO_ARBGA_H
#define MAME_MACHINE_INTERPRO_ARBGA_H

#pragma once

class interpro_arbga_device : public device_t
{
public:
	interpro_arbga_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual void map(address_map &map);

	u32 sdepid_r() { return m_sdepid; }
	void sdepid_w(u32 data) { m_sdepid = data; }

	enum arbsnap_mask
	{
		ARBSNAP_GRANT   = 0x0000000f,
		ARBSNAP_HOGLOCK = 0x000000f0
	};
	u32 arbsnap_r() { return m_arbsnap; }
	void arbsnap_w(u32 data) { m_arbsnap = data; }
	u32 fixprils_r() { return m_fixprils; }
	void fixprils_w(u32 data) { m_fixprils = data; }
	u32 fixprims_r() { return m_fixprims; }
	void fixprims_w(u32 data) { m_fixprims = data; }

	u32 sysdomls_r() { return m_sysdomls; }
	void sysdomls_w(u32 data) { m_sysdomls = data; }
	u32 sysdomms_r() { return m_sysdomms; }
	void sysdomms_w(u32 data) { m_sysdomms = data; }

	enum tctrl_mask
	{
		TCTRL_UNUSED = 0x00000007,
		TCTRL_ENNEM  = 0x00000008,
		TCTRL_ENHOG  = 0x00000010
	};
	u32 tctrl_r() { return m_tctrl; }
	void tctrl_w(u32 data) { m_tctrl = data; }

	u8 inem_r() { return m_inem; }
	void inem_w(u8 data) { m_inem = data; }
	u8 enem_r() { return m_enem; }
	void enem_w(u8 data) { m_enem = data; }

	u32 hog_r() { return m_hog; }
	void hog_w(u32 data) { m_hog = data; }
	u32 lock_r() { return m_lock; }
	void lock_w(u32 data) { m_lock = data; }
	u32 lockprs_r() { return m_lockprs; }
	void lockprs_w(u32 data) { m_lockprs = data; }

	u32 hiblockls_r() { return m_hiblockls; }
	void hiblockls_w(u32 data) { m_hiblockls = data; }
	u32 hiblockms_r() { return m_hiblockms; }
	void hiblockms_w(u32 data) { m_hiblockms = data; }

	u32 arbrev_r() { return m_arbrev; }
	void arbrev_w(u32 data) { m_arbrev = data; }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	u32 m_sdepid;
	u32 m_arbsnap;
	u32 m_fixprils;
	u32 m_fixprims;
	u32 m_sysdomls;
	u32 m_sysdomms;
	u32 m_tctrl;
	u8 m_inem;
	u8 m_enem;
	u32 m_hog;
	u32 m_lock;
	u32 m_lockprs;
	u32 m_hiblockls;
	u32 m_hiblockms;
	u32 m_arbrev;
};

// device type definition
DECLARE_DEVICE_TYPE(INTERPRO_ARBGA, interpro_arbga_device)

#endif // MAME_MACHINE_INTERPRO_ARBGA_H
