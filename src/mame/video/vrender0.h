// license:BSD-3-Clause
// copyright-holders:ElSemi
#ifndef MAME_VIDEO_VRENDER0_H
#define MAME_VIDEO_VRENDER0_H

#pragma once


/***************************************************************************
 TYPE DEFINITIONS
 ***************************************************************************/

class vr0video_device : public device_t,
						public device_video_interface
{
public:
	template <typename T> vr0video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock, T &&cpu_tag)
		: vr0video_device(mconfig, tag, owner, clock)
	{
		m_cpu.set_tag(std::forward<T>(cpu_tag));
	}

	vr0video_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock);

	void set_areas(uint8_t *textureram, uint16_t *frameram);
	void regs_map(address_map &map);
	void execute_drawing();
	uint32_t screen_update(screen_device &screen, bitmap_ind16 &bitmap, const rectangle &cliprect);
	auto idleskip_cb() { return m_idleskip_cb.bind(); }

	DECLARE_READ16_MEMBER(flip_count_r);
	DECLARE_WRITE16_MEMBER(flip_count_w);

protected:
	// device-level overrides
	virtual void device_start() override;
	virtual void device_reset() override;

private:
	int vrender0_ProcessPacket(uint32_t PacketPtr, uint16_t *Dest);

	required_device<cpu_device> m_cpu;
	devcb_write_line  m_idleskip_cb;

	struct RenderStateInfo
	{
		uint32_t Tx;
		uint32_t Ty;
		uint32_t Txdx;
		uint32_t Tydx;
		uint32_t Txdy;
		uint32_t Tydy;
		uint32_t SrcAlphaColor;
		uint32_t SrcBlend;
		uint32_t DstAlphaColor;
		uint32_t DstBlend;
		uint32_t ShadeColor;
		uint32_t TransColor;
		uint32_t TileOffset;
		uint32_t FontOffset;
		uint32_t PalOffset;
		uint32_t PaletteBank;
		uint32_t TextureMode;
		uint32_t PixelFormat;
		uint32_t Width;
		uint32_t Height;
	};


	uint16_t m_InternalPalette[256];
	uint32_t m_LastPalUpdate;

	RenderStateInfo m_RenderState;

	uint8_t *m_textureram;
	uint16_t *m_frameram;

	DECLARE_READ16_MEMBER( cmd_queue_front_r );
	DECLARE_WRITE16_MEMBER( cmd_queue_front_w );

	DECLARE_READ16_MEMBER( cmd_queue_rear_r );
	uint16_t m_queue_rear, m_queue_front;

	DECLARE_READ16_MEMBER( bank1_select_r );
	DECLARE_WRITE16_MEMBER( bank1_select_w );
	bool m_bank1_select;

	DECLARE_READ16_MEMBER( display_bank_r );
	uint8_t m_display_bank;

	DECLARE_READ16_MEMBER( render_control_r );
	DECLARE_WRITE16_MEMBER( render_control_w );
	bool m_draw_select;
	bool m_render_reset;
	bool m_render_start;
	uint8_t m_dither_mode;
	uint8_t m_flip_count;
};

DECLARE_DEVICE_TYPE(VIDEO_VRENDER0, vr0video_device)

#endif // MAME_VIDEO_VRENDER0_H
