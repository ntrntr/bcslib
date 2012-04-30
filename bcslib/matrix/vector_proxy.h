/**
 * @file vector_proxy.h
 *
 * A special class to support vector-based traversal and calculation
 *
 * @author Dahua Lin
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef BCSLIB_COLUMN_TRAVERSER_H_
#define BCSLIB_COLUMN_TRAVERSER_H_

#include <bcslib/matrix/dense_matrix.h>
#include <bcslib/matrix/bits/vecwise_internal.h>

namespace bcs
{

	template<class Expr>
	class vecwise_reader
	{
#ifdef BCS_USE_STATIC_ASSERT
		static_assert(bcs::has_matrix_interface<Expr, IMatrixXpr>::value,
				"Expr must be a model of IMatrixXpr");
#endif

		typedef typename
				select_type<
					has_matrix_interface<Expr, IMatrixView>::value,
					typename
					select_type<
						has_matrix_interface<Expr, IDenseMatrix>::value,
						detail::const_vecwise_dense_tag,
						detail::const_vecwise_view_tag>::type,
					detail::const_vecwise_obscure_tag>::type vec_kind_t;

		typedef detail::vecwise_reader_impl<Expr, vec_kind_t> internal_t;

	public:
		typedef typename matrix_traits<Expr>::value_type value_type;

		BCS_ENSURE_INLINE
		vecwise_reader(const Expr& mat) : m_internal(mat) { }

		BCS_ENSURE_INLINE value_type load_scalar(index_t i) const
		{
			return m_internal.load_scalar(i);
		}

		BCS_ENSURE_INLINE void operator ++ ()
		{
			m_internal.move_next();
		}

		BCS_ENSURE_INLINE void operator -- ()
		{
			m_internal.move_prev();
		}

		BCS_ENSURE_INLINE void operator += (index_t n)
		{
			m_internal.move(n);
		}

		BCS_ENSURE_INLINE void operator -= (index_t n)
		{
			m_internal.move(-n);
		}

	private:
		internal_t m_internal;
	};


	template<class Expr>
	class vecwise_writer
	{
#ifdef BCS_USE_STATIC_ASSERT
		static_assert(bcs::has_matrix_interface<Expr, IRegularMatrix>::value,
				"Expr must be a model of IMatrixXpr");

		static_assert(!(matrix_traits<Expr>::is_readonly), "Expr must NOT be read-only.");
#endif

		typedef typename
				select_type<
					has_matrix_interface<Expr, IDenseMatrix>::value,
						detail::vecwise_dense_tag,
						detail::vecwise_regular_tag>::type vec_kind_t;

		typedef detail::vecwise_writer_impl<Expr, vec_kind_t> internal_t;

	public:
		typedef typename matrix_traits<Expr>::value_type value_type;

		BCS_ENSURE_INLINE
		vecwise_writer(Expr& mat) : m_internal(mat) { }

		BCS_ENSURE_INLINE void store_scalar(index_t i, const value_type& v)
		{
			m_internal.store_scalar(i, v);
		}

		BCS_ENSURE_INLINE void operator ++ ()
		{
			m_internal.move_next();
		}

		BCS_ENSURE_INLINE void operator -- ()
		{
			m_internal.move_prev();
		}

		BCS_ENSURE_INLINE void operator += (index_t n)
		{
			m_internal.move(n);
		}

		BCS_ENSURE_INLINE void operator -= (index_t n)
		{
			m_internal.move(-n);
		}

	private:
		internal_t m_internal;
	};


	template<class SMat, class DMat>
	BCS_ENSURE_INLINE
	inline void copy_vec(const index_t len, vecwise_reader<SMat>& reader, vecwise_writer<DMat>& writer)
	{
		static_assert(bcs::is_same<
				typename matrix_traits<SMat>::value_type,
				typename matrix_traits<DMat>::value_type>::value,
				"SMat and DMat must have the same value type.");

		for (index_t i = 0; i < len; ++i)
		{
			writer.store_scalar(i, reader.load_scalar(i));
		}
	}

	template<class Reductor, class Mat>
	BCS_ENSURE_INLINE
	inline typename Reductor::accum_type accum_vec(Reductor reduc, const index_t len,
			vecwise_reader<Mat>& vec)
	{
		// pre-condition: len > 0

		typename Reductor::accum_type s = reduc(vec.load_scalar(0));
		for (index_t i = 1; i < len; ++i) s = reduc(s, vec.load_scalar(i));
		return s;
	}

	template<class Reductor, class LMat, class RMat>
	BCS_ENSURE_INLINE
	inline typename Reductor::accum_type accum_vec(Reductor reduc, const index_t len,
			vecwise_reader<LMat>& lvec, vecwise_reader<RMat>& rvec)
	{
		// pre-condition: len > 0

		typename Reductor::accum_type s = reduc(lvec.load_scalar(0), rvec.load_scalar(0));
		for (index_t i = 1; i < len; ++i) s = reduc(s, lvec.load_scalar(i), rvec.load_scalar(i));
		return s;
	}

}

#endif /* COLUMN_TRAVERSER_H_ */