// license:BSD-3-Clause
// copyright-holders:Olivier Galibert
#include "emu.h"
#include "h8s2320.h"

DEFINE_DEVICE_TYPE(H8S2320, h8s2320_device, "h8s2320", "Hitachi H8S/2320")
DEFINE_DEVICE_TYPE(H8S2321, h8s2321_device, "h8s2321", "Hitachi H8S/2321")
DEFINE_DEVICE_TYPE(H8S2322, h8s2322_device, "h8s2322", "Hitachi H8S/2322")
DEFINE_DEVICE_TYPE(H8S2323, h8s2323_device, "h8s2323", "Hitachi H8S/2323")
DEFINE_DEVICE_TYPE(H8S2324, h8s2324_device, "h8s2324", "Hitachi H8S/2324")
DEFINE_DEVICE_TYPE(H8S2326, h8s2326_device, "h8s2326", "Hitachi H8S/2326")
DEFINE_DEVICE_TYPE(H8S2327, h8s2327_device, "h8s2327", "Hitachi H8S/2327")
DEFINE_DEVICE_TYPE(H8S2328, h8s2328_device, "h8s2328", "Hitachi H8S/2328")
DEFINE_DEVICE_TYPE(H8S2329, h8s2329_device, "h8s2329", "Hitachi H8S/2329")


h8s2320_device::h8s2320_device(const machine_config &mconfig, device_type type, const char *tag, device_t *owner, uint32_t clock, uint32_t start) :
	h8s2000_device(mconfig, type, tag, owner, clock, address_map_constructor(FUNC(h8s2320_device::map), this)),
	m_intc(*this, "intc"),
	m_adc(*this, "adc"),
	m_dma(*this, "dma"),
	m_dma0(*this, "dma:0"),
	m_dma1(*this, "dma:1"),
	m_dtc(*this, "dtc"),
	m_port1(*this, "port1"),
	m_port2(*this, "port2"),
	m_port3(*this, "port3"),
	m_port4(*this, "port4"),
	m_port5(*this, "port5"),
	m_port6(*this, "port6"),
	m_porta(*this, "porta"),
	m_portb(*this, "portb"),
	m_portc(*this, "portc"),
	m_portd(*this, "portd"),
	m_porte(*this, "porte"),
	m_portf(*this, "portf"),
	m_portg(*this, "portg"),
	m_timer8_0(*this, "timer8_0"),
	m_timer8_1(*this, "timer8_1"),
	m_timer16(*this, "timer16"),
	m_timer16_0(*this, "timer16:0"),
	m_timer16_1(*this, "timer16:1"),
	m_timer16_2(*this, "timer16:2"),
	m_timer16_3(*this, "timer16:3"),
	m_timer16_4(*this, "timer16:4"),
	m_timer16_5(*this, "timer16:5"),
	m_watchdog(*this, "watchdog"),
	m_tend_cb(*this),
	m_ram_start(start),
	m_syscr(0)
{
}

h8s2320_device::h8s2320_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2320, tag, owner, clock, 0xffec00)
{
}

h8s2321_device::h8s2321_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2321, tag, owner, clock, 0xffec00)
{
}

h8s2322_device::h8s2322_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2322, tag, owner, clock, 0xffdc00)
{
}

h8s2323_device::h8s2323_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2323, tag, owner, clock, 0xffdc00)
{
}

h8s2324_device::h8s2324_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2324, tag, owner, clock, 0xff7c00)
{
}

h8s2326_device::h8s2326_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2326, tag, owner, clock, 0xffdc00)
{
}

h8s2327_device::h8s2327_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2327, tag, owner, clock, 0xffdc00)
{
}

h8s2328_device::h8s2328_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2328, tag, owner, clock, 0xffdc00)
{
}

h8s2329_device::h8s2329_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	h8s2320_device(mconfig, H8S2329, tag, owner, clock, 0xff7c00)
{
}

