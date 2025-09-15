#ifndef INC_INTERVALPRECISION
#define INC_INTERVALPRECISION

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
 *							Also added the method bool contain() for test if a float or interval is included in the interval
 * 01.07	HVE/JUL-6-2014	Corrected an error in /= for the software emulation of of float & double
 * 01.08	HVE/JUL-13-2014	Added Hardware support for interval arithmetic when applicable. Also fix several errors in the
 *							implementation of sqrt, log, log10, exp and pow functions. Also added new method is_class(), is_empty()
 * 01.09	HVE/JUL-15-2014	Added support for Sin(), Cos() and Tan() interval trigonometric functions.
 * 01.10	HVE/JUL-17-2014	Added support for atan() interval trigonometric function
 * 01.11	HVE/JUL-22-2014 Found a bug that floating point was not reset to near (default by IEEE754) after a hardware supported multiplication
 * 01.12	HVE/JUL-22-2014	Added support for asin() interval trigonometric function
 * 01.13	HVE/JUL-29-2014	Added support for interval versions of LN2, LN10 and PI
 * 01.14	HVE/AUG-10-2014	Added support for mixed mode arithmetic for interval +,- classes
 * 01.15	HVE/JUN-20-2015	Fixed and un-declare variable x when compiling with no interval hardware support
 * 01.16	HVE/Jul-07-2019	Moved Hardware support up prior to the template class definition to make the code more portable and added <iostream> header
 * 01.17    HVE/Jul-07-2019 Make the code more portable to a GCC environment
 * 01.18	HVE/24-Mar-2021 Updated license info
 * 01.19	HVE/4-Jul-2021	Added software interval runding via towsum and twoproduct functions and other functions
 * 01.20	HVE/5-Jul-2021	Replace deprecreated headers with current headers
 * 01.21	HVE/15-Jul-2021 Decpreated hardware support for interval arithmetic since this was not portable and didnt takes advantages of the latest 
 *							Intel instructions set. instead if used only software emaulation of intervals.
 * 01.22	HVE/29-Jul-2021 Corrected bugs in all the trigonometir functions and added interval version of hyperbolic functions
 *							sinh(), cosh(), tanh(), asinh(), acosh(), atanh().
 * 01.23	HVE/30-Jul-2021 Added intervalsection(), unionsection(), boolean precedes(), interior()
 * 02.01	HVE/19-FEB-2024	Rewritten and optimized 
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

#include <limits>
#include <string>
#include <float.h>
#include <algorithm>
#include <utility>
#include <cmath>
#include <sstream>
#include <type_traits>
#include <stdexcept>

#if defined(_MSVC_LANG)
	#if _MSVC_LANG < 201402L
		#error The intervalprecision.h code requires c++14 or higher
	#endif
#else
	#if __cplusplus < 201402L
		#error The intervalprecision.h code requires c++14 or higher
	#endif
#endif

#define PHASE4	// Add support for float_precision
//#define PHASE5	// use simplify interval operations by always convert interval to its closed form and then perform the operation and leave the interval closed

// The eight different interval classification
// # ZERO			a=0 && b=0
// # POSITIVE0		a==0 && b>0
// # POSITIVE1		a>0 && b>0
// # POSITIVE		a>=0 && b>0
// # NEGATIVE0		a<0 && b==0
// # NEGATIVE1		a<0 && b<0
// # NEGATIVE		a<0 && b<=0
// # MIXED			a<0 && b>0
enum int_class { NO_CLASS, ZERO, POSITIVE0, POSITIVE1, POSITIVE, NEGATIVE0, NEGATIVE1, NEGATIVE, MIXED };


// The four different ronding modes
// # ROUND_NEAR  Rounded result is the closest to the infinitely precise result.
// # ROUND_DOWN  Rounded result is close to but no greater than the infinitely precise result.
// # ROUND_UP    Rounded result is close to but no less than the infinitely precise result.
// # ROUND ZERO  Rounded result is close to but no greater in absolute value than the infinitely precise result.
//enum round_mode { ROUND_NEAR, ROUND_UP, ROUND_DOWN, ROUND_ZERO };

// The five different interval types
// # Close   a<=x<=b	[a,b]
// # Left open a<x<=b	(a,b]	same as Right close
// # Right open a<=x<b	[a,b)	same as Left close
// # Open a<x<b			(a,b)
// # EMPTY interval 
enum interval_type { EMPTY, CLOSE, LEFT_OPEN, RIGHT_OPEN, OPEN, RIGHT_CLOSE=LEFT_OPEN, LEFT_CLOSE=RIGHT_OPEN };

// There are 5 decorations values in the IEEE1788 standard
// Common:	COM, bounded, nonempty and the computed interval(fx) is bounded
// Defined & Continuous: DAC, nonempty and continuous
// Defined: DEF, nonempty
// Trivial: TRV, always true, no information is given
// Ill-formed: ILL, Not an Interval (NAI)
// Compute: COMPUTE is just a trigger for that the interval decoration has to be re-computed for the interval
//
enum interval_decoration {COMPUTE, ILL, TRV, DEF, DAC, COM};
// 
// Interval class
// Realistically the class Type can be float, double. Any other type is not supported

// Some needed forward declaration.
template<typename IT> inline IT infinity_interval();
template<typename IT> inline IT underflow_interval();

