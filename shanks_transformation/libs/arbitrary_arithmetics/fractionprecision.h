#ifndef INC_FRACTIONPRECISION
#define INC_FRACTIONPRECISION

/*
 *******************************************************************************
 *
 *
 *                       Copyright (c) 2019-2023
 *                       Henrik Vestermark
 *                       Denmark
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
 * Module name     :   fractionprecision.h
 * Module ID Nbr   :   
 * Description     :   Arbitrary fraction precision class
 *                     Actually it a general fraction class that works with both
 *                     standard types like int, or int_precision 
 * --------------------------------------------------------------------------
 * Change Record   :   
 *
 * Version	Author/Date		Description of changes
 * -------  -----------		----------------------
 * 01.01	HVE/15-JUL-2019	Initial release
 * 01.02	HVE/12-Aug-2020	Change precision type from unsinged int to size_t to enable both 32 and 64b it target.
 * 01.03	HVE/24-Mar-2021 Updated license info
 * 01.04	HVE/20-Nov-2022	Adding long long and unsigned long long casting operator, added iszero() and isone() method
 * 01.05	HVE/27-Mar-2023	Added abs both as a mthod and as a frunction for fraction_precision
 *
 * End of Change Record
 * --------------------------------------------------------------------------
*/

/* define version string */
static char _VFP_[] = "@(#)fractionprecision.h 01.05 -- Copyright (C) Henrik Vestermark";

#include <iostream>

#if defined(_MSVC_LANG)
	#if _MSVC_LANG < 201402L
		#error The fractionprecision.h code requires c++14 or higher
	#endif
#else
	#if __cplusplus < 201402L
		#error The fractionprecision.h code requires c++14 or higher
	#endif
#endif

// Complex Precision template class for fraction arithmetic
// Notice construction,assignments always guarantees that normalized fraction sign is always in the numerator. As a consequence the sign method just return the sign of the numerator
template<class _Ty> class fraction_precision {
   _Ty n, d;
   public:
      typedef _Ty value_type;

      // constructor
	  fraction_precision(const _Ty& a = (_Ty)0, const _Ty& b = (_Ty)1) : n(a), d(b) { normalize(); }		// fraction constructions
	  fraction_precision(const _Ty& whole, const _Ty& a, const _Ty& b) : n(a + whole*b), d(b) { normalize(); }  // mixed number constructions
      // constructor for any other type to _Ty
      template<class _X> fraction_precision( const fraction_precision<_X>& a ) : n((_Ty)a.numerator()), d((_Ty)a.denominator()) {}
      
      // Coordinate functions
      _Ty numerator() const { return n; }						// return numerator
      _Ty denominator() const { return d; }						// return denominator
      _Ty numerator( const _Ty& a )   { return ( n = a ); }		// Set mumerator
	  _Ty numerator( const fraction_precision<_Ty>& a )   { return ( n = a.numerator() ); }  // Set numerator from another fraction
      _Ty denominator( const _Ty& b )   { return ( d = b ); }	// Set denominator
	  _Ty denominator( const fraction_precision<_Ty>& b )   { return ( d = b.denominator() ); } // Set denominator from another fraction
	  
	  // Methods
	  fraction_precision<_Ty>& abs();
	  fraction_precision<_Ty>& normalize();
	  fraction_precision<_Ty>& inverse();
	  bool iszero() const;	// Test for zero and return true or false
	  bool isone() const;	// Test for one and return true or false
	  _Ty whole() const;	// return the whole number 
	  _Ty reduce() const;	// Reduce the fraction by removing and returning the whole number from the fraction
	   // // Conversion methods. Safer and less ambiguous than overloading implicit/explicit conversion operators
	 // std::string fraction_precision<int_precision>& toString() { return n.toString() + "/" + d.toString(); }
																			

	  // Implicit/explicit conversion operators
	  operator long long() const		{ return (long long)(n) / (long long)(d); }
	  operator long() const				{ return (long)(n) / (long)(d); }
	  operator int() const				{ return (int)n / (int)d; };
	  operator short() const			{ return (short)n / (short)d; };
	  operator char() const				{ return (char)n / (char)d; };
	  operator unsigned long long() const { return (unsigned long long)(n) / (unsigned long long)(d); }
	  operator unsigned long() const	{ return (unsigned long)( n / d ); }
	  operator unsigned int() const		{ return (unsigned int)( n / d ); }
	  operator unsigned short() const	{ return (unsigned short)( n / d ); }
	  operator unsigned char() const	{ return (unsigned char)(n / d );}
	  operator double() const			{ return (double)n / (double)d; }
	  operator float() const			{ return (float)n / (float)d; }
	  operator int_precision() const	{ return (int_precision)n / (int_precision)d; }

      // Essential operators
	  fraction_precision<_Ty>& operator= (const fraction_precision<_Ty>&);
	  fraction_precision<_Ty>& operator+=(const fraction_precision<_Ty>&);
	  fraction_precision<_Ty>& operator-=(const fraction_precision<_Ty>&);
	  fraction_precision<_Ty>& operator*=(const fraction_precision<_Ty>&);
	  fraction_precision<_Ty>& operator/=(const fraction_precision<_Ty>&);
		 
	  class divide_by_zero {};
   };