void h8s2320_device::map(address_map &map)
{
	map(m_ram_start, 0xfffbff).ram();

	map(0xfffe80, 0xfffe80).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tcr_r), FUNC(h8_timer16_channel_device::tcr_w));
	map(0xfffe81, 0xfffe81).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tmdr_r), FUNC(h8_timer16_channel_device::tmdr_w));
	map(0xfffe82, 0xfffe83).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tior_r), FUNC(h8_timer16_channel_device::tior_w));
	map(0xfffe84, 0xfffe84).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tier_r), FUNC(h8_timer16_channel_device::tier_w));
	map(0xfffe85, 0xfffe85).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tsr_r), FUNC(h8_timer16_channel_device::tsr_w));
	map(0xfffe86, 0xfffe87).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tcnt_r), FUNC(h8_timer16_channel_device::tcnt_w));
	map(0xfffe88, 0xfffe8f).rw(m_timer16_3, FUNC(h8_timer16_channel_device::tgr_r), FUNC(h8_timer16_channel_device::tgr_w));
	map(0xfffe90, 0xfffe90).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tcr_r), FUNC(h8_timer16_channel_device::tcr_w));
	map(0xfffe91, 0xfffe91).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tmdr_r), FUNC(h8_timer16_channel_device::tmdr_w));
	map(0xfffe92, 0xfffe92).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tior_r), FUNC(h8_timer16_channel_device::tior_w));
	map(0xfffe94, 0xfffe94).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tier_r), FUNC(h8_timer16_channel_device::tier_w));
	map(0xfffe95, 0xfffe95).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tsr_r), FUNC(h8_timer16_channel_device::tsr_w));
	map(0xfffe96, 0xfffe97).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tcnt_r), FUNC(h8_timer16_channel_device::tcnt_w));
	map(0xfffe98, 0xfffe9b).rw(m_timer16_4, FUNC(h8_timer16_channel_device::tgr_r), FUNC(h8_timer16_channel_device::tgr_w));
	map(0xfffea0, 0xfffea0).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tcr_r), FUNC(h8_timer16_channel_device::tcr_w));
	map(0xfffea1, 0xfffea1).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tmdr_r), FUNC(h8_timer16_channel_device::tmdr_w));
	map(0xfffea2, 0xfffea2).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tior_r), FUNC(h8_timer16_channel_device::tior_w));
	map(0xfffea4, 0xfffea4).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tier_r), FUNC(h8_timer16_channel_device::tier_w));
	map(0xfffea5, 0xfffea5).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tsr_r), FUNC(h8_timer16_channel_device::tsr_w));
	map(0xfffea6, 0xfffea7).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tcnt_r), FUNC(h8_timer16_channel_device::tcnt_w));
	map(0xfffea8, 0xfffeab).rw(m_timer16_5, FUNC(h8_timer16_channel_device::tgr_r), FUNC(h8_timer16_channel_device::tgr_w));
	map(0xfffeb0, 0xfffeb0).w(m_port1, FUNC(h8_port_device::ddr_w));
	map(0xfffeb1, 0xfffeb1).w(m_port2, FUNC(h8_port_device::ddr_w));
	map(0xfffeb2, 0xfffeb2).w(m_port3, FUNC(h8_port_device::ddr_w));
	map(0xfffeb4, 0xfffeb4).w(m_port5, FUNC(h8_port_device::ddr_w));
	map(0xfffeb5, 0xfffeb5).w(m_port6, FUNC(h8_port_device::ddr_w));
	map(0xfffeb9, 0xfffeb9).w(m_porta, FUNC(h8_port_device::ddr_w));
	map(0xfffeba, 0xfffeba).w(m_portb, FUNC(h8_port_device::ddr_w));
	map(0xfffebb, 0xfffebb).w(m_portc, FUNC(h8_port_device::ddr_w));
	map(0xfffebc, 0xfffebc).w(m_portd, FUNC(h8_port_device::ddr_w));
	map(0xfffebd, 0xfffebd).w(m_porte, FUNC(h8_port_device::ddr_w));
	map(0xfffebe, 0xfffebe).w(m_portf, FUNC(h8_port_device::ddr_w));
	map(0xfffebf, 0xfffebf).w(m_portg, FUNC(h8_port_device::ddr_w));
	map(0xfffec0, 0xfffec1).rw(m_intc, FUNC(h8s_intc_device::icr_r), FUNC(h8s_intc_device::icr_w));
	map(0xfffec2, 0xfffec2).rw(m_intc, FUNC(h8s_intc_device::icrc_r), FUNC(h8s_intc_device::icrc_w));
	map(0xfffec4, 0xfffecd).rw(m_intc, FUNC(h8s_intc_device::ipr_r), FUNC(h8s_intc_device::ipr_w));
	map(0xfffece, 0xfffece).rw(m_intc, FUNC(h8s_intc_device::iprk_r), FUNC(h8s_intc_device::iprk_w));

	if(type() != H8S2321) {
		map(0xfffee0, 0xfffee1).rw(m_dma0, FUNC(h8s_dma_channel_device::marah_r), FUNC(h8s_dma_channel_device::marah_w));
		map(0xfffee2, 0xfffee3).rw(m_dma0, FUNC(h8s_dma_channel_device::maral_r), FUNC(h8s_dma_channel_device::maral_w));
		map(0xfffee4, 0xfffee5).rw(m_dma0, FUNC(h8s_dma_channel_device::ioara_r), FUNC(h8s_dma_channel_device::ioara_w));
		map(0xfffee6, 0xfffee7).rw(m_dma0, FUNC(h8s_dma_channel_device::etcra_r), FUNC(h8s_dma_channel_device::etcra_w));
		map(0xfffee8, 0xfffee9).rw(m_dma0, FUNC(h8s_dma_channel_device::marbh_r), FUNC(h8s_dma_channel_device::marbh_w));
		map(0xfffeea, 0xfffeeb).rw(m_dma0, FUNC(h8s_dma_channel_device::marbl_r), FUNC(h8s_dma_channel_device::marbl_w));
		map(0xfffeec, 0xfffeed).rw(m_dma0, FUNC(h8s_dma_channel_device::ioarb_r), FUNC(h8s_dma_channel_device::ioarb_w));
		map(0xfffeee, 0xfffeef).rw(m_dma0, FUNC(h8s_dma_channel_device::etcrb_r), FUNC(h8s_dma_channel_device::etcrb_w));
		map(0xfffef0, 0xfffef1).rw(m_dma1, FUNC(h8s_dma_channel_device::marah_r), FUNC(h8s_dma_channel_device::marah_w));
		map(0xfffef2, 0xfffef3).rw(m_dma1, FUNC(h8s_dma_channel_device::maral_r), FUNC(h8s_dma_channel_device::maral_w));
		map(0xfffef4, 0xfffef5).rw(m_dma1, FUNC(h8s_dma_channel_device::ioara_r), FUNC(h8s_dma_channel_device::ioara_w));
		map(0xfffef6, 0xfffef7).rw(m_dma1, FUNC(h8s_dma_channel_device::etcra_r), FUNC(h8s_dma_channel_device::etcra_w));
		map(0xfffef8, 0xfffef9).rw(m_dma1, FUNC(h8s_dma_channel_device::marbh_r), FUNC(h8s_dma_channel_device::marbh_w));
		map(0xfffefa, 0xfffefb).rw(m_dma1, FUNC(h8s_dma_channel_device::marbl_r), FUNC(h8s_dma_channel_device::marbl_w));
		map(0xfffefc, 0xfffefd).rw(m_dma1, FUNC(h8s_dma_channel_device::ioarb_r), FUNC(h8s_dma_channel_device::ioarb_w));
		map(0xfffefe, 0xfffeff).rw(m_dma1, FUNC(h8s_dma_channel_device::etcrb_r), FUNC(h8s_dma_channel_device::etcrb_w));
		map(0xffff00, 0xffff00).rw(m_dma, FUNC(h8s_dma_device::dmawer_r), FUNC(h8s_dma_device::dmawer_w));
		map(0xffff01, 0xffff01).rw(m_dma, FUNC(h8s_dma_device::dmatcr_r), FUNC(h8s_dma_device::dmatcr_w));
		map(0xffff02, 0xffff03).rw(m_dma0, FUNC(h8s_dma_channel_device::dmacr_r), FUNC(h8s_dma_channel_device::dmacr_w));
		map(0xffff04, 0xffff05).rw(m_dma1, FUNC(h8s_dma_channel_device::dmacr_r), FUNC(h8s_dma_channel_device::dmacr_w));
		map(0xffff06, 0xffff07).rw(m_dma, FUNC(h8s_dma_device::dmabcr_r), FUNC(h8s_dma_device::dmabcr_w));
	}
	map(0xffff2c, 0xffff2c).rw(m_intc, FUNC(h8s_intc_device::iscrh_r), FUNC(h8s_intc_device::iscrh_w));
	map(0xffff2d, 0xffff2d).rw(m_intc, FUNC(h8s_intc_device::iscrl_r), FUNC(h8s_intc_device::iscrl_w));
	map(0xffff2e, 0xffff2e).rw(m_intc, FUNC(h8s_intc_device::ier_r), FUNC(h8s_intc_device::ier_w));
	map(0xffff2f, 0xffff2f).rw(m_intc, FUNC(h8s_intc_device::isr_r), FUNC(h8s_intc_device::isr_w));
	map(0xffff30, 0xffff35).rw(m_dtc, FUNC(h8_dtc_device::dtcer_r), FUNC(h8_dtc_device::dtcer_w));
	map(0xffff37, 0xffff37).rw(m_dtc, FUNC(h8_dtc_device::dtvecr_r), FUNC(h8_dtc_device::dtvecr_w));
	map(0xffff39, 0xffff39).rw(FUNC(h8s2320_device::syscr_r), FUNC(h8s2320_device::syscr_w));

	map(0xffff50, 0xffff50).r(m_port1, FUNC(h8_port_device::port_r));
	map(0xffff51, 0xffff51).r(m_port2, FUNC(h8_port_device::port_r));
	map(0xffff52, 0xffff52).r(m_port3, FUNC(h8_port_device::port_r));
	map(0xffff53, 0xffff53).r(m_port4, FUNC(h8_port_device::port_r));
	map(0xffff54, 0xffff54).r(m_port5, FUNC(h8_port_device::port_r));
	map(0xffff55, 0xffff55).r(m_port6, FUNC(h8_port_device::port_r));
	map(0xffff59, 0xffff59).r(m_porta, FUNC(h8_port_device::port_r));
	map(0xffff5a, 0xffff5a).r(m_portb, FUNC(h8_port_device::port_r));
	map(0xffff5b, 0xffff5b).r(m_portc, FUNC(h8_port_device::port_r));
	map(0xffff5c, 0xffff5c).r(m_portd, FUNC(h8_port_device::port_r));
	map(0xffff5d, 0xffff5d).r(m_porte, FUNC(h8_port_device::port_r));
	map(0xffff5e, 0xffff5e).r(m_portf, FUNC(h8_port_device::port_r));
	map(0xffff5f, 0xffff5f).r(m_portg, FUNC(h8_port_device::port_r));
	map(0xffff60, 0xffff60).rw(m_port1, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff61, 0xffff61).rw(m_port2, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff62, 0xffff62).rw(m_port3, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff64, 0xffff64).rw(m_port5, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff65, 0xffff65).rw(m_port6, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff69, 0xffff69).rw(m_porta, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff6a, 0xffff6a).rw(m_portb, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff6b, 0xffff6b).rw(m_portc, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff6c, 0xffff6c).rw(m_portd, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff6d, 0xffff6d).rw(m_porte, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff6e, 0xffff6e).rw(m_portf, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff6f, 0xffff6f).rw(m_portg, FUNC(h8_port_device::dr_r), FUNC(h8_port_device::dr_w));
	map(0xffff70, 0xffff70).rw(m_porta, FUNC(h8_port_device::pcr_r), FUNC(h8_port_device::pcr_w));
	map(0xffff71, 0xffff71).rw(m_portb, FUNC(h8_port_device::pcr_r), FUNC(h8_port_device::pcr_w));
	map(0xffff72, 0xffff72).rw(m_portc, FUNC(h8_port_device::pcr_r), FUNC(h8_port_device::pcr_w));
	map(0xffff73, 0xffff73).rw(m_portd, FUNC(h8_port_device::pcr_r), FUNC(h8_port_device::pcr_w));
	map(0xffff74, 0xffff74).rw(m_porte, FUNC(h8_port_device::pcr_r), FUNC(h8_port_device::pcr_w));
	map(0xffff76, 0xffff76).rw(m_port3, FUNC(h8_port_device::odr_r), FUNC(h8_port_device::odr_w));
	map(0xffff77, 0xffff77).rw(m_porta, FUNC(h8_port_device::odr_r), FUNC(h8_port_device::odr_w));
	map(0xffff78, 0xffff78).rw(m_sci[0], FUNC(h8_sci_device::smr_r), FUNC(h8_sci_device::smr_w));
	map(0xffff79, 0xffff79).rw(m_sci[0], FUNC(h8_sci_device::brr_r), FUNC(h8_sci_device::brr_w));
	map(0xffff7a, 0xffff7a).rw(m_sci[0], FUNC(h8_sci_device::scr_r), FUNC(h8_sci_device::scr_w));
	map(0xffff7b, 0xffff7b).rw(m_sci[0], FUNC(h8_sci_device::tdr_r), FUNC(h8_sci_device::tdr_w));
	map(0xffff7c, 0xffff7c).rw(m_sci[0], FUNC(h8_sci_device::ssr_r), FUNC(h8_sci_device::ssr_w));
	map(0xffff7d, 0xffff7d).r(m_sci[0], FUNC(h8_sci_device::rdr_r));
	map(0xffff7e, 0xffff7e).rw(m_sci[0], FUNC(h8_sci_device::scmr_r), FUNC(h8_sci_device::scmr_w));
	map(0xffff80, 0xffff80).rw(m_sci[1], FUNC(h8_sci_device::smr_r), FUNC(h8_sci_device::smr_w));
	map(0xffff81, 0xffff81).rw(m_sci[1], FUNC(h8_sci_device::brr_r), FUNC(h8_sci_device::brr_w));
	map(0xffff82, 0xffff82).rw(m_sci[1], FUNC(h8_sci_device::scr_r), FUNC(h8_sci_device::scr_w));
	map(0xffff83, 0xffff83).rw(m_sci[1], FUNC(h8_sci_device::tdr_r), FUNC(h8_sci_device::tdr_w));
	map(0xffff84, 0xffff84).rw(m_sci[1], FUNC(h8_sci_device::ssr_r), FUNC(h8_sci_device::ssr_w));
	map(0xffff85, 0xffff85).r(m_sci[1], FUNC(h8_sci_device::rdr_r));
	map(0xffff86, 0xffff86).rw(m_sci[1], FUNC(h8_sci_device::scmr_r), FUNC(h8_sci_device::scmr_w));
	map(0xffff88, 0xffff88).rw(m_sci[2], FUNC(h8_sci_device::smr_r), FUNC(h8_sci_device::smr_w));
	map(0xffff89, 0xffff89).rw(m_sci[2], FUNC(h8_sci_device::brr_r), FUNC(h8_sci_device::brr_w));
	map(0xffff8a, 0xffff8a).rw(m_sci[2], FUNC(h8_sci_device::scr_r), FUNC(h8_sci_device::scr_w));
	map(0xffff8b, 0xffff8b).rw(m_sci[2], FUNC(h8_sci_device::tdr_r), FUNC(h8_sci_device::tdr_w));
	map(0xffff8c, 0xffff8c).rw(m_sci[2], FUNC(h8_sci_device::ssr_r), FUNC(h8_sci_device::ssr_w));
	map(0xffff8d, 0xffff8d).r(m_sci[2], FUNC(h8_sci_device::rdr_r));
	map(0xffff8e, 0xffff8e).rw(m_sci[2], FUNC(h8_sci_device::scmr_r), FUNC(h8_sci_device::scmr_w));
	map(0xffff90, 0xffff97).r(m_adc, FUNC(h8_adc_device::addr8_r));
	map(0xffff98, 0xffff98).rw(m_adc, FUNC(h8_adc_device::adcsr_r), FUNC(h8_adc_device::adcsr_w));
	map(0xffff99, 0xffff99).rw(m_adc, FUNC(h8_adc_device::adcr_r), FUNC(h8_adc_device::adcr_w));

	map(0xffffb0, 0xffffb0).rw(m_timer8_0, FUNC(h8_timer8_channel_device::tcr_r), FUNC(h8_timer8_channel_device::tcr_w));
	map(0xffffb1, 0xffffb1).rw(m_timer8_1, FUNC(h8_timer8_channel_device::tcr_r), FUNC(h8_timer8_channel_device::tcr_w));
	map(0xffffb2, 0xffffb2).rw(m_timer8_0, FUNC(h8_timer8_channel_device::tcsr_r), FUNC(h8_timer8_channel_device::tcsr_w));
	map(0xffffb3, 0xffffb3).rw(m_timer8_1, FUNC(h8_timer8_channel_device::tcsr_r), FUNC(h8_timer8_channel_device::tcsr_w));
	map(0xffffb4, 0xffffb7).rw(m_timer8_0, FUNC(h8_timer8_channel_device::tcor_r), FUNC(h8_timer8_channel_device::tcor_w)).umask16(0xff00);
	map(0xffffb4, 0xffffb7).rw(m_timer8_1, FUNC(h8_timer8_channel_device::tcor_r), FUNC(h8_timer8_channel_device::tcor_w)).umask16(0x00ff);
	map(0xffffb8, 0xffffb8).rw(m_timer8_0, FUNC(h8_timer8_channel_device::tcnt_r), FUNC(h8_timer8_channel_device::tcnt_w));
	map(0xffffb9, 0xffffb9).rw(m_timer8_1, FUNC(h8_timer8_channel_device::tcnt_r), FUNC(h8_timer8_channel_device::tcnt_w));
	map(0xffffbc, 0xffffbd).rw(m_watchdog, FUNC(h8_watchdog_device::wd_r), FUNC(h8_watchdog_device::wd_w));
	map(0xffffbe, 0xffffbf).rw(m_watchdog, FUNC(h8_watchdog_device::rst_r), FUNC(h8_watchdog_device::rst_w));
	map(0xffffc0, 0xffffc0).rw(m_timer16, FUNC(h8_timer16_device::tstr_r), FUNC(h8_timer16_device::tstr_w));
	map(0xffffc1, 0xffffc1).rw(m_timer16, FUNC(h8_timer16_device::tsyr_r), FUNC(h8_timer16_device::tsyr_w));

	map(0xffffd0, 0xffffd0).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tcr_r), FUNC(h8_timer16_channel_device::tcr_w));
	map(0xffffd1, 0xffffd1).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tmdr_r), FUNC(h8_timer16_channel_device::tmdr_w));
	map(0xffffd2, 0xffffd3).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tior_r), FUNC(h8_timer16_channel_device::tior_w));
	map(0xffffd4, 0xffffd4).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tier_r), FUNC(h8_timer16_channel_device::tier_w));
	map(0xffffd5, 0xffffd5).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tsr_r), FUNC(h8_timer16_channel_device::tsr_w));
	map(0xffffd6, 0xffffd7).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tcnt_r), FUNC(h8_timer16_channel_device::tcnt_w));
	map(0xffffd8, 0xffffdf).rw(m_timer16_0, FUNC(h8_timer16_channel_device::tgr_r), FUNC(h8_timer16_channel_device::tgr_w));
	map(0xffffe0, 0xffffe0).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tcr_r), FUNC(h8_timer16_channel_device::tcr_w));
	map(0xffffe1, 0xffffe1).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tmdr_r), FUNC(h8_timer16_channel_device::tmdr_w));
	map(0xffffe2, 0xffffe2).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tior_r), FUNC(h8_timer16_channel_device::tior_w));
	map(0xffffe4, 0xffffe4).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tier_r), FUNC(h8_timer16_channel_device::tier_w));
	map(0xffffe5, 0xffffe5).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tsr_r), FUNC(h8_timer16_channel_device::tsr_w));
	map(0xffffe6, 0xffffe7).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tcnt_r), FUNC(h8_timer16_channel_device::tcnt_w));
	map(0xffffe8, 0xffffeb).rw(m_timer16_1, FUNC(h8_timer16_channel_device::tgr_r), FUNC(h8_timer16_channel_device::tgr_w));
	map(0xfffff0, 0xfffff0).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tcr_r), FUNC(h8_timer16_channel_device::tcr_w));
	map(0xfffff1, 0xfffff1).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tmdr_r), FUNC(h8_timer16_channel_device::tmdr_w));
	map(0xfffff2, 0xfffff2).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tior_r), FUNC(h8_timer16_channel_device::tior_w));
	map(0xfffff4, 0xfffff4).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tier_r), FUNC(h8_timer16_channel_device::tier_w));
	map(0xfffff5, 0xfffff5).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tsr_r), FUNC(h8_timer16_channel_device::tsr_w));
	map(0xfffff6, 0xfffff7).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tcnt_r), FUNC(h8_timer16_channel_device::tcnt_w));
	map(0xfffff8, 0xfffffb).rw(m_timer16_2, FUNC(h8_timer16_channel_device::tgr_r), FUNC(h8_timer16_channel_device::tgr_w));
}

