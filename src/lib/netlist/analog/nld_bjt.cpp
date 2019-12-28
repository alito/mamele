// license:GPL-2.0+
// copyright-holders:Couriersud

#include "netlist/solver/nld_solver.h"
#include "netlist/nl_setup.h"
#include "nlid_twoterm.h"

namespace netlist
{
namespace analog
{
	class diode
	{
	public:
		diode()
		: m_Is(nlconst::magic(1e-15))
		, m_VT(nlconst::magic(0.0258))
		, m_VT_inv(plib::reciprocal(m_VT))
		{}

		diode(nl_fptype Is, nl_fptype n)
		{
			m_Is = Is;
			m_VT = nlconst::magic(0.0258) * n;
			m_VT_inv = plib::reciprocal(m_VT);
		}
		void set(nl_fptype Is, nl_fptype n) noexcept
		{
			m_Is = Is;
			m_VT = nlconst::magic(0.0258) * n;
			m_VT_inv = plib::reciprocal(m_VT);
		}
		nl_fptype I(nl_fptype V) const noexcept { return m_Is * plib::exp(V * m_VT_inv) - m_Is; }
		nl_fptype g(nl_fptype V) const noexcept { return m_Is * m_VT_inv * plib::exp(V * m_VT_inv); }
		nl_fptype V(nl_fptype I) const noexcept { return plib::log1p(I / m_Is) * m_VT; } // log1p(x)=log(1.0 + x)
		nl_fptype gI(nl_fptype I) const noexcept { return m_VT_inv * (I + m_Is); }

	private:
		nl_fptype m_Is;
		nl_fptype m_VT;
		nl_fptype m_VT_inv;
	};

	// -----------------------------------------------------------------------------
	// nld_Q - Base classes
	// -----------------------------------------------------------------------------

	/// \brief Class representing the bjt model parameters.
	///
	///  This is the model representation of the bjt model. Typically, SPICE uses
	///  the following parameters. A "Y" in the first column indicates that the
	///  parameter is actually used in netlist.
	///
	/// | NL? | name | parameter                                                             | units |  default |         example | area |
	/// |:---:|------|-----------------------------------------------------------------------|-------|---------:|----------------:|:----:|
	/// |  Y  | IS   | transport saturation current                                          | A     |   1E-016 |          1E-015 |   *  |
	/// |  Y  | BF   | ideal maximum forward beta                                            | -     |      100 |             100 |      |
	/// |  Y  | NF   | forward current emission coefficient                                  | -     |        1 |               1 |      |
	/// |     | VAF  | forward Early voltage                                                 | V     | infinite |             200 |      |
	/// |     | IKF  | corner for forward beta high current roll-off                         | A     | infinite |            0.01 |   *  |
	/// |     | ISE  | B-E leakage saturation current                                        | A     |        0 | 0.0000000000001 |   *  |
	/// |     | NE   | B-E leakage emission coefficient                                      | -     |      1.5 |               2 |      |
	/// |  Y  | BR   | ideal maximum reverse beta                                            | -     |        1 |             0.1 |      |
	/// |  Y  | NR   | reverse current emission coefficient                                  | -     |        1 |               1 |      |
	/// |     | VAR  | reverse Early voltage                                                 | V     | infinite |             200 |      |
	/// |     | IKR  | corner for reverse beta high current roll-off                         | A     | infinite |            0.01 |   *  |
	/// |     | ISC  | leakage saturation current                                            | A     |        0 |               8 |      |
	/// |     | NC   | leakage emission coefficient                                          | -     |        2 |             1.5 |      |
	/// |     | RB   | zero bias base resistance                                             |       |        0 |             100 |   *  |
	/// |     | IRB  | current where base resistance falls halfway to its min value          | A     | infinite |             0.1 |   *  |
	/// |     | RBM  | minimum base resistance at high currents                              |       |       RB |              10 |   *  |
	/// |     | RE   | emitter resistance                                                    |       |        0 |               1 |   *  |
	/// |     | RC   | collector resistance                                                  |       |        0 |              10 |   *  |
	/// |  Y  | CJE  | B-E zero-bias depletion capacitance                                   | F     |        0 |             2pF |   *  |
	/// |     | VJE  | B-E built-in potential                                                | V     |     0.75 |             0.6 |      |
	/// |     | MJE  | B-E junction exponential factor                                       | -     |     0.33 |            0.33 |      |
	/// |     | TF   | ideal forward transit time                                            | sec   |        0 |           0.1ns |      |
	/// |     | XTF  | coefficient for bias dependence of TF                                 | -     |        0 |                 |      |
	/// |     | VTF  | voltage describing VBC  dependence of TF                              | V     | infinite |                 |      |
	/// |     | ITF  | high-current parameter  for effect on TF                              | A     |        0 |                 |   *  |
	/// |     | PTF  | excess phase at freq=1.0/(TF*2PI) Hz                                  | deg   |        0 |                 |      |
	/// |  Y  | CJC  | B-C zero-bias depletion capacitance                                   | F     |        0 |             2pF |   *  |
	/// |     | VJC  | B-C built-in potential                                                | V     |     0.75 |             0.5 |      |
	/// |     | MJC  | B-C junction exponential factor                                       | -     |     0.33 |             0.5 |      |
	/// |     | XCJC | fraction of B-C depletion capacitance connected to internal base node | -     |        1 |                 |      |
	/// |     | TR   | ideal reverse transit time                                            | sec   |        0 |            10ns |      |
	/// |     | CJS  | zero-bias collector-substrate capacitance                             | F     |        0 |             2pF |   *  |
	/// |     | VJS  | substrate junction built-in potential                                 | V     |     0.75 |                 |      |
	/// |     | MJS  | substrate junction exponential factor                                 | -     |        0 |             0.5 |      |
	/// |     | XTB  | forward and reverse beta temperature exponent                         | -     |        0 |                 |      |
	/// |     | EG   | energy gap for temperature effect on IS                               | eV    |     1.11 |                 |      |
	/// |     | XTI  | temperature exponent for effect on IS                                 | -     |        3 |                 |      |
	/// |     | KF   | flicker-noise coefficient                                             | -     |        0 |                 |      |
	/// |     | AF   | flicker-noise exponent                                                | -     |        1 |                 |      |
	/// |     | FC   | coefficient for forward-bias depletion capacitance formula            | -     |      0.5 |                 |      |
	/// |     | TNOM | Parameter measurement temperature                                     | C     |       27 |              50 |      |
	///
	class bjt_model_t : public param_model_t
	{
	public:
		bjt_model_t(device_t &device, const pstring &name, const pstring &val)
		: param_model_t(device, name, val)
		, m_IS (*this, "IS")
		, m_BF (*this, "BF")
		, m_NF (*this, "NF")
		, m_BR (*this, "BR")
		, m_NR (*this, "NR")
		, m_CJE(*this, "CJE")
		, m_CJC(*this, "CJC")
		{}

