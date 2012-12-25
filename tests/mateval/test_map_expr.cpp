/**
 * @file test_map_expr.cpp
 *
 * @brief Unit testing of the framework for map expression
 *
 * @author Dahua Lin
 */

#include "../test_base.h"

#define DEFAULT_M_VALUE 9
#define DEFAULT_N_VALUE 8

#include "../multimat_supp.h"

#include <light_mat/matrix/matrix_classes.h>
#include <light_mat/mateval/map_expr.h>

using namespace lmat;
using namespace lmat::test;

typedef atags::scalar scalar_tag;
typedef atags::simd<default_simd_kind> simd_tag;

template<typename STag1, typename DTag, int M, int N>
void test_mapexpr_1()
{
	index_t m = M == 0 ? DM : M;
	index_t n = N == 0 ? DN : N;

	typedef mat_host<STag1, double, M, N> shost1_t;
	typedef mat_host<DTag, double, M, N> dhost_t;

	typedef typename shost1_t::cmat_t smat1_t;
	typedef typename dhost_t::mat_t dmat_t;

	shost1_t s1_h(m, n);
	dhost_t d_h(m, n);

	s1_h.fill_rand();

	smat1_t s1 = s1_h.get_cmat();
	dmat_t d = d_h.get_mat();

	typedef map_expr<sqr_, smat1_t> expr_t;
	expr_t e = make_map_expr(sqr_(), s1);

	ASSERT_EQ( e.nrows(), m );
	ASSERT_EQ( e.ncolumns(), n );
	ASSERT_EQ( e.nelems(), m * n);

	d = e;

	dense_matrix<double> r(m, n);
	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
			r(i, j) = math::sqr(s1(i, j));
	}

	double tol = 1.0e-14;
	ASSERT_MAT_APPROX(m, n, d, r, tol);
}


template<typename STag1, typename STag2, typename DTag, int M, int N>
void test_mapexpr_2()
{
	index_t m = M == 0 ? DM : M;
	index_t n = N == 0 ? DN : N;

	typedef mat_host<STag1, double, M, N> shost1_t;
	typedef mat_host<STag2, double, M, N> shost2_t;
	typedef mat_host<DTag, double, M, N> dhost_t;

	typedef typename shost1_t::cmat_t smat1_t;
	typedef typename shost2_t::cmat_t smat2_t;
	typedef typename dhost_t::mat_t dmat_t;

	shost1_t s1_h(m, n);
	shost2_t s2_h(m, n);
	dhost_t d_h(m, n);

	s1_h.fill_rand();
	s2_h.fill_rand();

	smat1_t s1 = s1_h.get_cmat();
	smat2_t s2 = s2_h.get_cmat();
	dmat_t d = d_h.get_mat();

	typedef map_expr<sub_, smat1_t, smat2_t> expr_t;
	expr_t e = make_map_expr(sub_(), s1, s2);

	ASSERT_EQ( e.nrows(), m );
	ASSERT_EQ( e.ncolumns(), n );
	ASSERT_EQ( e.nelems(), m * n);

	d = e;

	dense_matrix<double> r(m, n);
	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
			r(i, j) = s1(i, j) - s2(i, j);
	}

	double tol = 1.0e-14;
	ASSERT_MAT_APPROX(m, n, d, r, tol);

	// other forms

	double cv = 2.5;

	d = make_map_expr_fix2(sub_(), s1, cv);
	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
			r(i, j) = s1(i, j) - cv;
	}
	ASSERT_MAT_APPROX(m, n, d, r, tol);

	d = make_map_expr_fix1(sub_(), cv, s2);
	for (index_t j = 0; j < n; ++j)
	{
		for (index_t i = 0; i < m; ++i)
			r(i, j) = cv - s2(i, j);
	}
	ASSERT_MAT_APPROX(m, n, d, r, tol);

}


// Unary expressions