void h8s2320_device::device_add_mconfig(machine_config &config)
{
	H8S_INTC(config, m_intc, *this);
	H8_ADC_2320(config, m_adc, *this, m_intc, 28);
	if(type() != H8S2321) {
		H8S_DMA(config, m_dma, *this);
		H8S_DMA_CHANNEL(config, m_dma0, *this, m_dma, m_intc);
		H8S_DMA_CHANNEL(config, m_dma1, *this, m_dma, m_intc);
	}
	H8_DTC(config, m_dtc, *this, m_intc, 24);
	H8_PORT(config, m_port1, *this, h8_device::PORT_1, 0x00, 0x00);
	H8_PORT(config, m_port2, *this, h8_device::PORT_2, 0x00, 0x00);
	H8_PORT(config, m_port3, *this, h8_device::PORT_3, 0xc0, 0xc0);
	H8_PORT(config, m_port4, *this, h8_device::PORT_4, 0x00, 0x00);
	H8_PORT(config, m_port5, *this, h8_device::PORT_5, 0xf0, 0xf0);
	H8_PORT(config, m_port6, *this, h8_device::PORT_6, 0x00, 0x00);
	H8_PORT(config, m_porta, *this, h8_device::PORT_A, 0x00, 0x00);
	H8_PORT(config, m_portb, *this, h8_device::PORT_B, 0x00, 0x00);
	H8_PORT(config, m_portc, *this, h8_device::PORT_C, 0x00, 0x00);
	H8_PORT(config, m_portd, *this, h8_device::PORT_D, 0x00, 0x00);
	H8_PORT(config, m_porte, *this, h8_device::PORT_E, 0x00, 0x00);
	H8_PORT(config, m_portf, *this, h8_device::PORT_F, 0x00, 0x00);
	H8_PORT(config, m_portg, *this, h8_device::PORT_G, 0xe0, 0xe0);
	H8H_TIMER8_CHANNEL(config, m_timer8_0, *this, m_intc, 64, 65, 66, m_timer8_1, h8_timer8_channel_device::CHAIN_OVERFLOW, true,  false);
	H8H_TIMER8_CHANNEL(config, m_timer8_1, *this, m_intc, 68, 69, 70, m_timer8_0, h8_timer8_channel_device::CHAIN_A,        false, false);
	H8_TIMER16(config, m_timer16, *this, 6, 0x00);
	H8S_TIMER16_CHANNEL(config, m_timer16_0, *this, 4, 0x60, m_intc, 32,
									h8_timer16_channel_device::DIV_1,
									h8_timer16_channel_device::DIV_4,
									h8_timer16_channel_device::DIV_16,
									h8_timer16_channel_device::DIV_64,
									h8_timer16_channel_device::INPUT_A,
									h8_timer16_channel_device::INPUT_B,
									h8_timer16_channel_device::INPUT_C,
									h8_timer16_channel_device::INPUT_D);
	H8S_TIMER16_CHANNEL(config, m_timer16_1, *this, 2, 0x4c, m_intc, 40,
									h8_timer16_channel_device::DIV_1,
									h8_timer16_channel_device::DIV_4,
									h8_timer16_channel_device::DIV_16,
									h8_timer16_channel_device::DIV_64,
									h8_timer16_channel_device::INPUT_A,
									h8_timer16_channel_device::INPUT_B,
									h8_timer16_channel_device::DIV_256,
									h8_timer16_channel_device::CHAIN).set_chain(m_timer16_2);
	H8S_TIMER16_CHANNEL(config, m_timer16_2, *this, 2, 0x4c, m_intc, 44,
									h8_timer16_channel_device::DIV_1,
									h8_timer16_channel_device::DIV_4,
									h8_timer16_channel_device::DIV_16,
									h8_timer16_channel_device::DIV_64,
									h8_timer16_channel_device::INPUT_A,
									h8_timer16_channel_device::INPUT_B,
									h8_timer16_channel_device::INPUT_C,
									h8_timer16_channel_device::DIV_1024);
	H8S_TIMER16_CHANNEL(config, m_timer16_3, *this, 4, 0x60, m_intc, 48,
									h8_timer16_channel_device::DIV_1,
									h8_timer16_channel_device::DIV_4,
									h8_timer16_channel_device::DIV_16,
									h8_timer16_channel_device::DIV_64,
									h8_timer16_channel_device::INPUT_A,
									h8_timer16_channel_device::DIV_1024,
									h8_timer16_channel_device::DIV_256,
									h8_timer16_channel_device::DIV_4096);
	H8S_TIMER16_CHANNEL(config, m_timer16_4, *this, 2, 0x4c, m_intc, 56,
									h8_timer16_channel_device::DIV_1,
									h8_timer16_channel_device::DIV_4,
									h8_timer16_channel_device::DIV_16,
									h8_timer16_channel_device::DIV_64,
									h8_timer16_channel_device::INPUT_A,
									h8_timer16_channel_device::INPUT_C,
									h8_timer16_channel_device::DIV_1024,
									h8_timer16_channel_device::CHAIN).set_chain(m_timer16_5);
	H8S_TIMER16_CHANNEL(config, m_timer16_5, *this, 2, 0x4c, m_intc, 60,
									h8_timer16_channel_device::DIV_1,
									h8_timer16_channel_device::DIV_4,
									h8_timer16_channel_device::DIV_16,
									h8_timer16_channel_device::DIV_64,
									h8_timer16_channel_device::INPUT_A,
									h8_timer16_channel_device::INPUT_C,
									h8_timer16_channel_device::DIV_256,
									h8_timer16_channel_device::INPUT_D);
	H8_SCI(config, m_sci[0], 0, *this, m_intc, 80, 81, 82, 83);
	H8_SCI(config, m_sci[1], 1, *this, m_intc, 84, 85, 86, 87);
	H8_SCI(config, m_sci[2], 2, *this, m_intc, 88, 89, 90, 91);
	H8_WATCHDOG(config, m_watchdog, *this, m_intc, 25, h8_watchdog_device::H);
}