		value_t m_IS;  //!< transport saturation current
		value_t m_BF;  //!< ideal maximum forward beta
		value_t m_NF;  //!< forward current emission coefficient
		value_t m_BR;  //!< ideal maximum reverse beta
		value_t m_NR;  //!< reverse current emission coefficient
		value_t m_CJE; //!< B-E zero-bias depletion capacitance
		value_t m_CJC; //!< B-C zero-bias depletion capacitance

	};

	// Have a common start for transistors

	NETLIB_OBJECT(QBJT)
	{
	public:
		enum q_type {
			BJT_NPN,
			BJT_PNP
		};

		NETLIB_CONSTRUCTOR_EX(QBJT, const pstring &model = "NPN")
		, m_model(*this, "MODEL", model)
		, m_qtype(BJT_NPN)
		{
		}

		NETLIB_IS_DYNAMIC(true)

		//NETLIB_RESETI();
		NETLIB_UPDATEI();

		q_type qtype() const noexcept { return m_qtype; }
		bool is_qtype(q_type atype) const noexcept { return m_qtype == atype; }
		void set_qtype(q_type atype) noexcept { m_qtype = atype; }
	protected:

		bjt_model_t m_model;
	private:
		q_type m_qtype;
	};

	// -----------------------------------------------------------------------------
	// nld_QBJT_switch
	// -----------------------------------------------------------------------------


	//
	//         + -              C
	//   B ----VVV----+         |
	//                |         |
	//                Rb        Rc
	//                Rb        Rc
	//                Rb        Rc
	//                |         |
	//                +----+----+
	//                     |
	//                     E
	//

	NETLIB_OBJECT_DERIVED(QBJT_switch, QBJT)
	{
		NETLIB_CONSTRUCTOR_DERIVED(QBJT_switch, QBJT)
			, m_RB(*this, "m_RB", true)
			, m_RC(*this, "m_RC", true)
			, m_BC(*this, "m_BC", true)
			, m_gB(nlconst::magic(1e-9))
			, m_gC(nlconst::magic(1e-9))
			, m_V(nlconst::zero())
			, m_state_on(*this, "m_state_on", 0)
		{
			register_subalias("B", m_RB.m_P);
			register_subalias("E", m_RB.m_N);
			register_subalias("C", m_RC.m_P);

			connect(m_RB.m_N, m_RC.m_N);
			connect(m_RB.m_P, m_BC.m_P);
			connect(m_RC.m_P, m_BC.m_N);
		}

		NETLIB_RESETI();
		NETLIB_UPDATEI();
		NETLIB_UPDATE_PARAMI();
		NETLIB_UPDATE_TERMINALSI();

	private:
		nld_twoterm m_RB;
		nld_twoterm m_RC;
		nld_twoterm m_BC;

		nl_fptype m_gB; // base conductance / switch on
		nl_fptype m_gC; // collector conductance / switch on
		nl_fptype m_V; // internal voltage source
		state_var<unsigned> m_state_on;

	private:
	};

