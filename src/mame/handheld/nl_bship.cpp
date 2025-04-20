// license:CC0-1.0
// copyright-holders:hap
// thanks-to:Sean Riddle
/*

Milton Bradley Electronic Battleship model 4750A (hh_tms1k.cpp)

TODO:
- OPENDRAIN parameters are not fully known
- DYNAMIC_MIN_TIMESTEP 1e-6 is a bit better, but too taxing for MAME,
  maybe it could be made faster with OPTIMIZE_FRONTIER
- Q2 should be a 2N3703 (same, just different max voltage)
- D6 should be a 1K60? (germanium, type unknown, but looks like 1K60)
- move 1N754A to netlist library?

There are no labels on the PCB for the components. The commented-out ones are
on the PCB, but not used for sound.

Noise source (used for explosion sound) is a zener diode, this is partially
HLE'd, as I don't think MAME's netlist simulates that.

*/

#include "netlist/devices/net_lib.h"

#define HLE_NOISE_GEN (1)


NETLIST_START(bship)
{
	SOLVER(Solver, 48000)
	PARAM(Solver.ACCURACY, 1e-7)
	PARAM(Solver.DYNAMIC_TS, 1)
	PARAM(Solver.DYNAMIC_MIN_TIMESTEP, 2e-6)

	ANALOG_INPUT(VBATT1, 9)
	ANALOG_INPUT(VBATT2, 9)

	NET_MODEL("OPENDRAIN FAMILY(TYPE=MOS OVL=0.6 OVH=0.6 ORL=1.0 ORH=1.0)")

	LOGIC_INPUT(O0, 0, "OPENDRAIN")
	LOGIC_INPUT(O1, 0, "OPENDRAIN")
	LOGIC_INPUT(O2, 0, "OPENDRAIN")
	LOGIC_INPUT(O3, 0, "OPENDRAIN")
	//LOGIC_INPUT(O4, 0, "OPENDRAIN")
	LOGIC_INPUT(O5, 0, "OPENDRAIN")
	LOGIC_INPUT(O6, 0, "OPENDRAIN")
	LOGIC_INPUT(O7, 0, "OPENDRAIN")
	NET_C(VBATT1, O0.VDD, O1.VDD, O2.VDD, O3.VDD, O5.VDD, O6.VDD, O7.VDD)
	NET_C(GND, O0.VSS, O1.VSS, O2.VSS, O3.VSS, O5.VSS, O6.VSS, O7.VSS)

	RES(R1, RES_M(4.7))
	RES(R2, RES_K(51))
	RES(R3, RES_K(51))
	RES(R4, RES_K(47))
	RES(R5, RES_K(3.3))
	RES(R6, RES_K(100))
	RES(R7, RES_K(20))
	RES(R8, RES_K(51))
	RES(R9, RES_K(10))
	RES(R10, RES_K(4.7))
	RES(R11, RES_K(9.1))
	RES(R12, RES_K(22))
	RES(R13, RES_K(51))
	RES(R14, RES_K(51))
	//RES(R15, RES_K(100))
	RES(R16, RES_K(330))
	RES(R17, RES_K(15))
	RES(R18, RES_K(470))
	RES(R19, RES_K(51))
	RES(R20, RES_K(51))
	RES(R21, RES_M(1))
	RES(R22, RES_K(51))
	//RES(R23, 33)
	RES(R24, RES_K(4.7))
	RES(R25, RES_K(100))
	RES(R26, RES_K(39))
	RES(R27, RES_K(51))
	RES(R28, RES_K(100))
	RES(R29, RES_M(1.5))
	RES(R30, RES_K(51))
	RES(R31, RES_M(1))
	//RES(R32, RES_K(10))
	RES(R33, RES_M(1))
	RES(SPK1, 8)

	CAP(C1, CAP_U(6.8))
	CAP(C2, CAP_U(4.7))
	//CAP(C3, CAP_P(47))
	CAP(C4, CAP_U(0.1))
	CAP(C5, CAP_U(1))
	CAP(C6, CAP_U(0.1))
	CAP(C7, CAP_U(0.022))
	CAP(C8, CAP_U(0.1))
	CAP(C9, CAP_U(1))
	CAP(C10, CAP_U(1))
	CAP(C11, CAP_U(1))
	//CAP(C12, CAP_U(0.1))
	//CAP(C13, CAP_U(6.8))

	DIODE(D1, "1N4148")
	DIODE(D2, "1N4148")
	DIODE(D3, "1N4148")
	DIODE(D4, "1N4148")
	ZDIODE(D5, "D(BV=6.8 IBV=0.020 NBV=1)") // 1N754A
	DIODE(D6, "1N34A") // 1K60?
	DIODE(D7, "1N4148")
	//DIODE(D8, "1N4148")

	QBJT_EB(Q1, "2N3704") // NPN
	QBJT_EB(Q2, "2N3702") // 2N3703 PNP
	//QBJT_EB(Q3, "2N3704") // NPN
	QBJT_EB(Q4, "2N3704") // NPN
	QBJT_EB(Q5, "2N3704") // NPN

	LM324_DIP(IC1) // LM324N
	CD4016_DIP(IC2) // TP4016AN
	NE555_DIP(IC3) // NE555P

	// O0
	NET_C(O0.Q, R14.1, D4.A)
	NET_C(R14.2, GND)
	NET_C(D4.K, R5.1)
	NET_C(R5.2, R4.2, C2.1, R1.1, IC1.10, IC2.9)
	NET_C(C2.2, GND)
	NET_C(R1.2, GND)
	NET_C(R4.1, D3.K)

	// O1
	NET_C(O1.Q, R3.1, D2.A)
	NET_C(R3.2, GND)
	NET_C(D2.K, R24.2, R26.2, C7.1, IC3.2, IC3.6)
	NET_C(R24.1, IC2.3)
	NET_C(R26.1, R11.2, IC2.4, IC3.7)
	NET_C(C7.2, GND)

	NET_C(R11.1, R12.1, IC2.10)
	NET_C(R12.2, VBATT1)

	// O2
	NET_C(O2.Q, R2.1, D1.A)
	NET_C(R2.2, GND)
	NET_C(D1.K, R21.2, R22.1, IC1.6)

	NET_C(R21.1, C4.1, IC1.7)
	NET_C(R7.2, C4.2)
	NET_C(R7.1, R6.2, R16.2, IC1.13)

	NET_C(R6.1, Q1.E, Q2.E, SPK1.1)
	NET_C(SPK1.2, C1.1)
	NET_C(C1.2, GND)

	NET_C(R22.2, C6.1)
	NET_C(C6.2, R18.2, IC1.1)
	NET_C(R18.1, R17.1, IC1.2)
	NET_C(R17.2, C8.2)
	NET_C(C8.1, R28.1, R29.1, Q4.C)

	NET_C(R29.2, C10.1, D5.A)
	NET_C(C10.2, GND)

#if HLE_NOISE_GEN
	// zener diode noise
	CLOCK(NOISE_CLOCK, 10000)
	NET_C(NOISE_CLOCK.GND, GND)
	NET_C(NOISE_CLOCK.VCC, VBATT1)

	SYS_NOISE_MT_N(NOISE, 0.0075)
	NET_C(NOISE_CLOCK.Q, NOISE.I)
	NET_C(Q4.B, NOISE.1)
	NET_C(D5.K, NOISE.2)

#else
	// should be simply like this
	NET_C(D5.K, Q4.B)
#endif

	// O3
	NET_C(O3.Q, D3.A, R8.1, IC2.13)
	NET_C(R8.2, GND)

	// O5
	NET_C(O5.Q, R30.1, D6.A, D7.A)
	NET_C(R30.2, GND)
	NET_C(D6.K, R28.2, C9.1)
	NET_C(C9.2, GND)

	NET_C(D7.K, R31.2, R33.1, C11.1)
	NET_C(R33.2, GND)
	NET_C(C11.2, GND)
	NET_C(R31.1, Q5.B)
	NET_C(Q5.C, IC3.3)
	NET_C(Q5.E, R16.1)

	// O6
	NET_C(O6.Q, R27.1, IC2.6)
	NET_C(R27.2, GND)

	// O7
	NET_C(O7.Q, R13.1, IC2.12)
	NET_C(R13.2, GND)

	// misc
	NET_C(VBATT1, IC1.4, IC2.14, IC3.4, IC3.8)
	NET_C(VBATT2, Q1.C)
	NET_C(GND, Q2.C, Q4.E, IC1.11, IC2.1, IC2.7, IC3.1)
	HINT(IC3.5, NC)

	NET_C(IC1.3, R19.1, R20.1, C5.1, IC1.5, IC1.12)
	NET_C(R19.2, VBATT1)
	NET_C(R20.2, GND)
	NET_C(C5.2, GND)

	NET_C(IC1.8, IC1.9, IC2.11)
	NET_C(IC1.14, Q1.B, Q2.B)

	NET_C(IC2.2, R25.1, IC2.5)
	NET_C(R25.2, VBATT1)

	NET_C(IC2.8, R9.1, R10.1)
	NET_C(R9.2, GND)
	NET_C(R10.2, VBATT1)
}
