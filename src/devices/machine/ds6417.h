// license:BSD-3-Clause
// copyright-holders:Carl

// Only known to be used by the Tandy VIS

#ifndef MAME_MACHINE_DS6417_H_
#define MAME_MACHINE_DS6417_H_


class ds6417_device :  public device_t,
							public device_image_interface
{
public:
	// construction/destruction
	ds6417_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	virtual iodevice_t image_type() const noexcept override { return IO_MEMCARD; }

	virtual bool is_readable()  const noexcept override { return true; }
	virtual bool is_writeable() const noexcept override { return true; }
	virtual bool is_creatable() const noexcept override { return true; }
	virtual bool must_be_loaded() const noexcept override { return false; }
	virtual bool is_reset_on_load() const noexcept override { return false; }
	virtual const char *file_extensions() const noexcept override { return "bin"; }

	virtual image_init_result call_load() override;
	virtual image_init_result call_create(int format_type, util::option_resolution *format_options) override;

	DECLARE_WRITE_LINE_MEMBER(data_w) { if(!m_read) m_data = state; }
	DECLARE_WRITE_LINE_MEMBER(clock_w);
	DECLARE_WRITE_LINE_MEMBER(reset_w) { if(!state && m_reset) reset(); m_reset = state; }
	DECLARE_READ_LINE_MEMBER(data_r) { return m_read ? m_data : 0; }

protected:
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	uint8_t calccrc(uint8_t bit, uint8_t crc) const;
	enum {
		CMD_READ = 0x06,
		CMD_WRITE = 0x11,
		CMD_READPROT = 0x05,
		CMD_WRITEPROT = 0x0e,
		CMD_READMASK = 0x18,
		CMD_READCRC = 0x03
	};
	bool m_reset;
	bool m_clk;
	bool m_data;
	bool m_read;
	bool m_start;
	u8 m_count;
	u8 m_shiftreg;
	u8 m_command;
	u8 m_crc;
	u8 m_selbits;
	u32 m_addr;
	u16 m_select;
	u16 m_selectval;
};

DECLARE_DEVICE_TYPE(DS6417, ds6417_device)

#endif /* MAME_MACHINE_DS6417_H_ */