//
template<class IT> class interval {
	IT left;		// Left interval
	IT right;		// Right interval
	enum interval_type type;  // Interval type CLOSE, OPEN, LEFT_OPEN, RIGHT_OPEN, (RIGHT_CLOSE is synonym for LEFT_OPEN and LEFT_CLOSE same as RIGHT_OPEN
	enum interval_decoration decoration; // Decoration type COM,DAC,DEF,TRV, ILL

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
	std::pair<IT, IT> two_sum(const IT& a, const IT& b)
	{
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
	IT split(const IT& a)
	{
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
	std::pair<IT, IT> two_prod(const IT& a, const IT& b)
	{
		const std::pair<IT, IT> x(split(a));
		const std::pair<IT, IT> y(split(b));
		const IT p(a * b);
		// check for an overflow condition
		const IT t1(abs(p)>ldexp(1,-1021)? (-p*0.5+(x.first*0.5)*y.first)*2.0 : -p+x.first*y.first);
		const IT t2(t1 + x.first * y.second);
		const IT t3(t2 + x.second * y.first);
		const IT err(t3 + x.second * y.second);
		//_IT errold;
		//if (abs(p) > ldexp(1, -1021))  // avoiding overflow
		//	err = x.second * y.second - ((((p * 0.5 - (x.first * 0.5) * y.first) * 2) - x.second * y.first) - x.first * y.second);
		//else
		//	err = x.second * y.second - (((p - x.first * y.first) - x.second * y.first) - x.first * y.second);
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
	static std::pair<IT, IT> fasttwo_sum(const IT& a, const IT& b)
	{
		const IT sum(a + b);
		if (abs(a) > abs(b))
		{
			IT tmp(sum - a);
			IT err(b - tmp);
			if (sum == infinity_interval<IT>()) // If overflow occurs then set err=0
				err = IT(0);
			return std::make_pair(sum, err);
		}
		else
		{
			IT tmp(sum - b);
			IT err(a - tmp);
			if (sum == infinity_interval<IT>()) // If overflow occurs then set err=0
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
	std::pair<IT, IT> fasttwo_prod(const IT& a, const IT& b)
	{
		const IT p(a*b);
		IT err(fma(a, b, -p));

		if (p == infinity_interval<IT>()) // If overflow occurs then set err=0
			err = IT(0);
		if (p != IT(0) && abs(p) < underflow_interval<IT>()) // Test for underflow conditions
		{// Recalculate the err using the scale products
			int aexp, bexp;
			IT ascale(frexp(a, &aexp)); // ascale [1.0,2.0)
			IT bscale(frexp(b, &bexp));// bscale [1.0,2.0)
			IT p2(ascale * bscale );
			err = fma(ascale, bscale, - p2);
		}
		return std::make_pair(p, err);
	}

	public:
		typedef IT value_type;

#ifdef PHASE4
		// Check that this class is only for float or double
		static_assert(
			std::is_floating_point<IT>::value||std::is_same<IT,float_precision>::value,
			"IT must be one of the float types: float, double, long double or float_precision"
			);
#else
		// Check that this class is only for float or double
		static_assert(
			std::is_floating_point<IT>::value,
			"IT must be one of the float types: float, double, or long double"
			);
#endif

		// constructor. zero, one or two arguments for type IT
		interval();				// Empty interval
		interval(const IT&);	// Singleton interval
		// Regular interval with interval_type (default CLOSE)
		interval(const IT&, const IT&, const enum interval_type t=CLOSE);
	  
		// Constructor for mixed type IT != _X (base types). Allows auto construction of e.g. interval<double> x(float)
		// Notice that the phase one constructor above is still valid when both the interval type IT and the argument 
		// is also of the same type
		template<typename _X> interval(const _X&); // Singleton interval
		// Regular interval with interval_type (default CLOSE)
		template<typename _X, typename _Y> interval(const _X& , const _Y&, const enum interval_type t=CLOSE);
		// constructor for any other type to IT. Both up and down conversions are possible
		// constructor for an interval<_X> argument
		template<typename _X> interval(const interval<_X>& a); 
	 
		// Coordinate functions. 
		IT rightinterval() const;			// Return rightinterval bound
		IT leftinterval() const;			// Return leftinterval bound
		IT rightinterval(const IT&);		// Set and return rightinterval bound
		IT leftinterval(const IT&);			// Set and return leftinterval bound
		enum interval_type intervaltype() const;	// Return interval type
		enum interval_type intervaltype(const enum interval_type); // Set and return interval type
		enum interval_decoration intervaldecoration() const; //Return the decoration information
		enum interval_decoration intervaldecoration(const enum interval_decoration); // Set and return decoration information
		

		// IEEE1788 standard functions
		IT inf(bool toclose=false) const;	// Return infimum of interval 
		IT sup(bool toclose=false) const;	// Return supremum of interval
		IT mid() const;	// Return midpoint of interval
		IT rad() const;	// Return radius of interval	
		IT wid() const;	// Return width of interval
		IT mig() const;	// Return Mignitude. inf(|x|)
		IT mag() const;	// Return Magnitude. sup(|x|)
		
		// Is methods as required per IEEE 1788 standard
		bool isEmpty() const;
		bool isEntire() const;
		bool isPoint() const;
		bool isImproper() const;
		bool isProper() const;
	
		bool in(const IT& i);	// Check if an pointis within the interval

		// Miscellaneous but usefull coordinate functions
		enum int_class isClass() const;
		std::string toString() const;	// Convert interval to a string
		
		// Conversion Operators
		operator short() const;				 
		operator int() const;			
		operator long() const;			 
		operator long long() const;		 
		operator unsigned short() const;	 
		operator unsigned int() const;	
		operator unsigned long() const;	
		operator unsigned long long() const;
		operator double() const;			 
		operator float() const;	
		operator float_precision() const;

		// Essential operators
		interval<IT>& operator= ( const interval<IT>& );
		interval<IT>& operator+=( const interval<IT>& );
		interval<IT>& operator-=( const interval<IT>& );
		interval<IT>& operator*=( const interval<IT>& );
		interval<IT>& operator/=( const interval<IT>& );
		interval<IT>& operator&=( const interval<IT>& ); // Intersection
		interval<IT>& operator|=( const interval<IT>& ); // Union
		interval<IT>& operator^=( const interval<IT>& ); // minus intersection

		// Friends
		//friend std::ostream& operator<<(std::ostream& strm, interval<IT>& a);

		// Exception class. Not used
		class out_of_range : public std::logic_error {
		public:
			explicit out_of_range(const std::string& message)
				: std::logic_error(message) {}
		};

		class divide_by_zero : public std::logic_error {
		public:
			explicit divide_by_zero(const std::string& message)
				: std::logic_error(message) {}
		};
		class domain_error : public std::logic_error {
		public:
			explicit domain_error(const std::string& message)
				: std::logic_error(message) {}
		};

		//class bad_int_syntax	{};
		//class bad_float_syntax	{};
		//class out_of_range		{};
		//class divide_by_zero	{};
		//class domain_error		{};
		//class base_error		{};
   };


// Unary and Binary arithmetic
// Arithmetic +,-,*,/ Binary and Unary
template<class IT> interval<IT> operator+( const interval<IT>&, const interval<IT>& ); 
template<class IT> interval<IT> operator+( const interval<IT>& );	// Unary
template<class IT> interval<IT> operator-( const interval<IT>&, const interval<IT>& );
template<class IT> interval<IT> operator-( const interval<IT>& );	// Unary 
template<class IT> interval<IT> operator*(const interval<IT>&, const interval<IT>&);
template<class IT> interval<IT> operator/( const interval<IT>&, const interval<IT>& );

// Boolean Comparison Operators
template<class IT> bool operator==(const interval<IT>&, const interval<IT>&);
template<class IT> bool operator!=(const interval<IT>&, const interval<IT>&);
template<class IT> bool operator>=(const interval<IT>&, const interval<IT>&);
template<class IT> bool operator<=(const interval<IT>&, const interval<IT>&);
template<class IT> bool operator>(const interval<IT>&, const interval<IT>&);
template<class IT> bool operator<(const interval<IT>&, const interval<IT>&);

// Other functions
template<class IT> interval<IT> abs(const interval<IT>&);
template<class IT> IT intervaldistance(const interval<IT>&, const interval<IT>&);
inline enum interval_type compute_interval_type(const enum interval_type, const enum interval_type);

// Elementary functions
template<class IT> inline interval<IT> sqr(const interval<IT>&);	// x^2
template<class IT> inline interval<IT> sqrt(const interval<IT>&);	// sqrt(x)

// Arithmetic binary and monadic operators for mixed arithmetic
template <class IT, class _X> interval<IT> operator+( const interval<IT>&, const _X&);
template <class IT, class _X> interval<IT> operator+( const _X&, const interval<IT>&);
template <class IT, class _X> interval<IT> operator-(const interval<IT>&, const _X&);
template <class IT, class _X> interval<IT> operator-(const _X&, const interval<IT>&);
template <class IT, class _X> interval<IT> operator*(const interval<IT>&, const _X&);
template <class IT, class _X> interval<IT> operator*(const _X&, const interval<IT>&);
template <class IT, class _X> interval<IT> operator/(const interval<IT>&, const _X&);
template <class IT, class _X> interval<IT> operator/(const _X&, const interval<IT>&);
 
// Boolean operators for mixed arithmetic
template <class IT, class _X> bool operator==(const interval<IT>&, const _X&);
template <class IT, class _X> bool operator==(const _X&, const interval<IT>&);
template <class IT, class _X> bool operator==(const interval<IT>&, const interval<_X>&);
template <class IT, class _X> bool operator!=(const interval<IT>&, const _X&);
template <class IT, class _X> bool operator!=(const _X&, const interval<IT>&);
template <class IT, class _X> bool operator!=(const interval<IT>&, const interval<_X>&);
template <class IT, class _X> bool operator>=(const interval<IT>&, const _X&);
template <class IT, class _X> bool operator>=(const _X&, const interval<IT>&);
template <class IT, class _X> bool operator>=(const interval<IT>&, const interval<_X>&);
template <class IT, class _X> bool operator<=(const interval<IT>&, const _X&);
template <class IT, class _X> bool operator<=(const _X&, const interval<IT>&);
template <class IT, class _X> bool operator<=(const interval<IT>&, const interval<_X>&);
template <class IT, class _X> bool operator>(const interval<IT>&, const _X&);
template <class IT, class _X> bool operator>(const _X&, const interval<IT>&);
template <class IT, class _X> bool operator>(const interval<IT>&, const interval<_X>&);
template <class IT, class _X> bool operator<(const interval<IT>&, const _X&);
template <class IT, class _X> bool operator<(const _X&, const interval<IT>&);
template <class IT, class _X> bool operator<(const interval<IT>&, const interval<_X>&);

template<class IT> inline bool subset(const interval<IT>&, const interval<IT>&);
template<class IT> inline bool interior(const interval<IT>&, const interval<IT>&);
template<class IT> inline bool precedes(const interval<IT>&, const interval<IT>&);
template<class IT> inline int inclusion(const interval<IT>&, const interval<IT>&);
template<class IT> inline std::pair<interval<IT>, interval<IT> > join(const interval<IT>&, const interval<IT>&);
template<class IT> inline interval<IT> intersection(const interval<IT>&, const interval<IT>&);

template<class IT> inline bool empty(const interval<IT>&);
template<class IT> inline bool entire(const interval<IT>&);

/////////////////////////////////////////////////////////////////////////////////////
//
// Manifest Interval Constants like PI, e, LN2 and LN10
// and infinity.
//	PI=3.14159265358979323846264
//	e=2.71828182845904523536
//	ln2=0.69314718055994530942
//	ln10=2.30258509299404568402
// 
/////////////////////////////////////////////////////////////////////////////////////

#ifdef PHASE4
// Inifinity declaration for the various types
template<typename T> inline T infinity_interval();
// Specialization for the infinity for float, double and float_precision
template<> inline float infinity_interval<float>() { return std::numeric_limits<float>::infinity(); }
template<> inline double infinity_interval<double>() { return std::numeric_limits<double>::infinity(); }
//template<> inline long double infinity_interval<long double>() { return std::numeric_limits<long double>::infinity(); }
template<> inline float_precision infinity_interval<float_precision>() { return FP_INFINITY; }

// Underflow declaration for the various types
template<typename T> inline T underflow_interval();
// Specialization for the underflow minimum for float, double and float_precision
template<> inline float underflow_interval<float>() { return FLT_MIN; }
template<> inline double underflow_interval<double>() { return DBL_MIN; }
template<> inline float_precision underflow_interval<float_precision>() { return float_precision(0); }

// Get PI at the precision for IT (float_precision also based on the precision)
template<typename IT> constexpr interval<IT> pi_interval(const size_t precision = float_precision_ctrl.precision())
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(3.141'592'50), IT(3.141'592'74));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(3.141'592'653'589'793'1), IT(3.141'592'653'589'793'6));
	else if  (std::is_same<IT, float_precision> ::value)
	{
		float_precision pi(0, precision, ROUND_DOWN); 
		// Get PI with one higher decimal accuracy to be able to get left side of interval correctly
		pi=_float_table(_PI, precision+1); 
		return interval<float_precision>(pi, nextafter(pi,FP_INFINITY));
	}
	else
		static_assert(std::is_floating_point<IT>::value||std::is_same<IT,float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template<typename IT> constexpr interval<IT> e_interval(const size_t precision = float_precision_ctrl.precision())
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(2.718'281'75), IT(2.718'281'98));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(2.718'281'828'459'045'1), IT(2.718'281'828'459'045'5));
	else if (std::is_same<IT, float_precision> ::value)
	{
		float_precision e1(0, precision, ROUND_DOWN);
		// Get e with one higher decimal accuracy to be able to get left side of interval correctly
		e1 = _float_table(_EXP1, precision + 1);
		return interval<float_precision>(e1, nextafter(e1, FP_INFINITY));
	}
	else
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template<typename IT> constexpr interval<IT> ln2_interval(const size_t precision = float_precision_ctrl.precision())
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(0.693'147'123), IT(0.693'147'182));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(0.693'147'180'559'945'29), IT(0.693'147'180'559'945'40));
	else if (std::is_same<IT, float_precision> ::value)
	{
		float_precision ln2(0, precision, ROUND_DOWN);
		// Get LN2 with one higher decimal accuracy to be able to get left side of interval correctly
		ln2 = _float_table(_LN2, precision + 1);
		return interval<float_precision>(ln2, nextafter(ln2, FP_INFINITY));
	}
	else
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template<typename IT> constexpr interval<IT> ln10_interval(const size_t precision = float_precision_ctrl.precision())
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(2.302'584'89), IT(2.302'585'12));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(2.302'585'092'994'045'5), IT(2.302'585'092'994'045'9));
	else if (std::is_same<IT, float_precision> ::value)
	{
		float_precision ln10(0, precision, ROUND_DOWN);
		// Get LN2 with one higher decimal accuracy to be able to get left side of interval correctly
		ln10 = _float_table(_LN10, precision + 1);
		return interval<float_precision>(ln10, nextafter(ln10, FP_INFINITY));
	}
	else
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, long double or float_precision.");
}

#else

// Inifinity declaration for the various types
template<typename T> inline T infinity_interval();
// Specialization for the infinity for float, double
template<> inline float infinity_interval<float>() { return std::numeric_limits<float>::infinity(); }
template<> inline double infinity_interval<double>() { return std::numeric_limits<double>::infinity(); }

// Underflow declaration for the various types
template<typename T> inline T underflow_interval();
// Specialization for the minimum value for float, double
template<> inline float underflow_interval<float>() { return FLT_MIN; }
template<> inline double underflow_interval<double>() { return DBL_MIN; }

// Get PI at the precision for IT (float_precision also based on the precision)
template<typename IT> constexpr interval<IT> pi_interval()
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(3.141'592'50), IT(3.141'592'74));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(3.141'592'653'589'793'1), IT(3.141'592'653'589'793'6));
	else 
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, or long double.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template<typename IT> constexpr interval<IT> e_interval()
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(2.718'281'75), IT(2.718'281'98));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(2.718'281'828'459'045'1), IT(2.718'281'828'459'045'5));
	else 
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, or long double.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template<typename IT> constexpr interval<IT> ln2_interval()
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(0.693'147'123), IT(0.693'147'182));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(0.693'147'180'559'945'29), IT(0.693'147'180'559'945'40));
	else 
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, or long double.");
}

// Get e at the precision for IT.(float_precision also based on the precision)
template<typename IT> constexpr interval<IT> ln10_interval()
{
	if constexpr (std::is_same<IT, float>::value)
		return interval<IT>(IT(2.302'584'89), IT(2.302'585'12));
	else if constexpr (std::is_same<IT, double>::value)
		return interval<IT>(IT(2.302'585'092'994'045'5), IT(2.302'585'092'994'045'9));
	else
		static_assert(std::is_floating_point<IT>::value || std::is_same<IT, float_precision>::value, "Unsupported type for pi_interval.Type must be float, double, or long double.");
}

#endif

/////////////////////////////////////////////////////////////////////////////////////
//
// END of Interval constants
// 
/////////////////////////////////////////////////////////////////////////////////////

template<class IT> inline interval<IT> floor(const interval<IT>&);	// floor(x)
template<class IT> inline interval<IT> ceil(const interval<IT>&);	// ceil(x)
template<class IT> inline interval<IT> sgn(const interval<IT>&);		// sqn(x)
template<class IT> inline interval<IT> log(const interval<IT>&);		// log(x)
template<class IT> inline interval<IT> log10(const interval<IT>&);	// log10(x)
template<class IT> inline interval<IT> exp(const interval<IT>&);		// exp(x)
template<class IT> inline interval<IT> pow(const interval<IT>&, const interval<IT>&);	// pow(x,y)==x^y

// Trigonometric functions
template<class IT> inline interval<IT> sin(const interval<IT>&);		// sin(x)
template<class IT> inline interval<IT> cos(const interval<IT>&);		// cos(x)
template<class IT> inline interval<IT> tan(const interval<IT>&);		// tan(x)
template<class IT> inline interval<IT> asin(const interval<IT>&);	// asin(x)
template<class IT> inline interval<IT> acos(const interval<IT>&);	// acos(x)
template<class IT> inline interval<IT> atan(const interval<IT>&);	// atan(x)

// Hyperbolic functions
template<class IT> inline interval<IT> sinh(const interval<IT>&);	// sin(x)
template<class IT> inline interval<IT> cosh(const interval<IT>&);	// cos(x)
template<class IT> inline interval<IT> tanh(const interval<IT>&);	// tan(x)
template<class IT> inline interval<IT> asinh(const interval<IT>&);	// asin(x)
template<class IT> inline interval<IT> acosh(const interval<IT>&);	// acos(x)
template<class IT> inline interval<IT> atanh(const interval<IT>&);	// atan(x)


/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// END of PHASE 2 and Phase 3 declartion
//
/////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// cin and cout operators
//
/////////////////////////////////////////////////////////////////////////////

// Output Operator <<
//
template<class _Ty> inline std::ostream& operator<<(std::ostream& strm, interval<_Ty>& a)
{
	if (a.intervaltype() == EMPTY)
		return strm << "EMPTY";
	return strm << (a.intervaltype() == LEFT_OPEN || a.intervaltype() == OPEN ? "(" : "[") << a.leftinterval() << "," << a.rightinterval() << (a.intervaltype() == RIGHT_OPEN || a.intervaltype() == OPEN ? ")" : "]"); }