template<class _Ty> std::ostream& operator<<( std::ostream& strm, const fraction_precision<_Ty>& a )
	{ return strm << a.numerator() << "/" << a.denominator(); }

template<class _Ty> std::istream& operator>>( std::istream& strm, fraction_precision<_Ty>& f ) 
   {
   _Ty n, d; char ch;
   strm >> n;
   strm >> std::noskipws >> ch;  
   if (ch == '/') 
	   strm >> d;
   else 
	   strm.putback(ch), d = (_Ty)0;
   if(!strm.fail())
		f = fraction_precision<_Ty>( n, d );
   return strm;
   }


// Arithmetic
template<class _Ty> fraction_precision<_Ty> operator+( const fraction_precision<_Ty>&, const fraction_precision<_Ty>& );	// Binary
template<class _Ty> fraction_precision<_Ty> operator+( const fraction_precision<_Ty>& );									// Unary
template<class _Ty> fraction_precision<_Ty> operator-( const fraction_precision<_Ty>&, const fraction_precision<_Ty>& );	// Binary
template<class _Ty> fraction_precision<_Ty> operator-( const fraction_precision<_Ty>& );									// Unary
template<class _Ty> fraction_precision<_Ty> operator*( const fraction_precision<_Ty>&, const fraction_precision<_Ty>& );	// Binary
template<class _Ty> fraction_precision<_Ty> operator/( const fraction_precision<_Ty>&, const fraction_precision<_Ty>& );	// Binary
template<class _Ty> fraction_precision<_Ty> operator++(fraction_precision<_Ty>&);		// Prefix
template<class _Ty> fraction_precision<_Ty> operator++(fraction_precision<_Ty>&, int);	// Postfix
template<class _Ty> fraction_precision<_Ty> operator--(fraction_precision<_Ty>&);		// Prefix
template<class _Ty> fraction_precision<_Ty> operator--(fraction_precision<_Ty>&, int);	// Postfix
                                                                                                                       
// Boolean Comparision Operators
template<class _Ty> inline bool operator==( const fraction_precision<_Ty>&, const fraction_precision<_Ty>& );
template<class _Ty> inline bool operator!=( const fraction_precision<_Ty>&, const fraction_precision<_Ty>& );
template<class _Ty> inline bool operator>=( const fraction_precision<_Ty>&, const fraction_precision<_Ty>&);
template<class _Ty> inline bool operator> ( const fraction_precision<_Ty>&, const fraction_precision<_Ty>&);
template<class _Ty> inline bool operator<=( const fraction_precision<_Ty>&, const fraction_precision<_Ty>&);
template<class _Ty> inline bool operator< ( const fraction_precision<_Ty>&, const fraction_precision<_Ty>&);

// Fraction precision functions
fraction_precision<int_precision> bernoulli(size_t);


