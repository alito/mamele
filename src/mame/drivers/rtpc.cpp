// license:BSD-3-Clause
// copyright-holders:Patrick Mackinlay

/*
 * IBM RT PC.
 *
 * Sources:
 *   - http://bitsavers.org/pdf/ibm/pc/rt/75X0232_RT_PC_Technical_Reference_Volume_1_Jun87.pdf
 *   - http://www.cs.cmu.edu/afs/andrew.cmu.edu/usr/shadow/www/ibmrt.html
 *   - http://ps-2.kev009.com/ohlandl/6152/rt_index.html
 *
 * TODO:
 *   - finish refactoring iocc
 *   - additional machine variants
 *   - configurable RAM size
 */
/*
 * https://www-01.ibm.com/common/ssi/ShowDoc.wss?docURL=/common/ssi/rep_ca/6/897/ENUS186-006/index.html
 * https://www-01.ibm.com/common/ssi/ShowDoc.wss?docURL=/common/ssi/rep_ca/1/897/ENUS187-021/index.html
 * https://www-01.ibm.com/common/ssi/ShowDoc.wss?docURL=/common/ssi/rep_ca/0/897/ENUS188-120/index.html
 *
 *   Model  Chassis  CPU  RAM      HDD  Release   Price    Notes
 *    010    6151    032  1M/4M    40M  Jan 1986  $11,700
 *    015    6151    032  2M/4M    70M  Nov 1986  $10,050
 *    020    6150    032  1M/4M    40M  Jan 1986  $14,945
 *    025    6150    032  2M/4M    70M  Jan 1986  $17,940
 *    A25    6150    032  2M/4M    70M  Jan 1986  $19,510  5080 attachment/no keyboard
 *    115    6151    Adv  4M/16M   70M  Feb 1987  $10,600  AFPA
 *    125    6150    Adv  4M/16M   70M  Feb 1987  $16,100  AFPA
 *    B25    6150    Adv  4M/16M   70M  Feb 1987  $17,670  AFPA, 5080 attachment/no keyboard
 *    130    6151    Enh  16M     114M  Jul 1988  $23,220  EAFPA
 *    135    6150    Enh  16M     114M  Jul 1988  $30,595  EAFPA
 *    B35    6150    Enh  16M     114M  Jul 1988  $32,165  EAFPA, 5080 attachment/no keyboard
 *
 * 032 (aka SGP), 170ns (23.5294 MHz crystal / 4 == 5.882350 MHz == 170ns), 1MB/2MB/4MB memory boards
 * Advanced, 100ns 4MB (6151) external (6150) (presume ~40MHz crystal/4), 4MB/8MB memory boards
 * Enhanced, 80ns, 16MB soldered, EAFPA standard, CMOS (49.400 MHz crystal/4 == 12.350MHz == 80.971ns)
 *
 * AFPA is M68881 @ 20MHz
 * EAFPA is AD90221-2 ADSP-3210 (multiplier) + AD90222-2 ADSP-3221 (fp alu) + AD90220-2 ADSP-1401 (program sequencer)
 *
 * system processor real memory address map
 *   0000'0000-00ff'ffff 16MB memory management unit
 *   0100'0000-efff'ffff -- not defined
 *
 * system board I/O addresses (all in segment F)
 *   f000'0000-f0ff'ffff 16MB i/o channel i/o map
 *   f100'0000-f3ff'ffff 48MB reserved
 *   f400'0000-f4ff'ffff 16MB i/o channel memory map
 *   f500'0000-f7ff'ffff reserved?
 *   f800'0000-fbff'ffff reserved
 *   fc00'0000-fdff'ffff mc68881
 *   fe00'0000-feff'ffff fpa2
 *   ff00'0000-ffff'ffff 16MB floating point accelerator
 *
 * system processor i/o address map
 *   00'0000-7f'ffff 8MB not defined
 *   80'0000-80'ffff 64KB processor channel device initialization
 *   81'0000-81'ffff 64KB memory management unit
 *   82'0000-ff'ffff 8064KB not defined
 *
 * advanced processor
 *   80'0400 i/o interface 1 i/o base
 *   80'0100 i/o interface 2 i/o base
 *
 */
 /*
  * WIP
  *  - diagnostic disk 1 fails with code 67
  *  - aix vrm disk 1 fails with alternating code a6/13
  */

#include "emu.h"

// cpu and memory
#include "cpu/romp/romp.h"
#include "cpu/mcs51/mcs51.h"

// various hardware
#include "machine/rosetta.h"
#include "machine/rtpc_iocc.h"
#include "machine/am9517a.h"
#include "machine/pic8259.h"
#include "machine/i8255.h"
#include "machine/mc146818.h"
#include "machine/z80scc.h"
#include "machine/timer.h"