	// -----------------------------------------------------------------------------
	// nld_QBJT_EB
	// -----------------------------------------------------------------------------


	NETLIB_OBJECT_DERIVED(QBJT_EB, QBJT)
	{
	public:
		NETLIB_CONSTRUCTOR_DERIVED(QBJT_EB, QBJT)
		, m_gD_BC(*this, "m_D_BC")
		, m_gD_BE(*this, "m_D_BE")
		, m_D_CB(*this, "m_D_CB", true)
		, m_D_EB(*this, "m_D_EB", true)
		, m_D_EC(*this, "m_D_EC", true)
		, m_alpha_f(0)
		, m_alpha_r(0)
		{
			register_subalias("E", m_D_EB.m_P);   // Cathode
			register_subalias("B", m_D_EB.m_N);   // Anode

			register_subalias("C", m_D_CB.m_P);   // Cathode

			connect(m_D_EB.m_P, m_D_EC.m_P);
			connect(m_D_EB.m_N, m_D_CB.m_N);
			connect(m_D_CB.m_P, m_D_EC.m_N);

			if (m_model.m_CJE > nlconst::zero())
			{
				create_and_register_subdevice("m_CJE", m_CJE);
				connect("B", "m_CJE.1");
				connect("E", "m_CJE.2");
			}
			if (m_model.m_CJC > nlconst::zero())
			{
				create_and_register_subdevice("m_CJC", m_CJC);
				connect("B", "m_CJC.1");
				connect("C", "m_CJC.2");
			}

		}

	protected:

		NETLIB_RESETI();
		NETLIB_UPDATEI();
		NETLIB_UPDATE_PARAMI();
		NETLIB_UPDATE_TERMINALSI();

	private:
		generic_diode<diode_e::BIPOLAR> m_gD_BC;
		generic_diode<diode_e::BIPOLAR> m_gD_BE;

		nld_twoterm m_D_CB;  // gcc, gce - gcc, gec - gcc, gcc - gce | Ic
		nld_twoterm m_D_EB;  // gee, gec - gee, gce - gee, gee - gec | Ie
		nld_twoterm m_D_EC;  // 0, -gec, -gcc, 0 | 0

		nl_fptype m_alpha_f;
		nl_fptype m_alpha_r;

		NETLIB_SUB_UPTR(analog, C) m_CJE;
		NETLIB_SUB_UPTR(analog, C) m_CJC;
	};


	// ----------------------------------------------------------------------------------------
	// nld_Q
	// ----------------------------------------------------------------------------------------

	NETLIB_UPDATE(QBJT)
	{
	//    netlist().solver()->schedule1();
	}

	// ----------------------------------------------------------------------------------------
	// nld_QBJT_switch
	// ----------------------------------------------------------------------------------------


	NETLIB_RESET(QBJT_switch)
	{
		NETLIB_NAME(QBJT)::reset();
		const auto zero(nlconst::zero());

		m_state_on = 0;

		m_RB.set_G_V_I(exec().gmin(), zero, zero);
		m_RC.set_G_V_I(exec().gmin(), zero, zero);

		m_BC.set_G_V_I(exec().gmin() / nlconst::magic(10.0), zero, zero);

	}

	NETLIB_UPDATE(QBJT_switch)
	{
		// FIXME: this should never be called
		if (!m_RB.m_P.net().isRailNet())
			m_RB.m_P.solve_now();   // Basis
		else if (!m_RB.m_N.net().isRailNet())
			m_RB.m_N.solve_now();   // Emitter
		else if (!m_RC.m_P.net().isRailNet())
			m_RC.m_P.solve_now();   // Collector
	}


	NETLIB_UPDATE_PARAM(QBJT_switch)
	{
		nl_fptype IS = m_model.m_IS;
		nl_fptype BF = m_model.m_BF;
		nl_fptype NF = m_model.m_NF;
		//nl_fptype VJE = m_model.dValue("VJE", 0.75);

		set_qtype((m_model.type() == "NPN") ? BJT_NPN : BJT_PNP);

		nl_fptype alpha = BF / (nlconst::one() + BF);

		diode d(IS, NF);

		// Assume 5mA Collector current for switch operation

		const auto cc(nlconst::magic(0.005));
		m_V = d.V(cc / alpha);

		// Base current is 0.005 / beta
		// as a rough estimate, we just scale the conductance down

		m_gB = plib::reciprocal((m_V/(cc / BF)));

		//m_gB = d.gI(0.005 / alpha);

		if (m_gB < exec().gmin())
			m_gB = exec().gmin();
		m_gC =  d.gI(cc); // very rough estimate
	}

