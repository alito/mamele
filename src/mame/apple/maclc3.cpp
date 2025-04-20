// license:BSD-3-Clause
// copyright-holders:R. Belmont
/****************************************************************************

    drivers/maclc3.cpp
    Mac LC III ("Vail")
    Mac LC 520 ("Hook")

    By R. Belmont

    These are basically identical '030 machines, except the LC III is a pizzabox
    while the LC 520 is an all-in-one with a 14" Sony Trinitron built-in.
    LC III uses the Egret ADB microcontroller while LC 520 uses the later Cuda.
    Everything else is the same.

****************************************************************************/

#include "emu.h"

#include "bus/nscsi/devices.h"
#include "bus/rs232/rs232.h"
#include "cpu/m68000/m68030.h"
#include "machine/ram.h"
#include "machine/timer.h"
#include "machine/z80scc.h"
#include "machine/nscsi_bus.h"
#include "machine/ncr5380.h"
#include "cuda.h"
#include "egret.h"
#include "macadb.h"
#include "macscsi.h"
#include "mactoolbox.h"
#include "sonora.h"

#include "emupal.h"
#include "screen.h"
#include "softlist_dev.h"

namespace {

static constexpr u32 C7M = 7833600;
static constexpr u32 C15M = (C7M * 2);

class macvail_state : public driver_device
{
public:
	macvail_state(const machine_config &mconfig, device_type type, const char *tag) :
		driver_device(mconfig, type, tag),
		m_maincpu(*this, "maincpu"),
		m_macadb(*this, "macadb"),
		m_ram(*this, RAM_TAG),
		m_sonora(*this, "sonora"),
		m_scsibus1(*this, "scsi"),
		m_ncr5380(*this, "scsi:7:ncr5380"),
		m_scsihelp(*this, "scsihelp"),
		m_scc(*this, "scc"),
		m_egret(*this, "egret"),
		m_cuda(*this, "cuda")
	{
	}

	void maclc3_base(machine_config &config);
	void maclc3(machine_config &config);
	void maclc520(machine_config &config);
	void base_map(address_map &map);
	void maclc3_map(address_map &map);
	void maclc520_map(address_map &map);

private:
	required_device<m68030_device> m_maincpu;
	optional_device<macadb_device> m_macadb;
	required_device<ram_device> m_ram;
	required_device<sonora_device> m_sonora;
	required_device<nscsi_bus_device> m_scsibus1;
	required_device<ncr5380_device> m_ncr5380;
	required_device<mac_scsi_helper_device> m_scsihelp;
	required_device<z80scc_device> m_scc;
	optional_device<egret_device> m_egret;
	optional_device<cuda_device> m_cuda;

	virtual void machine_start() override;

	u16 scc_r(offs_t offset)
	{
		u16 result = m_scc->dc_ab_r(offset);
		return (result << 8) | result;
	}
	void scc_w(offs_t offset, u16 data)
	{
		m_scc->dc_ab_w(offset, data >> 8);
	}

	u16 scsi_r(offs_t offset, u16 mem_mask = ~0);
	void scsi_w(offs_t offset, u16 data, u16 mem_mask = ~0);
	u32 scsi_drq_r(offs_t offset, u32 mem_mask = ~0);
	void scsi_drq_w(offs_t offset, u32 data, u32 mem_mask = ~0);

	void scsi_berr_w(u8 data)
	{
		m_maincpu->pulse_input_line(M68K_LINE_BUSERROR, attotime::zero);
	}

