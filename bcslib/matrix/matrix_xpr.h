/**
 * @file matrix_xpr.h
 *
 * The basis for matrix expressions
 *
 * @author Dahua Lin
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef BCSLIB_MATRIX_XPR_H_
#define BCSLIB_MATRIX_XPR_H_

#include <bcslib/matrix/matrix_base.h>

namespace bcs
{

	/******************************************************
	 *
	 *  The concept of expression optimizer
	 *
	 *  - typedef result_expr_type;
	 *  - typedef return_type;
	 *  - a static optimize function;
	 *
	 * The concept of expression evaluator
	 *
	 * - the static evaluate function:
	 *
	 * 		evaluate(optimized_expr, dst);
	 *
	 ******************************************************/

	template<class Expr> struct expr_optimizer;

	template<class Expr> struct expr_evaluator;

	template<class Expr>
	BCS_ENSURE_INLINE
	typename expr_optimizer<Expr>::return_type
	optimize_expr(const IMatrixXpr<Expr, typename matrix_traits<Expr>::value_type>& expr)
	{
		return expr_optimizer<Expr>::optimize(expr);
	}


	template<typename T, class Expr, class DMat>
	BCS_ENSURE_INLINE
	void evaluate_to(const IMatrixXpr<Expr, T>& expr, IRegularMatrix<DMat, T>& dst)
	{
		typedef typename expr_optimizer<Expr>::result_expr_type optim_expr_type;
		typedef expr_evaluator<optim_expr_type> evaluator_t;

		evaluator_t::evaluate(optimize_expr(expr), dst.derived());
	}


	// forward declaration of generic expression types

	template<typename Fun, class Arg> struct unary_ewise_expr;
	template<typename Fun, class LArg, class RArg> struct binary_ewise_expr;

	template<typename Fun, class Arg> struct unary_colwise_redux_expr;
	template<typename Fun, class LArg, class RArg> struct binary_colwise_redux_expr;

	template<typename Fun, class Arg> struct unary_rowwise_redux_expr;
	template<typename Fun, class LArg, class RArg> struct binary_rowwise_redux_expr;


}

#endif
