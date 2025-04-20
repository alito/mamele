// license:BSD-3-Clause
// copyright-holders:Wilbert Pol
#ifndef MAME_BUS_MSX_SLOT_DISK_H
#define MAME_BUS_MSX_SLOT_DISK_H

#pragma once

#include "slot.h"
#include "rom.h"
#include "machine/wd_fdc.h"
#include "machine/upd765.h"
#include "imagedev/floppy.h"


// WD FDC accessed through 7ffx
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_FD1793, msx_slot_disk1_fd1793_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_MB8877, msx_slot_disk1_mb8877_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_WD2793_N, msx_slot_disk1_wd2793_n_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_WD2793_N_2_DRIVES, msx_slot_disk1_wd2793_n_2_drives_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_WD2793, msx_slot_disk1_wd2793_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_WD2793_0, msx_slot_disk1_wd2793_0_device) // No drives, for nms8260
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_WD2793_SS, msx_slot_disk1_wd2793_ss_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK1_WD2793_2_DRIVES, msx_slot_disk1_wd2793_2_drives_device)
// WD FDC accessed through 7fbx
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK2_FD1793_SS, msx_slot_disk2_fd1793_ss_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK2_MB8877, msx_slot_disk2_mb8877_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK2_MB8877_SS, msx_slot_disk2_mb8877_ss_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK2_MB8877_2_DRIVES, msx_slot_disk2_mb8877_2_drives_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK2_WD2793, msx_slot_disk2_wd2793_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK2_WD2793_2_DRIVES, msx_slot_disk2_wd2793_2_drives_device)
// TC8566 accessed through 7ff8-7fff
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK3_TC8566, msx_slot_disk3_tc8566_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK3_TC8566_2_DRIVES, msx_slot_disk3_tc8566_2_drives_device)
// TC8566 accessed through 7ff0-7ff7 (used in Turob-R, untested)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK4_TC8566, msx_slot_disk4_tc8566_device)
// WD FDC accessed through i/o ports 0xd0-0xd4
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK5_WD2793, msx_slot_disk5_wd2793_device)
// WD FDC accessed through 7ff0-7ff? (used in Toshiba HX34)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK6_WD2793_N, msx_slot_disk6_wd2793_n_device)
// MB FDC accessed through 7ff8-7ffc (used in Canon V-30F)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK7_MB8877, msx_slot_disk7_mb8877_device)
// WD FDC accessed through 7f8x
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK8_MB8877, msx_slot_disk8_mb8877_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK8_WD2793_2_DRIVES, msx_slot_disk8_wd2793_2_drives_device)
// WD FDC accessed through 7ffx, slightly different from DISK1 (used in PHC-77)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK9_WD2793_N, msx_slot_disk9_wd2793_n_device)
// WD FDC accessed through 7ffx, slightly different from DISK1 (used in Victor HC-90 / HC-95)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK10_MB8877, msx_slot_disk10_mb8877_device)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK10_MB8877_2_DRIVES, msx_slot_disk10_mb8877_2_drives_device)
// WD FDC accessed through 7ffx, slightly different from DISK1 (used in Yamaha YIS-805)
DECLARE_DEVICE_TYPE(MSX_SLOT_DISK11_WD2793, msx_slot_disk11_wd2793_device)


class msx_slot_disk_device : public msx_slot_rom_device
{
protected:
	static constexpr int NO_DRIVES = 0;
	static constexpr int DRIVES_1 = 1;
	static constexpr int DRIVES_2 = 2;
	static constexpr int DRIVES_3 = 3;
	static constexpr int DRIVES_4 = 4;
	static constexpr bool DS = true;
	static constexpr bool SS = false;

	msx_slot_disk_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	static void floppy_formats(format_registration &fr);
	void add_drive_mconfig(machine_config &config, int nr_of_drives, bool double_sided);

	optional_device<floppy_connector> m_floppy0;
	optional_device<floppy_connector> m_floppy1;
	optional_device<floppy_connector> m_floppy2;
	optional_device<floppy_connector> m_floppy3;
	floppy_image_device *m_floppy;
};


class msx_slot_wd_disk_device : public msx_slot_disk_device
{
protected:
	static constexpr bool FORCE_READY = true;
	static constexpr bool NO_FORCE_READY = false;

	msx_slot_wd_disk_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	virtual void device_start() override;
	template <typename FDCType> void add_mconfig(machine_config &config, FDCType &&type, bool force_ready, int nr_of_drives, bool double_sided);

	required_device<wd_fdc_analog_device_base> m_fdc;
	output_finder<> m_led;
};


class msx_slot_tc8566_disk_device : public msx_slot_disk_device
{
protected:
	msx_slot_tc8566_disk_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock);

	void add_mconfig(machine_config &config, int nr_of_drives);

	required_device<tc8566af_device> m_fdc;
};


