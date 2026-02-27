#ifndef INC_INTERVALPRECISION
#define INC_INTERVALPRECISION
#pragma once

/*
 *******************************************************************************
 *
 *
 *                       Copyright (c) 2002-2024
 *                       Henrik Vestermark
 *                       Denmark, USA
 *
 *                       All Rights Reserved
 *
 *   This source file is subject to the terms and conditions of the
 *   Henrik Vestermark Software License Agreement which restricts the manner
 *   in which it may be used.
 *   Mail: hve@hvks.com
 *
 *******************************************************************************
 */

/*
 *******************************************************************************
 *
 *
 * Module name     : intervaldouble.h
 * Module ID Nbr   :
 * Description     : Interval arithmetic template class
 *                   Works with both float and double and doesnt require any
 *					 special floating point control as the previous version did.
 *					 use software emulation of rounding control via the twosum and
 *					 twoproduct
 * --------------------------------------------------------------------------
 * Change Record   :
 *
 * Version	Author/Date		Description of changes
 * -------  --------------	----------------------
 * 01.01	HVE/020209		Initial release
 * 01.02    HVE/030421		Optimized the * operator to reduce the 8 multiplications to 2.
 * 01.03	HVE/JUN-26-2014	Added is_empty(), contains_zero() method to the class
 * 01.04	HVE/JUN-27-2014	Corrected several errors in in cin >> template function
 * 01.05	HVE/JUN-28-2014	Added is_class() method for getting the interval classification
 *							and width() method for the interval width
 * 01.06	HVE/JUN-30-2014	An error was corrected for interval subtraction of float_preicsion numbers
 *							Also added the method bool contain() for test if a float or
 * interval is included in the interval 01.07	HVE/JUL-6-2014	Corrected an error in /= for the software emulation of
 * of float & double 01.08	HVE/JUL-13-2014	Added Hardware support for interval arithmetic when applicable. Also fix
 * several errors in the implementation of sqrt, log, log10, exp and pow functions. Also added new method is_class(),
 * is_empty() 01.09	HVE/JUL-15-2014	Added support for Sin(), Cos() and Tan() interval trigonometric functions. 01.10
 *	HVE/JUL-17-2014	Added support for atan() interval trigonometric function 01.11	HVE/JUL-22-2014 Found a bug that
 * floating point was not reset to near (default by IEEE754) after a hardware supported multiplication 01.12
 *	HVE/JUL-22-2014	Added support for asin() interval trigonometric function 01.13	HVE/JUL-29-2014	Added support
 * for interval versions of LN2, LN10 and PI 01.14	HVE/AUG-10-2014	Added support for mixed mode arithmetic for
 * interval +,- classes 01.15	HVE/JUN-20-2015	Fixed and un-declare variable x when compiling with no interval hardware
 * support 01.16	HVE/Jul-07-2019	Moved Hardware support up prior to the template class definition to make the
 * code more portable and added <iostream> header 01.17    HVE/Jul-07-2019 Make the code more portable to a GCC
 * environment 01.18	HVE/24-Mar-2021 Updated license info 01.19	HVE/4-Jul-2021	Added software interval runding
 * via towsum and twoproduct functions and other functions 01.20	HVE/5-Jul-2021	Replace deprecreated headers
 * with current headers 01.21	HVE/15-Jul-2021 Decpreated hardware support for interval arithmetic since this was not
 * portable and didnt takes advantages of the latest Intel instructions set. instead if used only software emaulation of
 * intervals. 01.22	HVE/29-Jul-2021 Corrected bugs in all the trigonometir functions and added interval version of
 * hyperbolic functions sinh(), cosh(), tanh(), asinh(), acosh(), atanh(). 01.23	HVE/30-Jul-2021 Added
 * intervalsection(), unionsection(), boolean precedes(), interior() 02.01	HVE/19-FEB-2024	Rewritten and optimized
 * 02.02	HVE/28-Mar-2024	optimized for float_precision types
 * 02.03	HVE/17-Apr-2024	Method isEntire and function entire(), empty() are added,
 * 02.04	HVE/22-Apr-2024	Added the decoration attribute
 * 02.05	HVE/20-Jul-2024	Fix a bug in sqrt(x) when the x interval contains negative numbers
 * 02.06	HVE/24-Jul-2024	Fix a bug in comparison < and > operator
 *
 * End of Change Record
 * --------------------------------------------------------------------------
 */

/* define version string */

static char _VinterP_[] = "@(#)intervalprecision.h 02.06 -- Copyright (C) Henrik Vestermark";

#include <algorithm>
#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

static_assert(__cplusplus >= 201703L, "The intervalprecision.h code requires c++17 or higher.");

