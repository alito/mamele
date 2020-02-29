// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef NLD_GENERIC_MODELS_H_
#define NLD_GENERIC_MODELS_H_

///
/// \file nld_generic_models.h
///

#include "netlist/nl_base.h"
#include "netlist/nl_setup.h"

namespace netlist
{
namespace analog
{

	// -----------------------------------------------------------------------------
	// A generic capacitor model
	// -----------------------------------------------------------------------------

	enum class capacitor_e
	{
		VARIABLE_CAPACITY,
		CONSTANT_CAPACITY
	};

	template <capacitor_e TYPE>
	class generic_capacitor
	{
	};

	template <>
	class generic_capacitor<capacitor_e::VARIABLE_CAPACITY>
	{
	public:
		generic_capacitor(device_t &dev, const pstring &name)
		: m_h(dev, name + ".m_h", nlconst::zero())
		, m_c(dev, name + ".m_c", nlconst::zero())
		, m_v(dev, name + ".m_v", nlconst::zero())
		, m_gmin(nlconst::zero())
		{
		}

		static capacitor_e type() noexcept { return capacitor_e::VARIABLE_CAPACITY; }

		// Circuit Simulation, page 284, 5.360
		// q(un+1) - q(un) = int(un, un+1, C(U)) = (C0+C1)/2 * (un+1-un)
		// The direct application of formulas 5.359 and 5.360 has
		// issues with pulses. Therefore G and Ieq are expressed differently
		// so that G depends on un+1 only and Ieq on un only.
		// In both cases, i = G * un+1 + Ieq

		nl_fptype G(nl_fptype cap) const noexcept
		{
			//return m_h * cap +  m_gmin;
			return m_h * nlconst::half() * (cap + m_c) +  m_gmin;
			//return m_h * cap +  m_gmin;
		}

		nl_fptype Ieq(nl_fptype cap, nl_fptype v) const noexcept
		{
			plib::unused_var(v);
			//return -m_h * 0.5 * ((cap + m_c) * m_v + (cap - m_c) * v) ;
			return -m_h * nlconst::half() * (cap + m_c) * m_v;
			//return -m_h * cap * m_v;
		}

		void timestep(nl_fptype cap, nl_fptype v, nl_fptype step) noexcept
		{
			m_h = plib::reciprocal(step);
			m_c = cap;
			m_v = v;
		}

		void setparams(nl_fptype gmin) noexcept { m_gmin = gmin; }

	private:
		state_var<nl_fptype> m_h;
		state_var<nl_fptype> m_c;
		state_var<nl_fptype> m_v;
		nl_fptype m_gmin;
	};

	// "Circuit simulation", page 274
	template <>
	class generic_capacitor<capacitor_e::CONSTANT_CAPACITY>
	{
	public:
		generic_capacitor(device_t &dev, const pstring &name)
		: m_h(dev, name + ".m_h", nlconst::zero())
		, m_v(dev, name + ".m_v", nlconst::zero())
		, m_gmin(nlconst::zero())
		{
		}

		static capacitor_e type() noexcept { return capacitor_e::CONSTANT_CAPACITY; }
		nl_fptype G(nl_fptype cap) const noexcept { return cap * m_h +  m_gmin; }
		nl_fptype Ieq(nl_fptype cap, nl_fptype v) const noexcept
		{
			plib::unused_var(v);
			return - G(cap) * m_v;
		}

		void timestep(nl_fptype cap, nl_fptype v, nl_fptype step) noexcept
		{
			plib::unused_var(cap);
			m_h = plib::reciprocal(step);
			m_v = v;
		}
		void setparams(nl_fptype gmin) noexcept { m_gmin = gmin; }
	private:
		state_var<nl_fptype> m_h;
		state_var<nl_fptype> m_v;
		nl_fptype m_gmin;
	};

	// -----------------------------------------------------------------------------
	// A generic diode model to be used in other devices (Diode, BJT ...)
	// -----------------------------------------------------------------------------

	enum class diode_e
	{
		BIPOLAR,
		MOS
	};