class msx_slot_disk1_base_device : public msx_slot_wd_disk_device
{
protected:
	msx_slot_disk1_base_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;

private:
	void set_control(u8 data);
	void set_side_control(u8 data);
	u8 side_control_r();
	u8 control_r();
	u8 status_r();

	u8 m_side_control;
	u8 m_control;
};

class msx_slot_disk1_fd1793_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_fd1793_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_mb8877_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_mb8877_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_wd2793_n_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_wd2793_n_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_wd2793_n_2_drives_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_wd2793_n_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_wd2793_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_wd2793_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_wd2793_0_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_wd2793_0_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_wd2793_ss_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_wd2793_ss_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk1_wd2793_2_drives_device : public msx_slot_disk1_base_device
{
public:
	msx_slot_disk1_wd2793_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};


class msx_slot_disk2_base_device : public msx_slot_wd_disk_device
{
protected:
	msx_slot_disk2_base_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;

private:
	void set_control(u8 data);
	u8 status_r();

	u8 m_control;
};

class msx_slot_disk2_fd1793_ss_device : public msx_slot_disk2_base_device
{
public:
	msx_slot_disk2_fd1793_ss_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk2_mb8877_device : public msx_slot_disk2_base_device
{
public:
	msx_slot_disk2_mb8877_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk2_mb8877_ss_device : public msx_slot_disk2_base_device
{
public:
	msx_slot_disk2_mb8877_ss_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk2_mb8877_2_drives_device : public msx_slot_disk2_base_device
{
public:
	msx_slot_disk2_mb8877_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk2_wd2793_device : public msx_slot_disk2_base_device
{
public:
	msx_slot_disk2_wd2793_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};

class msx_slot_disk2_wd2793_2_drives_device : public msx_slot_disk2_base_device
{
public:
	msx_slot_disk2_wd2793_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};



class msx_slot_disk3_tc8566_device : public msx_slot_tc8566_disk_device
{
public:
	msx_slot_disk3_tc8566_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	msx_slot_disk3_tc8566_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
};


class msx_slot_disk3_tc8566_2_drives_device : public msx_slot_disk3_tc8566_device
{
public:
	msx_slot_disk3_tc8566_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};


class msx_slot_disk4_tc8566_device : public msx_slot_tc8566_disk_device
{
public:
	msx_slot_disk4_tc8566_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
	virtual void device_start() override;
};


class msx_slot_disk5_wd2793_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk5_wd2793_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void control_w(u8 control);
	u8 status_r();

	u8 m_control;
};


class msx_slot_disk6_wd2793_n_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk6_wd2793_n_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void set_side_motor();
	void select_drive();
	u8 side_motor_r();
	u8 select0_r();
	u8 select1_r();
	u8 status_r();
	void side_motor_w(u8 data);
	void select0_w(u8 data);
	void select1_w(u8 data);

	u8 m_side_motor;
	u8 m_drive_select0;
	u8 m_drive_select1;
};


class msx_slot_disk7_mb8877_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk7_mb8877_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void set_drive_side_motor();
	void select_drive();
	void side_motor_w(u8 data);
	u8 status_r();

	u8 m_drive_side_motor;
};


class msx_slot_disk8_mb8877_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk8_mb8877_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	msx_slot_disk8_mb8877_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void set_control(u8 data);
	u8 status_r();

	u8 m_control;
};

class msx_slot_disk8_wd2793_2_drives_device : public msx_slot_disk8_mb8877_device
{
public:
	msx_slot_disk8_wd2793_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};


class msx_slot_disk9_wd2793_n_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk9_wd2793_n_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void control_w(u8 data);
	u8 status_r();

	u8 m_control;
};


class msx_slot_disk10_mb8877_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk10_mb8877_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	msx_slot_disk10_mb8877_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, u32 clock);

	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void control_w(u8 data);
	u8 status_r();

	u8 m_control;
};

class msx_slot_disk10_mb8877_2_drives_device : public msx_slot_disk10_mb8877_device
{
public:
	msx_slot_disk10_mb8877_2_drives_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_add_mconfig(machine_config &config) override;
};


class msx_slot_disk11_wd2793_device : public msx_slot_wd_disk_device
{
public:
	msx_slot_disk11_wd2793_device(const machine_config &mconfig, const char *tag, device_t *owner, u32 clock);

protected:
	virtual void device_start() override;
	virtual void device_reset() override;
	virtual void device_post_load() override;
	virtual void device_add_mconfig(machine_config &config) override;

private:
	void control_w(u8 data);
	void side_control_w(u8 data);
	u8 side_control_r();
	u8 control_r();
	u8 status_r();

	u8 m_side_control;
	u8 m_control;
};

#endif // MAME_BUS_MSX_SLOT_DISK_H