namespace intprec {
// #define PHASE4	// Add support for float_precision
// #define PHASE5	// use simplify interval operations by always convert interval to its closed form and then
// perform the operation and leave the interval closed

// Interval class
// Realistically the class Type can be float, double. Any other type is not supported

// Some needed forward declaration.
template <FloatLike IT>
inline IT infinity_interval();
// Specialization for the minimum value for float, double, ong double, mpfr::mpreal
template <>
inline float infinity_interval<float>() {
    return std::numeric_limits<float>::infinity();
}
template <>
inline double infinity_interval<double>() {
    return std::numeric_limits<double>::infinity();
}
template <>
inline long double infinity_interval<long double>() {
    return std::numeric_limits<long double>::infinity();
}
#ifdef __MPREAL_H__
template <>
inline mpfr::mpreal infinity_interval<mpfr::mpreal>() {
    return mpfr::const_infinity();
}
#endif
#ifdef SHANKS_ENABLE_PROFILING
template <>
inline shanks::profiling::OperationCounting<float> infinity_interval<shanks::profiling::OperationCounting<float>>() {
    return shanks::profiling::OperationCounting<float>(infinity_interval<float>());
}
template <>
inline shanks::profiling::OperationCounting<double> infinity_interval<shanks::profiling::OperationCounting<double>>() {
    return shanks::profiling::OperationCounting<double>(infinity_interval<double>());
}
template <>
inline shanks::profiling::OperationCounting<long double>
infinity_interval<shanks::profiling::OperationCounting<long double>>() {
    return shanks::profiling::OperationCounting<long double>(infinity_interval<long double>());
}
#ifdef __MPREAL_H__
template <>
inline shanks::profiling::OperationCounting<mpfr::mpreal>
infinity_interval<shanks::profiling::OperationCounting<mpfr::mpreal>>() {
    return shanks::profiling::OperationCounting<mpfr::mpreal>(infinity_interval<mpfr::mpreal>());
}
#endif
#endif

// Underflow declaration for the various types
template <FloatLike T>
inline T underflow_interval();
// Specialization for the minimum value for float, double
template <>
inline float underflow_interval<float>() {
    return FLT_MIN;
}
template <>
inline double underflow_interval<double>() {
    return DBL_MIN;
}
template <>
inline long double underflow_interval<long double>() {
    return LDBL_MIN;
}
#ifdef __MPREAL_H__
template <>
inline mpfr::mpreal underflow_interval<mpfr::mpreal>() {
    return mpfr::minval();
}
#endif
#ifdef SHANKS_ENABLE_PROFILING
template <>
inline shanks::profiling::OperationCounting<float> underflow_interval<shanks::profiling::OperationCounting<float>>() {
    return shanks::profiling::OperationCounting<float>(underflow_interval<float>());
}
template <>
inline shanks::profiling::OperationCounting<double> underflow_interval<shanks::profiling::OperationCounting<double>>() {
    return shanks::profiling::OperationCounting<double>(underflow_interval<double>());
}
template <>
inline shanks::profiling::OperationCounting<long double>
underflow_interval<shanks::profiling::OperationCounting<long double>>() {
    return shanks::profiling::OperationCounting<long double>(underflow_interval<long double>());
}
#ifdef __MPREAL_H__
template <>
inline shanks::profiling::OperationCounting<mpfr::mpreal>
underflow_interval<shanks::profiling::OperationCounting<mpfr::mpreal>>() {
    return shanks::profiling::OperationCounting<mpfr::mpreal>(underflow_interval<mpfr::mpreal>());
}
#endif
#endif

//	Implement the twosum algorithm.
//  Assuming round to nearest mode (default IEEE754)
//	sum=(a+b)
//	a1=sum-b
//	b1=sum-a
//	da=a-a1
//	db=b-b1
//	err=da+db
//	return sum, err
//	if err is negative then sum=Round_up(a+b), and round_down(a+b)=previous(sum)
//	if err is positive then sum=Round_down(a+b), and Round_up(a+b)=succesor(sum)
//	The twosum algorithm requires 6 floating point operations
template <FloatLike IT>
std::pair<IT, IT> interval<IT>::two_sum(const IT& a, const IT& b) {
    const IT sum(a + b);
    const IT a1(sum - b);
    const IT b1(sum - a);
    const IT da(a - a1);
    const IT db(b - b1);
    const IT err(da + db);
    return std::make_pair(sum, err);
}

// Split argument into a right and left. (Dekker's method)
// double has 53bits in mantissa and shifting is therefore (53+1)/2=27
// float has 24bits in mantissa and shifting is therefore (24+1)/2=12
template <FloatLike IT>
IT interval<IT>::split(const IT& a) {
    const IT C(a * double((1 << 27) + 1));
    IT xright(C - (C - a));
    IT xleft(a - xright);
    return std::make_pair(xright, xleft);
}

// The standard two product algorithm  (by RUMP's method)
// (xh,xl)=split(a)
// (yh,yl)=split(b)
// p=a*b
// t1=-p+xh*yh
// t2=t1+xh*yl
// t3=t2+xl*yh
// err=t3+xl*yl
// return (p,err)
//	if err is negative then sum=Round_up(a*b), and round_down(a*b)=previous(sum)
//	if err is positive then sum=Round_down(a*b), and Round_up(a*b)=successor(sum)
// The fast two product algorithm requires 17 floating point instruction and a comparison
template <FloatLike IT>
std::pair<IT, IT> interval<IT>::two_prod(const IT& a, const IT& b) {
    const std::pair<IT, IT> x(split(a));
    const std::pair<IT, IT> y(split(b));
    const IT p(a * b);
    // check for an overflow condition
    const IT t1(utils::math<IT>::abs(p) > ldexp(1, -1021) ? (-p * 0.5 + (x.first * 0.5) * y.first) * 2.0
                                                          : -p + x.first * y.first);
    const IT t2(t1 + x.first * y.second);
    const IT t3(t2 + x.second * y.first);
    const IT err(t3 + x.second * y.second);
    //_IT errold;
    // if (abs(p) > ldexp(1, -1021))  // avoiding overflow
    //	err = x.second * y.second - ((((p * 0.5 - (x.first * 0.5) * y.first) * 2) - x.second * y.first) - x.first *
    // y.second); else 	err = x.second * y.second - (((p - x.first * y.first) - x.second * y.first) - x.first *
    // y.second);
    return std::make_pair(p, err);
}
//	Implement the fast twosum algorithm
//  Assuming round to nearest mode (default IEEE754)
//	sum=(a+b)
//	a1=sum-a
//	err=b-a1
//	return sum, err
//	if err is negative then sum=Round_up(a+b), and round_down(a+b)=previous(sum)
//	if err is positive then sum=Round_down(a+b), and Round_up(a+b)=successor(sum)
// The fast two sum algorithm requires only 3 floating point instructions and a comparison
// Note if an overflow occurs the sum will be +infinity and the error is set to 0.
template <FloatLike IT>
std::pair<IT, IT> interval<IT>::fasttwo_sum(const IT& a, const IT& b) {
    const IT sum(a + b);
    if (utils::math<IT>::abs(a) > utils::math<IT>::abs(b)) {
        IT tmp(sum - a);
        IT err(b - tmp);
        if (sum == infinity_interval<IT>())  // If overflow occurs then set err=0
            err = IT(0);
        return std::make_pair(sum, err);
    } else {
        IT tmp(sum - b);
        IT err(a - tmp);
        if (sum == infinity_interval<IT>())  // If overflow occurs then set err=0
            err = IT(0);
        return std::make_pair(sum, err);
    }
}
//	The fast two product algorithm
//	p=a*b
//	err=fma(a,b,-p)	// fma is required in IEEE754 and either implemented in hardware or software
//	return (p,err)
//	if err is negative then sum=Round_up(a*b), and round_down(a*b)=previous(sum)
//	if err is positive then sum=Round_down(a*b), and Round_up(a*b)=succesor(sum)
// The fast two product algorithm requires only 2 floating point instructions and a comparison
// Note if an overflow occurs the product will be +infinity and the error is set to 0.
template <FloatLike IT>
std::pair<IT, IT> interval<IT>::fasttwo_prod(const IT& a, const IT& b) {
    const IT p(a * b);
    IT err(utils::math<IT>::fma(a, b, -p));
    if (p == infinity_interval<IT>())  // If overflow occurs then set err=0
        err = IT(0);
    if (p != IT(0) && utils::math<IT>::abs(p) < underflow_interval<IT>())  // Test for underflow conditions
    {  // Recalculate the err using the scale products
        int aexp, bexp;
        IT ascale(utils::helpers<IT>::frexp(a, &aexp));  // ascale [1.0,2.0)
        IT bscale(utils::helpers<IT>::frexp(b, &bexp));  // bscale [1.0,2.0)
        IT p2(ascale * bscale);
        err = utils::math<IT>::fma(ascale, bscale, -p2);
    }
    return std::make_pair(p, err);
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// BEGIN Constructors
///
//////////////////////////////////////////////////////////////////////////////////////

// Construct an empty interval
template <FloatLike IT>
inline interval<IT>::interval() : left(IT(0)), right(IT(0)), type(EMPTY), decoration(TRV) {}  // Set EMPTY interval type

// Construct a singleton interval
// Since IT is either float or double and the argument is of the same type
// we can't catch any conversion error for up or down conversion of the argument
template <FloatLike IT>
inline interval<IT>::interval(const IT& d)
    : left(d), right(d), type(CLOSE), decoration(COM) {}  // Default is closed type

// Construct a regular interval
// Since IT is either float or double and the argument is of the same type
// we can't catch any conversion error for up or down conversion of the argument
template <FloatLike IT>
inline interval<IT>::interval(const IT& l, const IT& h, const enum interval_type t)
    : left(l), right(h), type(t), decoration(COM) {}

// Constructor for creating mixed-type intervals when IT != _X (base types), enabling automatic
// construction of intervals from different types (e.g., interval<double> x = float).
// For initializations with integral types, check the value against the max without loss of precision,
// adjust the interval to ensure it fits within the bounds of float or double values.
// If float or double limits are exceeded, sets the left interval to the correct lower bound,
// while the right interval is adjusted accordingly.
// Note: This template constructor is preferred over the simpler constructor
template <FloatLike IT>
template <typename _X>
inline interval<IT>::interval(const _X& x) {
    const bool isIntegral = std::is_integral<_X>::value;
    const bool isTargetFloat = std::is_same<IT, float>::value;
    const bool isSourceDoubleOrLongDouble = std::is_same<_X, double>::value || std::is_same<_X, long double>::value;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);

    // up promoting is accurate
    left = IT(x);
    right = IT(x);
    if (isTargetFloat && isSourceDoubleOrLongDouble) {  // Downpromoting from double to float.
        // Uppromotion from float to double is always accurate
        auto adjustBoundaries = [&](const _X& val) {
            _X e = val - _X(left);
            if (e < _X(0)) left = utils::helpers<IT>::nextafter(left, -infi);
            e = val - _X(right);
            if (e > _X(0)) right = utils::helpers<IT>::nextafter(right, infi);
        };
        adjustBoundaries(x);
    }
    if (isIntegral) {  // Handle integer promotion to IT
        const intmax_t absX = intmax_t(utils::math<IT>::abs(x));
        auto maxFloat = 16'777'216;              // 2^24
        auto maxDouble = 9'007'199'254'740'992;  // 2^53
        bool exceedsFloat = isTargetFloat && absX > maxFloat;
        bool exceedsDouble = !isTargetFloat && absX > maxDouble;

        if (exceedsFloat || exceedsDouble) {
            _X e = x - _X(left);
            if (e > _X(0)) right = utils::helpers<IT>::nextafter(right, infi);
            if (e < _X(0)) left = utils::helpers<IT>::nextafter(left, -infi);
        }
    }
    type = CLOSE;
    decoration = this->intervaldecoration(COMPUTE);
}

// Constructor for creating mixed type intervals (IT != _X, base types) with two or three arguments,
// facilitating automatic construction of intervals from different types (e.g., interval<double> x = {float, float}).
// For initializations with integral types, it verifies the value against the maximum that can be handled
// without loss of precision, adjusting the interval to fit within the bounds of float or double values.
// Should the float or double limits be exceeded, the left interval is set to the correct lower bound,
// while the right interval is adjusted accordingly.
// This template function is preferred over the simpler constructor from phase1, except when the argument
// type matches the interval class type (IT).
// To simplify implementation, the single argument constructor is called twice. Then, the minimum of the
// two left intervals and the maximum of the two right intervals are determined to establish the final interval bounds.
template <FloatLike IT>
template <typename _X, typename _Y>
inline interval<IT>::interval(const _X& x, const _Y& y, const enum interval_type t) {
    const interval<IT> ll(x);  // Call mixed singleton constructor
    const interval<IT> rr(y);  // Call mixed singleton constructor

    left = std::min(ll.inf(), rr.inf());
    right = std::max(ll.sup(), rr.sup());
    type = t;
    decoration = COM;
    // if x>y originally was improper then return it as an improper interval
    if (IT(x) > IT(y)) std::swap(left, right);
    // Recalculate the decoration type to match the interval
    this->intervaldecoration(COMPUTE);
    return;
}

// constructor for any other interval<_X> to interval<IT>.
// e.g. interval<float> i1(2,3);
// interval<float> i2(i1);
template <FloatLike IT>
template <typename _X>
inline interval<IT>::interval(const interval<_X>& a)  //
{
    // Call two argument mixed constructor
    const interval<IT> x(a.leftinterval(), a.rightinterval(), a.intervaltype());
    left = x.left;
    right = x.right;
    type = x.type;
    decoration = x.decoration;
}

//////////////////////////////////////////////////////////////////////////////////////
//
// END Constructors
//
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
//
// BEGIN Conversion operators
//
//////////////////////////////////////////////////////////////////////////////////////

// Conversion Operators
template <FloatLike IT>
inline interval<IT>::operator short() const {  // Conversion to short
    return static_cast<short>(mid());
}
template <FloatLike IT>
inline interval<IT>::operator int() const {  // Conversion to int
    return static_cast<int>(mid());
}

template <FloatLike IT>
inline interval<IT>::operator long() const {  // Conversion to long
    return static_cast<long>(mid());
}
template <FloatLike IT>
inline interval<IT>::operator long long() const {  // Conversion to long long
    return static_cast<long long>(mid());
}

template <FloatLike IT>
inline interval<IT>::operator unsigned short() const {  // Conversion to unsigned short
    return static_cast<unsigned short>(mid());
}
template <FloatLike IT>
inline interval<IT>::operator unsigned int() const {  // Conversion to unsgined int
    return static_cast<unsigned int>(mid());
}

template <FloatLike IT>
inline interval<IT>::operator unsigned long() const {  // Conversion to unsigned long
    return static_cast<unsigned long>(mid());
}

template <FloatLike IT>
inline interval<IT>::operator unsigned long long() const {  // Conversion to long long
    return static_cast<unsigned long long>(mid());
}

template <FloatLike IT>
inline interval<IT>::operator double() const {  // Conversion to double
    return static_cast<double>(mid());
}
template <FloatLike IT>
inline interval<IT>::operator float() const {  // Conversion to float
    return static_cast<float>(mid());
}

// template<FloatLike IT> inline interval<IT>::operator float_precision() const
//{	// Conversion to float
//	return static_cast<float_precision>(mid());
// }

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Conversion operators
///
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///
/// BEGIN Methods
///
//////////////////////////////////////////////////////////////////////////////////////

// Coordinate functions.

// Return the right interval "as is"
template <FloatLike IT>
inline IT interval<IT>::rightinterval() const {
    return right;
}

// Return the left interval "as is"
template <FloatLike IT>
inline IT interval<IT>::leftinterval() const {
    return left;
}

// Set the right interval "as is".
// If interval is empty set the interval type to CLOSE
template <FloatLike IT>
inline IT interval<IT>::rightinterval(const IT& r) {
    if (type == EMPTY) type = CLOSE;
    right = r;
    return right;
}

// Set the left interval "as is".
// If interval is empty set the interval type to CLOSE
template <FloatLike IT>
inline IT interval<IT>::leftinterval(const IT& l) {
    if (type == EMPTY) type = CLOSE;
    left = l;
    return left;
}

// Return the current intervaltype
template <FloatLike IT>
inline enum interval_type interval<IT>::intervaltype() const {
    return type;
}

// Set the interval type
// The below table is for a proper interval
//					CLOSE []	OPEN ()		LEFT_OPEN (]	RIGHT_OPEN [)
//	CLOSE		[]	#			-,+			-,#				#,+
//	OPEN		()	+,-			#			#,-				+,#
//	LEFT_OPEN	(]	+,#			#,+			#				+,+
//	RIGHT_OPEN	[)	#,-			-,#			_,_				#
//
//	For improper intervals we preserve the Improperness in the result.
//
template <FloatLike IT>
inline enum interval_type interval<IT>::intervaltype(const enum interval_type to) {
    interval<IT> x = *this;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);

    if (x.type != to) {
        if (this->isImproper()) {  // If improper swap the interval and switch the half open intervals
            std::swap(x.left, x.right);
            if (x.type == LEFT_OPEN)
                x.type = RIGHT_OPEN;
            else if (x.type == RIGHT_OPEN)
                x.type = LEFT_OPEN;
        }

        switch (x.type) {
            case CLOSE:
                // if the interval is already CLOSE then do nothing
                if (to == LEFT_OPEN || to == OPEN) x.left = utils::helpers<IT>::nextafter(x.left, -infi);
                if (to == RIGHT_OPEN || to == OPEN) x.right = utils::helpers<IT>::nextafter(x.right, infi);
                break;
            case OPEN:
                // if the interval is already Open then do nothing
                if (to == RIGHT_OPEN || to == CLOSE) x.left = utils::helpers<IT>::nextafter(x.left, infi);
                if (to == LEFT_OPEN || to == CLOSE) x.right = utils::helpers<IT>::nextafter(x.right, -infi);
                break;
            case LEFT_OPEN:
                // If the interval is already RIGHT_CLOSE same as LEFT_OPEN then do nothing
                if (to == RIGHT_OPEN || to == CLOSE) x.left = utils::helpers<IT>::nextafter(x.left, infi);
                if (to == RIGHT_OPEN || to == OPEN) x.right = utils::helpers<IT>::nextafter(x.right, infi);
                break;
            case RIGHT_OPEN:
                // If the interval is already LEFT_CLOSE then do nothing
                if (to == LEFT_OPEN || to == OPEN) x.left = utils::helpers<IT>::nextafter(x.left, -infi);
                if (to == LEFT_OPEN || to == CLOSE) x.right = utils::helpers<IT>::nextafter(x.right, -infi);
                break;
        }
        x.type = to;
        if (this->isImproper()) {
            std::swap(x.left, x.right);
        }
        *this = x;
    }
    return x.type;
}

// Return the current interval decoration
template <FloatLike IT>
inline enum interval_decoration interval<IT>::intervaldecoration() const {
    return decoration;
}