	template <diode_e TYPE>
	class generic_diode
	{
	public:
		generic_diode(device_t &dev, const pstring &name)
		: m_Vd(dev, name + ".m_Vd", nlconst::magic(0.7))
		, m_Id(dev, name + ".m_Id", nlconst::zero())
		, m_G(dev,  name + ".m_G", nlconst::magic(1e-15))
		, m_Vt(nlconst::zero())
		, m_Vmin(nlconst::zero()) // not used in MOS model
		, m_Is(nlconst::zero())
		, m_logIs(nlconst::zero())
		, m_gmin(nlconst::magic(1e-15))
		, m_VtInv(nlconst::zero())
		, m_Vcrit(nlconst::zero())
		{
			set_param(
				nlconst::magic(1e-15)
			  , nlconst::magic(1)
			  , nlconst::magic(1e-15)
			  , nlconst::magic(300.0));
		}

		// Basic math
		//
		// I(V) = f(V)
		//
		// G(V) = df/dV(V)
		//
		// Ieq(V) = I(V) - V * G(V)
		//
		//
		void update_diode(nl_fptype nVd) noexcept
		{
			if (TYPE == diode_e::BIPOLAR)
			{
				//printf("%s: %g %g\n", m_name.c_str(), nVd, (nl_fptype) m_Vd);
				if (nVd > m_Vcrit)
				{
					// if the old voltage is less than zero and new is above
					// make sure we move enough so that matrix and current
					// changes.
					const nl_fptype old = std::max(nlconst::zero(), m_Vd());
					const nl_fptype d = std::min(+fp_constants<nl_fptype>::DIODE_MAXDIFF(), nVd - old);
					const nl_fptype a = plib::abs(d) * m_VtInv;
					m_Vd = old + nlconst::magic(d < 0 ? -1.0 : 1.0) * plib::log1p(a) * m_Vt;
				}
				else
					m_Vd = std::max(-fp_constants<nl_fptype>::DIODE_MAXDIFF(), nVd);

				if (m_Vd < m_Vmin)
				{
					m_G = m_gmin;
					m_Id = - m_Is;
				}
				else
				{
					const auto IseVDVt = plib::exp(m_logIs + m_Vd * m_VtInv);
					m_Id = IseVDVt - m_Is;
					m_G = IseVDVt * m_VtInv + m_gmin;
				}
			}
			else if (TYPE == diode_e::MOS)
			{
				m_Vd = nVd;
				if (nVd < nlconst::zero())
				{
					m_G = m_Is * m_VtInv + m_gmin;
					m_Id = m_G * m_Vd;
				}
				else // log stepping should already be done in mosfet
				{
					const auto IseVDVt = plib::exp(std::min(+fp_constants<nl_fptype>::DIODE_MAXVOLT(), m_logIs + m_Vd * m_VtInv));
					m_Id = IseVDVt - m_Is;
					m_G = IseVDVt * m_VtInv + m_gmin;
				}
			}
		}

		void set_param(nl_fptype Is, nl_fptype n, nl_fptype gmin, nl_fptype temp) noexcept
		{
			m_Is = Is;
			m_logIs = plib::log(Is);
			m_gmin = gmin;

			m_Vt = n * temp * nlconst::k_b() / nlconst::Q_e();

			m_Vmin = nlconst::magic(-5.0) * m_Vt;

			// Vcrit : f(V) has smallest radius of curvature rho(V) == min(rho(v))
			m_Vcrit = m_Vt * plib::log(m_Vt / m_Is / nlconst::sqrt2());
			m_VtInv = plib::reciprocal(m_Vt);
		}


		nl_fptype I() const noexcept { return m_Id; }
		nl_fptype G() const noexcept  { return m_G; }
		nl_fptype Ieq() const noexcept  { return (m_Id - m_Vd * m_G); }
		nl_fptype Vd() const noexcept  { return m_Vd; }

		// owning object must save those ...

	private:
		state_var<nl_fptype> m_Vd;
		state_var<nl_fptype> m_Id;
		state_var<nl_fptype> m_G;

		nl_fptype m_Vt;
		nl_fptype m_Vmin;
		nl_fptype m_Is;
		nl_fptype m_logIs;
		nl_fptype m_gmin;

		nl_fptype m_VtInv;
		nl_fptype m_Vcrit;
	};


} // namespace analog
} // namespace netlist

#endif // NLD_GENERIC_MODELS_H_