// Input operator >>
//
template<class _Ty> inline std::istream& operator>>( std::istream& strm, interval<_Ty>& c ) 
   {
   _Ty l, u; char ch, lbrack='[', rbrack=']';
   if( strm >> ch && ch != '[' && ch != '(')
      strm.putback(ch), strm >> l, u = l;
   else
   {
	   lbrack = ch;
	   if (strm >> l >> ch && ch != ',')
	   {
		   if (ch == ']' || ch == ')')
		   {
			   rbrack = ch;
			   u = l;
		   }
		   else
			   strm.putback(ch); // strm.setstate(std::ios::failbit);
	   }
	   else
		   if (strm >> u >> ch && ch != ']' && ch != ')')
			   strm.putback(ch); //, strm.setstate(ios_base::failbit);
		   else
			   rbrack = ch;
   }
	
   if (!strm.fail())
   {
	   enum interval_type t = CLOSE;
	   if (lbrack == '(' && rbrack == ')')
		   t = OPEN;
	   else if (lbrack == '(')
		   t = LEFT_OPEN;
	   else if (rbrack == ')')
		   t = RIGHT_OPEN;
	   c = interval<_Ty>(l,u,t);
   }

   return strm;
   }

//////////////////////////////////////////////////////////////////////////////////////
///
/// BEGIN Constructors
///
//////////////////////////////////////////////////////////////////////////////////////

// Construct an empty interval
template<class IT> inline  interval<IT>::interval() 
	:left(IT(0)), right(IT(0)), type(EMPTY), decoration(TRV) {}	// Set EMPTY interval type

// Construct a singleton interval
// Since IT is either float or double and the argument is of the same type 
// we can't catch any conversion error for up or down conversion of the argument
template<class IT> inline  interval<IT>::interval(const IT& d)
	: left(d), right(d), type(CLOSE), decoration(COM) {}	// Default is closed type

// Construct a regular interval
// Since IT is either float or double and the argument is of the same type 
// we can't catch any conversion error for up or down conversion of the argument
template<class IT> inline  interval<IT>::interval(const IT& l, const IT& h, const enum interval_type t)
	: left(l), right(h), type(t), decoration(COM) {}