// Set and  Return the interval decoration
template <FloatLike IT>
inline enum interval_decoration interval<IT>::intervaldecoration(const enum interval_decoration to) {
    decoration = to;
    if (to == COMPUTE) {  // Compute the decoration type
        if (type == EMPTY)
            decoration = TRV;
        else if (utils::helpers<IT>::isfinite(left) && utils::helpers<IT>::isfinite(right))
            decoration = COM;  // is bounded
        else if (utils::helpers<IT>::isnan(left) || utils::helpers<IT>::isnan(right))
            decoration = ILL;  // ill formed e.g. one or both intervals is NAN
        else
            decoration = DAC;  // Must be unbounded, but otherwise good
    }
    return decoration;
}

// compute the infimum(greatest lower bound) of an interval, taking into account the type of interval
// (closed, open, left-open, or right-open) and whether the interval is proper
// (left endpoint is less than or equal to right endpoint) or improper(left endpoint is greater than the right
// endpoint).
template <FloatLike IT>
inline IT interval<IT>::inf(bool toclose) const {
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    if (isEmpty())                           // If empty return +infinity
        return infi;
    // For a closed interval, directly return the minimum of left and right.
#ifdef PHASE5
    if (!toclose) return std::min(left, right);
#endif
    if (type == CLOSE) return std::min(left, right);

    IT adjustedLeft = left;
    IT adjustedRight = right;

    // Adjust left boundary for open intervals
    if (type == LEFT_OPEN || type == OPEN)
        adjustedLeft = utils::helpers<IT>::nextafter(left, (left <= right) ? infi : -infi);

    // Adjust right boundary for open intervals, taking into account improper intervals
    if (type == RIGHT_OPEN || type == OPEN)
        adjustedRight = utils::helpers<IT>::nextafter(right, (left <= right) ? -infi : infi);

    // Return the minimum of the adjusted boundaries
    return std::min(adjustedLeft, adjustedRight);
}

// Optimizing the function for calculating the supremum(least upper bound) of an interval follows
// a similar approach to optimizing the infimum calculation
template <FloatLike IT>
inline IT interval<IT>::sup(bool toclose) const {
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    if (isEmpty())                           // If empty return -infinity
        return -infi;
    // For a closed interval, directly return the maximum of left and right.
#ifdef PHASE5
    if (!toclose) return max(left, right);
#endif
    if (type == CLOSE) return std::max(left, right);

    IT adjustedLeft = left;
    IT adjustedRight = right;

    // Adjust left boundary for open intervals
    if (type == LEFT_OPEN || type == OPEN)
        adjustedLeft = utils::helpers<IT>::nextafter(left, (left <= right) ? infi : -infi);

    // Adjust right boundary for open intervals, considering proper and improper intervals
    if (type == RIGHT_OPEN || type == OPEN)
        adjustedRight = utils::helpers<IT>::nextafter(right, (left <= right) ? -infi : infi);

    // Return the maximum of the adjusted boundaries
    return std::max(adjustedLeft, adjustedRight);
}

// Return interval midpoint, computed as the interval is closed to ensure correct computation
// if empty return no value
template <FloatLike IT>
inline IT interval<IT>::mid() const {
    if (isEmpty()) return utils::helpers<IT>::get_nan();
    if (right == left)
        return left;
    else
        return (right + left) / IT(2);
}

// Return interval radius
// Notice that radius is negative for improper intervals
template <FloatLike IT>
inline IT interval<IT>::rad() const {
    if (isEmpty()) return utils::helpers<IT>::get_nan();
    IT r((right - left) / IT(2));
    return r;
}

// Return interval width
template <FloatLike IT>
inline IT interval<IT>::wid() const {
    if (isEmpty()) return utils::helpers<IT>::get_nan();
    IT r(right - left);
    if (r < IT(0)) r = -r;
    return r;
}

// Return mignitude of class
template <FloatLike IT>
inline IT interval<IT>::mig() const {
    if (isEmpty()) return utils::helpers<IT>::get_nan();
    const IT l(utils::math<IT>::abs(inf()));
    const IT r(utils::math<IT>::abs(sup()));
    return std::min(l, r);
}

// Return magnitude of interval
template <FloatLike IT>
inline IT interval<IT>::mag() const {
    if (isEmpty()) return utils::helpers<IT>::get_nan();
    const IT l(utils::math<IT>::abs(inf()));
    const IT r(utils::math<IT>::abs(sup()));
    return std::max(l, r);
}

// Required is... methods
template <FloatLike IT>
inline bool interval<IT>::isProper() const {
    return left <= right;
}
template <FloatLike IT>
inline bool interval<IT>::isImproper() const {
    return right < left;
}
template <FloatLike IT>
inline bool interval<IT>::isPoint() const {
    return left == right;
}
template <FloatLike IT>
inline bool interval<IT>::isEmpty() const {
    return type == EMPTY;
}
template <FloatLike IT>
inline bool interval<IT>::isEntire() const {
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    return left == -infi && right == infi;
}

// Return interval classification
template <FloatLike IT>
inline enum int_class interval<IT>::isClass() const {
    if (left == IT(0) && right == IT(0)) return ZERO;
    if (left == IT(0) && right > IT(0)) return POSITIVE0;
    if (left > IT(0) && right > IT(0)) return POSITIVE1;
    if (left >= IT(0) && right > IT(0)) return POSITIVE;
    if (left < IT(0) && right == IT(0)) return NEGATIVE0;
    if (left < IT(0) && right < IT(0)) return NEGATIVE1;
    if (left < IT(0) && right <= IT(0)) return NEGATIVE;
    if (left < IT(0) && right > IT(0)) return MIXED;
    return NO_CLASS;
}

// Check if a point p is within the interval bounds
template <FloatLike IT>
inline bool interval<IT>::in(const IT& p) {
    return inf() <= p && p <= sup();
}