	WRITE_LINE_MEMBER(cuda_reset_w)
	{
		m_maincpu->set_input_line(INPUT_LINE_HALT, state);
		m_maincpu->set_input_line(INPUT_LINE_RESET, state);
	}
};

void macvail_state::machine_start()
{
	m_sonora->set_ram_info((u32 *) m_ram->pointer(), m_ram->size());
}

/***************************************************************************
    ADDRESS MAPS
***************************************************************************/
void macvail_state::base_map(address_map &map)
{
	// RAM, ROM, and base I/O mappings come from Sonora (LCIII) / Ardbeg (LC520)
	map(0x40000000, 0x600fffff).m(m_sonora, FUNC(sonora_device::map));

	map(0x50004000, 0x50005fff).rw(FUNC(macvail_state::scc_r), FUNC(macvail_state::scc_w)).mirror(0x00f00000);
	map(0x50006000, 0x50007fff).rw(FUNC(macvail_state::scsi_drq_r), FUNC(macvail_state::scsi_drq_w)).mirror(0x00f00000);
	map(0x50010000, 0x50011fff).rw(FUNC(macvail_state::scsi_r), FUNC(macvail_state::scsi_w)).mirror(0x00f00000);
	map(0x50012000, 0x50013fff).rw(FUNC(macvail_state::scsi_drq_r), FUNC(macvail_state::scsi_drq_w)).mirror(0x00f00000);
}

void macvail_state::maclc3_map(address_map &map)
{
	base_map(map);
	map(0x5ffffffc, 0x5fffffff).lr32(NAME([](offs_t offset) { return 0xa55a0001; }));
}

void macvail_state::maclc520_map(address_map &map)
{
	base_map(map);
	map(0x5ffffffc, 0x5fffffff).lr32(NAME([](offs_t offset) { return 0xa55a0100; }));
}

u16 macvail_state::scsi_r(offs_t offset, u16 mem_mask)
{
	const int reg = (offset >> 3) & 0xf;
	const bool pseudo_dma = (reg == 6) && (offset == 0x130);

	return m_scsihelp->read_wrapper(pseudo_dma, reg) << 8;
}

void macvail_state::scsi_w(offs_t offset, u16 data, u16 mem_mask)
{
	const int reg = (offset >> 3) & 0xf;
	const bool pseudo_dma = (reg == 0) && (offset == 0x100);

	m_scsihelp->write_wrapper(pseudo_dma, reg, data >> 8);
}

u32 macvail_state::scsi_drq_r(offs_t offset, u32 mem_mask)
{
	switch (mem_mask)
	{
	case 0xff000000:
		return m_scsihelp->read_wrapper(true, 6) << 24;

	case 0xffff0000:
		return (m_scsihelp->read_wrapper(true, 6) << 24) | (m_scsihelp->read_wrapper(true, 6) << 16);

	case 0xffffffff:
		return (m_scsihelp->read_wrapper(true, 6) << 24) | (m_scsihelp->read_wrapper(true, 6) << 16) | (m_scsihelp->read_wrapper(true, 6) << 8) | m_scsihelp->read_wrapper(true, 6);

	default:
		logerror("scsi_drq_r: unknown mem_mask %08x\n", mem_mask);
	}

	return 0;
}

void macvail_state::scsi_drq_w(offs_t offset, u32 data, u32 mem_mask)
{
	switch (mem_mask)
	{
	case 0xff000000:
		m_scsihelp->write_wrapper(true, 0, data >> 24);
		break;

	case 0xffff0000:
		m_scsihelp->write_wrapper(true, 0, data >> 24);
		m_scsihelp->write_wrapper(true, 0, data >> 16);
		break;

	case 0xffffffff:
		m_scsihelp->write_wrapper(true, 0, data >> 24);
		m_scsihelp->write_wrapper(true, 0, data >> 16);
		m_scsihelp->write_wrapper(true, 0, data >> 8);
		m_scsihelp->write_wrapper(true, 0, data & 0xff);
		break;

	default:
		logerror("scsi_drq_w: unknown mem_mask %08x\n", mem_mask);
		break;
	}
}

/***************************************************************************
    DEVICE CONFIG
***************************************************************************/

static INPUT_PORTS_START( macadb )
INPUT_PORTS_END

/***************************************************************************
    MACHINE DRIVERS
***************************************************************************/

void macvail_state::maclc3_base(machine_config &config)
{
	M68030(config, m_maincpu, 25000000);
	m_maincpu->set_dasm_override(std::function(&mac68k_dasm_override), "mac68k_dasm_override");

	RAM(config, m_ram);
	m_ram->set_default_size("4M");
	m_ram->set_extra_options("8M,16M,32M,48M,64M,80M");

	NSCSI_BUS(config, "scsi");
	NSCSI_CONNECTOR(config, "scsi:0", mac_scsi_devices, nullptr);
	NSCSI_CONNECTOR(config, "scsi:1", mac_scsi_devices, nullptr);
	NSCSI_CONNECTOR(config, "scsi:2", mac_scsi_devices, nullptr);
	NSCSI_CONNECTOR(config, "scsi:3", mac_scsi_devices, nullptr);
	NSCSI_CONNECTOR(config, "scsi:4", mac_scsi_devices, "cdrom");
	NSCSI_CONNECTOR(config, "scsi:5", mac_scsi_devices, nullptr);
	NSCSI_CONNECTOR(config, "scsi:6", mac_scsi_devices, "harddisk");
	NSCSI_CONNECTOR(config, "scsi:7").option_set("ncr5380", NCR53C80).machine_config([this](device_t *device)
	{
		ncr53c80_device &adapter = downcast<ncr53c80_device &>(*device);
		adapter.drq_handler().set(m_scsihelp, FUNC(mac_scsi_helper_device::drq_w));
	});

	MAC_SCSI_HELPER(config, m_scsihelp);
	m_scsihelp->scsi_read_callback().set(m_ncr5380, FUNC(ncr53c80_device::read));
	m_scsihelp->scsi_write_callback().set(m_ncr5380, FUNC(ncr53c80_device::write));
	m_scsihelp->scsi_dma_read_callback().set(m_ncr5380, FUNC(ncr53c80_device::dma_r));
	m_scsihelp->scsi_dma_write_callback().set(m_ncr5380, FUNC(ncr53c80_device::dma_w));
	m_scsihelp->cpu_halt_callback().set_inputline(m_maincpu, INPUT_LINE_HALT);
	m_scsihelp->timeout_error_callback().set(FUNC(macvail_state::scsi_berr_w));

	SOFTWARE_LIST(config, "hdd_list").set_original("mac_hdd");
	SOFTWARE_LIST(config, "flop35hd_list").set_original("mac_hdflop");

	SCC85C30(config, m_scc, C7M);
	m_scc->configure_channels(3'686'400, 3'686'400, 3'686'400, 3'686'400);
	m_scc->out_int_callback().set(m_sonora, FUNC(sonora_device::scc_irq_w));
	m_scc->out_txda_callback().set("printer", FUNC(rs232_port_device::write_txd));
	m_scc->out_txdb_callback().set("modem", FUNC(rs232_port_device::write_txd));

	rs232_port_device &rs232a(RS232_PORT(config, "printer", default_rs232_devices, nullptr));
	rs232a.rxd_handler().set(m_scc, FUNC(z80scc_device::rxa_w));
	rs232a.dcd_handler().set(m_scc, FUNC(z80scc_device::dcda_w));
	rs232a.cts_handler().set(m_scc, FUNC(z80scc_device::ctsa_w));

	rs232_port_device &rs232b(RS232_PORT(config, "modem", default_rs232_devices, nullptr));
	rs232b.rxd_handler().set(m_scc, FUNC(z80scc_device::rxb_w));
	rs232b.dcd_handler().set(m_scc, FUNC(z80scc_device::dcdb_w));
	rs232b.cts_handler().set(m_scc, FUNC(z80scc_device::ctsb_w));

	SONORA(config, m_sonora, C15M);
	m_sonora->set_maincpu_tag("maincpu");
	m_sonora->set_rom_tag("bootrom");

	MACADB(config, m_macadb, C15M);
}

void macvail_state::maclc3(machine_config &config)
{
	maclc3_base(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &macvail_state::maclc3_map);

	EGRET(config, m_egret, EGRET_341S0851);
	m_egret->reset_callback().set(FUNC(macvail_state::cuda_reset_w));
	m_egret->linechange_callback().set(m_macadb, FUNC(macadb_device::adb_linechange_w));
	m_egret->via_clock_callback().set(m_sonora, FUNC(sonora_device::cb1_w));
	m_egret->via_data_callback().set(m_sonora, FUNC(sonora_device::cb2_w));
	m_macadb->adb_data_callback().set(m_egret, FUNC(egret_device::set_adb_line));
	config.set_perfect_quantum(m_maincpu);

	m_sonora->pb3_callback().set(m_egret, FUNC(egret_device::get_xcvr_session));
	m_sonora->pb4_callback().set(m_egret, FUNC(egret_device::set_via_full));
	m_sonora->pb5_callback().set(m_egret, FUNC(egret_device::set_sys_session));
	m_sonora->cb2_callback().set(m_egret, FUNC(egret_device::set_via_data));
}

void macvail_state::maclc520(machine_config &config)
{
	maclc3_base(config);
	m_maincpu->set_addrmap(AS_PROGRAM, &macvail_state::maclc520_map);

	CUDA(config, m_cuda, CUDA_341S0060);
	m_cuda->reset_callback().set(FUNC(macvail_state::cuda_reset_w));
	m_cuda->linechange_callback().set(m_macadb, FUNC(macadb_device::adb_linechange_w));
	m_cuda->via_clock_callback().set(m_sonora, FUNC(sonora_device::cb1_w));
	m_cuda->via_data_callback().set(m_sonora, FUNC(sonora_device::cb2_w));
	m_macadb->adb_data_callback().set(m_cuda, FUNC(cuda_device::set_adb_line));
	config.set_perfect_quantum(m_maincpu);

	m_sonora->pb3_callback().set(m_cuda, FUNC(cuda_device::get_treq));
	m_sonora->pb4_callback().set(m_cuda, FUNC(cuda_device::set_byteack));
	m_sonora->pb5_callback().set(m_cuda, FUNC(cuda_device::set_tip));
	m_sonora->cb2_callback().set(m_cuda, FUNC(cuda_device::set_via_data));
}

ROM_START( maclc3 )
	ROM_REGION32_BE(0x100000, "bootrom", 0)
	ROM_LOAD( "ecbbc41c.rom", 0x000000, 0x100000, CRC(e578f5f3) SHA1(c77df3220c861f37a2c553b6ee9241b202dfdffc) )
ROM_END

ROM_START( maclc520 )
	ROM_REGION32_BE(0x100000, "bootrom", 0)
	ROM_LOAD( "ede66cbd.rom", 0x000000, 0x100000, CRC(a893cb0f) SHA1(c54ee2f45020a4adeb7451adce04cd6e5fb69790) )
ROM_END

} // anonymous namespace

COMP(1993, maclc3, 0, 0, maclc3, macadb, macvail_state, empty_init, "Apple Computer", "Macintosh LC III", MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND)
COMP(1993, maclc520, 0, 0, maclc520, macadb, macvail_state, empty_init, "Apple Computer", "Macintosh LC 520", MACHINE_SUPPORTS_SAVE | MACHINE_IMPERFECT_SOUND)