// Constructor for creating mixed-type intervals when IT != _X (base types), enabling automatic
// construction of intervals from different types (e.g., interval<double> x = float).
// For initializations with integral types, check the value against the max without loss of precision,
// adjust the interval to ensure it fits within the bounds of float or double values.
// If float or double limits are exceeded, sets the left interval to the correct lower bound,
// while the right interval is adjusted accordingly.
// Note: This template constructor is preferred over the simpler constructor
template<class IT> template<typename _X> inline interval<IT>::interval(const _X& x)  
{
	const bool isIntegral = std::is_integral<_X>::value;
	const bool isTargetFloat = std::is_same<IT, float>::value;
	const bool isSourceDoubleOrLongDouble = std::is_same<_X, double>::value || std::is_same<_X, long double>::value;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);

	// up promoting is accurate
	left = IT(x);
	right = IT(x);
	if(isTargetFloat&& isSourceDoubleOrLongDouble)
	{	// Downpromoting from double to float. 
		// Uppromotion from float to double is always accurate
		auto adjustBoundaries = [&](const _X& val) 
		{
			_X e = val - _X(left);
			if (e < _X(0)) left = nextafter(left, -infi);
			e = val - _X(right);
			if (e > _X(0)) right = nextafter(right, +infi);
		};
		adjustBoundaries(x);
	}
	if (isIntegral)
	{	// Handle integer promotion to IT
		const intmax_t absX = intmax_t(abs(x));
		auto maxFloat = 16'777'216; // 2^24
		auto maxDouble = 9'007'199'254'740'992; // 2^53
		bool exceedsFloat = isTargetFloat && absX > maxFloat;
		bool exceedsDouble = !isTargetFloat && absX > maxDouble;

		if (exceedsFloat || exceedsDouble)
		{
			_X e = x - _X(left);
			if (e > _X(0)) right = nextafter(right, +infi);
			if (e < _X(0)) left = nextafter(left, -infi);
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
template<class IT> template<typename _X, typename _Y> inline interval<IT>::interval(const _X& x, const _Y& y, const enum interval_type t)
{
	const interval<IT> ll(x);	// Call mixed singleton constructor
	const interval<IT> rr(y);	// Call mixed singleton constructor

	left = std::min(ll.inf(), rr.inf());
	right = std::max(ll.sup(), rr.sup());
	type = t;
	decoration = COM;
	//if x>y originally was improper then return it as an improper interval
	if (IT(x) > IT(y))
		std::swap(left, right);
	// Recalculate the decoration type to match the interval
	this->intervaldecoration(COMPUTE);
	return;
}

// constructor for any other interval<_X> to interval<IT>. 
// e.g. interval<float> i1(2,3); 
// interval<float> i2(i1);
template<class IT> template<typename _X> inline interval<IT>::interval(const interval<_X>& a) //
{
	// Call two argument mixed constructor
	const interval<IT> x(a.leftinterval(), a.rightinterval(), a.intervaltype());
	left=x.left;
	right= x.right;
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
template<class IT> inline interval<IT>::operator short() const
{	// Conversion to short 	
	return static_cast<short>(mid());
}
template<class IT> inline interval<IT>::operator int() const
{	// Conversion to int 	
	return static_cast<int>(mid());
}
	 
template<class IT> inline interval<IT>::operator long() const
{	// Conversion to long 	
	return static_cast<long>(mid());
}
template<class IT> inline interval<IT>::operator long long() const
{	// Conversion to long long 	
	return static_cast<long long>(mid());
}

template<class IT> inline interval<IT>::operator unsigned short() const
{	// Conversion to unsigned short 	
	return static_cast<unsigned short>(mid());
}
template<class IT> inline interval<IT>::operator unsigned int() const
{	// Conversion to unsgined int 	
	return static_cast<unsigned int>(mid());
}

template<class IT> inline interval<IT>::operator unsigned long() const
{	// Conversion to unsigned long 	
	return static_cast<unsigned long>(mid());
}

template<class IT> inline interval<IT>::operator unsigned long long() const
{	// Conversion to long long 	
	return static_cast<unsigned long long>(mid());
}

template<class IT> inline interval<IT>::operator double() const
{	// Conversion to double
	return static_cast<double>(mid());
}
template<class IT> inline interval<IT>::operator float() const
{	// Conversion to float
	return static_cast<float>(mid());
}

template<class IT> inline interval<IT>::operator float_precision() const
{	// Conversion to float
	return static_cast<float_precision>(mid());
}

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
template<class IT> inline IT interval<IT>::rightinterval() const 
{ return right; }

// Return the left interval "as is"
template<class IT> inline IT interval<IT>::leftinterval() const 
{ return left; }

// Set the right interval "as is". 
// If interval is empty set the interval type to CLOSE
template<class IT> inline IT interval<IT>::rightinterval(const IT& r) 
{ 
	if (type == EMPTY)
		type = CLOSE;
	right = r;
	return right;
}

// Set the left interval "as is". 
// If interval is empty set the interval type to CLOSE
template<class IT> inline IT interval<IT>::leftinterval(const IT& l)
{ 	
	if (type == EMPTY)
		type = CLOSE;
	left = l;
	return left;
}

// Return the current intervaltype
template<class IT> inline enum interval_type interval<IT>::intervaltype() const 
{ return type; }

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
template<class IT> inline enum interval_type interval<IT>::intervaltype(const enum interval_type to)
{
	interval<IT> x = *this;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);

	if (x.type != to)
	{
		if (this->isImproper())
		{	// If improper swap the interval and switch the half open intervals
			std::swap(x.left, x.right);
			if (x.type == LEFT_OPEN)
				x.type = RIGHT_OPEN;
			else
				if (x.type == RIGHT_OPEN)
					x.type = LEFT_OPEN;
		}

		switch (x.type)
		{
		case CLOSE: 
			// if the interval is already CLOSE then do nothing
			if (to == LEFT_OPEN || to == OPEN)
				x.left = nextafter(x.left, -infi);
			if (to == RIGHT_OPEN || to == OPEN)
				x.right = nextafter(x.right, +infi);
			break;
		case OPEN: 
			// if the interval is already Open then do nothing
			if (to == RIGHT_OPEN || to == CLOSE)
				x.left = nextafter(x.left, +infi);
			if (to == LEFT_OPEN || to == CLOSE)
				x.right = nextafter(x.right, -infi);
			break;
		case LEFT_OPEN: 
			// If the interval is already RIGHT_CLOSE same as LEFT_OPEN then do nothing
			if (to == RIGHT_OPEN || to == CLOSE)
				x.left = nextafter(x.left, +infi);
			if (to == RIGHT_OPEN || to == OPEN)
				x.right = nextafter(x.right, +infi);
			break;
		case RIGHT_OPEN:	
			// If the interval is already LEFT_CLOSE then do nothing
			if (to == LEFT_OPEN || to == OPEN)
				x.left = nextafter(x.left, -infi);
			if (to == LEFT_OPEN || to == CLOSE)
				x.right = nextafter(x.right, -infi);
			break;
		}
		x.type = to;
		if (this->isImproper())
		{
			std::swap(x.left, x.right);
		}
		*this = x;
	}
	return x.type;
}

// Return the current interval decoration
template<class IT> inline enum interval_decoration interval<IT>::intervaldecoration() const
{
	return decoration;
}

// Set and  Return the interval decoration
template<class IT> inline enum interval_decoration interval<IT>::intervaldecoration(const enum interval_decoration to)
{	
	decoration = to;
	if(to==COMPUTE)
	{ // Compute the decoration type
		if (type == EMPTY)
			decoration = TRV;
		else if (isfinite(left) && isfinite(right))
			decoration = COM; // is bounded
		else if( isnan(left)||isnan(right))
			decoration=ILL;	// ill formed e.g. one or both intervals is NAN
		else decoration = DAC;	// Must be unbounded, but otherwise good
	}
	return decoration;
}

// compute the infimum(greatest lower bound) of an interval, taking into account the type of interval
// (closed, open, left-open, or right-open) and whether the interval is proper 
// (left endpoint is less than or equal to right endpoint) or improper(left endpoint is greater than the right endpoint).
template<class IT> inline IT interval<IT>::inf(bool toclose) const
{
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	if (isEmpty())	// If empty return +infinity
		return +infi;
	// For a closed interval, directly return the minimum of left and right.
#ifdef PHASE5
	if(!toclose)
		return min(left, right);
#endif
	if (type == CLOSE) 
		return std::min(left, right);

	IT adjustedLeft = left;
	IT adjustedRight = right;
	
	// Adjust left boundary for open intervals
	if (type == LEFT_OPEN || type == OPEN) 
		adjustedLeft = nextafter(left, (left <= right) ? +infi : -infi);

	// Adjust right boundary for open intervals, taking into account improper intervals
	if (type == RIGHT_OPEN || type == OPEN) 
		adjustedRight = nextafter(right, (left <= right) ? -infi: +infi);

	// Return the minimum of the adjusted boundaries
	return std::min(adjustedLeft, adjustedRight);
}

// Optimizing the function for calculating the supremum(least upper bound) of an interval follows
// a similar approach to optimizing the infimum calculation
template<class IT> inline IT interval<IT>::sup(bool toclose) const 
{
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	if (isEmpty())	// If empty return -infinity
		return -infi;
	// For a closed interval, directly return the maximum of left and right.
#ifdef PHASE5
	if(!toclose)
		return max(left, right);
#endif
	if (type == CLOSE)
		return std::max(left, right);

	IT adjustedLeft = left;
	IT adjustedRight = right;

	// Adjust left boundary for open intervals
	if (type == LEFT_OPEN || type == OPEN)
		adjustedLeft = nextafter(left, (left <= right) ? +infi : -infi);

	// Adjust right boundary for open intervals, considering proper and improper intervals
	if (type == RIGHT_OPEN || type == OPEN)
		adjustedRight = nextafter(right, (left <= right) ? -infi : +infi);

	// Return the maximum of the adjusted boundaries
	return std::max(adjustedLeft, adjustedRight);
}


// Return interval midpoint, computed as the interval is closed to ensure correct computation
// if empty return no value
template<class IT> inline IT interval<IT>::mid() const { 
	if (isEmpty())
		return FP_QUIET_NAN;
	if (right == left) return left; else  return (right + left) / IT(2); 
}

// Return interval radius
// Notice that radius is negative for improper intervals
template<class IT> inline IT interval<IT>::rad() const { 
	if (isEmpty())
		return FP_QUIET_NAN; 
	IT r((right - left) / IT(2));
	return r;
}

// Return interval width 
template<class IT> inline IT interval<IT>::wid() const {
	if (isEmpty())
		return FP_QUIET_NAN; 
	IT r(right - left);
	if (r < IT(0)) r = -r; 
	return r;
}

// Return mignitude of class
template<class IT> inline IT interval<IT>::mig() const {
	if (isEmpty())
		return FP_QUIET_NAN;
	const IT l(abs(inf()));
	const IT r(abs(sup()));
	return min(l, r);
}

// Return magnitude of interval
template<class IT> inline IT interval<IT>::mag() const {
	if (isEmpty())
		return FP_QUIET_NAN;
	const IT l(abs(inf()));
	const IT r(abs(sup()));
	return max(l, r);
}

// Required is... methods
template<class IT> inline bool interval<IT>::isProper() const 
{ return left<=right; }
template<class IT> inline bool interval<IT>::isImproper() const 
{ return right<left; }
template<class IT> inline bool interval<IT>::isPoint() const 
{ return left==right; }
template<class IT> inline bool interval<IT>::isEmpty() const 
{ return type==EMPTY; }
template<class IT> inline bool interval<IT>::isEntire() const
{
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	return left==-infi && right==+infi;
}

// Return interval classification
template<class IT> inline enum int_class interval<IT>::isClass() const 
{
	if (left == IT(0) && right == IT(0)) return ZERO;
	if (left == IT(0) && right >  IT(0)) return POSITIVE0;
	if (left >  IT(0) && right >  IT(0)) return POSITIVE1;
	if (left >= IT(0) && right >  IT(0)) return POSITIVE;
	if (left <  IT(0) && right == IT(0)) return NEGATIVE0;
	if (left <  IT(0) && right <  IT(0)) return NEGATIVE1;
	if (left <  IT(0) && right <= IT(0)) return NEGATIVE;
	if (left <  IT(0) && right >  IT(0)) return MIXED;
	return NO_CLASS;
}

// Check if a point p is within the interval bounds
template<class IT> inline bool interval<IT>::in(const IT& p)
{
	return inf() <= p && p <= sup();
}


// Return the interval as a String representation
template<class IT> inline std::string interval<IT>::toString() const
{
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
template<class IT> inline interval<IT>& interval<IT>::operator=( const interval<IT>& rhs )
{
	left = rhs.left;
	right = rhs.right;
	type = rhs.type;
	decoration = rhs.decoration;
	return *this;
}

#ifdef PHASE4
// This is where the specilization for float_precision replace the generic template for +,-,*,/

// += operator. Specilization for float_precision class
// Always return an "proper" and closed [] interval
// a:=a+[EMPTY] or b:=[EMPTY]+b or [EMPTY]:=[EMPTY]+[EMPTY]
template <>
inline interval<float_precision>& interval<float_precision>::operator+=(const interval<float_precision>& rhs)
{
	// Handle EMPTY interval first
	if (rhs.type == EMPTY)
		return *this;
	if (type == EMPTY)
		return (*this = rhs);

	const float_precision infi(infinity_interval<float_precision>());// infi(INFINITY);
	const float_precision c0(0);
	// Get maximum precision of operand a and b
	const size_t max_prec(std::max(
		std::max(left.precision(), left.precision()),
		std::max(rhs.leftinterval().precision(), rhs.rightinterval().precision())
	));
	// Neither a or b is [EMPTY]
	std::pair<float_precision, float_precision> xleft = fasttwo_sum(inf(), rhs.inf());
	std::pair<float_precision, float_precision> xright = fasttwo_sum(sup(), rhs.sup());
	// Any adjustment?
	if (xleft.second < c0 )
		xleft.first = nextafter(xleft.first, -infi);
	if (xright.second > c0)
		xright.first = nextafter(xright.first, +infi);
	left.precision(max_prec);
	right.precision(max_prec);
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
	if (abs(left) == infi || abs(right) == infi)
		decoration = std::min(decoration, DAC);
	return *this;
}

// -= operator. Specilization for float_precision class
// Always return an "proper" and closed [] interval
// a=a-[EMPTY] or -b=[EMPTY]-b or [EMPTY]=[EMPTY]-[EMPTY]
//
template <>
inline interval<float_precision>& interval<float_precision>::operator-=(const interval<float_precision>& rhs)
{
	// Handle EMPTY interval first
	if (rhs.type == EMPTY)
		return *this;
	if (type == EMPTY)
		return (*this = -rhs);

	const float_precision infi(infinity_interval<float_precision>());// infi(INFINITY);
	const float_precision c0(0);
	// Get maximum precision of operand a and b
	const size_t max_prec(std::max(
		std::max(left.precision(), left.precision()),
		std::max(rhs.leftinterval().precision(), rhs.rightinterval().precision())
	));
	// Neither a or b is [EMPTY]
	std::pair<float_precision, float_precision> xleft = fasttwo_sum(inf(), -rhs.sup());
	std::pair<float_precision, float_precision> xright = fasttwo_sum(sup(), -rhs.inf());
	if (xleft.second < c0)
		xleft.first = nextafter(xleft.first, -infi);
	if (xright.second > c0)
		xright.first = nextafter(xright.first, +infi);
	left.precision(max_prec);
	right.precision(max_prec);
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
	if (abs(left) == infi || abs(right) == infi)
		decoration = std::min(decoration, DAC);
	return *this;
}


// specilization for the float_precision class. 
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
template <>
inline interval<float_precision>& interval<float_precision>::operator*=(const interval<float_precision>& rhs)
{
	// Handle EMPTY interval first  ∅
	if (type == EMPTY)
		return *this;
	if (rhs.type == EMPTY)
		return (*this = rhs);

	const float_precision c0(0);
	const float_precision infi(infinity_interval<float_precision>());// infi(INFINITY);
	// Get maximum precision of operand a and b
	const size_t max_prec(std::max(
		std::max(left.precision(), left.precision()),
		std::max(rhs.leftinterval().precision(), rhs.rightinterval().precision())
	));
	// Neither a or b is ∅
	// Extract intervals through inf() and sup()
	float_precision al(inf());
	float_precision ar(sup());
	float_precision bl(rhs.inf());
	float_precision br(rhs.sup());
	left.precision(max_prec);
	right.precision(max_prec);
	al.precision(max_prec);
	ar.precision(max_prec);
	bl.precision(max_prec);
	br.precision(max_prec);

	auto multiply = [&](const float_precision& x, const float_precision& y)
	{
		std::pair<float_precision, float_precision> tmp = interval<float_precision>::fasttwo_prod(x, y);
		interval<float_precision> res(tmp.first, tmp.first);
		if (tmp.second < c0)
			res.left = nextafter(tmp.first, -infi);
		if (tmp.second > c0)
			res.right = nextafter(tmp.first, +infi);
		return res;
	};
	// The initialization is done to preserve the precision when float_precision is a float_precision arbitrary type
	// For_IT as float, double or long double it has no effect
	interval<float_precision> itmp(al, ar);
#ifdef PHASE5
	type = compute_interval_type(type, rhs.type);
#else
	type = CLOSE;
#endif
	// Set decoration
	decoration = std::min(decoration, rhs.decoration);
	// Shortcuts
	if (al >= c0 && bl >= c0)
	{// Both intervals positive
		itmp = multiply(al, bl);
		left = itmp.left;
		itmp = multiply(ar, br);
		right = itmp.right;
		return *this;
	}
	if (ar < c0 && br < c0)
	{// Both intervals negative 
		itmp = multiply(al, bl);
		right = itmp.right;
		itmp = multiply(ar, br);
		left = itmp.left;
		return *this;
	}
	if (al >= c0 && br < c0)
	{// [A] interval positive, [B] interval negative 
		itmp = multiply(ar, bl);
		left = itmp.left;
		itmp = multiply(al, br);
		right = itmp.right;
		return *this;
	}
	if (ar < c0 && bl >= c0)
	{// [A] interval negative, [B] interval positive
		itmp = multiply(al, br);
		left = itmp.left;
		itmp = multiply(ar, bl);
		right = itmp.right;
		return *this;
	}
	// Otherwise, we have a mixed interval. Make all 4 combinations
	itmp = multiply(al, bl);
	left = itmp.left;
	right = itmp.right;
	itmp = multiply(al, br);
	left = std::min(left, itmp.left);
	right = std::max(right, itmp.right);
	itmp = multiply(ar, bl);
	left = std::min(left, itmp.left);
	right = std::max(right, itmp.right);
	itmp = multiply(ar, br);
	left = std::min(left, itmp.left);
	right = std::max(right, itmp.right);
	
	// However if underflow or overflow then change it to DAC or TRV
	if (abs(left) == infi || abs(right) == infi)
		decoration = std::min(decoration, DAC);

	return *this;
}

// Works for all other classes
// [EMPTY]:=a/[EMPTY] or [EMPTY]:=[EMPTY]/b or [EMPTY]:=[EMPTY]/[EMPTY]
// Please note that this is for all integer classes. interval<int>, interval<long>,
// were there is no loss of precision
// There is specialization for both <int>
template <>
inline interval<float_precision>& interval<float_precision>::operator/=(const interval<float_precision>& rhs)
{
	// Handle EMPTY interval first
	if (type == EMPTY)
		return *this;
	if (rhs.type == EMPTY)
		return (*this = rhs);

	const float_precision c0(0);
	// Get maximum precision of operand a and b
	const size_t max_prec(std::max(
		std::max(left.precision(), left.precision()),
		std::max(rhs.leftinterval().precision(), rhs.rightinterval().precision())
	));
	const float_precision infi(infinity_interval<float_precision>());// infi(INFINITY);
	interval<float_precision> tmp(*this);  // Save a copy
	// Compute the reverse of y e.g. 1/y
	auto inverse = [&](const float_precision& y, const bool up)
	{
		float_precision res(float_precision(1) / y);
		const float_precision r(-fma(res, y, float_precision(-1)));
		if (up == false)
		{
			if (r.sign() < 0)
				res = nextafter(res, -infi);
		}
		else
		{
			if (r.sign() > 0)
				res = nextafter(res, +infi);
		}

		return res;
	};

	left.precision(max_prec);
	right.precision(max_prec);
	float_precision bl(rhs.inf());
	float_precision br(rhs.sup());
	bl.precision(max_prec);
	br.precision(max_prec);
	if (bl == c0 && br == c0)
	{
		left = -infi;
		right = +infi;
		*this *= tmp;
		decoration = ILL;
		return *this;
	}
	if (br == c0)
	{	// b.low is !=0
		right = inverse(bl, true);
		left = -infi;
		*this *= tmp;
		decoration = ILL;
		return *this;
	}
	if (bl == c0)
	{	// b.right is !=0
		left = inverse(br, false);
		right = +infi;
		*this *= tmp;
		decoration = ILL;
		return *this;
	}
	// neither b.inf() or b.sup() is zero
	left = inverse(br, false);
	right = inverse(bl, true);
	*this *= tmp;
	// Set decoration
	decoration = std::min(decoration, rhs.decoration);
	if (bl<float_precision(0) && br>float_precision(0))  // Do division interval include zero?
		decoration = std::min(decoration, TRV); // Then set the decoration to TRV
	return *this;
}

#endif


// += operator. Works for nearly all classes. 
// Always return an "proper" and closed [] interval
// a:=a+[EMPTY] or b:=[EMPTY]+b or [EMPTY]:=[EMPTY]+[EMPTY]
template<class IT> inline interval<IT>& interval<IT>::operator+=( const interval<IT>& rhs )
{
	// Handle EMPTY interval first
	if (rhs.type == EMPTY)
		return *this;
	if (type == EMPTY)
		return (*this = rhs);

	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT unfl(underflow_interval<IT>());
	// Neither a or b is [EMPTY]
	std::pair<IT,IT> xleft = fasttwo_sum(inf(), rhs.inf());
	std::pair<IT,IT> xright = fasttwo_sum(sup(), rhs.sup());
	// Any adjustment?
	if (xleft.second < IT(0))
		xleft.first=nextafter(xleft.first, -infi);
	if (xright.second > IT(0))
		xright.first = nextafter(xright.first, +infi);
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
	if (abs(left) == infi || abs(right) == infi)
		decoration = std::min(decoration, DAC);
	if (left!=IT(0)&&abs(left) == unfl || right!=IT(0)&&abs(right) == unfl)
		decoration = std::min(decoration, TRV);
	return *this;
}

// -= operator. Works all other classes. 
// Always return an "proper" and closed [] interval
// a=a-[EMPTY] or -b=[EMPTY]-b or [EMPTY]=[EMPTY]-[EMPTY]
//
template<class IT> inline interval<IT>& interval<IT>::operator-=( const interval<IT>& rhs )
{
	// Handle EMPTY interval first
	if (rhs.type == EMPTY)
		return *this;
	if (type == EMPTY)
		return (*this = -rhs);
	
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT unfl(underflow_interval<IT>());
	// Neither a or b is [EMPTY]
	std::pair<IT, IT> xleft = fasttwo_sum(inf(), -rhs.sup());
	std::pair<IT, IT> xright = fasttwo_sum(sup(), -rhs.inf());
	if (xleft.second < IT(0))
		xleft.first = nextafter(xleft.first, -infi);
	if (xright.second > IT(0))
		xright.first = nextafter(xright.first, +infi);
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
	if (abs(left) == infi || abs(right) == infi)
		decoration = std::min(decoration, DAC);
	if ((left != IT(0) && abs(left) == unfl) || (right != IT(0) && abs(right) == unfl))
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
template<class IT> inline interval<IT>& interval<IT>::operator*=( const interval<IT>& rhs )
{
	 // Handle EMPTY interval first  ∅
	 if (type == EMPTY)
		 return *this;
	 if (rhs.type == EMPTY)
		 return (*this=rhs);

	 const IT c0(0);
	 const IT infi(infinity_interval<IT>());// infi(INFINITY);
	 const IT unfl(underflow_interval<IT>());
	 // Neither a or b is ∅
	 // Extract intervals through inf() and sup()
	IT al(inf());
	IT ah(sup());
	IT bl(rhs.inf());
	IT bh(rhs.sup());

	auto multiply = [&](const IT& x, const IT& y) 
	{
		std::pair<IT, IT> tmp = interval<IT>::fasttwo_prod(x, y); 
		interval<IT> res(tmp.first,tmp.first);
		const IT infi(infinity_interval<IT>());// infi(INFINITY);
		if (tmp.second < c0)
			res.left = nextafter(tmp.first, -infi);
		if (tmp.second > c0)
			res.right = nextafter(tmp.first, +infi);
		return res;
	};
	// The initialization is done to preserve the precision when IT is a float_precision arbitrary type
	// For_IT as float, double or long double it has no effect
	interval<IT> itmp(al,ah);	
#ifdef PHASE5
	type = compute_interval_type(type, rhs.type);
#else
	type = CLOSE;
#endif
	// Set decoration
	decoration = std::min(decoration, rhs.decoration);
	// Shortcuts
	if (al >= c0 && bl >= c0)
	{// Both intervals positive
		itmp=multiply(al, bl);
		left = itmp.left;
		itmp = multiply(ah, bh);
		right = itmp.right;
		return *this;
	}
	if (ah < c0 && bh < c0)
	{// Both intervals negative 
		itmp = multiply(al, bl);
		right = itmp.right;
		itmp = multiply(ah, bh);
		left = itmp.left;
		return *this;
	}
	if (al >=c0 && bh < c0)
	{// [A] interval positive, [B] interval negative 
		itmp = multiply(ah, bl);
		left = itmp.left;
		itmp = multiply(al, bh);
		right = itmp.right;
		return *this;
	}
	if (ah < c0 && bl >= c0)
	{// [A] interval negative, [B] interval positive
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
	right = std::max(right,itmp.right);
	itmp = multiply(ah, bl);
	left = std::min(left, itmp.left);
	right = std::max(right, itmp.right);
	itmp = multiply(ah, bh);
	left = std::min(left, itmp.left);
	right = std::max(right, itmp.right);

	// However if underflow or overflow then change it to DAC or TRV
	if (abs(left) == infi || abs(right) == infi)
		decoration = std::min(decoration, DAC);
	if ((left != c0 && abs(left) == unfl) || (right != c0 && abs(right) == unfl))
		decoration = std::min(decoration, TRV);
	return *this;
}

// Works for all other classes
// [EMPTY]:=a/[EMPTY] or [EMPTY]:=[EMPTY]/b or [EMPTY]:=[EMPTY]/[EMPTY]
// Please note that this is for all integer classes. interval<int>, interval<long>,
// were there is no loss of precision
// There is specialization for both <int>
template<class IT> inline interval<IT>& interval<IT>::operator/=(const interval<IT>& rhs)
{
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	// Handle EMPTY interval first
	if (type == EMPTY)
		return *this;
	if (rhs.type == EMPTY)
		return (*this = rhs);

	interval<IT> tmp(*this);  // Save a copy
	// Compute the reverse of y e.g. 1/y
	auto inverse = [&](const IT& y, const bool up)
	{
		IT res(IT(1) / y);
		const IT r(-fma(res, y, IT(-1)));
		if (up == false)
		{
			if (r < IT(0))
				res = nextafter(res, -infi);
		}
		else
		{
			if (r > IT(0))
				res = nextafter(res, +infi);
		}

		return res;
	};

	IT bl(rhs.inf());
	IT bh(rhs.sup());
	if (bl == IT(0) && bh == IT(0))
	{
		left = -infi;
		right = +infi;
		*this *= tmp;
		decoration = ILL;
		return *this;
	}
	if (bh == IT(0))
	{	// b.low is !=0
		right = inverse(bl, true);
		left = -infi;
		*this *= tmp;
		decoration = ILL;
		return *this;
	}
	if (bl == IT(0))
	{	// b.right is !=0
		left = inverse(bh, false);
		right = +infi;
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
	if(bl<IT(0) &&bh>IT(0))  // Do division interval include zero?
		decoration = std::min(decoration, TRV); // Then set the decoration to TRV

	return *this;
}

// Return the intersection
//
template<class IT> inline interval<IT>& interval<IT>::operator&=(const interval<IT>& rhs)
{
	const IT aleft(rhs.inf());
	const IT aright(rhs.sup());
	const IT l(inf());
	const IT r(sup());
	if (aright > l || r < aleft)// No intersection
		this->type = EMPTY;
	else
	{
		left = std::max(aleft, l);
		right = std::min(aright,r);
		type = CLOSE;
	}

	return *this;
}

// Return the union.
// However not the correct union as two intervals if not overlapping. 
// But just the entire union of the two interval
// regardsless if the intervals is connected.
// use join() for the correct handling of the union operator.
template<class IT> inline interval<IT>& interval<IT>::operator|=(const interval<IT>& rhs)
{
	const IT aleft(rhs.inf());
	const IT aright(rhs.sup());
	const IT l(inf());
	const IT r(sup());
	if (right < l || r < left)
	{ // Non-overlapping intervals
		//????
	}
//	else
	{	// Overlapping
		left = std::min(aleft, l);
		right = std::max(aright, r);
	}
	return *this;
}

// Return the set minus
//
template<class IT> inline interval<IT>& interval<IT>::operator^=(const interval<IT>& rhs)
	{
	const IT aleft(rhs.inf());
	const IT aright(rhs.sup());
	const IT l(inf());
	const IT r(sup());
	if ( aleft < r && aright > l ) // intersection is not empty
		{
		if (aright <= l )
			left = aright;
		else
			if (aright >= r )
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
template<class IT> inline interval<IT> operator+(const interval<IT>& a, const interval<IT>& b)
{
	interval<IT> result(a);
	result += b;
	return result;
}

// Unary + operator
// Works for all classes
//
template<class IT> inline interval<IT> operator+(const interval<IT>& a)
{
	return a;
}

// Binary - operator
// Works for all classes
//
template<class IT> inline interval<IT> operator-(const interval<IT>& a, const interval<IT>& b)
{
	interval<IT> result(a);
	result -= b;
	return result;
}

// Unary - operator
// Works for all classes
//
template<class IT> inline interval<IT> operator-(const interval<IT>& a)
{
	interval<IT> result(a); // Ensure correct precision for IT=float_precision
	result = IT(0);
	result -= a;
	return result;
}

// Binary * operator
// Works for all classes
//
template<class IT> inline interval<IT> operator*(const interval<IT>& a, const interval<IT>& b)
{
	interval<IT> result(a);
	result *= b;
	return result;
}

// Binary / operator
// Works for all classes
//
template<class IT> inline interval<IT> operator/(const interval<IT>& a, const interval<IT>& b)
{
	interval<IT> result(a);

	if (result == b)
	{
		enum int_class intclass = b.isClass();
		if (intclass != ZERO && intclass != POSITIVE0 && intclass != NEGATIVE0)
		{
		//	result = interval<IT>(IT(1), IT(1));
		//	return result;
		}
	}
	result /= b; // Notice result/=b; return the maximum precision of the two operand And 
				// since we change this precision in the division call result is the precision of 
				// the maximum of the two operands
	return result;
}

// Binary + operator
// Works for all classes
//
template<class IT,class _X> inline interval<IT> operator+(const interval<IT>& a, const _X& b)
	{
	interval<IT> c(a);
	c += interval<IT>(IT(b));
	return c; 
	}

// Binary + operator
// Works for all classes
//
template<class IT,class _X> inline interval<IT> operator+( const _X& a, const interval<IT>& b)
	{
	interval<IT> c(b);
	c += interval<IT>(IT(a));
	return c;
	}

// Binary - operator
// Works for all classes
//
template<class IT, class _X> inline interval<IT> operator-(const interval<IT>& a, const _X& b)
	{
	interval<IT> c(a);
	c -= interval<IT>(IT(b));
	return c;
	}

// Binary - operator
// Works for all classes
//
template<class IT, class _X> inline interval<IT> operator-(const _X& a, const interval<IT>& b)
	{
	interval<IT> c(a);
	c -= b;
	return c;
	}

// Binary * operator
// Works for all classes
//
template<class IT, class _X> inline interval<IT> operator*(const interval<IT>& a, const _X& b)
	{
	interval<IT> c(a);
	c *= interval<IT>(IT(b));
	return c;
	}

// Binary * operator
// Works for all classes
//
template<class IT, class _X> inline interval<IT> operator*(const _X& a, const interval<IT>& b)
	{
	interval<IT> c(b);
	c *= interval<IT>(IT(a));
	return c;
	}


// Binary / operator
// Works for all classes
//
template<class IT, class _X> inline interval<IT> operator/(const interval<IT>& a, const _X& b)
	{
	interval<IT> c(a);
	c /= interval<IT>(IT(b));
	return c;
	}

// Binary / operator
// Works for all classes
//
template<class IT, class _X> inline interval<IT> operator/(const _X& a, const interval<IT>& b)
	{
	interval<IT> c(a);
	c /= b;
	return c;
	}


// Binary & operator
// Return the intersection
//
template<class IT> inline interval<IT> operator&( const interval<IT>& a, const interval<IT>& b )
	{
	interval<IT> c(a);
	c &= b;
	return c;
	}

// Binary | operator.
// Return the union
//
template<class IT> inline interval<IT> operator|(const interval<IT>& a, const interval<IT>& b)
	{
	interval<IT> c(a);
	c |= b;
	return c;
	}

// Binary ^ operator
// Return set minus
//
template<class IT> inline interval<IT> operator^(const interval<IT>& a, const interval<IT>& b)
	{
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
template<class IT> inline bool operator==(const interval<IT>& a, const interval<IT>& b)
{
	if (a.intervaltype() == EMPTY && b.intervaltype() == EMPTY)
		return true;	// Both EMPTY=> return true
	if (a.intervaltype() == EMPTY || b.intervaltype() == EMPTY)
		return false;	// One but not both are EMPTY => return false
	// Check for equality. Note intervaltype also has to match
	return a.inf() == b.inf() && a.sup() == b.sup() && a.intervaltype() == b.intervaltype();
}

// != operator
// Works for all classes
//
template<class IT> inline bool operator!=(const interval<IT>& a, const interval<IT>& b)
{
	return !(a == b);
}
// >= operator
// Works for all classes
//
template<class IT> inline bool operator>=(const interval<IT>& a, const interval<IT>& b)
{
	return !(a < b);
}
// <= operator
// Works for all classes
//
template<class IT> inline bool operator<=(const interval<IT>& a, const interval<IT>& b)
{
	return !(a > b);
}

// > operator
// Works for all classes
//
template<class IT> inline bool operator>(const interval<IT>& a, const interval<IT>& b)
{
	if (a.intervaltype() == EMPTY && b.intervaltype() == EMPTY)
		return false;
	if (a.intervaltype() == EMPTY || b.intervaltype() == EMPTY)
		return true;

	// Helper function to compare left boundaries
	auto isLeftGreater=[](const interval<IT> & a, const interval<IT> & b) 
	{
		if (a.inf() > b.inf()) return true;
		if (a.inf() < b.inf()) return false;
		// Handle inclusivity/exclusivity of left boundary
		const enum interval_type atype(a.intervaltype());
		const enum interval_type btype(b.intervaltype());
		if (( atype == CLOSE || atype== RIGHT_OPEN ) && (btype == LEFT_OPEN|| btype==OPEN)) return true;
		if ((atype == OPEN || atype == LEFT_OPEN) && (btype == CLOSE|| btype==RIGHT_OPEN)) return false;
		return false;
	};
	// Helper function to compare right boundaries
	auto isRightGreater=[](const interval<IT> & a, const interval<IT> & b) 
	{
		if (a.sup() > b.sup()) return true;
		if (a.sup() < b.sup()) return false;
		// Handle inclusivity/exclusivity of right boundary
		const enum interval_type atype(a.intervaltype());
		const enum interval_type btype(b.intervaltype());
		if ((atype == CLOSE || atype==LEFT_OPEN) && (btype == OPEN||btype==RIGHT_OPEN)) return true;
		if ((atype == OPEN || atype==RIGHT_OPEN) && (btype == CLOSE||btype==LEFT_OPEN)) return false;
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
template<class IT> inline bool operator<(const interval<IT>& a, const interval<IT>& b)
{
	if (a.intervaltype() == EMPTY && b.intervaltype() == EMPTY)
		return false;
	if (a.intervaltype() == EMPTY || b.intervaltype() == EMPTY)
		return true;

	// Helper function to compare left boundaries
	auto isLeftLess = [](const interval<IT>& a, const interval<IT>& b)
	{
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
	auto isRightLess = [](const interval<IT>& a, const interval<IT>& b)
	{
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
template<class IT, class _X> inline bool operator==(const interval<IT>& a, const _X& b)
{
	interval<IT> c(b);
	return a == c;
	}

// Binary == operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator==(const _X& a, const interval<IT>& b)
{
	interval<IT> c(a);
	return c == b;
	}

// Binary == operator
// Works for all classes
//
template<class IT, class _X> inline bool operator==(const interval<IT>& a, const interval<_X>& b)
{
	interval<IT> c(b);
	return a == c;
}


// Binary != operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator!=(const interval<IT>& a, const _X& b)
	{
	interval<IT> c(b);
	return !(a==c);
	}

// Binary != operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator!=(const _X& a, const interval<IT>& b)
	{
	interval<IT> c(a);
	return !(c==b);
	}

// != operator
// Works for all classes
//
template<class IT, class _X> inline bool operator!=(const interval<IT>& a, const interval<_X>& b)
	{
	interval<IT> c(b);
	return !(a == c);
	}

// Binary >= operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator>=(const interval<IT>& a, const _X& b)
{
	interval<IT> c(b);
	return a >= c;
	}

// Binary >= operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator>=(const _X& a, const interval<IT>& b)
{
	interval<IT> c(a);
	return c >= b;
}

// Binary >= operator
// Works for all classes
//
template<class IT, class _X> inline bool operator>=(const interval<IT>& a, const interval<_X>& b)
{  		
	interval<IT> c(b);
	return a >= c; 
}

// Binary >= operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator<=(const interval<IT>& a, const _X& b)
{
	interval<IT> c(b);
	return a <= c;
}

// Binary >= operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator<=(const _X& a, const interval<IT>& b)
{
	interval<IT> c(a);
	return c <= b;
}

// Binary >= operator
// Works for all classes
//
template<class IT, class _X> inline bool operator<=(const interval<IT>& a, const interval<_X>& b)
{
	interval<IT> c(b);
	return a <= c;
}

// Binary > operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator>(const interval<IT>& a, const _X& b)
{
	interval<IT> c(b);
	return a > c;
}

// Binary > operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator>(const _X& a, const interval<IT>& b)
{
	interval<IT> c(a);
	return c > b;
}

// Binary > operator
// Works for all classes
//
template<class IT, class _X> inline bool operator>(const interval<IT>& a, const interval<_X>& b)
{
	interval<IT> c(b);
	return a > c;
}

// Binary < operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator<(const interval<IT>& a, const _X& b)
{
	interval<IT> c(b);
	return a < c;
}

// Binary < operator
// Works for all mixed classes
//
template<class IT, class _X> inline bool operator<(const _X& a, const interval<IT>& b)
{
	interval<IT> c(a);
	return c < b;
}

// Binary < operator
// Works for all classes
//
template<class IT, class _X> inline bool operator<(const interval<IT>& a, const interval<_X>& b)
{
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
template<class IT> inline interval<IT> abs( const interval<IT>& a )
	{
	if (a.inf() >= IT(0) ) // Entirely positive
		return a;
	else
		if (a.sup() < IT(0) ) // Entirely negative
			return -a;

	return interval<IT>(IT(0), max(-a.inf(),a.sup()),a.intervaltype());
	}

//////////////////////////////////////////////////////////////////////////////////////
///
/// Interval distance() between two interval numbers
///
//////////////////////////////////////////////////////////////////////////////////////

template<class IT> inline IT intervaldistance(const interval<IT>& a, const interval<IT>& b)
	{
	return max(abs(a.leftinterval() - b.leftinterval()), abs(a.rightinterval() - b.rightinterval()));
	}


// Return union
// the name join is used since union is a reserved word in c++
// it follow the IEEE 1788 standard by returning two intervals if the interval a and b is not connected
// otherwise if return the joined interval and the second interval of thepair returned is the empty
// interval
// Notice the |= operates or the binary operator | return the union of the two intervals by combining it to one interval
//
template<class IT> inline std::pair<interval<IT>, interval<IT> > join(const interval<IT>& a, const interval<IT>& b)
{
	if (a.sup() < b.inf())  // interval do not connect
	{	// return two interval
		return std::make_pair <interval<IT>, interval<IT> >(a, b);
	}
	// Return the union of ther two intervals.
	interval<IT> c(min(a.inf(),b.inf()),max(a.sup(),b.sup()));
	interval<IT> d; // Empty set
	return std::make_pair <interval<IT>, interval<IT> >(c, d);
}

// Return the interval intersection of the two intervals.
template<class IT> inline interval<IT> intersection(const interval<IT>& a, const interval<IT>& b)
{
	interval<IT> c(a);
	c &= b;
	return c;
}

// if a is a subset of b then return true otherwise false
template<class IT> inline bool subset(const interval<IT>& a, const interval<IT>& b)
{
	if (b.inf() <= a.inf() && a.sup() <= b.sup())
		return true;
	return false;
}

// if a is an interior of b then return true otherwise false
template<class IT> inline bool interior(const interval<IT>& a, const interval<IT>& b)
{
	if (b.inf() < a.inf() && a.sup() < b.sup())
		return true;
	return false;
}

// if a precedes b then return true otherwise false
template<class IT> inline bool precedes(const interval<IT>& a, const interval<IT>& b)
{
	if (a.sup() < b.inf() )
		return true;
	return false;
}

// inclusion between two intervals. 
// If a is a subset of b then return +1, 
// if b is a subset of a then return +1 
// otherwise return 0
template<class IT> inline int inclusion(const interval<IT>& a, const interval<IT>& b)
{
	if( subset(a, b))
		return -1;
	if (subset(b, a))
		return +1;
	return 0;
}

// empty(). The function version of the method .isEmpty()
template<class IT> inline bool empty(const interval<IT>& a)
{
	return a.isEmpty();
}

// empty(). The function version of the method .isEmpty()
template<class IT> inline bool entire(const interval<IT>& a)
{
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
inline enum interval_type compute_interval_type(enum interval_type a, enum interval_type b)
{
	if (a == b) return a;		// return the same interval types as either of the operand
	if (a == CLOSE) return b;	// Since open prevails return b interval_type
	if (b == CLOSE)	return a;	// Since open prevails return a interval type
	return OPEN;				// Otherwise the interval is open
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
template<class IT> inline interval<IT> sqr(const interval<IT>& x)
{	
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT unfl(underflow_interval<IT>());

	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	IT left(x.inf());
	IT right(x.sup());
	IT tmpl(left);
	IT tmpr(right);
	interval<IT> r(left,right,x.intervaltype());// Ensure correct precision for IT=float_precision

	r.leftinterval(IT(0));  // set left interval to zero
	left *= left;	//square left interval
	right *= right;	// square right interval
	r.rightinterval(max(left, right));
	// Contained zero?
	if ( tmpl > IT(0) && tmpr > IT(0))
		r.leftinterval(min(left, right));
	// Set decoration
	r.intervaldecoration(std::min(r.intervaldecoration(), x.intervaldecoration()));
	// However if underflow or overflow then change it to DAC or TRV
	if (abs(left) == infi || abs(right) == infi)
		r.intervaldecoration(std::min(r.intervaldecoration(), DAC));
	if (left != IT(0) && abs(left) == unfl || right != IT(0) && abs(right) == unfl)
		r.intervaldecoration(std::min(r.intervaldecoration(), TRV));
	return r;
}

// sqrt(x)
// wortk for all clases. The initialization of local variable is done to ensure correct precision when called
// with the IT=float_precision
//
template<class IT> inline interval<IT> sqrt(const interval<IT>& x)
{	
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	// Initialized the local variable, to ensure right precision for float_precision types
	// Notice IEEE1788 requires us to ignore outside of domain range. e.g. negative numbers
	// sqrt([-1,1])==[0,1] or sqrt([-2,-1])==[EMPTY]
	if (x.isEmpty() || x.sup() < IT(0))
	{
		interval<IT> res=interval<IT>();	// Return the EMPTY interval;
		res.intervaldecoration(ILL);
		return res;
	}
	// Find leftinterval bound
	const IT leftadjust(max(x.inf(), IT(0)));
	IT left(sqrt(leftadjust)); 
	IT r(-fma(left,left,-leftadjust));
	if (isinf(left) && isinf(x.inf()))
		r = 0;	// When both is infinity
	if (r < IT(0) )
		left=nextafter(left, -infi);

	// Find rightinterval bound
	const IT rightadjust(max(x.sup(), IT(0)));
	IT right(sqrt(rightadjust));
	if (isinf(right) && isinf(x.sup()))
		r = 0;	// When both is infinity
	else 
		r = -fma(right, right, -rightadjust);
	if (r > IT(0) )
		right=nextafter(right, +infi);

	interval<IT> res(left, right);
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	if(x.inf()<IT(0))
		res.intervaldecoration(std::min(res.intervaldecoration(), TRV));
	return res;
}

// floor(x)
// the interval returned is always CLOSED
template<class IT> inline interval<IT> floor(const interval<IT>& x)
{
	const IT left(x.inf(true));
	const IT right(x.sup(true));
	interval<IT> res(floor(left), floor(right));
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// ceil(x)
template<class IT> inline interval<IT> ceil(const interval<IT>& x)
{
	const IT left(x.inf(true));
	const IT right(x.sup(true));
	interval<IT> res(ceil(left), ceil(right));
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// sgn(x)
// return either a singleton interval if left>0 [1], left==0 & right=0 [0], right<0 [-1]
// or a range interval is left <0 and right==0 [-1,0], left<0 & right >0 [-1,1] or left==0 and right>0 [0,1]
//
template<class IT> inline interval<IT> sgn(const interval<IT>& x)
{
	const IT left(x.inf(true));
	const IT right(x.sup(true));
	IT left_min(IT(+1));
	IT right_max(IT(-1));

	if (left <= IT(0))
		left_min = IT(0);
	if (left < IT(0))
		left_min = IT(-1);
	if (right >= IT(0))
		right_max = IT(0);
	if (right > IT(0))
		right_max = IT(1);

	interval<IT> res(left_min, right_max ); 
	// set the proper interval decoration
	res.intervaldecoration(COM);
	return res;
}

// Not used anymore
static interval<double> interval_log(double x)
{
	// First handle the shortcuts
	if (x < 0) { return interval<double>(NAN, NAN); }
	if (x == 0) { return interval<double>(-INFINITY, -INFINITY); }
	if (x == 1) { return interval<double>(0); }
	if (x == 2) { return ln2_interval<double>(); }
	if (x == 10) { return ln10_interval<double>(); }

	const interval<double> c1(1.0);
	interval<double> zn, zsq, sum, delta;
	int i, exponent;
	std::cout << "\tlog(" << x << ")\n";	// DEBUG
	if (x == 4 || x == 8 || x == 16)	// DEBUG
		i = 1;							// DEBUG
	// Split the significant and exponent
	x = frexp(x, &exponent);
	if (x == 0.5)
	{// True Power of two. More accurate to just multiply (exponent-1) * LN2
		zn = ln2_interval<double>();
		zn *= interval<double>(exponent - 1);
		return zn;
	}
	zn = interval<double>(x);
	// Taylor series of log(x)
	// log(x)=2( z + z^3/3 + z^5/5 ...)
	// where z=(x-1)/(x+1) 
	// In order to get a fast Taylor series result we need to get the fraction closer to 1
	// The fraction part is [0.xxx,1] (base 2) after removing the exponent
	// Initialize the iteration  (zn-1)/(zn+1)
	zn = (zn - c1) / (zn + c1);
	zsq = zn * zn;
	sum = zn;
	
	interval<double> test;	// DEBUG
	double diff;		// DEBUG
	{// DEBUG
		test = sum * interval<double>(2.0);
		test += interval<double>(exponent) * ln2_interval<double>();
		diff = 2.0 * test.rad();
		std::cout << "\t[" << 1 << "] " << test.toString() << " diff=" << diff << "\n";
		// END DEBUG
	}

	// Iterate using taylor series log(x) == 2( z + z^3/3 + z^5/5 ... ) 
	for (i = 3;; i += 2)
	{
		zn *= zsq;
		delta = zn / interval<double>(i);
		if (sum.mid() + delta.mid() == sum.mid())
			break;
		sum += delta;
		{// DEBUG
			test = sum * interval<double>(2.0);
			test += interval<double>(exponent) * ln2_interval<double>();
			diff = 2.0*test.rad();
			std::cout << "\t["<< (i+1)/2 << "] " << test.toString() << " diff=" << diff << "\n";
			// END DEBUG
		}
	}
	sum *= interval<double>(2.0);// Finally multiply with 2

	if (exponent != 0)
	{	// restore the exponent
		sum += interval<double>(exponent) * ln2_interval<double>();
	}
	return sum;
}

// log(x)
template<class IT> inline interval<IT> log(const interval<IT>& x)
{
	if (x.isEmpty() )
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT l(x.inf());
	const IT r(x.sup());
	const bool isIEEE754Float = std::is_floating_point<IT>::value;
	
	if (r < IT(0))
	{	// entire interval < 0
		interval<IT> res = interval<IT>();	// Return the EMPTY interval;
		res.intervaldecoration(ILL); // Set ILL decoration
		return res;
	}

	// Initialize lower and upper with direct log calculations or -INFINITY for l <= 0
	IT lower((l <= IT(0)) ? -infi : log(l));
	IT upper((r <= IT(0)) ? -infi : log(r));

	// Apply shortcuts for well-known constants, adjusting for precision
	if (l == IT(1)) lower = IT(0);
	else if (isIEEE754Float && l == IT(2)) lower = ln2_interval<IT>().inf();
	else if (isIEEE754Float && l == IT(10)) lower = ln10_interval<IT>().inf();
	else lower = nextafter(lower, -infi); // Adjust for precision if not a shortcut value

	if (r == IT(1)) upper = IT(0);
	else if (isIEEE754Float && r == IT(2)) upper = ln2_interval<IT>().sup();
	else if (isIEEE754Float && r == IT(10)) upper = ln10_interval<IT>().sup();
	else upper = nextafter(upper, +infi); // Adjust for precision if not a shortcut value

	// Ensure lower is not mistakenly set to a non-NaN value when l <= 0
	if (l <= IT(0) && r > IT(0)) 
		lower = -infi;

	interval<IT> res(lower, upper);
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	if (x.inf() < IT(0)) // was x original < 0 
		res.intervaldecoration(std::min(res.intervaldecoration(), TRV));
	return res;
}

// log10(x)
template<class IT> inline interval<IT> log10(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT l(x.inf());
	const IT r(x.sup());

	if (r < IT(0))
	{	// entire interval < 0
		interval<IT> res = interval<IT>();	// Return the EMPTY interval;
		res.intervaldecoration(ILL); // Set ILL decoration
		return res;
	}

	// Initialize lower and upper bounds with direct log10 calculations or -INFINITY for l <= 0
	IT lower(l <= IT(0) ? -infi : log10(l));
	IT upper(r <= IT(0) ? -infi	: log10(r));

	// Apply shortcuts for well-known constants, adjusting for precision
	if (l == IT(1)) lower = IT(0);
	else if ( l == IT(10)) lower = IT(1);
	else lower = nextafter(lower, -infi); // Adjust for precision if not a shortcut value

	if (r == IT(1)) upper = IT(0);
	else if ( r == IT(10)) upper = IT(1);
	else upper = nextafter(upper, +infi); // Adjust for precision if not a shortcut value

	// Ensure lower is not mistakenly set to a non-NaN value when l <= 0
	if (l <= IT(0) && r > IT(0)) 
		lower = -infi;
	
	interval<IT> res(lower, upper);
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	if (x.inf() < IT(0)) // was x original < 0 
		res.intervaldecoration(std::min(res.intervaldecoration(), TRV));
	return res;
	}

// exp(x)
template<class IT> inline interval<IT> exp(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const bool isIEEE754Float = std::is_floating_point<IT>::value;
	const IT l(x.inf());
	const IT r(x.sup());
	IT leftexp(exp(l));
	IT rightexp(exp(r));

	// Directly handle the special cases with exact values
	if (l == IT(0)) leftexp = IT(1); // e^0 = 1, exact
	else 
		if (isIEEE754Float && l == IT(1))
			leftexp = e_interval<IT>().inf(); // e^1, use predefined constant
		else 
			leftexp = nextafter(leftexp, -infi); // Adjust unless it's a special case

	if (r == IT(0)) rightexp = IT(1); // e^0 = 1, exact
	else 
		if (isIEEE754Float && r == IT(1)) 
			rightexp = e_interval<IT>().sup(); // e^1, use predefined constant
		else 
			rightexp = nextafter(rightexp, +infi); // Adjust unless it's a special case

	// Create and return the interval from the calculated or adjusted values
	interval<IT> res(leftexp, rightexp);
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	if(abs(rightexp)==infi||abs(leftexp)==infi)
		res.intervaldecoration(std::min(res.intervaldecoration(), DAC));
	return res;
}


// pow(x,y) where x is an interval and y id a double
// 
template<class IT> inline interval<IT> pow(const interval<IT>& x, const IT y)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	// Check for special cases
	if (y == IT(0)) // Anything to the power of 0 is 1
		return interval<IT>(1);

	//interval<IT> lhs, rhs;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT l(x.inf());
	const IT r(x.sup());

	// Handle negative powers when the interval includes 0 to avoid division by zero
	/*
	if (y < 0 && l <= 0 && r >= 0) {
		if (l == 0) {
			// If l is 0, the lower bound goes to infinity when raised to a negative power
			return interval<IT>(0, nextafter(1 / pow(r, y), +INFINITY));
		}
		else if (r == 0) {
			// If r is 0, the upper bound goes to infinity when raised to a negative power
			return interval<IT>(nextafter(1 / pow(l, y), -INFINITY), +INFINITY);
		}
		else {
			// If the interval spans through 0, the result is (0, +INFINITY)
			return interval<IT>(0, +INFINITY);
		}
	}
	*/

	IT lp(pow(l, y));
	IT rp(pow(r, y));

	if (floor(l) != l || floor(r) != r)
	{	// if either is not an integer then we do not have an exact power
		lp = nextafter(lp, (lp > IT(0)) ? -infi : +infi);
		rp = nextafter(rp, (rp > IT(0)) ? +infi : -infi);
	}
	// else Both are integers => trust the result
	
	// Ensure correct interval ordering for the result
	return interval<IT>(min(lp, rp), max(lp, rp));
}


// pow(x) we have to do it manually
// x^y == exp( y * ln( x ) ) );
//			interval	singleton	
// x
// 
template<class IT> inline interval<IT> pow(const interval<IT>& x, const interval<IT>& y)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	interval<IT> c;

	if (y.isPoint())	// is y a singleton interval?
		return pow(x, y.inf());
	if (x.isPoint())	// x is a point, y is an interval
	{
		// ??
	}
	// Both x and y are intervals
	IT yi(y.inf());
	IT ys(y.sup());
	// if y is an integer?
	if (floor(yi) == yi && floor(ys) == ys)
	{ // raise to the power of an integer interval
		interval<IT> lhs(pow(x, yi));
		interval<IT> rhs(pow(x, ys));
		c = interval<IT>(min(lhs.inf(),rhs.inf()), max(lhs.sup(),rhs.sup()));
		return c;
	}

	// Otherwise do it the hard way
	c = log(x);
	c *= y;
	c = exp(c);
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
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires c++17)
// lambda functions. the variable l and r inherits the precision fromthe call to fmod()
template<class IT> inline interval<IT> sin(const interval<IT>& x) 
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT pi = [&]()->IT {
		if constexpr (std::is_floating_point<IT>::value) 
		{ // for floating point type
			return acos(IT(-1));	// More precise pi value
		}
		else 
		{ // For float_precision class. Use maximum precision of the left or right interval
			return _float_table(_PI, max(x.leftinterval().precision(),x.rightinterval().precision()));
		}
	}(); // The Lambda is immediately invokeed
	const IT twopi(IT(2) * pi);
	IT l(x.inf()); 
	IT r(x.sup()); 

	// If the interval width is >= 2pi, the sine function covers the full range [-1, 1]
	if (x.sup() - x.inf() >= twopi) 
		return interval<IT>(IT(-1), IT(1));

	// Calculate sine values at the interval's endpoints
	IT sin_l(sin(l));
	IT sin_r(sin(r));

	// Check for critical points within the interval
	IT sin_min(std::min(sin_l, sin_r));
	IT sin_max(std::max(sin_l, sin_r));

	// Check passing critical ponts by normalizing l and r 
	l=fmod(x.inf(), twopi); // Normalize l within a single period
	r=l + fmod(x.sup() - x.inf(), twopi); // Calculate r based on l and the interval width
	// Normalize angles to be within [0, 2*pi)
	if (l < IT(0))	l += twopi;
	if (r >= twopi)	r -= twopi;
	if (l <= pi / IT(2) && pi / IT(2) <= r) 
		sin_max = IT(1); // pi/2 is within interval
	if (l <= IT(1.5) * pi && IT(1.5) * pi <= r) 
		sin_min = IT(-1); // 3*pi/2 is within interval

	// Established a safety interval around the result to ensure correct bound for the computation
	if (sin_min != IT(-1)  && sin_min != IT(0) )
		sin_min = nextafter(sin_min, -infi);
	if( sin_max != IT(1) && sin_max != IT(0) )
		sin_max= nextafter(sin_max, +infi);

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
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires c++17)
// lambda functions. the variable l and r inherits the precision fromthe call to fmod()
template<class IT> inline interval<IT> cos(const interval<IT>& x) 
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT pi = [&]()->IT {
		if constexpr (std::is_floating_point<IT>::value)
		{ // for floating point type
			return acos(IT(-1));	// More precise pi value
		}
		else
		{ // For float_precision class. Use maximum precision of the left or right interval
			return _float_table(_PI, max(x.leftinterval().precision(), x.rightinterval().precision()));
		}
	}(); // The Lambda is immediately invokeed
	const IT twopi(IT(2) * pi);
	IT l(x.inf()); 
	IT r(x.sup()); 	
	
	// If the interval width is >= 2pi, the cos function covers the full range [-1, 1]
	if (x.sup() - x.inf() >= twopi)
		return interval<IT>(IT(-1), IT(1));

	// Calculate cosine values at the interval's endpoints
	IT cos_l(cos(l));
	IT cos_r(cos(r));

	// Check for critical points within the interval
	IT cos_min(std::min(cos_l, cos_r));
	IT cos_max(std::max(cos_l, cos_r));
	
	// Check passing critical ponts by normalizing l and r 
	l = fmod(x.inf(), twopi); // Normalize l within a single period
	r = l + fmod(x.sup() - x.inf(), twopi); // Calculate r based on l and the interval width
	// Normalize angles to be within [0, 2*pi)
	if (l < IT(0)) 	l += twopi;
	if (r >= twopi)	r -= twopi;
	if (r<l) 
		cos_max = IT(1.0); // 0 or 2*pi is within interval
	//if (l <= pi && pi <= r) 
	if(l<=pi && r >= pi)
		cos_min = IT(-1.0); // pi is within interval

	// Established a safety interval around the result to ensure correct bound for the computation
	if (cos_min != IT(-1) && cos_min != IT(0) )
		cos_min = nextafter(cos_min, -infi);
	if (cos_max != IT(1) && cos_max != IT(0) )
		cos_max = nextafter(cos_max, +infi);

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
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires c++17)
// lambda functions. the variable l and r inherits the precision fromthe call to fmod()
template<class IT> inline interval<IT> tan(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT pi = [&]()->IT {
		if constexpr (std::is_floating_point<IT>::value)
		{ // for floating point type
			return acos(IT(-1));	// More precise pi value
		}
		else
		{ // For float_precision class. Use maximum precision of the left or right interval
			return _float_table(_PI, max(x.leftinterval().precision(), x.rightinterval().precision()));
		}
	}(); // The Lambda is immediately invokeed
	const IT twopi(IT(2) * pi);
	IT l(fmod(x.inf(), twopi)); // Normalize l within a single period
	IT r(l + fmod(x.sup() - x.inf(), twopi)); // Calculate r based on l and the interval width
	
	// Normalize angles to be within [0, 2*pi)
	if (l < IT(0)) l += twopi;
	if (r >= twopi) r -= twopi;

	// Check if the interval includes a vertical asymptote
	if (x.sup() - x.inf() >= pi || (floor((l + pi / IT(2)) / pi) != floor((r + pi / IT(2)) / pi))) {
		// The function covers an entire period or crosses an asymptote, range is all real numbers
		interval<IT> res(-infi, +infi);
		return res;
	}

	// Calculate tangent values at the interval's endpoints
	IT tan_l(tan(x.inf()));
	IT tan_r(tan(x.sup()));

	// Given the properties of tan(x), if the interval does not include an asymptote,
	// the minimum and maximum can be directly computed from the interval's endpoints.
	IT tan_min(std::min(tan_l, tan_r));
	IT tan_max(std::max(tan_l, tan_r));

	// Established a safety interval around the result to ensure correct bound for the computation
	if (tan_min != pi)
		tan_min = nextafter(tan_min, -infi);
	if (tan_max != pi)
		tan_max = nextafter(tan_max, +infi);

	// Create and return the interval based on calculated min and max tangent values
	interval<IT> res(tan_min, tan_max);
	// set the proper interval decoration
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// asin(x)
// This function works for both the build in types: float, double or long double
// but also for the float_precision class. (arbitrary precision). This is done by ensure that
// local float_precision declaration is performed at the precision of x. Implemented via the use of constexpr (requires c++17)
// lambda functions. 
//
template<class IT> inline interval<IT> asin(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	// Check if the input interval exceeds the domain of arcsin
	if (x.inf() < IT(-1) || x.sup() > IT(1))
	{
		// arcsin is undefined for values outside the interval[-1, 1]
		// return the empty interval and set decoration to ILL
		interval<IT> res;
		res.intervaldecoration(TRV);
		return res;
	}
	
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	// Calculate arcsin values at the interval's endpoints
	const IT asin_l(asin(x.inf()));
	const IT asin_r(asin(x.sup())); 

	// Ensure the interval is correctly oriented
	IT asin_min(std::min(asin_l, asin_r));
	IT asin_max(std::max(asin_l, asin_r));

	// Established a safety interval around the result to ensure correct bound for the computation
	asin_min = nextafter(asin_min, -infi);
	asin_max = nextafter(asin_max, +infi);

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
template<class IT> inline interval<IT> acos(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	// Check if the input interval exceeds the domain of acos
	if (x.inf() < IT(-1) || x.sup() > IT(1))
	{
		// arcsin is undefined for values outside the interval[-1, 1]
		// return the empty interval and set decoration to ILL
		interval<IT> res;
		res.intervaldecoration(ILL);
		return res;
	}
	
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	// Calculate acos values at the interval's endpoints
	const IT acos_l(acos(x.sup())); // Note: we use sup here
	const IT acos_r(acos(x.inf())); // Note: we use inf here

	// Ensure the interval is correctly oriented
	IT acos_min(std::min(acos_l, acos_r));
	IT acos_max(std::max(acos_l, acos_r));

	// Established a safety interval around the result to ensure correct bound for the computation
	acos_min = nextafter(acos_min, -infi);
	acos_max = nextafter(acos_max, +infi);

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
template<class IT> inline interval<IT> atan(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	// Calculate atan values at the interval's endpoints
	const IT atan_l(atan(x.inf()));
	const IT atan_r(atan(x.sup()));

	// Ensure the interval is correctly oriented
	IT atan_min(std::min(atan_l, atan_r));
	IT atan_max(std::max(atan_l, atan_r));

	// Established a safety interval around the result to ensure correct bound for the computation
	atan_min = nextafter(atan_min, -infi);
	atan_max = nextafter(atan_max, +infi);

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
template<class IT> inline interval<IT> sinh(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const interval<IT> one(IT(1));	// Ensure correct precision for IT=float_precision
	const interval<IT> half(IT(0.5));	// Ensure correct precision for IT=float_precision
	const interval<IT> e(exp(x));
	interval<IT> res(half * (e - one / e));
	// set the proper interval decoration 
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// Use the identity. cosh(x)=0.5*(exp(x)+1/exp(x))
template<class IT> inline interval<IT> cosh(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	interval<IT> one(x);	// Ensure correct precision for IT=float_precision
	interval<IT> half(x);	// Ensure correct precision for IT=float_precision
	const interval<IT> e(exp(x));
	one = IT(1);
	half = IT(0.5);
	interval<IT> res(half * (e + one / e));
	// set the proper interval decoration 
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

//Use the identity. tanh(x)=(exp(x)-1/exp(x))/(exp(x)+1/exp(x))=(exp(x)^2-1)/(exp(x)^2+1)
template<class IT> inline interval<IT> tanh(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const interval<IT> one(1);
	interval<IT> e(exp(x));
	e *= e;
	interval<IT> res((e-one) /(e+one));
	// set the proper interval decoration 
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// Use the identity. asinh(x)=Ln(x+sqrt(x^2+1))
// asinh(x) is defined for the entire real domain
template<class IT> inline interval<IT> asinh(const interval<IT>& x) 
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const interval<IT> one(1);
	interval<IT> xsq(x);
	xsq *= xsq;
	interval<IT> res(log(x + sqrt(xsq + one)));
	// set the proper interval decoration 
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// Use the identity. acosh(x)=Ln(x+sqrt(x^2-1))
// acosh(x) is defined for x>=1
template<class IT> inline interval<IT> acosh(const interval<IT>& x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	const interval<IT> one(1);
	interval<IT> xsq(x);
	xsq *= xsq;
	interval<IT> res(log(x + sqrt(xsq - one)));
	// set the proper interval decoration 
	res.intervaldecoration(x.intervaldecoration());
	return res;
}

// Use the identity. atanh(x)=0.5*Ln((1+x)/(1-x))
// atanh(x) is defined for -1<x<1
template<class IT> inline interval<IT> atanh(const interval<IT>&x)
{
	if (x.isEmpty())
		return interval<IT>();	// Return the EMPTY interval;
	if (x.sup() < IT(-1) || x.inf() > IT(1))
	{	// Outside defined domain, return the empty interval
		// arctanh is undefined for values outside the interval[-1, 1]
		// return the empty interval and set decoration to ILL
		interval<IT> res;
		res.intervaldecoration(ILL);
		return res;
	}
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	IT ainf(x.inf());
	IT asup(x.sup());
	// Partial outside?
	ainf = std::max(ainf, IT(-1));
	asup = std::min(asup, IT(1));
	interval<IT> xadjusted(ainf, asup);
	const interval<IT> one(1);
	const interval<IT> half(0.5);
	interval<IT> res(log((xadjusted+one)/(-xadjusted+one))*half);
	// set the proper interval decoration 
	res.intervaldecoration(x.intervaldecoration());
	if (ainf == IT(-1) || asup == IT(1))
		res.intervaldecoration(TRV);
	return res;
}

//////////////////////////////////////////////////////////////////////////////////////
///
/// END Interval sinh(), cosh(), tanh(), asinh(), acosh(), atanh()
///
//////////////////////////////////////////////////////////////////////////////////////

// Sin template class for float or double
template<class IT> inline interval<IT> sinsimpel(const interval<IT>& x)
{
	const IT infi(infinity_interval<IT>());// infi(INFINITY);
	const IT pi = acos(IT(-1));	// More precise pi value
	const IT twopi(IT(2) * pi);
	IT l(fmod(x.inf(), twopi)); // Normalize l within a single period
	IT r(l + fmod(x.sup() - x.inf(), twopi)); // Calculate r based on l and the interval width

	// Normalize angles to be within [0, 2*pi)
	if (l < IT(0))
		l += twopi;
	if (r >= twopi)
		r -= twopi;

	// If the interval width is >= 2pi, the sine function covers the full range [-1, 1]
	if (x.sup() - x.inf() >= twopi)
		return interval<IT>(IT(-1), IT(1));

	// Calculate sine values at the interval's endpoints
	IT sin_l(sin(l));
	IT sin_r(sin(r));

	// Check for critical points within the interval
	IT sin_min(std::min(sin_l, sin_r));
	IT sin_max(std::max(sin_l, sin_r));
	if (l <= pi / IT(2) && pi / IT(2) <= r)
		sin_max = IT(1); // pi/2 is within interval
	if (l <= IT(1.5) * pi && IT(1.5) * pi <= r)
		sin_min = IT(-1); // 3*pi/2 is within interval

	// Established a safety interval around the result to ensure correct bound for the computation
	if (sin_min != IT(-1) && sin_min != IT(0))
		sin_min = nextafter(sin_min, -infi);
	if (sin_max != IT(1) && sin_max != IT(0))
		sin_max = nextafter(sin_max, +infi);

	// Create and return the interval based on calculated min and max sine values
	return interval<IT>(IT(sin_min), IT(sin_max));
}


#ifdef PHASE4

inline interval<float_precision> sinsimpel(const interval<float_precision>& x)
{
	const float_precision infi(infinity_interval<float_precision>());// infi(INFINITY);
	const float_precision pi = _float_table(_PI, std::max(x.leftinterval().precision(), x.rightinterval().precision()));
	const float_precision twopi(float_precision(2) * pi);
	float_precision l(fmod(x.inf(), twopi)); // Normalize l within a single period
	float_precision r(l + fmod(x.sup() - x.inf(), twopi)); // Calculate r based on l and the interval width

	// Normalize angles to be within [0, 2*pi)
	if (l < float_precision(0))
		l += twopi;
	if (r >= twopi)
		r -= twopi;

	// If the interval width is >= 2pi, the sine function covers the full range [-1, 1]
	if (x.sup() - x.inf() >= twopi)
		return interval<float_precision>(float_precision(-1), float_precision(1));

	// Calculate sine values at the interval's endpoints
	float_precision sin_l(sin(l));
	float_precision sin_r(sin(r));

	// Check for critical points within the interval
	float_precision sin_min(std::min(sin_l, sin_r));
	float_precision sin_max(std::max(sin_l, sin_r));
	if (l <= pi / float_precision(2) && pi / float_precision(2) <= r)
		sin_max = float_precision(1); // pi/2 is within interval
	if (l <= float_precision(1.5) * pi && float_precision(1.5) * pi <= r)
		sin_min = float_precision(-1); // 3*pi/2 is within interval

	// Established a safety interval around the result to ensure correct bound for the computation
	if (sin_min != float_precision(-1) && sin_min != float_precision(0))
		sin_min = nextafter(sin_min, -infi);
	if (sin_max != float_precision(1) && sin_max != float_precision(0))
		sin_max = nextafter(sin_max, +infi);

	// Create and return the interval based on calculated min and max sine values
	return interval<float_precision>(float_precision(sin_min), float_precision(sin_max));
}

#endif

#endif
