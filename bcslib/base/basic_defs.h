/**
 * @file basic_defs.h
 *
 * Library-wide basic definitions
 *
 * @author dhlin
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef BCSLIB_BASIC_DEFS_H
#define BCSLIB_BASIC_DEFS_H

#include <bcslib/base/config.h>

#include <cstddef>
#include <cstdint>
#include <utility>


// This is for temporary use,
// and will be replaced with nullptr when it is available in most compilers

#if ( (BCSLIB_COMPILER == BCSLIB_MSVC) || (BCSLIB_COMPILER == BCSLIB_GCC && __GNUC_MINOR__ >= 6) )
#define BCS_NULL nullptr
#else
#define BCS_NULL 0L
#endif

#define BCS_STATIC_ASSERT(cond) static_assert(cond, #cond)
#define BCS_STATIC_ASSERT_V(cond) static_assert(cond::value, #cond)

namespace bcs
{

	using std::int8_t;
	using std::int16_t;
	using std::int32_t;
	using std::int64_t;

	using std::uint8_t;
	using std::uint16_t;
	using std::uint32_t;
	using std::uint64_t;

	using std::ptrdiff_t;
	using std::size_t;

	using std::intptr_t;
	using std::uintptr_t;

	typedef int32_t index_t;

	using std::pair;
	using std::make_pair;

	template<typename T1, typename T2>
	inline pair<T1&, T2&> tie_pair(T1& t1, T2& t2)
	{
		return pair<T1&, T2&>(t1, t2);
	}


	/**
	 * The base class to make sure its derived classes are non-copyable
	 */
	class noncopyable
	{
	protected:
		noncopyable() { }
		~noncopyable() { }

	private:
		noncopyable(const noncopyable& );
		noncopyable& operator= (const noncopyable& );
	};


	/**
	 * A compile time size constant
	 */
	template<size_t N>
	struct size_constant
	{
		static const size_t value = N;
	};

}

#endif