void h8s2320_device::execute_set_input(int inputnum, int state)
{
	if(inputnum == H8_INPUT_LINE_TEND0 || inputnum == H8_INPUT_LINE_TEND1)
		m_tend_cb[inputnum - H8_INPUT_LINE_TEND0](state);
	else if(inputnum == H8_INPUT_LINE_DREQ0 || inputnum == H8_INPUT_LINE_DREQ1)
		m_dma->set_input(inputnum, state);
	else
		m_intc->set_input(inputnum, state);
}

bool h8s2320_device::exr_in_stack() const
{
	return m_syscr & 0x20;
}

int h8s2320_device::trace_setup()
{
	m_CCR |= F_I;
	m_EXR &= ~EXR_T;
	return 5;
}

int h8s2320_device::trapa_setup()
{
	m_CCR |= F_I;
	if(m_syscr & 0x20)
		m_EXR &= ~EXR_T;
	return 8;
}

void h8s2320_device::irq_setup()
{
	switch(m_syscr & 0x30) {
	case 0x00:
		m_CCR |= F_I;
		break;
	case 0x20:
		m_EXR = m_EXR & (EXR_NC);
		if(m_taken_irq_level == 8)
			m_EXR |= 7;
		else
			m_EXR |= m_taken_irq_level;
		break;
	}
}