// isa bus
#include "bus/isa/isa.h"
#include "bus/isa/isa_cards.h"
#include "bus/isa/mda.h"
#include "bus/isa/fdc.h"
#include "bus/isa/ide.h"

// busses and connectors
#include "bus/rs232/rs232.h"
#include "bus/rtpc/kbd_con.h"
#include "bus/rtpc/kbd.h"

#include "sound/spkrdev.h"
#include "machine/input_merger.h"

#include "imagedev/floppy.h"
#include "formats/pc_dsk.h"

#define LOG_GENERAL (1U << 0)
#define LOG_KLS     (1U << 1)

//#define VERBOSE (LOG_GENERAL)
#include "logmacro.h"

#include "debugger.h"

#include "rtpc.lh"

namespace {

class rtpc_state : public driver_device
{
public:
	rtpc_state(machine_config const &mconfig, device_type type, char const *tag)
		: driver_device(mconfig, type, tag)
		, m_cpu(*this, "cpu")
		, m_mcu(*this, "mcu")
		, m_mmu(*this, "mmu")
		, m_iocc(*this, "iocc")
		, m_dma(*this, "dma%u", 0U)
		, m_pic(*this, "pic%u", 0U)
		, m_ppi(*this, "ppi")
		, m_rtc(*this, "rtc")
		, m_scc(*this, "scc")
		, m_isa(*this, "isa")
		, m_kbd_con(*this, "kbd_con")
		, m_speaker(*this, "kbd_con:kbd:speaker")
		, m_ipl(*this, "ipl")
	{
	}

	void ibm6150(machine_config &config);
	void ibm6151(machine_config &config);

	void init_common();

protected:
	// driver_device overrides
	virtual void machine_start() override;
	virtual void machine_reset() override;

	template <bool SCC> void iocc_pio_map(address_map &map);

	void common(machine_config &config);

	void mcu_port1_w(u8 data);
	void mcu_port2_w(u8 data);
	void mcu_port3_w(u8 data);
	void ppi_portc_w(u8 data);

	void kls_cmd_w(offs_t offset, u16 data, u16 mem_mask);
	void crra_w(u8 data);
	void crrb_w(u8 data);
	void dia_w(u8 data);

	void mcu_timer(timer_device &timer, void *ptr, s32 param)
	{
		m_mcu_p3 ^= 0x10;
		m_mcu->set_input_line(MCS51_T0_LINE, BIT(m_mcu_p3, 4));
	}

	void speaker()
	{
		if (m_speaker)
			m_speaker->level_w(!BIT(m_mcu_p2, 7) ? m_mcu_p1 >> 6 : 0);
	}

	// devices
	required_device<romp_device> m_cpu;
	required_device<i8051_device> m_mcu;
	required_device<rosetta_device> m_mmu;
	required_device<rtpc_iocc_device> m_iocc;

	required_device_array<am9517a_device, 2> m_dma;
	required_device_array<pic8259_device, 2> m_pic;
	required_device<i8255_device> m_ppi;
	required_device<mc146818_device> m_rtc;
	optional_device<z80scc_device> m_scc;

	required_device<isa16_device> m_isa;
	required_device<rtpc_kbd_con_device> m_kbd_con;
	optional_device<speaker_sound_device> m_speaker;

	required_region_ptr<u32> m_ipl;

	u8 m_mcu_p0;
	u8 m_mcu_p1;
	u8 m_mcu_p2;
	u8 m_mcu_p3;

	u8 m_ppi_pb;
	u8 m_mcu_uart;

	u8 m_ch8er; // dma channel 8 enable register
	u8 m_crra; // component reset register a
	u8 m_crrb; // component reset register b

