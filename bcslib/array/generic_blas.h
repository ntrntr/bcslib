/**
 * @file array_blas.h
 *
 * Generic BLAS functions on 1D and 2D arrays
 * 
 * @author Dahua Lin
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef BCSLIB_ARRAY_BLAS_H_
#define BCSLIB_ARRAY_BLAS_H_

#include <bcslib/array/array1d.h>
#include <bcslib/array/array2d.h>
#include <bcslib/array/blas_base.h>

#include <type_traits>

namespace bcs
{
	namespace blas
	{

		/**************************************************
		 *
		 *  BLAS Level 1
		 *
		 **************************************************/

		// asum

		template<class Arr>
		inline typename std::enable_if<is_array_view<Arr>::value,
		typename array_view_traits<Arr>::value_type>::type
		asum(const Arr& x)
		{
			typedef typename array_view_traits<Arr>::value_type value_type;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			scoped_aview_read_proxy<Arr> xp(x);
			return _asum(make_cvec((int)get_num_elems(x), xp.pbase()));
		}

		// axpy

		template<class ArrX, class ArrY>
		inline typename std::enable_if<is_compatible_aviews<ArrX, ArrY>::value,
		void>::type
		axpy(const ArrX& x, ArrY& y, const typename array_view_traits<ArrY>::value_type& alpha)
		{
			typedef typename array_view_traits<ArrY>::value_type value_type;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			check_arg(is_dense_view(y), "blas::axpy: y must be a dense view.");

			scoped_aview_read_proxy<ArrX> xp(x);
			return _axpy(
					make_cvec((int)get_num_elems(x), xp.pbase()),
					make_vec((int)get_num_elems(y), ptr_base(y)), alpha);
		}

		// dot

		template<class ArrX, class ArrY>
		inline typename std::enable_if<is_compatible_aviews<ArrX, ArrY>::value,
		typename array_view_traits<ArrX>::value_type>::type
		dot(const ArrX& x, const ArrY& y)
		{
			typedef typename array_view_traits<ArrX>::value_type value_type;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			scoped_aview_read_proxy<ArrX> xp(x);
			scoped_aview_read_proxy<ArrY> yp(y);

			return _dot(
					make_cvec((int)get_num_elems(x), xp.pbase()),
					make_cvec((int)get_num_elems(y), yp.pbase()));
		}

		// nrm2

		template<class Arr>
		inline typename std::enable_if<is_array_view<Arr>::value,
		typename array_view_traits<Arr>::value_type>::type
		nrm2(const Arr& x)
		{
			typedef typename array_view_traits<Arr>::value_type value_type;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			scoped_aview_read_proxy<Arr> xp(x);
			return _nrm2(make_cvec((int)get_num_elems(x), xp.pbase()));
		}

		// rot

		template<class ArrX, class ArrY>
		inline typename std::enable_if<is_compatible_aviews<ArrX, ArrY>::value,
		void>::type
		rot(ArrX& x, ArrY& y,
				const typename array_view_traits<ArrY>::value_type& c,
				const typename array_view_traits<ArrY>::value_type& s)
		{
			typedef typename array_view_traits<ArrX>::value_type value_type;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			check_arg(is_dense_view(x), "blas::rot: x must be a dense view.");
			check_arg(is_dense_view(y), "blas::rot: y must be a dense view.");

			return _rot(
					make_vec((int)get_num_elems(x), ptr_base(x)),
					make_vec((int)get_num_elems(y), ptr_base(y)), c, s);
		}



		/**************************************************
		 *
		 *  BLAS Level 2
		 *
		 **************************************************/

		template<class ArrA, class ArrX, class ArrY>
		struct is_mv_compatible
		{
			typedef typename array_view_traits<ArrA>::value_type vtype_a;
			typedef typename array_view_traits<ArrX>::value_type vtype_x;
			typedef typename array_view_traits<ArrY>::value_type vtype_y;

			static const bool value =
					is_array_view_ndim<ArrA, 2>::value &&
					is_array_view_ndim<ArrX, 1>::value &&
					is_array_view_ndim<ArrY, 1>::value &&
					std::is_same<vtype_a, vtype_x>::value &&
					std::is_same<vtype_x, vtype_y>::value;
		};

		// gemv

		template<class ArrA, class ArrX, class ArrY>
		inline typename std::enable_if<is_mv_compatible<ArrA, ArrX, ArrY>::value,
		void>::type
		gemv(const ArrA& a, const ArrX& x, ArrY& y, char trans = 'N',
				const typename array_view_traits<ArrA>::value_type& alpha = 1,
				const typename array_view_traits<ArrA>::value_type& beta = 0)
		{
			typedef typename array_view_traits<ArrA>::value_type value_type;
			typedef typename array_view_traits<ArrA>::layout_order layout_order;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			check_arg(is_dense_view(y), "blas::gemv: y must be a dense view.");

			scoped_aview_read_proxy<ArrA> ap(a);
			scoped_aview_read_proxy<ArrX> xp(x);

			auto shape = get_array_shape(a);
			int m = (int)shape[0];
			int n = (int)shape[1];

			_gemv(
					make_cmat(m, n, ap.pbase(), trans, layout_order()), // a
					make_cvec((int)get_num_elems(x), xp.pbase()),		// x
					make_vec((int)get_num_elems(y), ptr_base(y)), 		// y
					alpha, beta ); 										// alpha, beta
		}

		// ger

		template<class ArrA, class ArrX, class ArrY>
		inline typename std::enable_if<is_mv_compatible<ArrA, ArrX, ArrY>::value,
		void>::type
		ger(ArrA& a, const ArrX& x, const ArrY& y, const typename array_view_traits<ArrA>::value_type& alpha = 1)
		{
			typedef typename array_view_traits<ArrA>::value_type value_type;
			typedef typename array_view_traits<ArrA>::layout_order layout_order;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			check_arg(is_dense_view(a), "blas::ger: a must be a dense view.");

			scoped_aview_read_proxy<ArrX> xp(x);
			scoped_aview_read_proxy<ArrY> yp(y);

			auto shape = get_array_shape(a);
			int m = (int)shape[0];
			int n = (int)shape[1];

			_ger(
					make_mat(m, n, ptr_base(a), layout_order()), 	// a
					make_cvec((int)get_num_elems(x), xp.pbase()), 	// x
					make_cvec((int)get_num_elems(y), yp.pbase()), 	// y
					alpha);											// alpha
		}

		// symv

		template<class ArrA, class ArrX, class ArrY>
		inline typename std::enable_if<is_mv_compatible<ArrA, ArrX, ArrY>::value,
		void>::type
		symv(const ArrA& a, const ArrX& x, ArrY& y,
				const typename array_view_traits<ArrA>::value_type& alpha = 1,
				const typename array_view_traits<ArrA>::value_type& beta = 0)
		{
			typedef typename array_view_traits<ArrA>::value_type value_type;
			typedef typename array_view_traits<ArrA>::layout_order layout_order;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			check_arg(is_dense_view(y), "blas::symv: y must be a dense view.");

			scoped_aview_read_proxy<ArrA> ap(a);
			scoped_aview_read_proxy<ArrX> xp(x);

			auto shape = get_array_shape(a);
			int m = (int)shape[0];
			int n = (int)shape[1];

			_symv(
					make_cmat(m, n, ap.pbase(), 'N', layout_order()), 	// a
					make_cvec((int)get_num_elems(x), xp.pbase()),		// x
					make_vec((int)get_num_elems(y), ptr_base(y)), 		// y
					alpha, beta ); 										// alpha, beta
		}



		/**************************************************
		 *
		 *  BLAS Level 3
		 *
		 **************************************************/

		template<class ArrA, class ArrB, class ArrC>
		struct is_mm_compatible
		{
			typedef typename array_view_traits<ArrA>::value_type vtype_a;
			typedef typename array_view_traits<ArrB>::value_type vtype_b;
			typedef typename array_view_traits<ArrC>::value_type vtype_c;

			typedef typename array_view_traits<ArrA>::layout_order lord_a;
			typedef typename array_view_traits<ArrB>::layout_order lord_b;
			typedef typename array_view_traits<ArrC>::layout_order lord_c;

			static const bool value =
					is_array_view_ndim<ArrA, 2>::value &&
					is_array_view_ndim<ArrB, 2>::value &&
					is_array_view_ndim<ArrC, 2>::value &&
					std::is_same<vtype_a, vtype_b>::value && std::is_same<vtype_b, vtype_c>::value &&
					std::is_same<lord_a, lord_b>::value && std::is_same<lord_b, lord_c>::value;
		};


		// gemm

		template<class ArrA, class ArrB, class ArrC>
		inline typename std::enable_if<is_mm_compatible<ArrA, ArrB, ArrC>::value,
		void>::type
		gemm(const ArrA& a, const ArrB& b, ArrC& c, char transa = 'N', char transb = 'N',
				const typename array_view_traits<ArrA>::value_type& alpha = 1,
				const typename array_view_traits<ArrA>::value_type& beta = 0)
		{
			typedef typename array_view_traits<ArrA>::value_type value_type;
			typedef typename array_view_traits<ArrA>::layout_order layout_order;
			BCS_STATIC_ASSERT_V(std::is_floating_point<value_type>);

			check_arg(is_dense_view(c), "blas::gemm: c must be a dense view.");

			auto shape_a = get_array_shape(a);
			auto shape_b = get_array_shape(b);
			auto shape_c = get_array_shape(c);

			int ma = (int)shape_a[0]; int na = (int)shape_a[1];
			int mb = (int)shape_b[0]; int nb = (int)shape_b[1];
			int mc = (int)shape_c[0]; int nc = (int)shape_c[1];

			scoped_aview_read_proxy<ArrA> ap(a);
			scoped_aview_read_proxy<ArrB> bp(b);

			_gemm(
					make_cmat(ma, na, ap.pbase(), transa, layout_order()), 	// a
					make_cmat(mb, nb, bp.pbase(), transb, layout_order()), 	// b
					make_mat(mc, nc, ptr_base(c), layout_order()), 			// c
					alpha, beta);											// alpha, beta

		}

	}

}

#endif 