void h8s2320_device::update_irq_filter()
{
	switch(m_syscr & 0x30) {
	case 0x00:
		if(m_CCR & F_I)
			m_intc->set_filter(2, -1);
		else
			m_intc->set_filter(0, -1);
		break;
	case 0x20:
		m_intc->set_filter(0, m_EXR & 7);
		break;
	}
}

void h8s2320_device::interrupt_taken()
{
	standard_irq_callback(m_intc->interrupt_taken(m_taken_irq_vector), m_NPC);
}

void h8s2320_device::internal_update(uint64_t current_time)
{
	uint64_t event_time = 0;

	add_event(event_time, m_adc->internal_update(current_time));
	add_event(event_time, m_sci[0]->internal_update(current_time));
	add_event(event_time, m_sci[1]->internal_update(current_time));
	add_event(event_time, m_sci[2]->internal_update(current_time));
	add_event(event_time, m_timer8_0->internal_update(current_time));
	add_event(event_time, m_timer8_1->internal_update(current_time));
	add_event(event_time, m_timer16_0->internal_update(current_time));
	add_event(event_time, m_timer16_1->internal_update(current_time));
	add_event(event_time, m_timer16_2->internal_update(current_time));
	add_event(event_time, m_timer16_3->internal_update(current_time));
	add_event(event_time, m_timer16_4->internal_update(current_time));
	add_event(event_time, m_timer16_5->internal_update(current_time));
	add_event(event_time, m_watchdog->internal_update(current_time));

	recompute_bcount(event_time);
}

void h8s2320_device::device_start()
{
	h8s2000_device::device_start();
	m_dma_device = m_dma;
	m_dtc_device = m_dtc;
}

void h8s2320_device::device_reset()
{
	h8s2000_device::device_reset();
	m_syscr = 0x01;
}

uint8_t h8s2320_device::syscr_r()
{
	return m_syscr;
}

void h8s2320_device::syscr_w(uint8_t data)
{
	m_syscr = data;
	m_mac_saturating = m_syscr & 0x80;
	update_irq_filter();
	logerror("syscr = %02x\n", data);
}
