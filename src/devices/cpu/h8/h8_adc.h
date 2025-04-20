// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
/***************************************************************************

    h8_adc.h

    H8 Analog to Digital Converter subsystem


***************************************************************************/

#ifndef MAME_CPU_H8_H8_ADC_H
#define MAME_CPU_H8_H8_ADC_H

#pragma once

#include "h8.h"
#include "h8_intc.h"

class h8_adc_device : public device_t {
public:
	template<typename T, typename U> void set_info(T &&cpu, U &&intc, int vect) { m_cpu.set_tag(std::forward<T>(cpu)), m_intc.set_tag(std::forward<U>(intc)); m_intc_vector = vect; }

	uint8_t addr8_r(offs_t offset);
	uint16_t addr16_r(offs_t offset);
	uint8_t adcsr_r();
	uint8_t adcr_r();
	void adcsr_w(uint8_t data);
	void adcr_w(uint8_t data);
	void adtrg_w(int state);

	void set_suspend(bool suspend);
	uint64_t internal_update(uint64_t current_time);

protected:
	h8_adc_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	required_device<h8_device> m_cpu;
	required_device<h8_intc_device> m_intc;
	int m_intc_vector;

	enum {
		T_SOFT  = 1<<0,
		T_TPU   = 1<<1,
		T_TIMER = 1<<2,
		T_EXT   = 1<<3
	};

	enum {
		F_ADF  = 0x80,
		F_ADIE = 0x40,
		F_ADST = 0x20
	};

	enum {
		IDLE = 0,
		ACTIVE = 1,
		HALTED = 2,
		REPEAT = 4,
		ROTATE = 8,
		DUAL = 16,
		BUFFER = 32,
		COUNTED = 64
	};

	uint16_t m_addr[8], m_buf[2];
	uint8_t m_adcsr, m_adcr;
	int m_register_mask;
	int m_trigger, m_start_mode, m_start_channel, m_end_channel, m_start_count;
	bool m_suspend_on_interrupt, m_analog_power_control;
	int m_mode, m_channel, m_count;
	bool m_analog_powered, m_adtrg;
	uint64_t m_next_event;

	virtual void device_start() override;
	virtual void device_reset() override;

	void sampling();
	void start_conversion();
	void conversion_wait(bool first, bool poweron, uint64_t current_time = 0);
	void buffer_value(int port, int buffer = 0);
	void commit_value(int reg, int buffer = 0);
	void timeout(uint64_t current_time);
	void done();

	virtual int conversion_time(bool first, bool poweron) = 0;
	virtual void mode_update() = 0;
	virtual void do_buffering(int buffer);
	virtual int get_channel_index(int count);
};

class h8_adc_3337_device : public h8_adc_device {
public:
	h8_adc_3337_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template <typename T, typename U> h8_adc_3337_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu, U &&intc, int vect)
		: h8_adc_3337_device(mconfig, tag, owner, 0)
	{
		set_info(cpu, intc, vect);
	}

protected:
	virtual int conversion_time(bool first, bool poweron) override;
	virtual void mode_update() override;
};

class h8_adc_3006_device : public h8_adc_device {
public:
	h8_adc_3006_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template <typename T, typename U> h8_adc_3006_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu, U &&intc, int vect)
		: h8_adc_3006_device(mconfig, tag, owner, 0)
	{
		set_info(cpu, intc, vect);
	}

protected:
	virtual int conversion_time(bool first, bool poweron) override;
	virtual void mode_update() override;
};

class h8_adc_2245_device : public h8_adc_device {
public:
	h8_adc_2245_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template <typename T, typename U> h8_adc_2245_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu, U &&intc, int vect)
		: h8_adc_2245_device(mconfig, tag, owner, 0)
	{
		set_info(cpu, intc, vect);
	}

protected:
	virtual int conversion_time(bool first, bool poweron) override;
	virtual void mode_update() override;
};

class h8_adc_2320_device : public h8_adc_device {
public:
	h8_adc_2320_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template <typename T, typename U> h8_adc_2320_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu, U &&intc, int vect)
		: h8_adc_2320_device(mconfig, tag, owner, 0)
	{
		set_info(cpu, intc, vect);
	}

protected:
	virtual int conversion_time(bool first, bool poweron) override;
	virtual void mode_update() override;
};

class h8_adc_2357_device : public h8_adc_device {
public:
	h8_adc_2357_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template <typename T, typename U> h8_adc_2357_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu, U &&intc, int vect)
		: h8_adc_2357_device(mconfig, tag, owner, 0)
	{
		set_info(cpu, intc, vect);
	}

protected:
	virtual int conversion_time(bool first, bool poweron) override;
	virtual void mode_update() override;
};

class h8_adc_2655_device : public h8_adc_device {
public:
	h8_adc_2655_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);
	template <typename T, typename U> h8_adc_2655_device(const machine_config &mconfig, const char *tag, device_t *owner, T &&cpu, U &&intc, int vect)
		: h8_adc_2655_device(mconfig, tag, owner, 0)
	{
		set_info(cpu, intc, vect);
	}

protected:
	virtual int conversion_time(bool first, bool poweron) override;
	virtual void mode_update() override;
	virtual void do_buffering(int buffer) override;
	virtual int get_channel_index(int count) override;
};

DECLARE_DEVICE_TYPE(H8_ADC_3337, h8_adc_3337_device)
DECLARE_DEVICE_TYPE(H8_ADC_3006, h8_adc_3006_device)
DECLARE_DEVICE_TYPE(H8_ADC_2245, h8_adc_2245_device)
DECLARE_DEVICE_TYPE(H8_ADC_2320, h8_adc_2320_device)
DECLARE_DEVICE_TYPE(H8_ADC_2357, h8_adc_2357_device)
DECLARE_DEVICE_TYPE(H8_ADC_2655, h8_adc_2655_device)

#endif // MAME_CPU_H8_H8_ADC_H