	NETLIB_UPDATE_TERMINALS(QBJT_switch)
	{
		const nl_fptype m = (is_qtype( BJT_NPN) ? 1 : -1);

		const unsigned new_state = (m_RB.deltaV() * m > m_V ) ? 1 : 0;
		if (m_state_on ^ new_state)
		{
			const auto zero(nlconst::zero());
			const nl_fptype gb = new_state ? m_gB : exec().gmin();
			const nl_fptype gc = new_state ? m_gC : exec().gmin();
			const nl_fptype v  = new_state ? m_V * m : zero;

			m_RB.set_G_V_I(gb,   v,   zero);
			m_RC.set_G_V_I(gc, zero, zero);
			m_state_on = new_state;
		}
	}


	// ----------------------------------------------------------------------------------------
	// nld_Q - Ebers Moll
	// ----------------------------------------------------------------------------------------


	NETLIB_UPDATE(QBJT_EB)
	{
		// FIXME: this should never be called
		if (!m_D_EB.m_P.net().isRailNet())
			m_D_EB.m_P.solve_now();   // Basis
		else if (!m_D_EB.m_N.net().isRailNet())
			m_D_EB.m_N.solve_now();   // Emitter
		else
			m_D_CB.m_N.solve_now();   // Collector
	}

	NETLIB_RESET(QBJT_EB)
	{
		NETLIB_NAME(QBJT)::reset();
		if (m_CJE)
		{
			m_CJE->reset();
			m_CJE->m_C.setTo(m_model.m_CJE);
		}
		if (m_CJC)
		{
			m_CJC->reset();
			m_CJC->m_C.setTo(m_model.m_CJC);
		}

	}

	NETLIB_UPDATE_TERMINALS(QBJT_EB)
	{
		const nl_fptype polarity = nlconst::magic(qtype() == BJT_NPN ? 1.0 : -1.0);

		m_gD_BE.update_diode(-m_D_EB.deltaV() * polarity);
		m_gD_BC.update_diode(-m_D_CB.deltaV() * polarity);

		const nl_fptype gee = m_gD_BE.G();
		const nl_fptype gcc = m_gD_BC.G();
		const nl_fptype gec =  m_alpha_r * gcc;
		const nl_fptype gce =  m_alpha_f * gee;
		const nl_fptype sIe = -m_gD_BE.I() + m_alpha_r * m_gD_BC.I();
		const nl_fptype sIc = m_alpha_f * m_gD_BE.I() - m_gD_BC.I();
		const nl_fptype Ie = (sIe + gee * m_gD_BE.Vd() - gec * m_gD_BC.Vd()) * polarity;
		const nl_fptype Ic = (sIc - gce * m_gD_BE.Vd() + gcc * m_gD_BC.Vd()) * polarity;

		// "Circuit Design", page 174

		m_D_EB.set_mat(      gee, gec - gee,  -Ie,
					   gce - gee, gee - gec,   Ie);
		m_D_CB.set_mat(      gcc, gce - gcc,  -Ic,
					   gec - gcc, gcc - gce,   Ic);
		m_D_EC.set_mat(        0,      -gec,    0,
							-gce,         0,    0);
	}


	NETLIB_UPDATE_PARAM(QBJT_EB)
	{
		nl_fptype IS = m_model.m_IS;
		nl_fptype BF = m_model.m_BF;
		nl_fptype NF = m_model.m_NF;
		nl_fptype BR = m_model.m_BR;
		nl_fptype NR = m_model.m_NR;
		//nl_fptype VJE = m_model.dValue("VJE", 0.75);

		set_qtype((m_model.type() == "NPN") ? BJT_NPN : BJT_PNP);

		m_alpha_f = BF / (nlconst::one() + BF);
		m_alpha_r = BR / (nlconst::one() + BR);

		m_gD_BE.set_param(IS / m_alpha_f, NF, exec().gmin(), nlconst::T0());
		m_gD_BC.set_param(IS / m_alpha_r, NR, exec().gmin(), nlconst::T0());
	}

} // namespace analog

namespace devices {
	NETLIB_DEVICE_IMPL_NS(analog, QBJT_EB, "QBJT_EB", "MODEL")
	NETLIB_DEVICE_IMPL_NS(analog, QBJT_switch, "QBJT_SW", "MODEL")
} // namespace devices

} // namespace netlist