// Return the interval as a String representation
template <FloatLike IT>
inline std::string interval<IT>::toString() const {
    std::string s;
    enum interval_type t = intervaltype();
    std::ostringstream strs;

    strs.precision(25);
    strs << (t == LEFT_OPEN || t == OPEN ? "(" : "[");
    strs << left;
    strs << ",";
    strs << right;
    strs << (t == RIGHT_OPEN || t == OPEN ? ")" : "]");

    return strs.str();
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Methods
///
//////////////////////////////////////////////////////////////////////////////////////
//
//////////////////////////////////////////////////////////////////////////////////////
//
//
//    Essential Operators =,+=,-=,*=,/=
//
//
//////////////////////////////////////////////////////////////////////////////////////

// Assignment operator. Works for all class types
//
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator=(const interval<IT>& rhs) {
    left = rhs.left;
    right = rhs.right;
    type = rhs.type;
    decoration = rhs.decoration;
    return *this;
}

// += operator. Works for nearly all classes.
// Always return an "proper" and closed [] interval
// a:=a+[EMPTY] or b:=[EMPTY]+b or [EMPTY]:=[EMPTY]+[EMPTY]
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator+=(const interval<IT>& rhs) {
    // Handle EMPTY interval first
    if (rhs.type == EMPTY) return *this;
    if (type == EMPTY) return (*this = rhs);

    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT unfl(underflow_interval<IT>());
    // Neither a or b is [EMPTY]
    std::pair<IT, IT> xleft = fasttwo_sum(inf(), rhs.inf());
    std::pair<IT, IT> xright = fasttwo_sum(sup(), rhs.sup());
    // Any adjustment?
    if (xleft.second < IT(0)) xleft.first = utils::helpers<IT>::nextafter(xleft.first, -infi);
    if (xright.second > IT(0)) xright.first = utils::helpers<IT>::nextafter(xright.first, infi);
    left = xleft.first;
    right = xright.first;
#ifdef PHASE5
    type = compute_interval_type(type, rhs.type);
#else
    type = CLOSE;
#endif
    // Set decoration
    decoration = std::min(decoration, rhs.decoration);
    // However if underflow or overflow then change it to DAC or TRV
    if (utils::math<IT>::abs(left) == infi || utils::math<IT>::abs(right) == infi)
        decoration = std::min(decoration, DAC);
    if ((left != IT(0) && utils::math<IT>::abs(left) == unfl) ||
        (right != IT(0) && utils::math<IT>::abs(right) == unfl))
        decoration = std::min(decoration, TRV);
    return *this;
}

// -= operator. Works all other classes.
// Always return an "proper" and closed [] interval
// a=a-[EMPTY] or -b=[EMPTY]-b or [EMPTY]=[EMPTY]-[EMPTY]
//
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator-=(const interval<IT>& rhs) {
    // Handle EMPTY interval first
    if (rhs.type == EMPTY) return *this;
    if (type == EMPTY) return (*this = -rhs);

    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT unfl(underflow_interval<IT>());
    // Neither a or b is [EMPTY]
    std::pair<IT, IT> xleft = fasttwo_sum(inf(), -rhs.sup());
    std::pair<IT, IT> xright = fasttwo_sum(sup(), -rhs.inf());
    if (xleft.second < IT(0)) xleft.first = utils::helpers<IT>::nextafter(xleft.first, -infi);
    if (xright.second > IT(0)) xright.first = utils::helpers<IT>::nextafter(xright.first, infi);
    left = xleft.first;
    right = xright.first;
#ifdef PHASE5
    type = compute_interval_type(type, rhs.type);
#else
    type = CLOSE;
#endif
    // Set Decoration
    decoration = std::min(decoration, rhs.decoration);
    // However if underflow or overflow then change it to DAC or TRV
    if (utils::math<IT>::abs(left) == infi || utils::math<IT>::abs(right) == infi)
        decoration = std::min(decoration, DAC);
    if ((left != IT(0) && utils::math<IT>::abs(left) == unfl) ||
        (right != IT(0) && utils::math<IT>::abs(right) == unfl))
        decoration = std::min(decoration, TRV);
    return *this;
}

// Works all other classes.
// [EMPTY]:=a*[EMPTY] or [EMPTY]:=[EMPTY]*b or [EMPTY]:=[EMPTY]*[EMPTY]
// Please note that this is for all integer classes. interval<int>, interval<long>,
// were there is no loss of precision
// Instead of doing the mindless low = MIN(low*a.right, low*a.low,right*a.low,right*a.right) and
// right = MAX(low*a.right, low*a.low,right*a.low,right*a.right) requiring a total of 8 multiplication
//
//   low, right, a.low, a.right    result
//    +     +     -     +        -  +  [ right*a.low, right*a.right ] 2205
//    +     +     -     -        -  -  [ right*a.low, low*a.right ]
//    +     +     +     +        +  +  [ low*a.low, right*a.right ]
//    -     +     +     +        -  +  [ low*a.right, right*a.right ]
//    -     +     -     +        -  +  [ MIN(low*a.right,right*a.low), MAX(low*a.low,right*a.right) ]
//    -     +     -     -        -  -  [ right*a.low, low*a.low ]
//    -     -     +     +        -  -  [ low*a.right, right,a.low ]
//    -     -     -     +        -  -  [ low*a.right, low*a.low ]
//    -     -     -     -        +  +  [ right*a.right, low * a.low ]
//
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator*=(const interval<IT>& rhs) {
    // Handle EMPTY interval first  ∅
    if (type == EMPTY) return *this;
    if (rhs.type == EMPTY) return (*this = rhs);

    const IT c0(0);
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT unfl(underflow_interval<IT>());
    // Neither a or b is ∅
    // Extract intervals through inf() and sup()
    IT al(inf());
    IT ah(sup());
    IT bl(rhs.inf());
    IT bh(rhs.sup());

    auto multiply = [&](const IT& x, const IT& y) {
        std::pair<IT, IT> tmp = interval<IT>::fasttwo_prod(x, y);
        interval<IT> res(tmp.first, tmp.first);
        const IT infi(infinity_interval<IT>());  // infi(INFINITY);
        if (tmp.second < c0) res.left = utils::helpers<IT>::nextafter(tmp.first, -infi);
        if (tmp.second > c0) res.right = utils::helpers<IT>::nextafter(tmp.first, infi);
        return res;
    };
    // The initialization is done to preserve the precision when IT is a float_precision arbitrary type
    // For_IT as float, double or long double it has no effect
    interval<IT> itmp(al, ah);
#ifdef PHASE5
    type = compute_interval_type(type, rhs.type);
#else
    type = CLOSE;
#endif
    // Set decoration
    decoration = std::min(decoration, rhs.decoration);
    // Shortcuts
    if (al >= c0 && bl >= c0) {  // Both intervals positive
        itmp = multiply(al, bl);
        left = itmp.left;
        itmp = multiply(ah, bh);
        right = itmp.right;
        return *this;
    }
    if (ah < c0 && bh < c0) {  // Both intervals negative
        itmp = multiply(al, bl);
        right = itmp.right;
        itmp = multiply(ah, bh);
        left = itmp.left;
        return *this;
    }
    if (al >= c0 && bh < c0) {  // [A] interval positive, [B] interval negative
        itmp = multiply(ah, bl);
        left = itmp.left;
        itmp = multiply(al, bh);
        right = itmp.right;
        return *this;
    }
    if (ah < c0 && bl >= c0) {  // [A] interval negative, [B] interval positive
        itmp = multiply(al, bh);
        left = itmp.left;
        itmp = multiply(ah, bl);
        right = itmp.right;
        return *this;
    }
    // Otherwise, we have a mixed interval. Make all 4 combinations
    itmp = multiply(al, bl);
    left = itmp.left;
    right = itmp.right;
    itmp = multiply(al, bh);
    left = std::min(left, itmp.left);
    right = std::max(right, itmp.right);
    itmp = multiply(ah, bl);
    left = std::min(left, itmp.left);
    right = std::max(right, itmp.right);
    itmp = multiply(ah, bh);
    left = std::min(left, itmp.left);
    right = std::max(right, itmp.right);

    // However if underflow or overflow then change it to DAC or TRV
    if (utils::math<IT>::abs(left) == infi || utils::math<IT>::abs(right) == infi)
        decoration = std::min(decoration, DAC);
    if ((left != c0 && utils::math<IT>::abs(left) == unfl) || (right != c0 && utils::math<IT>::abs(right) == unfl))
        decoration = std::min(decoration, TRV);
    return *this;
}

// Works for all other classes
// [EMPTY]:=a/[EMPTY] or [EMPTY]:=[EMPTY]/b or [EMPTY]:=[EMPTY]/[EMPTY]
// Please note that this is for all integer classes. interval<int>, interval<long>,
// were there is no loss of precision
// There is specialization for both <int>
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator/=(const interval<IT>& rhs) {
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    // Handle EMPTY interval first
    if (type == EMPTY) return *this;
    if (rhs.type == EMPTY) return (*this = rhs);

    interval<IT> tmp(*this);  // Save a copy
    // Compute the reverse of y e.g. 1/y
    auto inverse = [&](const IT& y, const bool up) {
        IT res(IT(1) / y);
        const IT r(-utils::math<IT>::fma(res, y, utils::cast<IT, int>()(-1.0)));

        if (up == false) {
            if (r < IT(0)) res = utils::helpers<IT>::nextafter(res, -infi);
        } else {
            if (r > IT(0)) res = utils::helpers<IT>::nextafter(res, infi);
        };

        return res;
    };

    IT bl(rhs.inf());
    IT bh(rhs.sup());
    if (bl == IT(0) && bh == IT(0)) {
        left = -infi;
        right = infi;
        *this *= tmp;
        decoration = ILL;
        return *this;
    }
    if (bh == IT(0)) {  // b.low is !=0
        right = inverse(bl, true);
        left = -infi;
        *this *= tmp;
        decoration = ILL;
        return *this;
    }
    if (bl == IT(0)) {  // b.right is !=0
        left = inverse(bh, false);
        right = infi;
        *this *= tmp;
        decoration = ILL;
        return *this;
    }
    // neither b.inf() or b.sup() is zero
    left = inverse(bh, false);
    right = inverse(bl, true);
    *this *= tmp;
    // Set decoration
    decoration = std::min(decoration, rhs.decoration);
    if (bl < IT(0) && bh > IT(0))                // Do division interval include zero?
        decoration = std::min(decoration, TRV);  // Then set the decoration to TRV

    return *this;
}

// Return the intersection
//
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator&=(const interval<IT>& rhs) {
    const IT aleft(rhs.inf());
    const IT aright(rhs.sup());
    const IT l(inf());
    const IT r(sup());
    if (aright > l || r < aleft)  // No intersection
        this->type = EMPTY;
    else {
        left = std::max(aleft, l);
        right = std::min(aright, r);
        type = CLOSE;
    }

    return *this;
}

// Return the union.
// However not the correct union as two intervals if not overlapping.
// But just the entire union of the two interval
// regardsless if the intervals is connected.
// use join() for the correct handling of the union operator.
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator|=(const interval<IT>& rhs) {
    const IT aleft(rhs.inf());
    const IT aright(rhs.sup());
    const IT l(inf());
    const IT r(sup());
    if (right < l || r < left) {  // Non-overlapping intervals
                                  //????
    }
    //	else
    {  // Overlapping
        left = std::min(aleft, l);
        right = std::max(aright, r);
    }
    return *this;
}

// Return the set minus
//
template <FloatLike IT>
inline interval<IT>& interval<IT>::operator^=(const interval<IT>& rhs) {
    const IT aleft(rhs.inf());
    const IT aright(rhs.sup());
    const IT l(inf());
    const IT r(sup());
    if (aleft < r && aright > l)  // intersection is not empty
    {
        if (aright <= l)
            left = aright;
        else if (aright >= r)
            right = aleft;
    }

    return *this;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Essential Operators
///
//////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//
//    Binary and Unary Operators +,-,*,/
//
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

// Binary + operator specialization for only interval<IT> arguments
// Works for all classes
//
template <FloatLike IT>
inline interval<IT> operator+(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> result(a);
    result += b;
    return result;
}

// Unary + operator
// Works for all classes
//
template <FloatLike IT>
inline interval<IT> operator+(const interval<IT>& a) {
    return a;
}

// Binary - operator
// Works for all classes
//
template <FloatLike IT>
inline interval<IT> operator-(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> result(a);
    result -= b;
    return result;
}

// Unary - operator
// Works for all classes
//
template <FloatLike IT>
inline interval<IT> operator-(const interval<IT>& a) {
    interval<IT> result(a);  // Ensure correct precision for IT=float_precision
    result = interval<IT>(0);
    result -= a;
    return result;
}

// Binary * operator
// Works for all classes
//
template <FloatLike IT>
inline interval<IT> operator*(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> result(a);
    result *= b;
    return result;
}

// Binary / operator
// Works for all classes
//
template <FloatLike IT>
inline interval<IT> operator/(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> result(a);

    if (result == b) {
        enum int_class intclass = b.isClass();
        if (intclass != ZERO && intclass != POSITIVE0 && intclass != NEGATIVE0) {
            //	result = interval<IT>(IT(1), IT(1));
            //	return result;
        }
    }
    result /= b;  // Notice result/=b; return the maximum precision of the two operand And
                  // since we change this precision in the division call result is the precision of
                  // the maximum of the two operands
    return result;
}

// Binary + operator
// Works for all classes
//
template <FloatLike IT, class _X>
inline interval<IT> operator+(const interval<IT>& a, const _X& b) {
    interval<IT> c(a);
    c += interval<IT>(IT(b));
    return c;
}

// Binary + operator
// Works for all classes
//
template <FloatLike IT, class _X>
inline interval<IT> operator+(const _X& a, const interval<IT>& b) {
    interval<IT> c(b);
    c += interval<IT>(IT(a));
    return c;
}

// Binary - operator
// Works for all classes
//
template <FloatLike IT, class _X>
inline interval<IT> operator-(const interval<IT>& a, const _X& b) {
    interval<IT> c(a);
    c -= interval<IT>(IT(b));
    return c;
}

// Binary - operator
// Works for all classes
//
template <FloatLike IT, class _X>
inline interval<IT> operator-(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    c -= b;
    return c;
}

// Binary * operator
// Works for all classes
//
// template<FloatLike IT, class _X> inline interval<IT> operator*(const interval<IT>& a, const _X& b)
//	{
//	interval<IT> c(a);
//	c *= interval<IT>(static_cast<IT>(b));
//	return c;
//	}

// Binary * operator
// Works for all classes
//
// template<FloatLike IT, class _X> inline interval<IT> operator*(const _X& a, const interval<IT>& b)
//	{
//	interval<IT> c(b);
//	c *= interval<IT>(IT(a));
//	return c;
//	}

// Binary / operator
// Works for all classes
//
template <FloatLike IT, class _X>
inline interval<IT> operator/(const interval<IT>& a, const _X& b) {
    interval<IT> c(a);
    c /= interval<IT>(IT(b));
    return c;
}

// Binary / operator
// Works for all classes
//
template <FloatLike IT, class _X>
inline interval<IT> operator/(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    c /= b;
    return c;
}

// Binary & operator
// Return the intersection
//
template <FloatLike IT>
inline interval<IT> operator&(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> c(a);
    c &= b;
    return c;
}

// Binary | operator.
// Return the union
//
template <FloatLike IT>
inline interval<IT> operator|(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> c(a);
    c |= b;
    return c;
}

// Binary ^ operator
// Return set minus
//
template <FloatLike IT>
inline interval<IT> operator^(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> c(a);
    c ^= b;
    return c;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Binary and Unary Operators
///
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///
/// Boolean Interval for ==, <=, >=, <, > and !=
///
//////////////////////////////////////////////////////////////////////////////////////

// == operator
// Works for all classes
//
template <FloatLike IT>
inline bool operator==(const interval<IT>& a, const interval<IT>& b) {
    if (a.intervaltype() == EMPTY && b.intervaltype() == EMPTY) return true;  // Both EMPTY=> return true
    if (a.intervaltype() == EMPTY || b.intervaltype() == EMPTY)
        return false;  // One but not both are EMPTY => return false
    // Check for equality. Note intervaltype also has to match
    return a.inf() == b.inf() && a.sup() == b.sup() && a.intervaltype() == b.intervaltype();
}

// != operator
// Works for all classes
//
template <FloatLike IT>
inline bool operator!=(const interval<IT>& a, const interval<IT>& b) {
    return !(a == b);
}
// >= operator
// Works for all classes
//
template <FloatLike IT>
inline bool operator>=(const interval<IT>& a, const interval<IT>& b) {
    return !(a < b);
}
// <= operator
// Works for all classes
//
template <FloatLike IT>
inline bool operator<=(const interval<IT>& a, const interval<IT>& b) {
    return !(a > b);
}

// > operator
// Works for all classes
//
template <FloatLike IT>
inline bool operator>(const interval<IT>& a, const interval<IT>& b) {
    if (a.intervaltype() == EMPTY && b.intervaltype() == EMPTY) return false;
    if (a.intervaltype() == EMPTY || b.intervaltype() == EMPTY) return true;

    // Helper function to compare left boundaries
    auto isLeftGreater = [](const interval<IT>& a, const interval<IT>& b) {
        if (a.inf() > b.inf()) return true;
        if (a.inf() < b.inf()) return false;
        // Handle inclusivity/exclusivity of left boundary
        const enum interval_type atype(a.intervaltype());
        const enum interval_type btype(b.intervaltype());
        if ((atype == CLOSE || atype == RIGHT_OPEN) && (btype == LEFT_OPEN || btype == OPEN)) return true;
        if ((atype == OPEN || atype == LEFT_OPEN) && (btype == CLOSE || btype == RIGHT_OPEN)) return false;
        return false;
    };
    // Helper function to compare right boundaries
    auto isRightGreater = [](const interval<IT>& a, const interval<IT>& b) {
        if (a.sup() > b.sup()) return true;
        if (a.sup() < b.sup()) return false;
        // Handle inclusivity/exclusivity of right boundary
        const enum interval_type atype(a.intervaltype());
        const enum interval_type btype(b.intervaltype());
        if ((atype == CLOSE || atype == LEFT_OPEN) && (btype == OPEN || btype == RIGHT_OPEN)) return true;
        if ((atype == OPEN || atype == RIGHT_OPEN) && (btype == CLOSE || btype == LEFT_OPEN)) return false;
        return false;
    };

    // Check if the left boundary of interval a is greater than that of b
    if (isLeftGreater(a, b)) return true;
    // Check if the left boundary of interval b is greater than that of a
    if (isLeftGreater(b, a)) return false;

    // If the left boundaries are equivalent, compare the right boundaries
    return isRightGreater(a, b);
}

// < operator
// Works for all classes
//
template <FloatLike IT>
inline bool operator<(const interval<IT>& a, const interval<IT>& b) {
    if (a.intervaltype() == EMPTY && b.intervaltype() == EMPTY) return false;
    if (a.intervaltype() == EMPTY || b.intervaltype() == EMPTY) return true;

    // Helper function to compare left boundaries
    auto isLeftLess = [](const interval<IT>& a, const interval<IT>& b) {
        if (a.inf() < b.inf()) return true;
        if (a.inf() > b.inf()) return false;
        // Handle inclusivity/exclusivity of left boundary
        const enum interval_type atype(a.intervaltype());
        const enum interval_type btype(b.intervaltype());
        if ((atype == CLOSE || atype == RIGHT_OPEN) && (btype == LEFT_OPEN || btype == OPEN)) return false;
        if ((atype == OPEN || atype == LEFT_OPEN) && (btype == CLOSE || btype == RIGHT_OPEN)) return true;
        return false;
    };

    // Helper function to compare right boundaries
    auto isRightLess = [](const interval<IT>& a, const interval<IT>& b) {
        if (a.sup() < b.sup()) return true;
        if (a.sup() > b.sup()) return false;
        // Handle inclusivity/exclusivity of right boundary
        const enum interval_type atype(a.intervaltype());
        const enum interval_type btype(b.intervaltype());
        if ((atype == CLOSE || atype == LEFT_OPEN) && (btype == OPEN || btype == RIGHT_OPEN)) return false;
        if ((atype == OPEN || atype == RIGHT_OPEN) && (btype == CLOSE || btype == LEFT_OPEN)) return true;
        return false;
    };

    // Check if the left boundary of interval a is greater than that of b
    if (isLeftLess(a, b)) return true;
    // Check if the left boundary of interval b is greater than that of a
    if (isLeftLess(b, a)) return false;

    // If the left boundaries are equivalent, compare the right boundaries
    return isRightLess(a, b);
}

// Binary == operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator==(const interval<IT>& a, const _X& b) {
    interval<IT> c(b);
    return a == c;
}

// Binary == operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator==(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    return c == b;
}

// Binary == operator
// Works for all classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator==(const interval<IT>& a, const interval<_X>& b) {
    interval<IT> c(b);
    return a == c;
}