	u8 m_ext[2]; // external serial registers
};

static double const speaker_levels[4] = { 0.0, 1.0 / 3.0, 2.0 / 3.0, 1.0 };

void rtpc_state::machine_start()
{
	m_mcu_p0 = 0;
	m_mcu_p1 = 0;
	m_mcu_p2 = 0;
	m_mcu_p3 = 0;

	m_ppi_pb = 0;

	m_crra = 0xff;
	m_crrb = 0xff;
}

void rtpc_state::machine_reset()
{
}

void rtpc_state::init_common()
{
	if (m_speaker)
		m_speaker->set_levels(4, speaker_levels);
}

template <bool SCC> void rtpc_state::iocc_pio_map(address_map &map)
{
	if (SCC)
	{
		map(0x00'8000, 0x00'8003).rw(m_scc, FUNC(z80scc_device::dc_ab_r), FUNC(z80scc_device::dc_ab_w));
		map(0x00'8020, 0x00'8020).lrw8([this]() { return m_ext[0]; }, "ext_a_r", [this](u8 data) { m_ext[0] = data; }, "ext_a_w");
		map(0x00'8040, 0x00'8040).lrw8([this]() { return m_ext[1]; }, "ext_b_r", [this](u8 data) { m_ext[1] = data; }, "ext_b_w");
		map(0x00'8060, 0x00'8060).lr8([this]() { return u8(m_scc->m1_r()); }, "intack");
	}

	// delay 1µs per byte written
	map(0x00'80e0, 0x00'80e3).lw8([this](u8 data) { m_cpu->eat_cycles(m_cpu->clock() / 1000000 + 1); }, "io_delay");

	map(0x00'8400, 0x00'8403).mirror(0x7c).rw(m_ppi, FUNC(i8255_device::read), FUNC(i8255_device::write));
	map(0x00'8400, 0x00'8401).mirror(0x78).w(FUNC(rtpc_state::kls_cmd_w));

	map(0x00'8800, 0x00'883f).rw(m_rtc, FUNC(mc146818_device::read_direct), FUNC(mc146818_device::write_direct));
	map(0x00'8840, 0x00'884f).mirror(0x10).rw(m_dma[0], FUNC(am9517a_device::read), FUNC(am9517a_device::write));
	map(0x00'8860, 0x00'887f).umask16(0xff00).rw(m_dma[1], FUNC(am9517a_device::read), FUNC(am9517a_device::write));
	map(0x00'8880, 0x00'8881).mirror(0x1e).rw(m_pic[0], FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0x00'88a0, 0x00'88a1).mirror(0x1e).rw(m_pic[1], FUNC(pic8259_device::read), FUNC(pic8259_device::write));
	map(0x00'88c0, 0x00'88c0).mirror(0x1f).rw(m_iocc, FUNC(rtpc_iocc_device::dbr_r), FUNC(rtpc_iocc_device::dbr_w));
	map(0x00'88e0, 0x00'88e0).mirror(0x1f).rw(m_iocc, FUNC(rtpc_iocc_device::dmr_r), FUNC(rtpc_iocc_device::dmr_w));

	map(0x00'8c00, 0x00'8c00).mirror(0x03).lrw8([this]() { return m_ch8er; }, "ch8er_r", [this](u8 data) { m_ch8er = data; }, "ch8er_w");
	map(0x00'8c20, 0x00'8c20).mirror(0x03).rw(m_iocc, FUNC(rtpc_iocc_device::ccr_r), FUNC(rtpc_iocc_device::ccr_w));
	map(0x00'8c40, 0x00'8c40).mirror(0x03).lr8([this]() { return m_crra; }, "crra_r");
	map(0x00'8c40, 0x00'8c40).mirror(0x03).w(FUNC(rtpc_state::crra_w));
	map(0x00'8c60, 0x00'8c60).mirror(0x03).lr8([this]() { return m_crrb; }, "crrb_r");
	map(0x00'8c60, 0x00'8c60).mirror(0x03).w(FUNC(rtpc_state::crrb_w));

	// memory config reg (cc=2x8M, dd=2x2M, 88=2x4M)
	map(0x00'8c80, 0x00'8c80).mirror(0x03).lr8([]() { return 0xf8; }, "mcr");
	map(0x00'8ca0, 0x00'8ca0).mirror(0x03).w(FUNC(rtpc_state::dia_w));

	map(0x01'0000, 0x01'07ff).rw(m_iocc, FUNC(rtpc_iocc_device::tcw_r), FUNC(rtpc_iocc_device::tcw_w));
	map(0x01'0800, 0x01'0801).mirror(0x7fc).rw(m_iocc, FUNC(rtpc_iocc_device::csr_r<1>), FUNC(rtpc_iocc_device::csr_w));
	map(0x01'0802, 0x01'0803).mirror(0x7fc).rw(m_iocc, FUNC(rtpc_iocc_device::csr_r<0>), FUNC(rtpc_iocc_device::csr_w));
}

void rtpc_state::mcu_port1_w(u8 data)
{
	// bit  function
	//  6   speaker volume 0
	//  7   speaker volume 1
	LOGMASKED(LOG_KLS, "mcu_port1_w volume %d\n", data >> 6);
	m_mcu_p1 = (m_mcu_p1 & 0x3f) | (data & 0xc0);

	// speaker volume wraps to ppi port b.6 and b.5
	m_ppi_pb &= ~0x60;
	m_ppi_pb |= (data >> 1) & 0x60;

	speaker();
}

void rtpc_state::mcu_port2_w(u8 data)
{
	// bit  dst
	//  0   ppi port c.0 (iid0)
	//  1   ppi port c.1 (iid1)
	//  2   ppi port c.2 (iid2)
	//  3   i/o channel system reset (active low)
	//  4   ppi port c.4
	//  5   (input)
	//  6   ppi port c.6 (-ack)
	//  7   speaker frequency

	// interrupts
	// 0 informational
	// 1 received byte from keyboard
	// 2 received byte from uart
	// 3 returning byte requested by system
	// 4 block transfer ready
	// 5 unassigned
	// 6 self-test performed
	// 7 error condition

	if ((data ^ m_mcu_p2) & 7)
		LOGMASKED(LOG_KLS, "mcu_port2_w interrupt %d\n", data & 7);

	if ((data ^ m_mcu_p2) & 8)
		LOGMASKED(LOG_KLS, "mcu_port2_w system reset %d\n", data & 8);

	m_ppi->pc4_w(BIT(data, 4));
	m_ppi->pc6_w(BIT(data, 6));

	m_mcu_p2 &= ~0xdf;
	m_mcu_p2 |= data & 0xdf;

	// speaker frequency wraps to ppi port b.7
	m_ppi_pb &= ~0x80;
	m_ppi_pb |= ~data & 0x80;

	speaker();
}

void rtpc_state::mcu_port3_w(u8 data)
{
	// bit  i/o  function
	//  0    i   uart rx
	//  1    o   uart tx
	//  2    i   kbd clock in
	//  3    i   obf/int1
	//  4    i   32 kHz
	//  5    i   kbd data in
	//  6    o   kbd data out
	//  7    o   kbd clock out

	LOGMASKED(LOG_KLS, "mcu_port3_w 0x%02x\n", data);

	m_kbd_con->data_write_from_mb(BIT(data, 6));
	m_kbd_con->clock_write_from_mb(BIT(data, 7));

	m_mcu_p3 = (m_mcu_p3 & ~0xc2) | (data & 0xc2);
}

void rtpc_state::ppi_portc_w(u8 data)
{
	LOGMASKED(LOG_KLS, "ppi_portc_w 0x%02x\n", data);

	// bit 3 -> i/o channel (irq)
	m_pic[0]->ir5_w(BIT(data, 3));

	// bit 5 -> mcu p2.5 (ibf)
	if (BIT(data, 5))
		m_mcu_p2 |= 0x20;
	else
		m_mcu_p2 &= ~0x20;

	// bit 7 -> mcu p3.3(-int1) (-obf)
	if (BIT(data, 7))
		m_mcu_p3 |= 0x08;
	else
		m_mcu_p3 &= ~0x08;
	m_mcu->set_input_line(MCS51_INT1_LINE, !BIT(data, 7));
}

void rtpc_state::kls_cmd_w(offs_t offset, u16 data, u16 mem_mask)
{
	LOGMASKED(LOG_KLS, "kls_cmd_w command 0x%02x data 0x%02x mask 0x%04x\n", u8(data), data >> 8, mem_mask);

	// 00cc cccc dddd dddd
	switch (mem_mask)
	{
	case 0xff00:
		m_ppi->write(0, data);
		break;
	case 0x00ff:
		m_ppi->write(1, data);
		break;
	case 0xffff:
		m_mcu_p1 = (m_mcu_p1 & ~0x3f) | (data & 0x3f);
		m_ppi->write(0, data >> 8);
		break;
	}
}

void rtpc_state::crra_w(u8 data)
{
	LOG("crra_w 0x%02x\n", data);

	// bit  function
	//  0   i/o slot 1
	//  1   i/o slot 2
	//  2   i/o slot 3
	//  3   i/o slot 4
	//  4   i/o slot 5
	//  5   i/o slot 6
	//  6   i/o slot 7
	//  7   i/o slot 8

	m_crra = data;
}

void rtpc_state::crrb_w(u8 data)
{
	LOG("crrb_w 0x%02x\n", data);

	// bit  function
	//  0   8530
	//  1   rs232 interface
	//  2   8051
	//  3   dmac1
	//  4   dmac2
	//  5   arbitor
	//  6   reserved
	//  7   reserved

	if (m_scc && BIT(data, 0))
		m_scc->reset();
	// TODO: rs232 if
	m_mcu->set_input_line(INPUT_LINE_RESET, !BIT(data, 2));
	// TODO: dmac !ready
	// TODO: arbitor

	m_crrb = data;
}

void rtpc_state::dia_w(u8 data)
{
	bool const state = BIT(data, 0);

	LOG("dia_w 0x%02x (%s)\n", data, machine().describe_context());

	m_pic[0]->ir0_w(state);
	m_pic[0]->ir1_w(state);
	m_pic[0]->ir2_w(state);
	m_pic[0]->ir3_w(state);
	m_pic[0]->ir4_w(state);
	m_pic[0]->ir5_w(state);
	m_pic[0]->ir6_w(state);
	m_pic[0]->ir7_w(state);

	m_pic[1]->ir0_w(state);
	m_pic[1]->ir1_w(state);
	m_pic[1]->ir2_w(state);
	m_pic[1]->ir3_w(state);
	m_pic[1]->ir4_w(state);
	m_pic[1]->ir5_w(state);
	m_pic[1]->ir6_w(state);
	m_pic[1]->ir7_w(state);
}

void rtpc_state::common(machine_config &config)
{
	ROMP(config, m_cpu, 23'529'400 / 4);
	m_cpu->set_mmu(m_mmu);
	m_cpu->set_iou(m_iocc);

	input_merger_device &reqi2(INPUT_MERGER_ANY_LOW(config, "reqi2"));
	reqi2.output_handler().set_inputline(m_cpu, INPUT_LINE_IRQ2).invert();

	// TODO: hole for 1M/4M memory boards
	// 2/2 rams 4M
	// 2/1 rams 4M
	// 1/1 rams 4M hole 1M
	// 4/0 rams 4M
	// 4/4 rams 16M hole 4M
	ROSETTA(config, m_mmu, m_cpu->clock(), rosetta_device::RAM_4M);
	m_mmu->set_mem(m_cpu, AS_PROGRAM);
	m_mmu->set_rom("ipl");
	m_mmu->out_pchk().set(reqi2, FUNC(input_merger_device::in_w<0>));
	m_mmu->out_mchk().set_inputline(m_cpu, INPUT_LINE_NMI);

	// keyboard, locator, speaker adapter
	// P8051AH 2075
	// 61X6310 8811
	// (c)IBM 1986
	// (c)INTEL '80
	I8051(config, m_mcu, 9.216_MHz_XTAL);
	m_mcu->port_in_cb<0>().set([this]() { return m_ppi->pa_r(); });
	m_mcu->port_out_cb<0>().set([this](u8 data) { m_mcu_p0 = data; });
	m_mcu->port_in_cb<1>().set([this]() { return m_mcu_p1 & 0x1f; });
	m_mcu->port_out_cb<1>().set(FUNC(rtpc_state::mcu_port1_w));
	m_mcu->port_in_cb<2>().set([this]() { return m_mcu_p2; });
	m_mcu->port_out_cb<2>().set(FUNC(rtpc_state::mcu_port2_w));
	m_mcu->port_out_cb<3>().set(FUNC(rtpc_state::mcu_port3_w));
	m_mcu->port_in_cb<3>().set([this]() { return m_mcu_p3 & 0x3d; });
	m_mcu->serial_tx_cb().set(
		[this](u8 data)
		{
			if (BIT(m_mcu_p1, 5))
			{
				m_mcu_uart = data;
				m_mcu->set_input_line(MCS51_RX_LINE, 1);
			}
			else
				logerror("uart tx 0x%02x\n", data);
		});
	m_mcu->serial_rx_cb().set([this]() { return m_mcu_uart; });
	//m_mcu->out_?().set_inputline(m_cpu, INPUT_LINE_IRQ0);

	TIMER(config, "mcu_timer").configure_periodic(FUNC(rtpc_state::mcu_timer), attotime::from_hz(32768));

	RTPC_IOCC(config, m_iocc, 0);
	m_iocc->out_int().set(reqi2, FUNC(input_merger_device::in_w<1>));
	m_iocc->out_rst().set_inputline(m_dma[0], INPUT_LINE_RESET);
	m_iocc->out_rst().append_inputline(m_dma[1], INPUT_LINE_RESET);

	// ISA bus
	ISA16(config, m_isa, 14'318'180 / 3);
	m_isa->set_memspace(m_iocc, AS_PROGRAM);
	m_isa->set_iospace(m_iocc, AS_IO);
	//m_isa->iochck_callback().set(FUNC(at_mb_device::iochck_w));

	// NEC
	// D8237AC-5
	// 8903HV101
	AM9517A(config, m_dma[0], m_isa->clock());
	// chan  usage
	//  0    serial port A
	//  1    serial port B
	//  2    diskette drive, serial port A
	//  3    pc network, serial port B
	m_isa->drq0_callback().set(m_dma[0], FUNC(am9517a_device::dreq_w<0>));
	m_isa->drq1_callback().set(m_dma[0], FUNC(am9517a_device::dreq_w<1>));
	m_isa->drq2_callback().set(m_dma[0], FUNC(am9517a_device::dreq_w<2>));
	m_isa->drq3_callback().set(m_dma[0], FUNC(am9517a_device::dreq_w<3>));
	m_dma[0]->out_hreq_callback().set(m_dma[0], FUNC(am9517a_device::hack_w));
	m_dma[0]->out_dack_callback<0>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<2>));
	m_dma[0]->out_dack_callback<1>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<1>));
	m_dma[0]->out_dack_callback<2>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<0>));
	m_dma[0]->out_dack_callback<3>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<3>));
	m_dma[0]->in_memr_callback().set(m_iocc, FUNC(rtpc_iocc_device::dma_b_r));
	m_dma[0]->out_memw_callback().set(m_iocc, FUNC(rtpc_iocc_device::dma_b_w));

	// NEC
	// D8237AC-5
	// 8903HV101
	AM9517A(config, m_dma[1], m_isa->clock());
	// chan  usage
	//  5    reserved
	//  6    reserved
	//  7    reserved
	//  8    286 coprocessor
	m_isa->drq5_callback().set(m_dma[1], FUNC(am9517a_device::dreq_w<1>));
	m_isa->drq6_callback().set(m_dma[1], FUNC(am9517a_device::dreq_w<2>));
	m_isa->drq7_callback().set(m_dma[1], FUNC(am9517a_device::dreq_w<3>));
	m_dma[1]->out_hreq_callback().set(m_dma[1], FUNC(am9517a_device::hack_w));
	m_dma[1]->out_dack_callback<0>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<4>));
	m_dma[1]->out_dack_callback<1>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<5>));
	m_dma[1]->out_dack_callback<2>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<6>));
	m_dma[1]->out_dack_callback<3>().set(m_iocc, FUNC(rtpc_iocc_device::dack_w<7>));
	m_dma[1]->in_memr_callback().set(m_iocc, FUNC(rtpc_iocc_device::dma_w_r));
	m_dma[1]->out_memw_callback().set(m_iocc, FUNC(rtpc_iocc_device::dma_w_w));

	// NEC
	// D8259AC
	PIC8259(config, m_pic[0]);
	m_pic[0]->out_int_callback().set_inputline(m_cpu, INPUT_LINE_IRQ3).invert();
	// irq  source
	//  0   iocc irq 0: dma tc
	//  1   iocc irq 10: (multiport async)
	//  2   iocc irq 9: pc network, (multiport async), enhanced color graphics adapter, 3278/79 emulation adapter
	//  3   iocc irq 3: serial port 2, pc network
	//  4   iocc irq 4: serial port 1
	//  5   iocc irq 1: kbd
	//  6   iocc irq 2: 8530
	//  7   iocc irq 7: parallel port 1, monochrome/printer
	m_isa->irq10_callback().set(m_pic[0], FUNC(pic8259_device::ir1_w));
	m_isa->irq2_callback().set(m_pic[0], FUNC(pic8259_device::ir2_w));
	m_isa->irq3_callback().set(m_pic[0], FUNC(pic8259_device::ir3_w));
	m_isa->irq4_callback().set(m_pic[0], FUNC(pic8259_device::ir4_w));
	m_isa->irq7_callback().set(m_pic[0], FUNC(pic8259_device::ir7_w));

	// NEC
	// D8259AC
	PIC8259(config, m_pic[1]);
	m_pic[1]->out_int_callback().set_inputline(m_cpu, INPUT_LINE_IRQ4).invert();
	// irq  source
	//  0   iocc irq 8: reserved
	//  1   iocc irq 11: (advanced monochrome graphics display) (multiport async)
	//  2   iocc irq 14: fixed disk
	//  3   iocc irq 12: (ibm rt pc streaming tape drive adapter)
	//  4   iocc irq 6: diskette drive
	//  5   iocc irq 5: parallel port 2
	//  6   iocc irq 15: (286 coprocessor)
	//  7   iocc irq 13: serial port ex ct1 irpt
	m_isa->irq11_callback().set(m_pic[1], FUNC(pic8259_device::ir1_w));
	m_isa->irq14_callback().set(m_pic[1], FUNC(pic8259_device::ir2_w));
	m_isa->irq12_callback().set(m_pic[1], FUNC(pic8259_device::ir3_w));
	m_isa->irq6_callback().set(m_pic[1], FUNC(pic8259_device::ir4_w));
	m_isa->irq5_callback().set(m_pic[1], FUNC(pic8259_device::ir5_w));
	m_isa->irq15_callback().set(m_pic[1], FUNC(pic8259_device::ir6_w));

	// P8255A-5
	// L6430434
	// (C)INTEL '81
	I8255A(config, m_ppi);
	// port A: read/write 8051
	// port B: input
	// port C lower: input
	// port C upper: 8051 handshake
	// port C & 0x20 -> irq
	m_ppi->in_pa_callback().set([this]() { return m_mcu_p0; });

	// TODO: bits 4-1 "non-adapter system board signals"
	// TODO: bit 0 "uart rxd signal"
	m_ppi->in_pb_callback().set([this]() { return m_ppi_pb; });

	m_ppi->out_pc_callback().set(FUNC(rtpc_state::ppi_portc_w));
	m_ppi->in_pc_callback().set([this]() { return m_mcu_p2 & 0x57; });

	RTPC_KBD_CON(config, m_kbd_con);
	m_kbd_con->option_add("kbd", RTPC_KBD);
	m_kbd_con->set_default_option("kbd");
	m_kbd_con->out_data_cb().set([this](int state) { if (state) m_mcu_p3 |= 0x20; else m_mcu_p3 &= ~0x20; });
	m_kbd_con->out_clock_cb().set(
		[this](int state)
		{
			if (state)
				m_mcu_p3 |= 0x04;
			else
				m_mcu_p3 &= ~0x04;
			m_mcu->set_input_line(MCS51_INT0_LINE, !state);
		});

	// MC146818AP
	// IL 0A46D8729
	MC146818(config, m_rtc, 32.768_kHz_XTAL);
	m_rtc->sqw().set(m_cpu, FUNC(romp_device::clk_w));
	m_rtc->sqw().append(
		[this](int state)
		{
			if (state)
				m_ppi_pb |= 0x08;
			else
				m_ppi_pb &= ~0x08;
		});
	m_rtc->irq().set_inputline(m_cpu, INPUT_LINE_IRQ1).invert();
	m_rtc->irq().append(
		[this](int state)
		{
			if (!state)
				m_ppi_pb |= 0x10;
			else
				m_ppi_pb &= ~0x10;
		});

	config.set_default_layout(layout_rtpc);
}

