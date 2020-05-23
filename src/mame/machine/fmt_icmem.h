// license:BSD-3-Clause
// copyright-holders:Barry Rodewald
/*********************************************************************

    fmt_icmem.h

    FM Towns IC Memory Card

*********************************************************************/

#ifndef MAME_MACHINE_FMT_ICMEM_H
#define MAME_MACHINE_FMT_ICMEM_H

#pragma once


/***************************************************************************
    FUNCTION PROTOTYPES
***************************************************************************/

class fmt_icmem_device : public device_t, public device_image_interface
{
public:
	// construction/destruction
	fmt_icmem_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual iodevice_t image_type() const noexcept override { return IO_MEMCARD; }

	virtual bool is_readable()  const noexcept override { return true; }
	virtual bool is_writeable() const noexcept override { return true; }
	virtual bool is_creatable() const noexcept override { return true; }
	virtual bool must_be_loaded() const noexcept override { return false; }
	virtual bool is_reset_on_load() const noexcept override { return false; }
	virtual const char *file_extensions() const noexcept override { return "icm"; }

	virtual image_init_result call_load() override;
	virtual void call_unload() override;
	virtual image_init_result call_create(int format_type, util::option_resolution *format_options) override;

	uint8_t static_mem_read(offs_t offset);
	void static_mem_write(offs_t offset, uint8_t data);
	uint8_t mem_read(offs_t offset);
	void mem_write(offs_t offset, uint8_t data);
	uint8_t status_r();
	uint8_t bank_r(offs_t offset);
	void bank_w(offs_t offset, uint8_t data);

protected:
	// device_t implementation
	virtual ioport_constructor device_input_ports() const override;
	virtual void device_start() override;

private:
	required_ioport m_writeprotect;
	std::unique_ptr<uint8_t[]> m_memcard_ram;
	bool m_change;
	bool m_attr_select;
	uint8_t m_detect;
	uint8_t m_bank;
};


// device type definition
DECLARE_DEVICE_TYPE(FMT_ICMEM, fmt_icmem_device)


#endif  // MAME_MACHINE_FMT_ICMEM_H