// Binary != operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator!=(const interval<IT>& a, const _X& b) {
    interval<IT> c(b);
    return !(a == c);
}

// Binary != operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator!=(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    return !(c == b);
}

// != operator
// Works for all classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator!=(const interval<IT>& a, const interval<_X>& b) {
    interval<IT> c(b);
    return !(a == c);
}

// Binary >= operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator>=(const interval<IT>& a, const _X& b) {
    interval<IT> c(b);
    return a >= c;
}

// Binary >= operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator>=(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    return c >= b;
}

// Binary >= operator
// Works for all classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator>=(const interval<IT>& a, const interval<_X>& b) {
    interval<IT> c(b);
    return a >= c;
}

// Binary >= operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator<=(const interval<IT>& a, const _X& b) {
    interval<IT> c(b);
    return a <= c;
}

// Binary >= operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator<=(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    return c <= b;
}

// Binary >= operator
// Works for all classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator<=(const interval<IT>& a, const interval<_X>& b) {
    interval<IT> c(b);
    return a <= c;
}

// Binary > operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator>(const interval<IT>& a, const _X& b) {
    interval<IT> c(b);
    return a > c;
}

// Binary > operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator>(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    return c > b;
}

// Binary > operator
// Works for all classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator>(const interval<IT>& a, const interval<_X>& b) {
    interval<IT> c(b);
    return a > c;
}

// Binary < operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator<(const interval<IT>& a, const _X& b) {
    interval<IT> c(b);
    return a < c;
}

// Binary < operator
// Works for all mixed classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator<(const _X& a, const interval<IT>& b) {
    interval<IT> c(a);
    return c < b;
}

