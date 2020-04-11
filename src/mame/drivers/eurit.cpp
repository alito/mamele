// license:BSD-3-Clause
// copyright-holders:AJR
/****************************************************************************

    Skeleton driver for Ascom Eurit Euro-ISDN Telefon.

****************************************************************************/

#include "emu.h"
#include "cpu/m37710/m37710.h"
#include "machine/am79c30.h"

class eurit_state : public driver_device
{
public:
	eurit_state(const machine_config &mconfig, device_type type, const char *tag)
		: driver_device(mconfig, type, tag)
		, m_maincpu(*this, "maincpu")
	{
	}

	void eurit30(machine_config &mconfig);

private:
	void p4_w(u8 data);
	void p6_w(u8 data);

	void mem_map(address_map &map);

	required_device<m37730s2_device> m_maincpu;

	u8 m_p4 = 0;
};

void eurit_state::p4_w(u8 data)
{
	m_p4 = data;
}

void eurit_state::p6_w(u8 data)
{
	if (BIT(data, 6))
		logerror("%s: Writing $%X to LCDC %s register\n", machine().describe_context(), (m_p4 & 0xf0) >> 4, BIT(data, 4) ? "data" : "instruction");
}


void eurit_state::mem_map(address_map &map)
{
	map(0x000480, 0x007fff).ram();
	map(0x008000, 0x00ffff).rom().region("firmware", 0x8000);
	map(0x040000, 0x05ffff).rom().region("firmware", 0);
	map(0x0c0000, 0x0c0007).rw("dsc", FUNC(am79c30a_device::read), FUNC(am79c30a_device::write));
}


static INPUT_PORTS_START(eurit30)
INPUT_PORTS_END

void eurit_state::eurit30(machine_config &config)
{
	M37730S2(config, m_maincpu, 8'000'000); // type and clock unknown
	m_maincpu->set_addrmap(AS_PROGRAM, &eurit_state::mem_map);
	m_maincpu->p4_out_cb().set(FUNC(eurit_state::p4_w));
	m_maincpu->p6_out_cb().set(FUNC(eurit_state::p6_w));

	am79c30a_device &dsc(AM79C30A(config, "dsc", 12'288'000));
	dsc.int_callback().set_inputline(m_maincpu, M37710_LINE_IRQ0);
}


ROM_START(eurit30)
	ROM_REGION16_LE(0x20000, "firmware", 0) // Firmware 2.210 deutsch
	ROM_LOAD("d_2.210", 0x00000, 0x20000, CRC(c77be0ac) SHA1(1eaba66dcb4f64cc33565ca85de25341572ddb2e))
ROM_END


SYST(1996, eurit30, 0, 0, eurit30, eurit30, eurit_state, empty_init, "Ascom", "Eurit 30", MACHINE_IS_SKELETON)
