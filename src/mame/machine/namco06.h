// license:BSD-3-Clause
// copyright-holders:Mike Harris, Aaron Giles
#ifndef MAME_MACHINE_NAMCO06_H
#define MAME_MACHINE_NAMCO06_H

#pragma once


/* device get info callback */
class namco_06xx_device : public device_t
{
public:
	namco_06xx_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	template <typename T> void set_maincpu(T &&tag) { m_nmicpu.set_tag(std::forward<T>(tag)); }

	template <unsigned N> auto chip_select_callback() { return m_chipsel[N].bind(); }
	template <unsigned N> auto rw_callback() { return m_rw[N].bind(); }
	template <unsigned N> auto read_callback() { return m_read[N].bind(); }
	template <unsigned N> auto write_callback() { return m_write[N].bind(); }

	uint8_t data_r(offs_t offset);
	void data_w(offs_t offset, uint8_t data);
	uint8_t ctrl_r();
	void ctrl_w(uint8_t data);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	void set_nmi(int state);

	TIMER_CALLBACK_MEMBER( nmi_generate );

	// internal state
	emu_timer *m_nmi_timer;
	uint8_t m_control;
	bool m_next_timer_state;
	bool m_nmi_stretch;
	bool m_rw_stretch;
	bool m_rw_change;


	required_device<cpu_device> m_nmicpu;

	devcb_write_line::array<4> m_chipsel;
	devcb_write_line::array<4> m_rw;
	devcb_read8::array<4> m_read;
	devcb_write8::array<4> m_write;
};

DECLARE_DEVICE_TYPE(NAMCO_06XX, namco_06xx_device)


#endif // MAME_MACHINE_NAMCO06_H