// Binary < operator
// Works for all classes
//
template <FloatLike IT, FloatLike _X>
inline bool operator<(const interval<IT>& a, const interval<_X>& b) {
    interval<IT> c(b);
    return a < c;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Boolean operators
///
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///
/// Interval abs()
///
//////////////////////////////////////////////////////////////////////////////////////

// abs([a,b])
// if a>=0 in [a,b] then |[a,b]|==[a,b]
// if b<0 in [a,b] then |[a,b]|=[-b,-a]
// if a<0 & b>0 in [a,b] then |[a,b]|=[0,max(-a,b)]
template <FloatLike IT>
inline interval<IT> abs(const interval<IT>& a) {
    if (a.inf() >= IT(0))  // Entirely positive
        return a;
    else if (a.sup() < IT(0))  // Entirely negative
        return -a;

    return interval<IT>(IT(0), std::max(-a.inf(), a.sup()), a.intervaltype());
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// Interval distance() between two interval numbers
///
//////////////////////////////////////////////////////////////////////////////////////

template <FloatLike IT>
inline IT intervaldistance(const interval<IT>& a, const interval<IT>& b) {
    return std::max(utils::math<IT>::abs(a.leftinterval() - b.leftinterval()),
                    utils::math<IT>::abs(a.rightinterval() - b.rightinterval()));
}

// Return union
// the name join is used since union is a reserved word in c++
// it follow the IEEE 1788 standard by returning two intervals if the interval a and b is not connected
// otherwise if return the joined interval and the second interval of thepair returned is the empty
// interval
// Notice the |= operates or the binary operator | return the union of the two intervals by combining it to one interval
//
template <FloatLike IT>
inline std::pair<interval<IT>, interval<IT>> join(const interval<IT>& a, const interval<IT>& b) {
    if (a.sup() < b.inf())  // interval do not connect
    {                       // return two interval
        return std::make_pair<interval<IT>, interval<IT>>(a, b);
    }
    // Return the union of ther two intervals.
    interval<IT> c(std::min(a.inf(), b.inf()), std::max(a.sup(), b.sup()));
    interval<IT> d;  // Empty set
    return std::make_pair<interval<IT>, interval<IT>>(c, d);
}

// Return the interval intersection of the two intervals.
template <FloatLike IT>
inline interval<IT> intersection(const interval<IT>& a, const interval<IT>& b) {
    interval<IT> c(a);
    c &= b;
    return c;
}

// if a is a subset of b then return true otherwise false
template <FloatLike IT>
inline bool subset(const interval<IT>& a, const interval<IT>& b) {
    if (b.inf() <= a.inf() && a.sup() <= b.sup()) return true;
    return false;
}

// if a is an interior of b then return true otherwise false
template <FloatLike IT>
inline bool interior(const interval<IT>& a, const interval<IT>& b) {
    if (b.inf() < a.inf() && a.sup() < b.sup()) return true;
    return false;
}

// if a precedes b then return true otherwise false
template <FloatLike IT>
inline bool precedes(const interval<IT>& a, const interval<IT>& b) {
    if (a.sup() < b.inf()) return true;
    return false;
}

// inclusion between two intervals.
// If a is a subset of b then return +1,
// if b is a subset of a then return +1
// otherwise return 0
template <FloatLike IT>
inline int inclusion(const interval<IT>& a, const interval<IT>& b) {
    if (subset(a, b)) return -1;
    if (subset(b, a)) return +1;
    return 0;
}

// empty(). The function version of the method .isEmpty()
template <FloatLike IT>
inline bool empty(const interval<IT>& a) {
    return a.isEmpty();
}

// empty(). The function version of the method .isEmpty()
template <FloatLike IT>
inline bool entire(const interval<IT>& a) {
    return a.isEntire();
}

// Handle interval type when binary operations of interval is performed
// In general open beats close, (open prevails) see below
//		[]	(]	[)	()
//	===================
//	[]	[]	(]	[)	()
//	(]	(]	(]	()	()
//	[)	[)	()	[)	()
//	()	()	()	()	()
//
inline enum interval_type compute_interval_type(enum interval_type a, enum interval_type b) {
    if (a == b) return a;      // return the same interval types as either of the operand
    if (a == CLOSE) return b;  // Since open prevails return b interval_type
    if (b == CLOSE) return a;  // Since open prevails return a interval type
    return OPEN;               // Otherwise the interval is open
}

/////////////////////////////////////////////////////////////////////////////////////
//
// END interval functions
//
/////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////
//
// Interval functions:
//		sgn(),
//		sqr(),
//		sqrt(),
//		log10(),
//		log(),
//		exp()
//		pow()
//		floor()
//		ceil()
//
// By default IEE754 round to nearest
/////////////////////////////////////////////////////////////////////////////////////

// sqr(x)=x^2
template <FloatLike IT>
inline interval<IT> sqr(const interval<IT>& x) {
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT unfl(underflow_interval<IT>());

    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    IT left(x.inf());
    IT right(x.sup());
    IT tmpl(left);
    IT tmpr(right);
    interval<IT> r(left, right, x.intervaltype());  // Ensure correct precision for IT=float_precision

    r.leftinterval(IT(0));  // set left interval to zero
    left *= left;           // square left interval
    right *= right;         // square right interval
    r.rightinterval(max(left, right));
    // Contained zero?
    if (tmpl > IT(0) && tmpr > IT(0)) r.leftinterval(min(left, right));
    // Set decoration
    r.intervaldecoration(std::min(r.intervaldecoration(), x.intervaldecoration()));
    // However if underflow or overflow then change it to DAC or TRV
    if (utils::math<IT>::abs(left) == infi || utils::math<IT>::abs(right) == infi)
        r.intervaldecoration(std::min(r.intervaldecoration(), DAC));
    if ((left != IT(0) && utils::math<IT>::abs(left) == unfl) ||
        (right != IT(0) && utils::math<IT>::abs(right) == unfl))
        r.intervaldecoration(std::min(r.intervaldecoration(), TRV));
    return r;
}

// sqrt(x)
// wortk for all clases. The initialization of local variable is done to ensure correct precision when called
// with the IT=float_precision
//
template <FloatLike IT>
inline interval<IT> sqrt(const interval<IT>& x) {
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    // Initialized the local variable, to ensure right precision for float_precision types
    // Notice IEEE1788 requires us to ignore outside of domain range. e.g. negative numbers
    // sqrt([-1,1])==[0,1] or sqrt([-2,-1])==[EMPTY]
    if (x.isEmpty() || x.sup() < IT(0)) {
        interval<IT> res = interval<IT>();  // Return the EMPTY interval;
        res.intervaldecoration(ILL);
        return res;
    }
    // Find leftinterval bound
    const IT leftadjust(std::max(x.inf(), IT(0)));
    IT left(utils::math<IT>::sqrt(leftadjust));
    IT r(-utils::math<IT>::fma(left, left, -leftadjust));
    if (utils::helpers<IT>::isinf(left) && utils::helpers<IT>::isinf(x.inf())) r = 0;  // When both is infinity
    if (r < IT(0)) left = utils::helpers<IT>::nextafter(left, -infi);

    // Find rightinterval bound
    const IT rightadjust(std::max(x.sup(), IT(0)));
    IT right(utils::math<IT>::sqrt(rightadjust));
    if (utils::helpers<IT>::isinf(right) && utils::helpers<IT>::isinf(x.sup()))
        r = 0;  // When both is infinity
    else
        r = -utils::math<IT>::fma(right, right, -rightadjust);
    if (r > IT(0)) right = utils::helpers<IT>::nextafter(right, infi);

    interval<IT> res(left, right);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    if (x.inf() < IT(0)) res.intervaldecoration(std::min(res.intervaldecoration(), TRV));
    return res;
}

// floor(x)
// the interval returned is always CLOSED
template <FloatLike IT>
inline interval<IT> floor(const interval<IT>& x) {
    const IT left(x.inf(true));
    const IT right(x.sup(true));
    interval<IT> res(utils::helpers<IT>::floor(left), utils::helpers<IT>::floor(right));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// ceil(x)
template <FloatLike IT>
inline interval<IT> ceil(const interval<IT>& x) {
    const IT left(x.inf(true));
    const IT right(x.sup(true));
    interval<IT> res(utils::helpers<IT>::ceil(left), utils::helpers<IT>::ceil(right));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// sgn(x)
// return either a singleton interval if left>0 [1], left==0 & right=0 [0], right<0 [-1]
// or a range interval is left <0 and right==0 [-1,0], left<0 & right >0 [-1,1] or left==0 and right>0 [0,1]
//
template <FloatLike IT>
inline interval<IT> sgn(const interval<IT>& x) {
    const IT left(x.inf(true));
    const IT right(x.sup(true));
    IT left_min(IT(+1));
    IT right_max(IT(-1));

    if (left <= IT(0)) left_min = IT(0);
    if (left < IT(0)) left_min = IT(-1);
    if (right >= IT(0)) right_max = IT(0);
    if (right > IT(0)) right_max = IT(1);

    interval<IT> res(left_min, right_max);
    // set the proper interval decoration
    res.intervaldecoration(COM);
    return res;
}

// log(x)
template <FloatLike IT>
inline interval<IT> log(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT l(x.inf());
    const IT r(x.sup());
    const bool isIEEE754Float = std::is_floating_point<IT>::value;

    if (r < IT(0)) {                        // entire interval < 0
        interval<IT> res = interval<IT>();  // Return the EMPTY interval;
        res.intervaldecoration(ILL);        // Set ILL decoration
        return res;
    }

    // Initialize lower and upper with direct log calculations or -INFINITY for l <= 0
    IT lower((l <= IT(0)) ? -infi : utils::math<IT>::log(l));
    IT upper((r <= IT(0)) ? -infi : utils::math<IT>::log(r));

    // Apply shortcuts for well-known constants, adjusting for precision
    if (l == IT(1))
        lower = IT(0);
    else if (isIEEE754Float && l == IT(2))
        lower = ln2_interval<IT>().inf();
    else if (isIEEE754Float && l == IT(10))
        lower = ln10_interval<IT>().inf();
    else
        lower = utils::helpers<IT>::nextafter(lower, -infi);  // Adjust for precision if not a shortcut value

    if (r == IT(1))
        upper = IT(0);
    else if (isIEEE754Float && r == IT(2))
        upper = ln2_interval<IT>().sup();
    else if (isIEEE754Float && r == IT(10))
        upper = ln10_interval<IT>().sup();
    else
        upper = utils::helpers<IT>::nextafter(upper, infi);  // Adjust for precision if not a shortcut value

    // Ensure lower is not mistakenly set to a non-NaN value when l <= 0
    if (l <= IT(0) && r > IT(0)) lower = -infi;

    interval<IT> res(lower, upper);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    if (x.inf() < IT(0))  // was x original < 0
        res.intervaldecoration(std::min(res.intervaldecoration(), TRV));
    return res;
}

// log10(x)
template <FloatLike IT>
inline interval<IT> log10(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT l(x.inf());
    const IT r(x.sup());

    if (r < IT(0)) {                        // entire interval < 0
        interval<IT> res = interval<IT>();  // Return the EMPTY interval;
        res.intervaldecoration(ILL);        // Set ILL decoration
        return res;
    }

    // Initialize lower and upper bounds with direct log10 calculations or -INFINITY for l <= 0
    IT lower(l <= IT(0) ? -infi : utils::math<IT>::log10(l));
    IT upper(r <= IT(0) ? -infi : utils::math<IT>::log10(r));

    // Apply shortcuts for well-known constants, adjusting for precision
    if (l == IT(1))
        lower = IT(0);
    else if (l == IT(10))
        lower = IT(1);
    else
        lower = utils::helpers<IT>::nextafter(lower, -infi);  // Adjust for precision if not a shortcut value

    if (r == IT(1))
        upper = IT(0);
    else if (r == IT(10))
        upper = IT(1);
    else
        upper = utils::helpers<IT>::nextafter(upper, infi);  // Adjust for precision if not a shortcut value

    // Ensure lower is not mistakenly set to a non-NaN value when l <= 0
    if (l <= IT(0) && r > IT(0)) lower = -infi;

    interval<IT> res(lower, upper);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    if (x.inf() < IT(0))  // was x original < 0
        res.intervaldecoration(std::min(res.intervaldecoration(), TRV));
    return res;
}

// exp(x)
template <FloatLike IT>
inline interval<IT> exp(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const bool isIEEE754Float = std::is_floating_point<IT>::value;
    const IT l(x.inf());
    const IT r(x.sup());
    IT leftexp(utils::math<IT>::exp(l));
    IT rightexp(utils::math<IT>::exp(r));

    // Directly handle the special cases with exact values
    if (l == IT(0))
        leftexp = IT(1);  // e^0 = 1, exact
    else if (isIEEE754Float && l == IT(1))
        leftexp = e_interval<IT>().inf();  // e^1, use predefined constant
    else
        leftexp = utils::helpers<IT>::nextafter(leftexp, -infi);  // Adjust unless it's a special case

    if (r == IT(0))
        rightexp = IT(1);  // e^0 = 1, exact
    else if (isIEEE754Float && r == IT(1))
        rightexp = e_interval<IT>().sup();  // e^1, use predefined constant
    else
        rightexp = utils::helpers<IT>::nextafter(rightexp, infi);  // Adjust unless it's a special case

    // Create and return the interval from the calculated or adjusted values
    interval<IT> res(leftexp, rightexp);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    if (utils::math<IT>::abs(rightexp) == infi || utils::math<IT>::abs(leftexp) == infi)
        res.intervaldecoration(std::min(res.intervaldecoration(), DAC));
    return res;
}

// pow(x,y) where x is an interval and y id a double
//
template <FloatLike IT>
inline interval<IT> pow(const interval<IT>& x, const IT y) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    // Check for special cases
    if (y == IT(0))  // Anything to the power of 0 is 1
        return interval<IT>(1);

    // interval<IT> lhs, rhs;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT l(x.inf());
    const IT r(x.sup());

    // Handle negative powers when the interval includes 0 to avoid division by zero
    /*
    if (y < 0 && l <= 0 && r >= 0) {
            if (l == 0) {
                    // If l is 0, the lower bound goes to infinity when raised to a negative power
                    return interval<IT>(0, utils::nextafter(1 / pow(r, y), +INFINITY));
            }
            else if (r == 0) {
                    // If r is 0, the upper bound goes to infinity when raised to a negative power
                    return interval<IT>(utils::nextafter(1 / pow(l, y), -INFINITY), +INFINITY);
            }
            else {
                    // If the interval spans through 0, the result is (0, +INFINITY)
                    return interval<IT>(0, +INFINITY);
            }
    }
    */

    IT lp(utils::math<IT>::pow(l, y));
    IT rp(utils::math<IT>::pow(r, y));

    if (utils::helpers<IT>::floor(l) != l ||
        utils::helpers<IT>::floor(r) != r) {  // if either is not an integer then we do not have an exact power
        lp = utils::helpers<IT>::nextafter(lp, (lp > IT(0)) ? -infi : infi);
        rp = utils::helpers<IT>::nextafter(rp, (rp > IT(0)) ? infi : -infi);
    }
    // else Both are integers => trust the result

    // Ensure correct interval ordering for the result
    return interval<IT>(std::min(lp, rp), std::max(lp, rp));
}

// pow(x) we have to do it manually
// x^y == exp( y * ln( x ) ) );
//			interval	singleton
// x
//
template <FloatLike IT>
inline interval<IT> pow(const interval<IT>& x, const interval<IT>& y) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    interval<IT> c;

    if (y.isPoint())  // is y a singleton interval?
        return pow(x, y.inf());
    if (x.isPoint())  // x is a point, y is an interval
    {
        // ??
    }
    // Both x and y are intervals
    // if y is an integer?
    if (utils::helpers<IT>::floor(y.inf()) == y.inf() &&
        utils::helpers<IT>::floor(y.sup()) == y.sup()) {  // raise to the power of an integer interval
        interval<IT> lhs(utils::math<interval<IT>>::pow(x, utils::cast<interval<IT>, IT>()(y.inf())));
        interval<IT> rhs(utils::math<interval<IT>>::pow(x, utils::cast<interval<IT>, IT>()(y.sup())));
        c = interval<IT>(std::min(lhs.inf(), rhs.inf()), std::max(lhs.sup(), rhs.sup()));
        return c;
    }

    // Otherwise do it the hard way
    c = utils::math<interval<IT>>::log(x);
    c *= y;
    c = utils::math<interval<IT>>::exp(c);
    return c;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Interval sqrt(), log10(), log(), exp(), pow()
///
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///
/// Interval sin(), cos(), tan(), asin(), acos(), atan()
///
//////////////////////////////////////////////////////////////////////////////////////

// sin(x)
// The function for calculating the sine over an interval can be significantly optimized and simplified
// to handle the periodic nature of the sine function and ensure it correctly covers the range of sine
// values within the specified interval.
// Here's an optimized approach that considers the sine function's properties :
// The sine function is periodic with a period of 2π, and its range is between - 1 and 1.
// For any input interval, the sine function's output interval might wrap around this range.
// If the interval's width is greater than or equal to 2π, the sine function covers its entire range of [−1,1].
// For intervals smaller than 2π, calculate the exact sine values at the interval's endpoints
// and check for any critical points (multiples of 2π / 2) within the interval to determine the maximum
// and minimum sine values.
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires
// c++17) lambda functions. the variable l and r inherits the precision fromthe call to fmod()
template <FloatLike IT>
inline interval<IT> sin(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT pi = [&]() -> IT {
        return utils::math<IT>::acos(IT(-1));  // More precise pi value
    }();  // The Lambda is immediately invokeed
    const IT twopi(IT(2) * pi);
    IT l(x.inf());
    IT r(x.sup());

    // If the interval width is >= 2pi, the sine function covers the full range [-1, 1]
    if (x.sup() - x.inf() >= twopi) return interval<IT>(IT(-1), IT(1));

    // Calculate sine values at the interval's endpoints
    IT sin_l(utils::math<IT>::sin(l));
    IT sin_r(utils::math<IT>::sin(r));

    // Check for critical points within the interval
    IT sin_min(std::min(sin_l, sin_r));
    IT sin_max(std::max(sin_l, sin_r));

    // Check passing critical ponts by normalizing l and r
    l = utils::helpers<IT>::fmod(x.inf(), twopi);                // Normalize l within a single period
    r = l + utils::helpers<IT>::fmod(x.sup() - x.inf(), twopi);  // Calculate r based on l and the interval width
    // Normalize angles to be within [0, 2*pi)
    if (l < IT(0)) l += twopi;
    if (r >= twopi) r -= twopi;
    if (l <= pi / IT(2) && pi / IT(2) <= r) sin_max = IT(1);       // pi/2 is within interval
    if (l <= IT(1.5) * pi && IT(1.5) * pi <= r) sin_min = IT(-1);  // 3*pi/2 is within interval

    // Established a safety interval around the result to ensure correct bound for the computation
    if (sin_min != IT(-1) && sin_min != IT(0)) sin_min = utils::helpers<IT>::nextafter(sin_min, -infi);
    if (sin_max != IT(1) && sin_max != IT(0)) sin_max = utils::helpers<IT>::nextafter(sin_max, infi);

    // Create and return the interval based on calculated min and max sine values
    interval<IT> res(sin_min, sin_max);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// Same layout as for the sin(x) with the needed change for cos(x)
// The normalization of the input interval l and r remains the same as for the sin(x),
// as it's based on the periodicity of the trigonometric functions.
// The critical points for maximum and minimum values are adjusted for the cos(x) function.Specifically,
// cos(x) reaches its maximum value of 1 at 0 and 2π, and its minimum value of - 1 at π.
// The check for these critical points within the given interval is updated to reflect the cosine function's behavior.
// The return statement creates and returns an interval of type IT based on the calculated minimum and maximum
// values of cos(x) within the specified interval.
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires
// c++17) lambda functions. the variable l and r inherits the precision fromthe call to fmod()
template <FloatLike IT>
inline interval<IT> cos(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT pi = [&]() -> IT {
        return utils::math<IT>::acos(IT(-1));  // More precise pi value
    }();  // The Lambda is immediately invokeed
    const IT twopi(IT(2) * pi);
    IT l(x.inf());
    IT r(x.sup());

    // If the interval width is >= 2pi, the cos function covers the full range [-1, 1]
    if (x.sup() - x.inf() >= twopi) return interval<IT>(IT(-1), IT(1));

    // Calculate cosine values at the interval's endpoints
    IT cos_l(utils::math<IT>::cos(l));
    IT cos_r(utils::math<IT>::cos(r));

    // Check for critical points within the interval
    IT cos_min(std::min(cos_l, cos_r));
    IT cos_max(std::max(cos_l, cos_r));

    // Check passing critical ponts by normalizing l and r
    l = utils::helpers<IT>::fmod(x.inf(), twopi);                // Normalize l within a single period
    r = l + utils::helpers<IT>::fmod(x.sup() - x.inf(), twopi);  // Calculate r based on l and the interval width
    // Normalize angles to be within [0, 2*pi)
    if (l < IT(0)) l += twopi;
    if (r >= twopi) r -= twopi;
    if (r < l) cos_max = IT(1.0);  // 0 or 2*pi is within interval
    // if (l <= pi && pi <= r)
    if (l <= pi && r >= pi) cos_min = IT(-1.0);  // pi is within interval

    // Established a safety interval around the result to ensure correct bound for the computation
    if (cos_min != IT(-1) && cos_min != IT(0)) cos_min = utils::utils::helpers<IT>::nextafter(cos_min, -infi);
    if (cos_max != IT(1) && cos_max != IT(0)) cos_max = utils::utils::helpers<IT>::nextafter(cos_max, infi);

    // Create and return the interval based on calculated min and max cosine values
    interval<IT> res(cos_min, cos_max);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// This code makes several key assumptions and considerations:
// It normalizes the input interval to a single period of 2π to manage the periodicity of tan(x).
// It checks if the interval crosses a vertical asymptote by examining the range of the interval and the relative
// positions of l and r.If the interval crosses an asymptote, the function can potentially take on all real values,
// so the interval is set to (−∞, ∞).
// If the interval does not include an asymptote, the function calculates the tangent at the endpoints of the interval
// and uses these to determine the minimum and maximum values of tan(x) within the interval.
// It returns an interval representing the range of tan(x) over the specified interval, taking into account the
// possibility of infinite values.
// This approach captures the basic behavior of the tangent function over an interval, but it simplifies the handling
// of asymptotes and does not account for multiple discontinuities within a larger interval.For more complex cases,
// additional logic would be required to segment the interval and handle each segment individually.
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires
// c++17) lambda functions. the variable l and r inherits the precision fromthe call to fmod()
template <FloatLike IT>
inline interval<IT> tan(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    const IT pi = [&]() -> IT {
        return utils::math<IT>::acos(IT(-1));  // More precise pi value
    }();  // The Lambda is immediately invokeed
    const IT twopi(IT(2) * pi);
    IT l(utils::helpers<IT>::fmod(x.inf(), twopi));                // Normalize l within a single period
    IT r(l + utils::helpers<IT>::fmod(x.sup() - x.inf(), twopi));  // Calculate r based on l and the interval width

    // Normalize angles to be within [0, 2*pi)
    if (l < IT(0)) l += twopi;
    if (r >= twopi) r -= twopi;

    // Check if the interval includes a vertical asymptote
    if (x.sup() - x.inf() >= pi ||
        (utils::helpers<IT>::floor((l + pi / IT(2)) / pi) != utils::helpers<IT>::floor((r + pi / IT(2)) / pi))) {
        // The function covers an entire period or crosses an asymptote, range is all real numbers
        interval<IT> res(-infi, infi);
        return res;
    }

    // Calculate tangent values at the interval's endpoints
    IT tan_l(utils::math<IT>::tan(x.inf()));
    IT tan_r(utils::math<IT>::tan(x.sup()));

    // Given the properties of tan(x), if the interval does not include an asymptote,
    // the minimum and maximum can be directly computed from the interval's endpoints.
    IT tan_min(std::min(tan_l, tan_r));
    IT tan_max(std::max(tan_l, tan_r));

    // Established a safety interval around the result to ensure correct bound for the computation
    if (tan_min != pi) tan_min = utils::helpers<IT>::nextafter(tan_min, -infi);
    if (tan_max != pi) tan_max = utils::helpers<IT>::nextafter(tan_max, infi);

    // Create and return the interval based on calculated min and max tangent values
    interval<IT> res(tan_min, tan_max);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// asin(x)
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires
// c++17) lambda functions.
//
template <FloatLike IT>
inline interval<IT> asin(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    // Check if the input interval exceeds the domain of arcsin
    if (x.inf() < IT(-1) || x.sup() > IT(1)) {
        // arcsin is undefined for values outside the interval[-1, 1]
        // return the empty interval and set decoration to ILL
        interval<IT> res;
        res.intervaldecoration(TRV);
        return res;
    }

    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    // Calculate arcsin values at the interval's endpoints
    const IT asin_l(utils::math<IT>::asin(x.inf()));
    const IT asin_r(utils::math<IT>::asin(x.sup()));

    // Ensure the interval is correctly oriented
    IT asin_min(std::min(asin_l, asin_r));
    IT asin_max(std::max(asin_l, asin_r));

    // Established a safety interval around the result to ensure correct bound for the computation
    asin_min = utils::helpers<IT>::nextafter(asin_min, -infi);
    asin_max = utils::helpers<IT>::nextafter(asin_max, infi);

    // Since arcsin is monotonically increasing in its domain, we directly return the interval
    // Create and return the interval based on calculated min and max tangent values
    interval<IT> res(asin_min, asin_max);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// acos(x)
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x.
//
template <FloatLike IT>
inline interval<IT> acos(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    // Check if the input interval exceeds the domain of acos
    if (x.inf() < IT(-1) || x.sup() > IT(1)) {
        // arcsin is undefined for values outside the interval[-1, 1]
        // return the empty interval and set decoration to ILL
        interval<IT> res;
        res.intervaldecoration(ILL);
        return res;
    }

    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    // Calculate acos values at the interval's endpoints
    const IT acos_l(utils::math<IT>::acos(x.sup()));  // Note: we use sup here
    const IT acos_r(utils::math<IT>::acos(x.inf()));  // Note: we use inf here

    // Ensure the interval is correctly oriented
    IT acos_min(std::min(acos_l, acos_r));
    IT acos_max(std::max(acos_l, acos_r));

    // Established a safety interval around the result to ensure correct bound for the computation
    acos_min = utils::helpers<IT>::nextafter(acos_min, -infi);
    acos_max = utils::helpers<IT>::nextafter(acos_max, infi);

    // Since acos is monotonically decreasing in its domain
    // Create and return the interval based on calculated min and max tangent values
    interval<IT> res(acos_min, acos_max);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// atan(x)
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x.
//
template <FloatLike IT>
inline interval<IT> atan(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    // Calculate atan values at the interval's endpoints
    const IT atan_l(utils::math<IT>::atan(x.inf()));
    const IT atan_r(utils::math<IT>::atan(x.sup()));

    // Ensure the interval is correctly oriented
    IT atan_min(std::min(atan_l, atan_r));
    IT atan_max(std::max(atan_l, atan_r));

    // Established a safety interval around the result to ensure correct bound for the computation
    atan_min = utils::helpers<IT>::nextafter(atan_min, -infi);
    atan_max = utils::helpers<IT>::nextafter(atan_max, infi);

    // Since atan is monotonically increasing in its domain, we directly return the interval
    // Create and return the interval based on calculated min and max tangent values
    interval<IT> res(atan_min, atan_max);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Interval sin(), cos(), tan(), asin(), acos(), atan()
///
//////////////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////////////
///
/// Interval sinh(), cosh(), tanh(), asinh(), acosh(), atanh()
///
//////////////////////////////////////////////////////////////////////////////////////

// Use the identity. sinh(x)=0.5*(exp(x)-1/exp(x))
template <FloatLike IT>
inline interval<IT> sinh(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const interval<IT> one(IT(1));           // Ensure correct precision for IT=float_precision
    const interval<IT> half(IT(0.5));        // Ensure correct precision for IT=float_precision
    const interval<IT> e(utils::math<interval<IT>>::exp(x));
    interval<IT> res(half * (e - one / e));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// Use the identity. cosh(x)=0.5*(exp(x)+1/exp(x))
template <FloatLike IT>
inline interval<IT> cosh(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    interval<IT> one(x);                     // Ensure correct precision for IT=float_precision
    interval<IT> half(x);                    // Ensure correct precision for IT=float_precision
    const interval<IT> e(utils::math<interval<IT>>::exp(x));
    one = interval<IT>(1);
    half = interval<IT>(0.5);
    interval<IT> res(half * (e + one / e));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// Use the identity. tanh(x)=(exp(x)-1/exp(x))/(exp(x)+1/exp(x))=(exp(x)^2-1)/(exp(x)^2+1)
template <FloatLike IT>
inline interval<IT> tanh(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const interval<IT> one(1);
    interval<IT> e(utils::math<IT>::exp(x));
    e *= e;
    interval<IT> res((e - one) / (e + one));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// Use the identity. asinh(x)=Ln(x+sqrt(x^2+1))
// asinh(x) is defined for the entire real domain
template <FloatLike IT>
inline interval<IT> asinh(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const interval<IT> one(1);
    interval<IT> xsq(x);
    xsq *= xsq;
    interval<IT> res(utils::math<interval<IT>>::log(x + utils::math<interval<IT>>::sqrt(xsq + one)));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// Use the identity. acosh(x)=Ln(x+sqrt(x^2-1))
// acosh(x) is defined for x>=1
template <FloatLike IT>
inline interval<IT> acosh(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();  // Return the EMPTY interval;
    const interval<IT> one(1);
    interval<IT> xsq(x);
    xsq *= xsq;
    interval<IT> res(utils::math<IT>::log(x + utils::math<IT>::sqrt(xsq - one)));
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    return res;
}

// Use the identity. atanh(x)=0.5*Ln((1+x)/(1-x))
// atanh(x) is defined for -1<x<1
template <FloatLike IT>
inline interval<IT> atanh(const interval<IT>& x) {
    if (x.isEmpty()) return interval<IT>();     // Return the EMPTY interval;
    if (x.sup() < IT(-1) || x.inf() > IT(1)) {  // Outside defined domain, return the empty interval
        // arctanh is undefined for values outside the interval[-1, 1]
        // return the empty interval and set decoration to ILL
        interval<IT> res;
        res.intervaldecoration(ILL);
        return res;
    }
    const IT infi(infinity_interval<IT>());  // infi(INFINITY);
    IT ainf(x.inf());
    IT asup(x.sup());
    // Partial outside?
    ainf = std::max(ainf, IT(-1));
    asup = std::min(asup, IT(1));
    interval<IT> xadjusted(ainf, asup);
    const interval<IT> one(1);
    const interval<IT> half(0.5);
    interval<IT> res(utils::math<interval<IT>>::log((xadjusted + one) / (-xadjusted + one)) * half);
    // set the proper interval decoration
    res.intervaldecoration(x.intervaldecoration());
    if (ainf == IT(-1) || asup == IT(1)) res.intervaldecoration(TRV);
    return res;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Interval sinh(), cosh(), tanh(), asinh(), acosh(), atanh()
///
//////////////////////////////////////////////////////////////////////////////////////

// Sin template class for float or double
template <FloatLike IT>
inline interval<IT> sinsimpel(const interval<IT>& x) {
    const IT infi(infinity_interval<IT>());       // infi(INFINITY);
    const IT pi = utils::math<IT>::acos(IT(-1));  // More precise pi value
    const IT twopi(IT(2) * pi);
    IT l(utils::helpers<IT>::fmod(x.inf(), twopi));                // Normalize l within a single period
    IT r(l + utils::helpers<IT>::fmod(x.sup() - x.inf(), twopi));  // Calculate r based on l and the interval width

    // Normalize angles to be within [0, 2*pi)
    if (l < IT(0)) l += twopi;
    if (r >= twopi) r -= twopi;

    // If the interval width is >= 2pi, the sine function covers the full range [-1, 1]
    if (x.sup() - x.inf() >= twopi) return interval<IT>(IT(-1), IT(1));

    // Calculate sine values at the interval's endpoints
    IT sin_l(utils::math<IT>::sin(l));
    IT sin_r(utils::math<IT>::sin(r));

    // Check for critical points within the interval
    IT sin_min(std::min(sin_l, sin_r));
    IT sin_max(std::max(sin_l, sin_r));
    if (l <= pi / IT(2) && pi / IT(2) <= r) sin_max = IT(1);       // pi/2 is within interval
    if (l <= IT(1.5) * pi && IT(1.5) * pi <= r) sin_min = IT(-1);  // 3*pi/2 is within interval

    // Established a safety interval around the result to ensure correct bound for the computation
    if (sin_min != IT(-1) && sin_min != IT(0)) sin_min = utils::helpers<IT>::nextafter(sin_min, -infi);
    if (sin_max != IT(1) && sin_max != IT(0)) sin_max = utils::helpers<IT>::nextafter(sin_max, infi);

    // Create and return the interval based on calculated min and max sine values
    return interval<IT>(IT(sin_min), IT(sin_max));
}

// Get PI at the precision for IT (float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> pi_interval(const size_t precision) {
    if constexpr (std::is_same<IT, float>::value)
        return interval<IT>(IT(3.141'592'50), IT(3.141'592'74));
    else if constexpr (std::is_same<IT, double>::value)
        return interval<IT>(IT(3.141'592'653'589'793'1), IT(3.141'592'653'589'793'6));
    else if constexpr (std::is_same<IT, long double>::value)
        return interval<IT>(IT(3.141'592'653'589'793'1), IT(3.141'592'653'589'793'6));
#ifdef __MPREAL_H__S
    else if constexpr (std::is_same<IT, mpfr::mpreal>::value)
        return interval<IT>(mpfr::const_pi(),
                            utils::helpers<IT>::nextafter(mpfr::const_pi(), mpfr::const_pi() + mpfr::mpreal(1.0f)));
#endif
#ifdef SHANKS_ENABLE_PROFILING
    else if constexpr (is_profiling<IT>::value) {
        interval<typename IT::value_type> temp = pi_interval<typename IT::value_type>(precision);
        return interval<IT>(utils::cast<IT, typename IT::value_type>()(temp.leftinterval()),
                            utils::cast<IT, typename IT::value_type>()(temp.rightinterval()));
    }
#endif
    else
        static_assert(isFloatLike<IT>::value,
                      "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> e_interval(const size_t precision) {
    if constexpr (std::is_same<IT, float>::value)
        return interval<IT>(IT(2.718'281'75), IT(2.718'281'98));
    else if constexpr (std::is_same<IT, double>::value)
        return interval<IT>(IT(2.718'281'828'459'045'1), IT(2.718'281'828'459'045'5));
    else if constexpr (std::is_same<IT, long double>::value)
        return interval<IT>(IT(2.718'281'828'459'045'1), IT(2.718'281'828'459'045'5));
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<IT, mpfr::mpreal>::value)
        return interval<IT>(mpfr::const_euler(), utils::helpers<IT>::nextafter(
                                                     mpfr::const_euler(), mpfr::const_euler() + mpfr::mpreal(1.0f)));
#endif
#ifdef SHANKS_ENABLE_PROFILING
    else if constexpr (is_profiling<IT>::value) {
        interval<typename IT::value_type> temp = e_interval<typename IT::value_type>(precision);
        return interval<IT>(utils::cast<IT, typename IT::value_type>()(temp.leftinterval()),
                            utils::cast<IT, typename IT::value_type>()(temp.rightinterval()));
    }
#endif
    else
        static_assert(isFloatLike<IT>::value,
                      "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> ln2_interval(const size_t precision) {
    if constexpr (std::is_same<IT, float>::value)
        return interval<IT>(IT(0.693'147'123), IT(0.693'147'182));
    else if constexpr (std::is_same<IT, double>::value)
        return interval<IT>(IT(0.693'147'180'559'945'29), IT(0.693'147'180'559'945'40));
    else if constexpr (std::is_same<IT, long double>::value)
        return interval<IT>(IT(0.693'147'180'559'945'29), IT(0.693'147'180'559'945'40));
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<IT, mpfr::mpreal>::value)
        return interval<IT>(mpfr::const_log2(),
                            utils::helpers<IT>::nextafter(mpfr::const_log2(), mpfr::const_log2() + mpfr::mpreal(1.0f)));
#endif
#ifdef SHANKS_ENABLE_PROFILING
    else if constexpr (is_profiling<IT>::value) {
        interval<typename IT::value_type> temp = ln2_interval<typename IT::value_type>(precision);
        return interval<IT>(utils::cast<IT, typename IT::value_type>()(temp.leftinterval()),
                            utils::cast<IT, typename IT::value_type>()(temp.rightinterval()));
    }
#endif
    else
        static_assert(isFloatLike<IT>::value,
                      "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> ln10_interval(const size_t precision) {
    if constexpr (std::is_same<IT, float>::value)
        return interval<IT>(IT(2.302'584'89), IT(2.302'585'12));
    else if constexpr (std::is_same<IT, double>::value)
        return interval<IT>(IT(2.302'585'092'994'045'5), IT(2.302'585'092'994'045'9));
    else if constexpr (std::is_same<IT, long double>::value)
        return interval<IT>(IT(2.302'585'092'994'045'5), IT(2.302'585'092'994'045'9));
#ifdef __MPREAL_H__
    else if constexpr (std::is_same<IT, mpfr::mpreal>::value)
        return interval<IT>(mpfr::log(mpfr::mpreal("10")),
                            utils::helpers<IT>::nextafter(mpfr::log(mpfr::mpreal("10")),
                                                          mpfr::log(mpfr::mpreal("10")) + mpfr::mpreal(1.0f)));
#endif
#ifdef SHANKS_ENABLE_PROFILING
    else if constexpr (is_profiling<IT>::value) {
        interval<typename IT::value_type> temp = ln10_interval<typename IT::value_type>(precision);
        return interval<IT>(utils::cast<IT, typename IT::value_type>()(temp.leftinterval()),
                            utils::cast<IT, typename IT::value_type>()(temp.rightinterval()));
    }
#endif
    else
        static_assert(isFloatLike<IT>::value,
                      "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// ------------------------------------------------------------------------------------------------------------------
//
// MIXED ARITHMETIC BINARY AND UNARY OPERATORS
//
template <FloatLike IT>
inline interval<IT> atan2(const interval<IT>& y, const interval<IT>& x) {
    using std::atan2;
    if (x.inf() > 0) {
        return interval<IT>(atan2(y.inf(), x.sup()), atan2(y.sup(), x.inf()));
    }
    // Naive fallback for now
    IT mid_y = y.mid();
    IT mid_x = x.mid();
    return interval<IT>(atan2(mid_y, mid_x));
}

template <FloatLike IT>
inline interval<IT> hypot(const interval<IT>& x, const interval<IT>& y) {
    return sqrt(sqr(x) + sqr(y));
}

// Arithmetic operators for integral types mixed arithmetic
template <FloatLike IT, std::integral _X>
inline interval<IT> operator+(const interval<IT>& a, const _X& b) {
    return a + interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator+(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) + b;
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator-(const interval<IT>& a, const _X& b) {
    return a - interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator-(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) - b;
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator*(const interval<IT>& a, const _X& b) {
    return a * interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator*(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) * b;
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator/(const interval<IT>& a, const _X& b) {
    return a / interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline interval<IT> operator/(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) / b;
}

// Boolean operators for integral types mixed arithmetic
template <FloatLike IT, std::integral _X>
inline bool operator==(const interval<IT>& a, const _X& b) {
    return a == interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline bool operator==(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) == b;
}
template <FloatLike IT, std::integral _X>
inline bool operator!=(const interval<IT>& a, const _X& b) {
    return a != interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline bool operator!=(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) != b;
}
template <FloatLike IT, std::integral _X>
inline bool operator>=(const interval<IT>& a, const _X& b) {
    return a >= interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline bool operator>=(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) >= b;
}
template <FloatLike IT, std::integral _X>
inline bool operator<=(const interval<IT>& a, const _X& b) {
    return a <= interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline bool operator<=(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) <= b;
}
template <FloatLike IT, std::integral _X>
inline bool operator>(const interval<IT>& a, const _X& b) {
    return a > interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline bool operator>(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) > b;
}
template <FloatLike IT, std::integral _X>
inline bool operator<(const interval<IT>& a, const _X& b) {
    return a < interval<IT>(b);
}
template <FloatLike IT, std::integral _X>
inline bool operator<(const _X& a, const interval<IT>& b) {
    return interval<IT>(a) < b;
}

template <class _Ty>
inline std::ostream& operator<<(std::ostream& strm, const interval<_Ty>& a) {
    if (a.intervaltype() == EMPTY) return strm << "EMPTY";
    return strm << (a.intervaltype() == LEFT_OPEN || a.intervaltype() == OPEN ? "(" : "[")
                << utils::helpers<_Ty>::to_string(a.leftinterval()) << ","
                << utils::helpers<_Ty>::to_string(a.rightinterval())
                << (a.intervaltype() == RIGHT_OPEN || a.intervaltype() == OPEN ? ")" : "]");
}

// Input operator >>
template <class _Ty>
inline std::istream& operator>>(std::istream& strm, interval<_Ty>& c) {
    _Ty l, u;
    char ch, lbrack = '[', rbrack = ']';
    if (strm >> ch && ch != '[' && ch != '(')
        strm.putback(ch), strm >> l, u = l;
    else {
        lbrack = ch;
        if (strm >> l >> ch && ch != ',') {
            if (ch == ']' || ch == ')') {
                rbrack = ch;
                u = l;
            } else
                strm.putback(ch);  // strm.setstate(std::ios::failbit);
        } else if (strm >> u >> ch && ch != ']' && ch != ')')
            strm.putback(ch);  //, strm.setstate(ios_base::failbit);
        else
            rbrack = ch;
    }

    if (!strm.fail()) {
        enum interval_type t = CLOSE;
        if (lbrack == '(' && rbrack == ')')
            t = OPEN;
        else if (lbrack == '(')
            t = LEFT_OPEN;
        else if (rbrack == ')')
            t = RIGHT_OPEN;
        c = interval<_Ty>(l, u, t);
    }

    return strm;
}
}  // namespace intprec

#endif