void rtpc_isa8_cards(device_slot_interface &device)
{
	device.option_add("mda", ISA8_MDA);
}

void rtpc_isa16_cards(device_slot_interface &device)
{
	// FIXME: need 16-bit combined hdc/fdc card
	device.option_add("hdc", ISA16_IDE);
	device.option_add("fdc", ISA8_FDC_AT);
}

void rtpc_state::ibm6150(machine_config &config)
{
	common(config);
	m_iocc->set_addrmap(2, &rtpc_state::iocc_pio_map<true>);

	SCC8530N(config, m_scc, 3'580'000);
	m_scc->configure_channels(3'072'000, 3'072'000, 3'072'000, 3'072'000);
	m_scc->out_int_callback().set(m_pic[0], FUNC(pic8259_device::ir6_w));

	// port: TXD, DTR, RTS, RI, RXD, DSR, CTS, DCD
	// external registers: DTR, DSR, RI
	// scc: TXD, RTS, RXD, CTS, DCD
	// scc: DTR/REQ, SYNC not connected

	rs232_port_device &port0(RS232_PORT(config, "serial0", default_rs232_devices, nullptr));
	port0.cts_handler().set(m_scc, FUNC(z80scc_device::ctsa_w));
	port0.dcd_handler().set(m_scc, FUNC(z80scc_device::dcda_w));
	port0.rxd_handler().set(m_scc, FUNC(z80scc_device::rxa_w));
	m_scc->out_rtsa_callback().set(port0, FUNC(rs232_port_device::write_rts));
	m_scc->out_txda_callback().set(port0, FUNC(rs232_port_device::write_txd));

	rs232_port_device &port1(RS232_PORT(config, "serial1", default_rs232_devices, nullptr));
	// TXD, DTR, RTS, RI, RXD, DSR, CTS, DCD
	port1.cts_handler().set(m_scc, FUNC(z80scc_device::ctsb_w));
	port1.dcd_handler().set(m_scc, FUNC(z80scc_device::dcdb_w));
	port1.rxd_handler().set(m_scc, FUNC(z80scc_device::rxb_w));
	m_scc->out_rtsb_callback().set(port1, FUNC(rs232_port_device::write_rts));
	m_scc->out_txdb_callback().set(port1, FUNC(rs232_port_device::write_txd));

	// ISA slots
	ISA16_SLOT(config, "isa1", 0, m_isa, rtpc_isa16_cards, "fdc",   false); // slot 1: disk/diskette adapter
	ISA16_SLOT(config, "isa2", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 2: option
	ISA8_SLOT(config,  "isa3", 0, m_isa, rtpc_isa8_cards,  "mda",   false); // slot 3: option
	ISA16_SLOT(config, "isa4", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 4: option
	ISA16_SLOT(config, "isa5", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 5: option
	ISA8_SLOT(config,  "isa6", 0, m_isa, rtpc_isa8_cards,  nullptr, false); // slot 6: option
	ISA16_SLOT(config, "isa7", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 7: option
	ISA16_SLOT(config, "isa8", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 8: coprocessor/option
}

void rtpc_state::ibm6151(machine_config &config)
{
	common(config);
	m_iocc->set_addrmap(2, &rtpc_state::iocc_pio_map<false>);

	// ISA slots
	ISA8_SLOT(config,  "isa1", 0, m_isa, rtpc_isa8_cards,  "mda",   false); // slot 1: option
	ISA16_SLOT(config, "isa2", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 2: option
	ISA16_SLOT(config, "isa3", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 2: option
	ISA16_SLOT(config, "isa4", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 4: option
	ISA16_SLOT(config, "isa5", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 5: coprocessor/option
	ISA16_SLOT(config, "isa6", 0, m_isa, rtpc_isa16_cards, nullptr, false); // slot 6: disk/diskette adapter
}

ROM_START(ibm6150)
	ROM_REGION32_BE(0x10000, "ipl", 0)
	ROM_SYSTEM_BIOS(0, "ipl", "IPL")
	ROMX_LOAD("79x3456.bin", 0x00000, 0x4000, CRC(0a45a9ba) SHA1(02ca637c6a871c180dbfebf2ec68d8ec5a998c76), ROM_BIOS(0) | ROM_SKIP(3))
	ROMX_LOAD("79x3458.bin", 0x00001, 0x4000, CRC(7bd08ab6) SHA1(aabcfbb8fa1a5f8a08fb5cfd90ca6fe05258fde9), ROM_BIOS(0) | ROM_SKIP(3))
	ROMX_LOAD("79x3460.bin", 0x00002, 0x4000, CRC(897586e0) SHA1(528772635903f27235ebba2622b03386b84e4e17), ROM_BIOS(0) | ROM_SKIP(3))
	ROMX_LOAD("79x3462.bin", 0x00003, 0x4000, CRC(12aca906) SHA1(58f95b95768ef131d8d9d552506a9fe9c9c6077d), ROM_BIOS(0) | ROM_SKIP(3))

	ROM_REGION(0x1000, "mcu", 0)
	ROM_LOAD("61x6310_8051.bin", 0x0000, 0x1000, CRC(296c16c1) SHA1(83858109c39d5be37e49f24d1db4e2b15f38843e))
ROM_END

ROM_START(ibm6151)
	ROM_REGION32_BE(0x10000, "ipl", 0)
	ROM_SYSTEM_BIOS(0, "ipl", "IPL")
	ROMX_LOAD("79x3456.bin", 0x00000, 0x4000, CRC(0a45a9ba) SHA1(02ca637c6a871c180dbfebf2ec68d8ec5a998c76), ROM_BIOS(0) | ROM_SKIP(3))
	ROMX_LOAD("79x3458.bin", 0x00001, 0x4000, CRC(7bd08ab6) SHA1(aabcfbb8fa1a5f8a08fb5cfd90ca6fe05258fde9), ROM_BIOS(0) | ROM_SKIP(3))
	ROMX_LOAD("79x3460.bin", 0x00002, 0x4000, CRC(897586e0) SHA1(528772635903f27235ebba2622b03386b84e4e17), ROM_BIOS(0) | ROM_SKIP(3))
	ROMX_LOAD("79x3462.bin", 0x00003, 0x4000, CRC(12aca906) SHA1(58f95b95768ef131d8d9d552506a9fe9c9c6077d), ROM_BIOS(0) | ROM_SKIP(3))

	// Version: 073
	// Date: 85 289 (16 Oct 1985)
	// Checksum: 0xc8 0xe8
	ROM_REGION(0x1000, "mcu", 0)
	ROM_LOAD("61x6310_8051.bin", 0x0000, 0x1000, CRC(296c16c1) SHA1(83858109c39d5be37e49f24d1db4e2b15f38843e))
ROM_END

#define rom_rtpc010 rom_ibm6151
#define rom_rtpc015 rom_ibm6151
#define rom_rtpc020 rom_ibm6150
#define rom_rtpc025 rom_ibm6150
#define rom_rtpca25 rom_ibm6150

} // anonymous namespace

/*   YEAR   NAME      PARENT  COMPAT  MACHINE   INPUT  CLASS        INIT          COMPANY                             FULLNAME               FLAGS */
COMP(1986,  rtpc010,  0,      0,      ibm6151,  0,     rtpc_state,  init_common,  "International Business Machines",  "IBM RT PC Model 010", MACHINE_NOT_WORKING)
COMP(1986,  rtpc015,  0,      0,      ibm6151,  0,     rtpc_state,  init_common,  "International Business Machines",  "IBM RT PC Model 015", MACHINE_NOT_WORKING)
COMP(1986,  rtpc020,  0,      0,      ibm6150,  0,     rtpc_state,  init_common,  "International Business Machines",  "IBM RT PC Model 020", MACHINE_NOT_WORKING)
COMP(1986,  rtpc025,  0,      0,      ibm6150,  0,     rtpc_state,  init_common,  "International Business Machines",  "IBM RT PC Model 025", MACHINE_NOT_WORKING)
COMP(1986,  rtpca25,  0,      0,      ibm6150,  0,     rtpc_state,  init_common,  "International Business Machines",  "IBM RT PC Model A25", MACHINE_NOT_WORKING)