#define DEF_MEXPR_TESTS_1( stag, dtag ) \
		MN_CASE( map_expr, unary_##stag##_##dtag ) { test_mapexpr_1<stag, dtag, M, N>(); } \
		BEGIN_TPACK( unary_map_expr_##stag##_##dtag ) \
		ADD_MN_CASE_3X3( map_expr, unary_##stag##_##dtag, DM, DN ) \
		END_TPACK

DEF_MEXPR_TESTS_1( cont, cont )
DEF_MEXPR_TESTS_1( cont, bloc )
DEF_MEXPR_TESTS_1( cont, grid )
DEF_MEXPR_TESTS_1( bloc, cont )
DEF_MEXPR_TESTS_1( bloc, bloc )
DEF_MEXPR_TESTS_1( bloc, grid )
DEF_MEXPR_TESTS_1( grid, cont )
DEF_MEXPR_TESTS_1( grid, bloc )
DEF_MEXPR_TESTS_1( grid, grid )

// Binary expression

#define DEF_MEXPR_TESTS_2( stag1, stag2, dtag ) \
		MN_CASE( map_expr, binary_##stag1##_##stag2##_##dtag ) { test_mapexpr_2<stag1, stag2, dtag, M, N>(); } \
		BEGIN_TPACK( binary_map_expr_##stag1##_##stag2##_##dtag ) \
		ADD_MN_CASE_3X3( map_expr, binary_##stag1##_##stag2##_##dtag, DM, DN ) \
		END_TPACK

DEF_MEXPR_TESTS_2( cont, cont, cont )
DEF_MEXPR_TESTS_2( cont, cont, bloc )
DEF_MEXPR_TESTS_2( cont, cont, grid )
DEF_MEXPR_TESTS_2( cont, bloc, cont )
DEF_MEXPR_TESTS_2( cont, bloc, bloc )
DEF_MEXPR_TESTS_2( cont, bloc, grid )
DEF_MEXPR_TESTS_2( cont, grid, cont )
DEF_MEXPR_TESTS_2( cont, grid, bloc )
DEF_MEXPR_TESTS_2( cont, grid, grid )

DEF_MEXPR_TESTS_2( bloc, cont, cont )
DEF_MEXPR_TESTS_2( bloc, cont, bloc )
DEF_MEXPR_TESTS_2( bloc, cont, grid )
DEF_MEXPR_TESTS_2( bloc, bloc, cont )
DEF_MEXPR_TESTS_2( bloc, bloc, bloc )
DEF_MEXPR_TESTS_2( bloc, bloc, grid )
DEF_MEXPR_TESTS_2( bloc, grid, cont )
DEF_MEXPR_TESTS_2( bloc, grid, bloc )
DEF_MEXPR_TESTS_2( bloc, grid, grid )

DEF_MEXPR_TESTS_2( grid, cont, cont )
DEF_MEXPR_TESTS_2( grid, cont, bloc )
DEF_MEXPR_TESTS_2( grid, cont, grid )
DEF_MEXPR_TESTS_2( grid, bloc, cont )
DEF_MEXPR_TESTS_2( grid, bloc, bloc )
DEF_MEXPR_TESTS_2( grid, bloc, grid )
DEF_MEXPR_TESTS_2( grid, grid, cont )
DEF_MEXPR_TESTS_2( grid, grid, bloc )
DEF_MEXPR_TESTS_2( grid, grid, grid )

BEGIN_MAIN_SUITE

	// unary

	ADD_TPACK( unary_map_expr_cont_cont )
	ADD_TPACK( unary_map_expr_cont_bloc )
	ADD_TPACK( unary_map_expr_cont_grid )
	ADD_TPACK( unary_map_expr_bloc_cont )
	ADD_TPACK( unary_map_expr_bloc_bloc )
	ADD_TPACK( unary_map_expr_bloc_grid )
	ADD_TPACK( unary_map_expr_grid_cont )
	ADD_TPACK( unary_map_expr_grid_bloc )
	ADD_TPACK( unary_map_expr_grid_grid )

	// binary

	ADD_TPACK( binary_map_expr_cont_cont_cont )
	ADD_TPACK( binary_map_expr_cont_cont_bloc )
	ADD_TPACK( binary_map_expr_cont_cont_grid )
	ADD_TPACK( binary_map_expr_cont_bloc_cont )
	ADD_TPACK( binary_map_expr_cont_bloc_bloc )
	ADD_TPACK( binary_map_expr_cont_bloc_grid )
	ADD_TPACK( binary_map_expr_cont_grid_cont )
	ADD_TPACK( binary_map_expr_cont_grid_bloc )
	ADD_TPACK( binary_map_expr_cont_grid_grid )

	ADD_TPACK( binary_map_expr_bloc_cont_cont )
	ADD_TPACK( binary_map_expr_bloc_cont_bloc )
	ADD_TPACK( binary_map_expr_bloc_cont_grid )
	ADD_TPACK( binary_map_expr_bloc_bloc_cont )
	ADD_TPACK( binary_map_expr_bloc_bloc_bloc )
	ADD_TPACK( binary_map_expr_bloc_bloc_grid )
	ADD_TPACK( binary_map_expr_bloc_grid_cont )
	ADD_TPACK( binary_map_expr_bloc_grid_bloc )
	ADD_TPACK( binary_map_expr_bloc_grid_grid )

	ADD_TPACK( binary_map_expr_grid_cont_cont )
	ADD_TPACK( binary_map_expr_grid_cont_bloc )
	ADD_TPACK( binary_map_expr_grid_cont_grid )
	ADD_TPACK( binary_map_expr_grid_bloc_cont )
	ADD_TPACK( binary_map_expr_grid_bloc_bloc )
	ADD_TPACK( binary_map_expr_grid_bloc_grid )
	ADD_TPACK( binary_map_expr_grid_grid_cont )
	ADD_TPACK( binary_map_expr_grid_grid_bloc )
	ADD_TPACK( binary_map_expr_grid_grid_grid )
END_MAIN_SUITE
