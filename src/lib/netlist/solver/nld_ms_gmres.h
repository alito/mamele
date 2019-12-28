// license:GPL-2.0+
// copyright-holders:Couriersud

#ifndef NLD_MS_GMRES_H_
#define NLD_MS_GMRES_H_

///
/// \file nld_ms_gmres.h
///

#include "nld_ms_direct.h"
#include "nld_solver.h"
#include "plib/gmres.h"
#include "plib/mat_cr.h"
#include "plib/parray.h"
#include "plib/vector_ops.h"

#include <algorithm>

namespace netlist
{
namespace solver
{

	template <typename FT, int SIZE>
	class matrix_solver_GMRES_t: public matrix_solver_direct_t<FT, SIZE>
	{
	public:

		using float_type = FT;

		// Sort rows in ascending order. This should minimize fill-in and thus
		// maximize the efficiency of the incomplete LUT.
		// This is already preconditioning.

		matrix_solver_GMRES_t(netlist_state_t &anetlist, const pstring &name,
			analog_net_t::list_t &nets,
			const solver_parameters_t *params,
			const std::size_t size)
			: matrix_solver_direct_t<FT, SIZE>(anetlist, name, nets, params, size)
			, m_ops(size, 0)
			, m_gmres(size)
			{
			const std::size_t iN = this->size();

			std::vector<std::vector<unsigned>> fill(iN);

			for (std::size_t k=0; k<iN; k++)
			{
				fill[k].resize(iN, decltype(m_ops.m_mat)::FILL_INFINITY);
				terms_for_net_t & row = this->m_terms[k];
				for (const auto &nz_j : row.m_nz)
				{
					fill[k][static_cast<mattype>(nz_j)] = 0;
				}
			}

			m_ops.build(fill);
			this->log_fill(fill, m_ops.m_mat);

			// build pointers into the compressed row format matrix for each terminal

			for (std::size_t k=0; k<iN; k++)
			{
				std::size_t cnt = 0;
				for (std::size_t j=0; j< this->m_terms[k].railstart();j++)
				{
					for (std::size_t i = m_ops.m_mat.row_idx[k]; i<m_ops.m_mat.row_idx[k+1]; i++)
						if (this->m_terms[k].m_connected_net_idx[j] == static_cast<int>(m_ops.m_mat.col_idx[i]))
						{
							this->m_mat_ptr[k][j] = &m_ops.m_mat.A[i];
							cnt++;
							break;
						}
				}
				nl_assert(cnt == this->m_terms[k].railstart());
				this->m_mat_ptr[k][this->m_terms[k].railstart()] = &m_ops.m_mat.A[m_ops.m_mat.diag[k]];
			}
		}

		unsigned vsolve_non_dynamic(const bool newton_raphson) override;

	private:

		using mattype = typename plib::pmatrix_cr_t<FT, SIZE>::index_type;

		//plib::mat_precondition_none<FT, SIZE> m_ops;
		plib::mat_precondition_ILU<FT, SIZE> m_ops;
		//plib::mat_precondition_diag<FT, SIZE> m_ops;
		plib::gmres_t<FT, SIZE> m_gmres;
	};

	// ----------------------------------------------------------------------------------------
	// matrix_solver - GMRES
	// ----------------------------------------------------------------------------------------

	template <typename FT, int SIZE>
	unsigned matrix_solver_GMRES_t<FT, SIZE>::vsolve_non_dynamic(const bool newton_raphson)
	{
		const std::size_t iN = this->size();

		m_ops.m_mat.set_scalar(plib::constants<FT>::zero());

		// populate matrix and V for first estimate
		this->fill_matrix_and_rhs();

		for (std::size_t k = 0; k < iN; k++)
		{
			this->m_new_V[k] = this->m_terms[k].template getV<float_type>();
		}

		const auto accuracy(static_cast<float_type>(this->m_params.m_accuracy));

		auto iter = std::max(plib::constants<std::size_t>::one(), this->m_params.m_gs_loops());
		auto gsl = m_gmres.solve(m_ops, this->m_new_V, this->m_RHS, iter, accuracy);

		this->m_iterative_total += gsl;
		this->m_stat_calculations++;

		if (gsl > iter)
		{
			this->m_iterative_fail++;
			return matrix_solver_direct_t<FT, SIZE>::vsolve_non_dynamic(newton_raphson);
		}

		bool err(false);
		if (newton_raphson)
			err = this->check_err();
		this->store();
		return (err) ? 2 : 1;
	}




} // namespace solver
} // namespace netlist

#endif // NLD_MS_GMRES_H_