//////////////////////////////////////////////////////////////////////
//
//
//    Class Methods
//		abs
//		normalize
//		inverse
// 		isone
//		iszero
//
//
//////////////////////////////////////////////////////////////////////


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		abs()
//	@return 	_Ty	-	set and  the abs of the fraction
//
//	@todo
//
// Description:
//
// Set the fraction to its absolute value and return it
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::abs()
{
	if (n < (_Ty)0)
		n = -n;
	if (d < (_Ty)0)
		d = -d;
	return *this;	// return the abs of the number
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		normalize()
//	@return 	fraction_precision<_Ty>	-	return normalized fraction
//
//	@todo
//
// Description:
//  Normalize the fraction (original 2015)
// 
//  A normalize fraction has been reduced by gcd().
//	if denominator is negative then the sign is moved up to the numerator
//	if numerator == 0 then denominator is set to 1
//	if denominator is zero a divide by zero exception is thrown
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::normalize() 
{	
	_Ty z = gcd(n, d);
	if (z == (_Ty)0)
		throw divide_by_zero();
	n /= z; 
	d /= z; 
	if (n == (_Ty)0)	// If numerator is zero then set denominator to 1.
		d = (_Ty)1;
	if (!(d >= (_Ty)0)) // Check for negative sign in the denominator
	{ 
		n *= (_Ty)-1; 
		d *= (_Ty)-1;
	} 
	return *this;
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		inverse()
//	@return 	fraction_precision<_Ty>	-	return inverse fraction
//
//	@todo
//
// Description:
//  Inverse the fraction (original 2015)
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::inverse()
{	_Ty z; 
	z = n; 
	n = d;
	d = z;
	if (!(d >= 0))
	{ 
		n *= (_Ty)-1;
		d *= (_Ty)-1;
	}
	return *this;
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		iszero()
//	@return 	boolean	-	return boolean value of comparision with zero
//
//	@todo
//
// Description:
//  test for zero in the numerator (original 2015)
//
template<class _Ty> bool fraction_precision<_Ty>::iszero() const 
{ // Test for zero and return true or false
	if (n == (_Ty)0) 
		return true; 
	return false; 
}	

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		isone()
//	@return 	boolean	-	return boolean value of comparision with one
//
//	@todo
//
// Description:
//  test for zero in the numerator and denominator (original 2015)
//
template<class _Ty> bool fraction_precision<_Ty>::isone() const
{// Test for one and return true or false

	if (n == (_Ty)1 && d == (_Ty)1)
		return true;
	return false;
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		reduce()
//	@return 	_Ty	-	return the whole number of the fraction
//
//	@todo
//
// Description:
//
// Reduce the faction to a proper fraction and return the whole number of the original fraction
//
template<class _Ty> _Ty fraction_precision<_Ty>::reduce() const 
{ 
	_Ty w = n / d; 
	n %= d;
	return w;
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		whole()
//	@return 	_Ty	-	return the whole number of the fraction
//
//	@todo
//
// Description:
//
// Reduce the fraction by removing and returning the whole number from the fraction
//
template<class _Ty> _Ty fraction_precision<_Ty>::whole() const
{ 
	return n / d;	// return the whole number
}						 

//////////////////////////////////////////////////////////////////////
//
//
//    End of Class Methods
//
//
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
//
//
//    Essentialsoperators =, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=
//
//
//////////////////////////////////////////////////////////////////////

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		operator=
//	@return 	static int_precision	-	return a=b
//	@param		"a"	-	Assignment operand
//
//	@todo
//
// Description:
//  Assign operator (oroginal 2015)
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::operator=(const fraction_precision<_Ty>& a)
{
	n = a.numerator(); 
	d = a.denominator();
	normalize(); 
	return *this;
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		operator+=
//	@return 	static int_precision	-	return a+=b
//	@param		"a"	-	Adding operand
//
//	@todo
//
// Description:
//  Assign operator (oroginal 2015)
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::operator+=(const fraction_precision<_Ty>& a)
{
	n *= a.denominator(); 
	n += d * a.numerator(); 
	d *= a.denominator();
	normalize(); 
	return *this; 
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		operator+=
//	@return 	static int_precision	-	return a-=b
//	@param		"a"	-	Subtracting operand
//
//	@todo
//
// Description:
//  Assign operator (oroginal 2015)
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::operator-=(const fraction_precision<_Ty>& a)
{
	n *= a.denominator();
	n -= d * a.numerator(); 
	d *= a.denominator(); 
	normalize();
	return *this; 
}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		operator+=
//	@return 	static int_precision	-	return a*=b
//	@param		"a"	-	Multiplying operand
//
//	@todo
//
// Description:
//  Assign operator (oroginal 2015)
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::operator*=(const fraction_precision<_Ty>& a)
{
	n *= a.numerator(); 
	d *= a.denominator(); 
	normalize();
	return *this;
}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		20/Nov/2022
//	@brief 		operator+=
//	@return 	static int_precision	-	return a/=b
//	@param		"a"	-	Dividing operand
//
//	@todo
//
// Description:
//  Assign operator (oroginal 2015)
//
template<class _Ty> fraction_precision<_Ty>& fraction_precision<_Ty>::operator/=(const fraction_precision<_Ty>& a)
{
	n *= a.denominator(); 
	d *= a.numerator(); 
	normalize();
	return *this;
}

//////////////////////////////////////////////////////////////////////
//
//
//    End Essentialsoperators =, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=
//
//
//////////////////////////////////////////////////////////////////////

// Arithmetic operators
//

// lhs + rhs
template<class _Ty> fraction_precision<_Ty> operator+( const fraction_precision<_Ty>& x, const fraction_precision<_Ty>& y )
   {
   fraction_precision<_Ty> c(x);
   c += y;
   return c;
   }

// +x
template<class _Ty> fraction_precision<_Ty> operator+( const fraction_precision<_Ty>& x )
   {
   return x;
   }

// lhs - rhs
template<class _Ty> fraction_precision<_Ty> operator-( const fraction_precision<_Ty>& x, const fraction_precision<_Ty>& y )
   {
   fraction_precision<_Ty> c(x);
   c -= y;
   return c;
   }

// -x
template<class _Ty> fraction_precision<_Ty> operator-( const fraction_precision<_Ty>& x )
   {
   fraction_precision<_Ty> c(x);
   c.numerator( -c.numerator() );
   return c;
   }

// lhs * rhs
template<class _Ty> fraction_precision<_Ty> operator*( const fraction_precision<_Ty>& x, const fraction_precision<_Ty>& y )
   {
   fraction_precision<_Ty> c(x);
   c *= y;
   return c;
   }

// lhs / rhs
template<class _Ty> fraction_precision<_Ty> operator/( const fraction_precision<_Ty>& x, const fraction_precision<_Ty>& y )
   {
   fraction_precision<_Ty> c(x);
   c /= y;
   return c;
   }

// Prefix ++
template<class _Ty> fraction_precision<_Ty> operator++(fraction_precision<_Ty>& a)
	{
	a.numerator(a.numerator()+a.denominator());
	return a;
	}

// Postfix ++
template<class _Ty> fraction_precision<_Ty> operator++(fraction_precision<_Ty>& a, int)
	{
	fraction_precision<_Ty> postfix_a(a);

	a.numerator(a.numerator()+a.denominator());
	return postfix_a;
	}

// Prefix --
template<class _Ty> fraction_precision<_Ty> operator--(fraction_precision<_Ty>& a)
	{
	a.numerator(a.numerator()-a.denominator());
	return a;
	}

// Postfix --
template<class _Ty> fraction_precision<_Ty> operator--(fraction_precision<_Ty>& a, int)
	{
	fraction_precision<_Ty> postfix_a(a);

	a.numerator(a.numerator()-a.denominator());
	return postfix_a;
	}


// lhs == rhs
template<class _Ty> bool operator==( const fraction_precision<_Ty>& lhs, const fraction_precision<_Ty>& rhs )
	{fraction_precision<_Ty> a, b;
	a = lhs; 
	b = rhs; 
	return a.numerator() == b.numerator() && a.denominator() == b.denominator();
	}

// lhs != rhs
template<class _Ty> bool operator!=( const fraction_precision<_Ty>& lhs, const fraction_precision<_Ty>& rhs )
	{
	return lhs == rhs ? false : true;
	}

// lhs >= rhs
// a/b>=c/d => a*d>=c*b
template<class _Ty> bool operator>=(const fraction_precision<_Ty>& lhs, const fraction_precision<_Ty>& rhs)
{
	bool bb;
	fraction_precision<_Ty> a, b;
	a = lhs;
	b = rhs; 
	bb= a.numerator()*b.denominator()>=b.numerator()*a.denominator();
	return bb;
	}

// lhs > rhs 
// a/b>c/d => a*d>c*b
template<class _Ty> bool operator>(const fraction_precision<_Ty>& lhs, const fraction_precision<_Ty>& rhs)
{
	bool bb;
	fraction_precision<_Ty> a, b;
	a = lhs;  
	b = rhs;  
	bb = a.numerator()*b.denominator() > b.numerator()*a.denominator();
	return bb;
}

// lhs <= rhs
template<class _Ty> bool operator<=(const fraction_precision<_Ty>& lhs, const fraction_precision<_Ty>& rhs)
	{
	return lhs > rhs ? false : true;
	}

// lhs < rhs
template<class _Ty> bool operator<(const fraction_precision<_Ty>& lhs, const fraction_precision<_Ty>& rhs)
	{
	return lhs >= rhs ? false : true;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		27/Mar/2023
//	@brief 		abs()
//	@return 	template<class _Ty> -	method abs()
//
//	@todo
//
// Description:
//  Absolute value of Operand (original 2015)
//
template<class _Ty> inline fraction_precision<_Ty> abs(const fraction_precision<_Ty>& a)
{
	_Ty b(abs(a.numerator()), abs(a.denominator()));
	return b;
}

///	@author Henrik Vestermark (hve@hvks.com)
///	@date  3/Feb/2017, revised 20/JUL/2019
///	@brief 			gcd - fraction_precision for Greatest Common Divisor
///	@return 		The greates common divisor of fraction precision a
///	@param "a"	-	First operand number 
///
///	@todo
///
/// Description:
///   gcd of fraction_precision. 
///   Call normalize that do a gcd()
//
template<class _TY> inline fraction_precision<_TY> gcd(const fraction_precision<_TY>& a )
	{
	return a.normalize();
	}
#endif
