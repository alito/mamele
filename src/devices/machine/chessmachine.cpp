// license:BSD-3-Clause
// copyright-holders:hap, Sandro Ronco
/*

The ChessMachine by Tasc

This is the 2nd (1992) version of The ChessMachine, it is used in:
- The ChessMachine DR, PC ISA card
- The ChessMachine EC, PC external module (printer port)
- The ChessMachine EC2, Amiga external module
- Mephisto Risc 1MB/II, chess computer

Hardware notes:
- VLSI-ARM VY86C010-12QC (ARM2 CPU)
- 1MB RAM (8*Siemens HYB514256B-60)
- 128 bytes PROM (4*DM74S288AN)
- 2 GALs for I/O handling

Unlike the SR model (devices/bus/isa/chessmsr.*), RAM size and CPU type
are the same on every known device.

Only 4 lines for I/O, so that part is much slower than the SR model.

There is no XTAL, it looks like there's a variable resistor for tweaking
CPU speed. It should be around 14-16MHz. The ARM CPU is rated 12MHz, they
probably went for this solution to get optimum possible speed for each module.

TODO:
- PC version still gives a sync error on boot sometimes, probably related to quantum
- is interrupt handling correct?
- timer shouldn't be needed for disabling bootrom, real ARM has already read the next opcode

*/

#include "emu.h"
#include "machine/chessmachine.h"


DEFINE_DEVICE_TYPE(CHESSMACHINE, chessmachine_device, "chessmachine", "Tasc ChessMachine")

//-------------------------------------------------
//  constructor
//-------------------------------------------------

chessmachine_device::chessmachine_device(const machine_config &mconfig, const char *tag, device_t *owner, uint32_t clock) :
	device_t(mconfig, CHESSMACHINE, tag, owner, clock),
	m_maincpu(*this, "maincpu"),
	m_bootrom(*this, "bootrom"),
	m_ram(*this, "ram"),
	m_disable_bootrom(*this, "disable_bootrom"),
	m_data_out(*this)
{ }



//-------------------------------------------------
//  device_start - device-specific startup
//-------------------------------------------------

void chessmachine_device::device_start()
{
	// resolve callbacks
	m_data_out.resolve_safe();

	// register for savestates
	save_item(NAME(m_bootrom_enabled));
	save_item(NAME(m_latch));
}



//-------------------------------------------------
//  external handlers
//-------------------------------------------------

void chessmachine_device::sync0_callback(void *ptr, s32 param)
{
	m_latch[0] = (m_latch[0] & 0x80) | param;
}

void chessmachine_device::data0_w(int state)
{
	machine().scheduler().synchronize(timer_expired_delegate(FUNC(chessmachine_device::sync0_callback), this), state ? 1 : 0);
}

void chessmachine_device::sync1_callback(void *ptr, s32 param)
{
	m_latch[0] = (m_latch[0] & 1) | param;

	// cause interrupt?
	m_maincpu->set_input_line(ARM_FIRQ_LINE, param ? ASSERT_LINE : CLEAR_LINE);
}

void chessmachine_device::data1_w(int state)
{
	machine().scheduler().synchronize(timer_expired_delegate(FUNC(chessmachine_device::sync1_callback), this), state ? 0x80 : 0);
}

void chessmachine_device::reset_w(int state)
{
	m_maincpu->set_input_line(INPUT_LINE_RESET, state ? ASSERT_LINE : CLEAR_LINE);

	if (!m_bootrom_enabled && state)
		install_bootrom(true);
}



//-------------------------------------------------
//  internal
//-------------------------------------------------

void chessmachine_device::install_bootrom(bool enable)
{
	address_space &program = m_maincpu->space(AS_PROGRAM);
	program.unmap_readwrite(0, m_bootrom.bytes() - 1);

	if (enable)
		program.install_rom(0, m_bootrom.bytes() - 1, m_bootrom);
	else
		program.install_ram(0, m_ram.bytes() - 1, m_ram);

	m_bootrom_enabled = enable;
}

u32 chessmachine_device::disable_bootrom_r()
{
	// disconnect bootrom from the bus after next opcode
	if (m_bootrom_enabled && !m_disable_bootrom->enabled() && !machine().side_effects_disabled())
		m_disable_bootrom->adjust(m_maincpu->cycles_to_attotime(5));

	return 0;
}

void chessmachine_device::main_map(address_map &map)
{
	map(0x00000000, 0x000fffff).ram().share("ram");
	map(0x00400000, 0x00400003).mirror(0x003ffffc).rw(FUNC(chessmachine_device::internal_r), FUNC(chessmachine_device::internal_w)).umask32(0x000000ff);
	map(0x01800000, 0x01800003).r(FUNC(chessmachine_device::disable_bootrom_r));
}

void chessmachine_device::device_add_mconfig(machine_config &config)
{
	ARM(config, m_maincpu, DERIVED_CLOCK(1,1));
	m_maincpu->set_addrmap(AS_PROGRAM, &chessmachine_device::main_map);
	m_maincpu->set_copro_type(arm_cpu_device::copro_type::VL86C020);

	TIMER(config, "disable_bootrom").configure_generic(FUNC(chessmachine_device::disable_bootrom));
}



//-------------------------------------------------
//  rom_region - device-specific ROM region
//-------------------------------------------------

ROM_START( chessmachine )
	ROM_REGION32_LE( 0x80, "bootrom", 0 )
	ROM_LOAD32_BYTE( "74s288.1", 0x00, 0x20, CRC(284114e2) SHA1(df4037536d505d7240bb1d70dc58f59a34ab77b4) )
	ROM_LOAD32_BYTE( "74s288.2", 0x01, 0x20, CRC(9f239c75) SHA1(aafaf30dac90f36b01f9ee89903649fc4ea0480d) )
	ROM_LOAD32_BYTE( "74s288.3", 0x02, 0x20, CRC(0455360b) SHA1(f1486142330f2c39a4d6c479646030d31443d1c8) )
	ROM_LOAD32_BYTE( "74s288.4", 0x03, 0x20, CRC(c7c9aba8) SHA1(cbb5b12b5917e36679d45bcbc36ea9285223a75d) )
ROM_END

const tiny_rom_entry *chessmachine_device::device_rom_region() const
{
	return ROM_NAME(chessmachine);
}
