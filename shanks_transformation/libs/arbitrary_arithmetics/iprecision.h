#ifndef INC_PRECISION
#define INC_PRECISION

/*
 *******************************************************************************
 *
 *
 *                       Copyright (c) 2002-2024
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
 * Module name     :   iprecision.h
 * Module ID Nbr   :
 * Description     :   Arbitrary integer precision class
 * --------------------------------------------------------------------------
 * Change Record   :
 *
 * Version	Author/Date		Description of changes
 * -------  ---------------	----------------------
 * 03.01	HVE/14-Aug-2021	Switch to new internal binary format iptype
 * 03.02	HVE/19-Oct-2021	Passed all float testing
 * 03.03	HVE/3-Nov-2021	Added support for !,&&,|| operator working on int_precision class
 * 03.04	HVE/19-Nov-2021	Fixed compiler bugs reported by GNU cmpler on Mac
 * 03.05	HVE/20-Nov-2021 A few bugs fixed and change to avoid compiler warnings
 * 03.06	HVE/21-Nov-2021 More cleaning up and improvements
 * 03.07	HVE/11-Dec-2021	Change atoip() to be a string reference
 * 03.08	HVE/19-Jan-2022 Rename karatsuba & schonhage_strassen to followed the name convention for function call.
 *							Renamed _int_precision_umul to _int_precision_umul_school and added _int_precision_umul as a common entry for multiplcation that
 *							branch out to the most optimal multiplication algorithm to call. e.g. school, karatsuba, FFT or Schohage_strassen
 * 03.09	HVE/21-Mar-2022 Added fixed size arbitrry integers by introduce the mLimit field in the int_precision class. Added the method .precision()
 * 03.10	HVE/25-Mar-2022	Fixed a bug in the _int_precision_unegate()
 * 03.11	HVE/7-Aug-2022	Handle a carry bug in the _int_precision_umul_add()
 * 03.12	HVE/24-Aug-2022	Cleaning up code
 * 03.13	HVE/26-Aug-2022	Added int_precision constructor for float and double for completeness
 * 03.14	HVE/5-Sep-2022	Restore the _int_precision_compare() to the previous pointer arguments
 * 03.15	HVE/12-Dec-2022	Added declaration of factorial(), fallingfactorial(), binomial()
 * 03.16	HVE/25-Mar-2023	Added declaration of Stirling number of the first, second and third kind (last also known as Lah number)
 * 03.17	HVE/4-May-2023	Added the declaration of the jacobi symbol function
 * 03.18	HVE/10-May-2023	Added the declaration of miller_rabbin and baillie_PSW primality tester function
 * 03.19	HVE/24-May-2023	Reintroduce the fast division for integer /, %.
 * 03.20	HVE/10-Jun-2023	Added the random_precision class for arbitrary precision PRNG
 * 03.21	HVE/19-Jun-2023	Added the following PRNGs as classes: chacha20, xoshiro256pp, xoshiro256ss, xoshiro512pp, xoshiro512ss
 * 03.22	HVE/01-Aug-2023	Code change to make the source compiled under gcc
 * 03.23	HVE/04-Aug-2023	added the fibonacci function
 * 03.24	HVE/18-Aug-2024 Fixed an issue reported by C++17 standard, that bool operator==(const random_precision<PrngType, ReturnType>& rhs) in random_precision 
 *							was missing the template argument. Same for bool operator!=(const random_precision<PrngType, ReturnType>& rhs)
 * 
 * End of Change Record
 * --------------------------------------------------------------------------
*/

/* define version string */
static char _VI_[] = "@(#)iprecision.h 03.24 -- Copyright (C) Henrik Vestermark";

// Configuration for int_precision
// If _INT_PRECESION_FAST_DIV_REM is defined it will use a magnitude faster div and rem integer operation.
// Set for using float_precision for int_precision division when appropriate
// Speed up int_precision division with a factor of 50 (+-)
#define _INT_PRECISION_FAST_DIV_REM		
// END Configuration

#include <climits>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <complex>   // Need <complex> to support FFT functions for fast multiplications
#include <cstdlib>
#include <random>	// Needed for random_precision class and PRNGs in general

static_assert(__cplusplus >= 201402L, "The iprecision.h code requires c++14 or higher.");

// THIS is the only configuration parameter to set or change.
typedef std::uintmax_t iptype;	// The default size of the internal binary vector type, an unsigned 64bit. It should ALWAYS be set to the 'biggest' integer type.
							// However performance will suffer if iptype < 64bit or not set to the maxium the enviroment can handle.
const unsigned int Bitsiptype = sizeof(iptype) * 8;  // Const use throughout the source which is the number of bits the iptype can hold.

// Definining som default base numbers
static const int BASE_2	  = 2;
static const int BASE_8   = 8;
static const int BASE_10  = 10;  // Default
static const int BASE_16  = 16;

static const int RADIX = BASE_10;			// Set internal base for the arbitrary precision. NOT USED ANYMORE

// this is only for a few instance where we still use decimal arithmetic
inline int CHAR_SIGN( char x )            { return x == '-' ? -1 : 1; }
inline unsigned char IDIGIT10( char x )   { return (unsigned char)( x - '0'); }
inline unsigned char ICHARACTER10( char x){ return (unsigned char)( x + '0'); }

extern std::string arbitrary_precision_version();

#undef TEMPLIFY

//
// @class int_precision
// @author Henrik Vestermark (hve@hvks.com)
// @date	14/Aug/2021
// @brief  This is an arbitrary integer class
//
// @todo
//
// Precision class
// Also number is always strip for leading zeros
// Since we always initiate to a valid int_precision number, it will never be a empty number e.g. mNumber.size()>=1
// The least significan number is at mNumber[0], the most signidicant number is a mNumber[n-1]
// For iptype = uint64_t the number in mNumber is stored as:
//		mNumber=mNumber[0]+mNumber[1]*2^64+mNumber[2]*(2^64)^2...mNumber[n-1]*(2^64)^(n-1)
// if iptype=uint3_t the number in mNumber is stored as:
//		mNumber=mNumber[0]+mNumber[1]*2^32+mNumber[2]*(2^32)^2...mNumber[n-1]*(2^32)^(n-1)
// for short we will use the notation that the least significant part of the number is stored in mNumber[0] and will be denoted a0, the most significant of the number in mNumber[n-1] as an-1
// the radix, R will be 2^64 for iptype=uint64_t and 2^32 for iptype=uint3_t etc.
// the number in mNumber can be written as:
//		mNumber=a0*R^0+a1*R^1+a2*R^2...an-1*R^n-1
//
class int_precision
	{
	int mSign;						// Sign of the int_precision. Version 2+ only. In version 2 sign has been separated from mNumber to avoid many uncessary copies and string.substr() calls
									// mSign is either +1 or -1. For mNumber==0 then sign is always +1
	size_t mLimit;					// By default int_precision i ulimited precision but it can be limit to force a certain size of an integer. e.g. 128bit has limit=2, 512bit has limit=8 etc.
									// unlimit preciion has a limit on UINTMAX_MAX or SIZE_MAX
	std::vector<iptype> mNumber;	// The binary vector of iptype that holds the integer. Per definition the vector when the constructor is invoked will always be initialized to zero if no argument is provided.

	public:
	// Constructor
	int_precision();										// No initialization
 	int_precision( char, const size_t=SIZE_MAX);					// When initialized through a char
    int_precision( unsigned char, const size_t=SIZE_MAX);			// When initialized through a unsigned char
    int_precision( short, const size_t=SIZE_MAX);					// When initialized through an short
    int_precision( unsigned short, const size_t=SIZE_MAX);			// When initialized through an unsigned short
    int_precision( int, const size_t=SIZE_MAX);						// When initialized through an int
    int_precision( unsigned int, const size_t=SIZE_MAX);				// When initialized through an unsigned int
    int_precision( long, const size_t=SIZE_MAX);						// When initialized through an long
    int_precision( unsigned long, const size_t=SIZE_MAX);			// When initialized through an unsigned long
	int_precision( long long, const size_t=SIZE_MAX);				// When initialized through an long. Same as int64_t
	int_precision( unsigned long long, const size_t=SIZE_MAX);		// When initialized through an unsigned long. Same as uintmax_t
    int_precision( const char *, const size_t=SIZE_MAX);			// When initialized through a char string
	int_precision(const float, const size_t=SIZE_MAX);				// When initialized through a float
	int_precision(const double, const size_t=SIZE_MAX);				// When initialized through a double
	int_precision( const std::string&, const size_t=SIZE_MAX);		// When initialized through a std::string
	int_precision( const std::vector<iptype>&, const size_t=SIZE_MAX);	// When initialized through a std::vector<iptype>. Notice sign will be 1 since vector<iptype> is unsigned
	int_precision( const int_precision&, const size_t=SIZE_MAX);		// When initialized through another int_precision

	//template<class _TY> inline int_precision(_TY c);		// Not working as intended

    // Coordinate memebr functions
	std::vector<iptype> *pointer();							// Return a pointer to mNumber
	std::vector<iptype> number() const;						// Return a copy of mNumber
	std::vector<iptype> number(std::vector<iptype> &mb);	// Set mNumber and return a copy of mNumber 
	iptype index(const size_t inx)	const;
	int sign() const;			// Return current sign
	int sign(int s);			// Set and return sign
	int change_sign();			// Toggle and return sign 
	size_t size() const;		// Return number of iptype digits. iptype is the allocation unit of typicall 64bit?
	size_t precision() const;	// Return the maximum fixed integer precision the variable can hold in number of iptype units
	size_t precision(const size_t p);	// Set a new fixed integer precision or arbitrary precision
	int_precision& abs();		// Change sign to + and return number
	// Start of Bit Methods
	void setbit(size_t i);		// Set bit at bi position i
	void resetbit(size_t i);	// Reset bit  bit position i
	void flipbit(size_t i);		// Flip bit at bit position i
	bool testbit(size_t i);		// Test bit at bit position i
	size_t ctz();				// Count trailing zeros
	size_t clz();				// Count leading zeros
	bool even() const;			// Test for even number
	bool odd() const;			// Test for odd number
	bool iszero() const;		// Test for zero and return true or false
	// End of Bit Methods
	// Conversion methods. Safer and less ambiguous than overloading implicit/explicit conversion operators
	std::string toString(const int);	//  Convert number to Decimal String with an optional base parameter
	
	// Implicit/explicit conversion operators	
	operator float() const;
	operator double() const;
#ifdef TEMPLIFY
	template <typename T> operator T() const {	return static_cast<T>(mNumber[0] * mSign);
#else
	operator long() const; 
    operator int() const;
    operator short() const;
    operator char() const;
    operator unsigned long() const;
    operator unsigned int() const;
    operator unsigned short() const;
    operator unsigned char() const;
	operator long long() const;
	operator unsigned long long() const;
#endif

    // Essential assignment operators
    int_precision& operator=( const int_precision& );
    int_precision& operator+=( const int_precision& );
    int_precision& operator-=( const int_precision& );
    int_precision& operator*=( const int_precision& );
    int_precision& operator/=( const int_precision& );
    int_precision& operator%=( const int_precision& );
    int_precision& operator>>=( const int_precision& );
    int_precision& operator<<=( const int_precision& );
	int_precision& operator&=( const int_precision& );
    int_precision& operator|=( const int_precision& );
	int_precision& operator^=( const int_precision& );

    // Specialization
	friend std::ostream& operator<<( std::ostream& strm, const int_precision& d );
	friend std::istream& operator>>( std::istream& strm, int_precision& d );

	
    // Exception class
    class bad_int_syntax {};
    class out_of_range   {};
    class divide_by_zero {};
	class domain_error {};
	};

	// Arithmetic
	template <class _Ty> inline int_precision operator+(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator+(const _Ty&, const int_precision&);
	inline int_precision operator+(const int_precision&);  // Unary
	inline int_precision operator++(int_precision&);       // Prefix Increment
	inline int_precision operator++(int_precision&, int);  // Postfix Increment

	template <class _Ty> inline int_precision operator-(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator-(const _Ty&, const int_precision&);
	inline int_precision operator-(const int_precision&);  // Unary
	inline int_precision operator--(int_precision&);       // Prefix Decrement
	inline int_precision operator--(int_precision&, int);  // Postfix Decrement

	template <class _Ty> inline int_precision operator*(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator*(const _Ty&, const int_precision&);
	template <class _Ty> inline int_precision operator/(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator/(const _Ty&, const int_precision&);
	template <class _Ty> inline int_precision operator%(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator%(const _Ty&, const int_precision&);
	template <class _Ty> inline int_precision operator<<(int_precision&, const _Ty&);
	inline int_precision operator<<(const int_precision&, const int_precision&);
	//template <class _Ty> inline int_precision operator<<( const _Ty&, const int_precision& );  // Dont allow to avoid overloading conflict in streams library??
	template <class _Ty> inline int_precision operator >> (int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator >> (const _Ty&, const int_precision&);

	template <class _Ty> inline int_precision operator&(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator&(const _Ty&, const int_precision&);
	inline int_precision operator|(const int_precision&, const int_precision&);
	template <class _Ty> inline int_precision operator|(int_precision&, const _Ty&);
	//template <class _Ty> inline int_precision operator|(const _Ty&, const int_precision&);

	template <class _Ty> inline int_precision operator ^(int_precision&, const _Ty&);
	template <class _Ty> inline int_precision operator ^(const _Ty&, const int_precision&);
	//inline int_precision operator|(int_precision&, const int_precision&);
	int_precision operator~(const int_precision&);        // Unary Negate

	// Boolean Comparision Operators
	template <class _Ty> inline bool operator==(int_precision&, const _Ty&);
	template <class _Ty> inline bool operator==(const _Ty&, const int_precision&);
	template <class _Ty> inline bool operator!=(int_precision&, const _Ty&);
	template <class _Ty> inline bool operator!=(const _Ty&, const int_precision&);
	template <class _Ty> inline bool operator>(int_precision&, const _Ty&);
	template <class _Ty> inline bool operator>(const _Ty&, const int_precision&);
	template <class _Ty> inline bool operator>=(int_precision&, const _Ty&);
	template <class _Ty> inline bool operator>=(const _Ty&, const int_precision&);
	template <class _Ty> inline bool operator<=(int_precision&, const _Ty&);
	template <class _Ty> inline bool operator<=(const _Ty&, const int_precision&);
	template <class _Ty> inline bool operator<(int_precision&, const _Ty&);
	template <class _Ty> inline bool operator<(const _Ty&, const int_precision&);

	inline bool operator&&(const int_precision&, const int_precision&);	// Logical AND
	inline bool operator||(const int_precision&, const int_precision&);	// Logical OR
	inline bool operator!(const int_precision&);						// Logical NOT

	// Integer Precision functions
	extern int_precision abs(const int_precision&);							// return |a|
	extern int_precision ipow(const int_precision&, const int_precision&);  // return a^b
	extern int_precision ipow_modulo(const int_precision&, const int_precision&, const int_precision&); // return a^b%c
	extern bool isprime(const int_precision&, const int=0);					// regular brute force prime checker
	template <class _TY> inline _TY gcd(const _TY, const _TY);					// gcd template function. Use slow Euclid algorithm
	extern int_precision gcd(const int_precision&, const int_precision&);		// return greatest comon divisor
	extern int_precision lcm(const int_precision&, const int_precision&);		// return least comon multiplier
	extern int_precision factorial(const int_precision&);						// return the factorial
	extern int_precision fallingfactorial(const int_precision&, const int_precision&);	// return the fallingfactorial
	extern int_precision risingfactorial(const int_precision&, const int_precision&);	// return the risingfactorial
	extern int_precision binomial(const int_precision&, const int_precision&);	// return the binomial()
	extern int_precision stirling_first(const int_precision& n, const int_precision& k, const bool=false); // Stirling number of the first kind
	extern int_precision stirling_second(const int_precision&, const int_precision&);		// Stirling number of the second kind
	extern int_precision stirling_third(const int_precision&, const int_precision&, const bool=false);	// Stirling number of the third kind or Lah number
	extern int_precision jacobi(const int_precision&, const int_precision&);				// Compute the Jacobi symbol
	extern bool miller_rabin(const int_precision&, const int);					// Miller Rabin primality tester
	extern bool baillie_PSW(const int_precision&, const bool=false);			// Baillie PSW primality tester
	template<class _Ty, class _type = uint64_t> class random_precision;			// random precision class
	extern int_precision fibonacci(const int_precision&);						// Fibonacci sequence

	// Core Support functions 
	double _int_precision_iptod(const int_precision *);					// Explicit conversion to double
	std::vector<iptype> _int_precision_atoip(const char *, int *);		// char * string to int_precision
	std::vector<iptype> _int_precision_atoip(const std::string&, int *);// STL String to int_precision
	std::string itostring(const int, const unsigned);
	std::string _int_precision_itoa(int_precision&, const int base = BASE_10);
	std::string _int_precision_itoa(int_precision *, const int base = BASE_10);
	std::string _int_precision_itoa(const std::vector<iptype> *, const int base = BASE_10);
	std::string _int_precision_itoa(const std::vector<iptype>&, const int base = BASE_10);
	size_t _int_precision_ctz(const iptype);							// Count trailing zero bits in iptype
	size_t _int_precision_ctz(const std::vector<iptype> &);				// Cout trailing zero bits in a vector<iptype>
	size_t _int_precision_clz(const iptype);							// Count leading zero bits in iptype
	size_t _int_precision_clz(const std::vector<iptype> &);				// Count leading zero bits in a vector<iptype>

	// Core Binary functions that works directly on vector<iptype> class and unsigned arithmetic
	std::vector<iptype> _int_precision_uadd(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_uadd_short(const std::vector<iptype>&, const iptype);
	std::vector<iptype> _int_precision_usub(int *, const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_usub_short(int *, const std::vector<iptype>&, const iptype);
	std::vector<iptype> _int_precision_umul(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_umul_school(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_umul_short(const std::vector<iptype>&, const iptype);
	template<class _TY> inline std::vector<_TY> _precision_umul64(const _TY, const _TY);
	std::vector<iptype> _int_precision_umul_fourier(const std::vector<iptype>&, const std::vector<iptype>&, int = 8);
	std::vector<iptype> _int_precision_umul_karatsuba(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_umul_linear(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_udiv(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_udiv_short(iptype *, const std::vector<iptype>&, const iptype);
	std::vector<iptype> _int_precision_urem(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_urem_short(const std::vector<iptype>&, const iptype);
	std::vector<iptype> _int_precision_udivrem(std::vector<iptype>&, std::vector<iptype>&, std::vector<iptype> *);
	std::vector<iptype> _int_precision_ushiftright(const std::vector<iptype>&, const size_t);
	std::vector<iptype> _int_precision_ushiftleft(const std::vector<iptype>&, const size_t);
	std::vector<iptype> _int_precision_unegate(const std::vector<iptype>&);
	std::vector<iptype> _int_precision_uand(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_uor(const std::vector<iptype>&, const std::vector<iptype>&);
	std::vector<iptype> _int_precision_uxor(const std::vector<iptype>&, const std::vector<iptype>&);

	int _int_precision_compare2(std::vector<iptype>&, std::vector<iptype>&);
	int _int_precision_compare(const std::vector<iptype> *, const std::vector<iptype> *);
	void _int_precision_strip_leading_zeros(std::vector<iptype>&);
	void _int_precision_strip_trailing_zeros(std::vector<iptype>&);


//////////////////////////////////////////////////////////////////////
//
//
//    Constructors
//
//
//////////////////////////////////////////////////////////////////////


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return		nothing
//	@param		"c"	-	the character integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initilize with a signed character
//
  /*
   template<class _TY> inline int_precision::int_precision(_TY c)
   {
	   const unsigned int md = sizeof(c) / sizeof(iptype);
	   if (c < (_TY)0)
	   {
		   mSign = -1; c = -c;
	   }
	   else
		   mSign = 1;
	   if (md > 1)
	   {
		   for (int i = 0; i < md; ++i)
		   {
			   mNumber[i] = (iptype)c; c >>= (_TY)Bitsiptype;
		   }
	   }
	   else
		   mNumber.push_back(c);
   }
   */

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Mar/2022
//	@brief 		int_precision::int_precision
//	@return		nothing
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initilize with an int_precision
//
inline int_precision::int_precision()
	{
	mSign = +1;
	mLimit = SIZE_MAX;
	mNumber.resize(1, 0);	// replace assign by resize
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Mar/2022
//	@brief 		int_precision::int_precision
//	@return		nothing
//	@param		"s"	-	the int_precision variable to assign to this
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initilize with an int_precision
//
inline int_precision::int_precision(const int_precision& s, const size_t limit)
	{
	mSign = s.mSign;
	mLimit = limit;
	mNumber = s.mNumber;
	if (mNumber.size() > mLimit)	// don't do mNumber.resize(mLimit) directly size it will realloacte the vector to an unrealistic size
		mNumber.resize(mLimit);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Mar/2022
//	@brief 		int_precision::int_precision
//	@return		nothing
//	@param		"v"	-	the vector<iptype> to assign to mNumber
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initilize with a vector<iptype> 
//
inline int_precision::int_precision( const std::vector<iptype>& v, const size_t limit) 
	{
	mSign = 1;
	mLimit = limit;
	mNumber = v;
	if (mNumber.size() > mLimit)	// don't do mNumber.resize(mLimit) directly size it will realloacte the vector to an unrealistic size
		mNumber.resize(mLimit);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"str"	-	Convert the character string number into a multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate input and convert to internal representation
//
inline int_precision::int_precision(const char *str, const  size_t limit)
	{
	std::string s(str);
	if (s.empty())
		{ throw bad_int_syntax(); return; }
	mLimit = limit;
	mNumber = _int_precision_atoip(s, &mSign);
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"str"	-	Convert the std::string number into a multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate input and convert to internal representation
//
//
inline int_precision::int_precision(const std::string& str, const size_t limit)
	{
	if (str.empty())
		{ throw bad_int_syntax(); return; }
	mLimit = limit;
	mNumber= _int_precision_atoip(str, &mSign );
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return		nothing
//	@param		"c"	-	the character integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initilize with a signed character
//
inline int_precision::int_precision( char c, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	if (c < 0)
		{
		mSign = -1; c = -c;
		}
	mNumber.push_back(c);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"uc"	-	the character integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initilize with an unsigned character
//
inline int_precision::int_precision( unsigned char uc, const size_t limit )
	{
	mSign = 1;
	mLimit = limit;
	mNumber.push_back(uc);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"us"	-	the binary integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision( short us, const size_t limit )
	{
	mSign = 1;
	mLimit = limit;
	if (us < 0)
		{
		mSign = -1; us = -us;
		}
	mNumber.push_back(us);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"s"	-	the binary integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision( unsigned short s, const size_t limit )
	{
	mSign = 1;
	mLimit = limit;
	mNumber.push_back(s);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"i"	-	the binary integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision( int i, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	if (i < 0)
		{
		mSign = -1; i = -i;
		}
	mNumber.push_back(i);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return		nothing
//	@param		"ui"	-	the binary unsigned integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision( unsigned int ui, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	mNumber.push_back(ui);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"l"	-	the binary long integer to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision( long l, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	if (l < 0)
		{
		mSign = -1; l = -l;
		}
	mNumber.push_back(l);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"ul"	-	the binary unsigned long to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision( unsigned long ul, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	mNumber.push_back(ul);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"ll"	-	the binary long long to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision(long long ll, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	if (ll < 0)
		{
		mSign = -1; ll = -ll;
		}
	mNumber.push_back(ll);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"ull"	-	the binary unsigned long long to convert to multi precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with an integer
//
inline int_precision::int_precision(unsigned long long ull, const size_t limit)
	{
	mSign = 1;
	mLimit = limit;
	mNumber.push_back(ull);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		26/Aug/2022
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"d"	-	the floatto convert to int_precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with a float
//
inline int_precision::int_precision(float d, const size_t limit)
	{// Call the constructor for double
	*this = int_precision((double)d,limit);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		26/Aug/2022
//	@brief 		int_precision::int_precision
//	@return 	nothing
//	@param		"d"	-	the double to convert to int_precision number
//
//	@todo
//
// Description:
//  Constructor
//  Validate and initialize with a double
//
inline int_precision::int_precision(double d, const size_t limit)
	{
	int expo;
	std::uintmax_t fpb;

	mNumber.resize(1, 0);
	mSign = +1;
	mLimit = limit;
	if (d<1.0&&d>-1.0)
		return;			// return 0
						// d>=|1|
	if (d < 0)
		{
		mSign = -1; d = -d;
		}
	// d>=1 therefore expo>=0
	fpb = *(std::uintmax_t *)&d;
	expo = (fpb >> 52) & 0x7ff;	// Extract the exponent
	expo -= 1023;				// unbiased the double exponent
								// Put the imaginary 1 in front of the number
	fpb &= 0xfffffffffffff;		// Mask out exponent  to get the mantissa
	fpb |= 0x10000000000000ull;	// Add the implicit 1  (1ull << 52)
	if (expo <= 52)
		{
		fpb >>= 52 - expo;
		expo = 0;
		}
	else
		expo -= 52;
	mNumber[0] = fpb;
	if (expo > 0)	// Do the remaining left shift
		mNumber = _int_precision_ushiftleft(mNumber, expo);
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	}

//////////////////////////////////////////////////////////////////////
//
//
//    Implicit conversions to base types: int, short, long, char, float & double
//
//
//////////////////////////////////////////////////////////////////////

#ifdef TEMPLIFY
#else
//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator long long
//	@return 	long long -
//
//	Description:
//  This is the main operator from int_precision to regular long, int, short & char
//  Any explicit or implicit copnversion first convert to standard c long type and then to any other
//  inbuild type long long, long, int, short, char. As a type long long >= long >= int >= short >= char
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator long long() const
	{// Conversion to long long
	return static_cast<long long>(mNumber[0] * mSign);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/20221
//	@brief 		int_precision::operator long
//	@return 	long 
//
//	Description:
//  This is the main operator from int_precision to regular long, int, short & char
//  Any explicit or implicit copnversion first convert to standard c long type and then to any other
//  inbuild type int, short, char. As a type long >= int >= short >= char
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator long() const
	{// Conversion to long
	return static_cast<long>(mNumber[0] * mSign);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator int
//	@return 	int	-
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to int
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator int() const
	{// Conversion to int
	return static_cast<int>(mNumber[0] * mSign);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator short
//	@return 	short	-
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to short
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator short() const
	{// Conversion to short
	return static_cast<short>(mNumber[0] * mSign);
    }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator char
//	@return 	char -
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to char
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator char() const
	{// Conversion to char
	return static_cast<char>(mNumber[0] * mSign); 
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator unsigned long long
//	@return 	unsigned long long -
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to int
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator unsigned long long() const
	{// Conversion to unsigned long long
	return static_cast<unsigned long long>(mNumber[0]);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator unsigned long
//	@return 	unsigned long -
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to int
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator unsigned long() const
	{// Conversion to unsigned long
	return static_cast<unsigned long>(mNumber[0] );
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8-Aug-2021
//	@brief 		int_precision::operator unsigned int
//	@return 	unsigned int -
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to int
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator unsigned int() const
	{// Conversion to unsigned int
	return static_cast<unsigned int>(mNumber[0] );
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator unsigned short
//	@return 	unsigned short -
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to short
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator unsigned short() const
	{// Conversion to unsigned short
	return static_cast<unsigned short>(mNumber[0] );
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		8/Aug/2021
//	@brief 		int_precision::operator unsigned char
//	@return 	unsigned char -
//
//	Description:
//  Any explicit or implicit copnversion first convert to standard c long type and then to char
//  As with regular C type conversion the conversion truncate to desired type and a possible
//  loss in precision is possible
//
inline int_precision::operator unsigned char() const
	{// Conversion to char
	return static_cast<unsigned char>(mNumber[0] );
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		9/Aug/2021
//	@brief 		int_preceision::operator double
//	@return 	return double -
//
//	Description:
//  Conversion from int_precision to double
//
inline int_precision::operator double() const
	{// Conversion to double
	return _int_precision_iptod(this);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		9/Aug/2021
//	@brief 		int_precision::operator float
//	@return 	return float -
//
//	Description:
//  Conversion from int_precision to float
//  Using the double conversion frist and then trunk to float using standard c conversion
//
inline int_precision::operator float() const
	{// Conversion to float
	return static_cast<float>(static_cast<double>(*this));
	}
#endif

//////////////////////////////////////////////////////////////////////
//
//    Class Methods:
//			pointer
//			number
//			sign
//			change_sign
//			size
//			abs
//			setbit
//			resetbit
//			flipbit
//			testbit
//			ctz
//			clz
//			even
//			odd
//			iszero
//			toString
//
//////////////////////////////////////////////////////////////////////


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::pointer
//	@return 	return a pointer to mNumber
//
// Description:
//  Return a pointer to mNumber
//
inline std::vector<iptype> *int_precision::pointer() { return &mNumber; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::number
//	@return 	return a copy of mNumber
//
// Description:
//  Return a copy of mNumber
//
inline std::vector<iptype> int_precision::number() const { return mNumber; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::number
//	@param		"mNumber"	-	New mNumber vector<iptype>
//	@return 	Set and return a copy of mNumber
//
// Description:
//  Set and Return a copy of mNumber
//
inline std::vector<iptype> int_precision::number(std::vector<iptype> &mb) { return mNumber = mb; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::index
//	@param		"inx"	-	Index into mNumber vector<iptype>
//	@return 	return the index of mNumber[inx]
//
// Description:
//  Return the indx of mNumber[inx]
//
inline iptype int_precision::index(const size_t inx) const { return mNumber[inx]; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::sign
//	@return 	return a copy of the sign
//
// Description:
//  Return a copy of mSign
//
inline int int_precision::sign() const { return mSign; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::number
//	@param		"mSign"	-	New mSign
//	@return 	Set and return a copy of the new sign
//
// Description:
//  Set and Return a copy of mSign
//
inline int int_precision::sign(int s) { return (mSign = s); }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::change_sign
//	@return 	Change and return a copy of the sign
//
// Description:
//  Change sign and Return a copy of mSign
//
inline int int_precision::change_sign() { mSign *= -1;  return mSign; }		// Toggle and return sign 

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::size
//	@return 	return the size of the mNumber vector<iptype>
//
// Description:
// Return the size of the mNumber vector<iptype>
//
inline size_t int_precision::size() const { return mNumber.size(); }		// Return the actual number of iptype digits

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::precision
//	@return 	return the maximum size the mNumber vector<iptype> cn hold
//
// Description:
// Return the maximum precision of the mNumber vector<iptype>
//
inline size_t int_precision::precision() const { return mLimit; }		// Return the maximum number of iptype digits

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Mar/2022
//	@brief 		int_precision::precision
//	@return 	return the size of the mNumber vector<iptype>
//  @param		"p"		-- The new fixed size integer preicsion or arbitrary precision
//
// Description:
// Set and Return the new maximum precision the mNumber vector<iptype> can hold
//
inline size_t int_precision::precision(const size_t p )  
	{ 
	mLimit = p==0 ? 1 : p;  //  Can only be set to a size >= 1
	if (mLimit < mNumber.size())
		mNumber.resize(mLimit);
	return mLimit; // Return the new number of maximum iptype digits mNumber can hold 
	}		

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::abs
//	@return 	return the absolute value of the int_precision object
//
// Description:
//  Return te absolue value of the int_precision object
//
inline int_precision& int_precision::abs() { mSign = 1; return *this; }		// Change sign to + and return number

// Start of Bit Methods
//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::setbit
//	@param		"i"	-	Bit position
//	@return 	void
//
// Description:
//  Set the bit in mNumber at bit position i
//
inline void int_precision::setbit(size_t i) {
	size_t n = i / Bitsiptype;
	if (n >= mNumber.size())
		mNumber.insert(mNumber.end(), n + 1 - mNumber.size(), 0);
	mNumber[n] |= (iptype)(1) << (i % Bitsiptype);
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::resetbit
//	@param		"i"	-	Bit position
//	@return 	void
//
// Description:
// Resetset the bit in mNumber at bit position i
//
inline void int_precision::resetbit(size_t i) {
	size_t n = i / Bitsiptype;
	if (n >= mNumber.size())
		mNumber.insert(mNumber.end(), n + 1 - mNumber.size(), 0);
	mNumber[n] &= ~((iptype)(1) << (i % Bitsiptype));
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::flipbit
//	@param		"i"	-	Bit position
//	@return 	void
//
// Description:
//  Flip the bit in mNumber at bit position i
//
inline void int_precision::flipbit(size_t i) {
	size_t n = i / Bitsiptype;
	if (n >= mNumber.size())
		mNumber.insert(mNumber.end(), n + 1 - mNumber.size(), 0);
	mNumber[n] ^= ~((iptype)(1) << (i % Bitsiptype));
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::testbit
//	@param		"i"	-	Bit position
//	@return 	boolean true or false
//
// Description:
//  Test the bit in mNumber at bit position i and return true if bit is set otherwise false
//
inline bool int_precision::testbit(size_t i) {
	size_t n = i / Bitsiptype;
	if (n >= mNumber.size())
		mNumber.insert(mNumber.end(), n + 1 - mNumber.size(), 0);
	return mNumber[n] & (iptype)(1) << (i % Bitsiptype) ? true : false;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::ctz
//	@param		"i"	-	Bit position
//	@return 	number of trailing zero bit in mNumber 
//
// Description:
// Return the number of trailing zero bits in mNumber vector<iptype>
//
inline size_t int_precision::ctz() { return _int_precision_ctz(mNumber); }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::ctz
//	@param		"i"	-	Bit position
//	@return 	number of leadingzero  bit in mNumber 
//
// Description:
// Return the number of leading zero bits in mNumber vector<iptype>
//
inline size_t int_precision::clz() { return _int_precision_clz(mNumber); }

// End of bit methods

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::even
//	@return 	return true or false if mNumber is even or odd 
//
// Description:
// Return true if mNumber number is even otherwise false
//
inline bool int_precision::even() const { return (mNumber[0] & 0x1) ? false : true; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::odfd
//	@return 	return true or false if mNumber is even or odd 
//
// Description:
// Return true if mNumber number is odd otherwise false
//
inline bool int_precision::odd() const { return (mNumber[0] & 0x1) ? true : false; }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::iszero
//	@return 	return true if mNumber number is zero
//
// Description:
// Return true if mNumber number is zero
//
inline bool int_precision::iszero()	const { return mNumber.size() == 1 && mNumber[0] == 0 ? true : false; }

// Conversion methods. Safer and less ambiguous than overloading implicit/explicit conversion operators
//	@author Henrik Vestermark (hve@hvks.com)
//	@date		21/Nov/2021
//	@brief 		int_precision::toString
//	@return 	return the decimal string number of the int_precision object
//
// Description:
// Return the decimal string number of the int_precision object
//
inline std::string int_precision::toString(const int base=BASE_10) { return _int_precision_itoa(this, base); }


//////////////////////////////////////////////////////////////////////
//
//
//    Essentialsoperators =, +=, -=, *=, /=, %=, <<=, >>=, &=, |=, ^=
//
//
//////////////////////////////////////////////////////////////////////

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator=
//	@return 	static int_precision	-	return a=b
//	@param		"rhs"	-	Assignment operand. Right hand side of operator
//
// Description:
//  Assign operator
//
inline int_precision& int_precision::operator=( const int_precision& rhs )
	{
	mSign = rhs.mSign;
	mNumber = rhs.mNumber;
	if (mNumber.size() > mLimit)	// Check Limit
		mNumber.resize(mLimit);
	return *this;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator+=
//	@return		static int_precision	-	return a +=b
//	@param		"rhs"	-	Adding operand. Right hand side of operator
//
// Description:
//  += operator. 
//
inline int_precision& int_precision::operator+=( const int_precision& rhs )
	{
	int wrap_flag, compare;

	if( rhs.mSign == mSign )
		mNumber = _int_precision_uadd(const_cast<std::vector<iptype>&>(rhs.mNumber), mNumber);  // Add and no change of sign
	else
		{
		compare = _int_precision_compare(const_cast<std::vector<iptype>*>(&rhs.mNumber), &mNumber );
		if (compare > 0) // Since we subctract less the wrap indicater need not to be checked
			{
			mSign = rhs.mSign;
			mNumber = _int_precision_usub(&wrap_flag, const_cast<std::vector<iptype>&>(rhs.mNumber), mNumber);  // Subtract and change to sign1
			}
		else
			if( compare < 0 )
				mNumber = _int_precision_usub( &wrap_flag, mNumber, const_cast<std::vector<iptype>&>(rhs.mNumber)); // Subtract and no change in sign
			else
				{// result is 0
				mSign = +1;  // Change to + sign, since -0 is not allowed for the internal representation
				mNumber = std::vector<iptype>(1,0);
				}
		}

	if (mNumber.size() > mLimit)	// Check Limit
		mNumber.resize(mLimit);
	return *this;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator-=
//	@return		static int_precision	-	return a -=b
//	@param		"rhs"	-	Subtracting operand. Right hand side of operator
//
// Description:
//  -= operator
//  The essential -= operator
//  n = n - a is the same as n = n + (-a);
//
inline int_precision& int_precision::operator-=( const int_precision& rhs )
	{
	int_precision temp(rhs);

	temp.change_sign();
	*this += temp;
	return *this;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator*=
//	@return 	static int_precision	-	return a *=b
//	@param		"rhs"	-	Multiplying operand. Right hand side of operator
//
// Description:
//  *= operator
//
inline int_precision& int_precision::operator*=( const int_precision& rhs )
	{
	mSign *= rhs.mSign;  // Resulting sign
	mNumber = _int_precision_umul(mNumber, const_cast<std::vector<iptype>&>(rhs.mNumber));
	if (mSign == -1 && mNumber.size() == 1 && mNumber[0] == (iptype)0)  // Avoid -0 as result +0 is right
		mSign = +1;

	if (mNumber.size() > mLimit)	// Check limit
		mNumber.resize(mLimit);
	return *this;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator/=
//	@return 	static int_precision	-	return a /=b
//	@param		"rhs"	-	Dividing operand. Right hand side of operator
//
// Description:
//  /= operator
//
inline int_precision& int_precision::operator/=( const int_precision& rhs )
	{
	iptype wrap;

#ifdef _INT_PRECISION_FAST_DIV_REM
	// do faster floating point division if denominator is greater than 2^32 otherwise a udiv_short is 1'000 times faster
	//  however this->size() need to be bigger than 70 digits which is size()>=4.
	// and the conditions this->size()>=a->size()
	if (mNumber.size()>=4 && mNumber.size()>=rhs.size() && rhs.mNumber.size() != 1 && (rhs.mNumber[0]>>32) != 0)  
		{
		extern int_precision _int_precision_fastdiv( const int_precision&, const int_precision& );
		int_precision b=*this;
		*this = _int_precision_fastdiv( b, rhs );
		return *this;
		}
#endif

	mSign *= rhs.mSign;  // Resulting sign after division
	if (rhs.mNumber.size() == 1 && (rhs.mNumber[0]>>32)==0 ) // Make short div if denominator <= 32 bit integer.
		mNumber = _int_precision_udiv_short( &wrap, mNumber, rhs.mNumber[0]);
	else
		{// Check for division of of number that can safely be done using 64bit binary division
		mNumber = _int_precision_udiv(mNumber, const_cast<std::vector<iptype>&>(rhs.mNumber));
 		}
	
	if (mSign == -1 && mNumber.size() == 1 && mNumber[0] == (iptype)0)  // Avoid -0 as result +0 is right
		mSign = +1;

	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	return *this;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator%=
//	@return 	static int_precision	-	return a %=b
//	@param		"rhs"	-	Modulus operand. Right hande side of operand
//
// Description:
//  %= operator
//
inline int_precision& int_precision::operator%=( const int_precision& rhs )
	{
#ifdef _INT_PRECISION_FAST_DIV_REM
	// do faster floating point division if denominator is greater than 2^32 otherwise a udiv_short is 1'000 times faster
	//  however this->size() need to be bigger than 70 digits which is size()>=4.
	// and the conditions this->size()>=a->size()
	if (mNumber.size() >= 4 && mNumber.size() >= rhs.size() && rhs.mNumber.size() != 1 && (rhs.mNumber[0] >> 32) != 0)		{
		extern int_precision _int_precision_fastrem( const int_precision&, const int_precision& );
		int_precision b=*this;
		*this =_int_precision_fastrem( b, rhs );
		return *this;
		}
#endif

	if (rhs.mNumber.size() == 1 && (rhs.mNumber.front() >> 32) == 0) // Make short rem 
		mNumber = _int_precision_urem_short( mNumber, rhs.mNumber[0]);  // Short rem and sign stay the same
	else
		// Check for remainder of of number that can safely be done using 64bit binary remainder
		mNumber = _int_precision_urem(mNumber, const_cast<std::vector<iptype>&>(rhs.mNumber));	// regular rem. sign stay the same
   
	if (mSign == -1 && mNumber.size() == 1 && mNumber[0] == (iptype)0)  // Avoid -0 as result +0 is right
	   mSign = +1;

	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	return *this;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator<<=
//	@return		static int_precision	-	return shifting a<<= b
//	@param		"rhs"	-	Shifting number. Right hand side of operand
//
// Description:
//  <<= operator
//
inline int_precision& int_precision::operator<<=( const int_precision& rhs )
	{
	size_t shift= static_cast<size_t>(rhs.mNumber[0]);
	mNumber = _int_precision_ushiftleft(mNumber, shift);
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
   return *this;
   }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator>>=
//	@return		int_precision	-	return shifting a>>= b
//	@param		"rhs"	-	Shifting number. Rand hand Side of operand
//
// Description:
//  >>= operator
//
inline int_precision& int_precision::operator>>=( const int_precision& rhs )
	{
	size_t shift= static_cast<size_t>(rhs.mNumber[0]);
	mNumber = _int_precision_ushiftright(mNumber, shift);
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	return *this;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator&=
//	@return 	int_precision	-	return a &=b
//	@param		"rhs"	-	Anding operand. Right Hand Side operand
//
// Description:
//  &= operator
//
inline int_precision& int_precision::operator&=( const int_precision& rhs )
   {
   mNumber = _int_precision_uand( mNumber, rhs.mNumber);
   if (mNumber.size() > mLimit)
	   mNumber.resize(mLimit);
   return *this;
   }

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator|=
//	@return 	int_precision	-	return a |=b
//	@param		"rhs"	-	Oring operand. Right Hand Side operand
//
// Description:
//  |= operator
//
inline int_precision& int_precision::operator|=( const int_precision& rhs)
	{
	mNumber = _int_precision_uor( mNumber, rhs.mNumber);
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	return *this;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator^=
//	@return 	int_precision	-	return a ^=b
//	@param		"rhs"	-	Xoring operand. Right Hand Side operand
//
// Description:
//  ^= operator
//
inline int_precision& int_precision::operator^=(const int_precision& rhs)
	{
	mNumber = _int_precision_uxor( mNumber, rhs.mNumber);
	if (mNumber.size() > mLimit)
		mNumber.resize(mLimit);
	return *this;
	}

//////////////////////////////////////////////////////////////////////
//
//
//    Arithmetic
//
//
//////////////////////////////////////////////////////////////////////


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/19/2006
//	@brief 		operator+
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision + <any other type>
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator+( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) += int_precision(rhs);
	}


// @author Henrik Vestermark (hve@hvks.com)
// @date		2/19/2006
//	@brief 		operator+
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for <any other type> + int_precision
//
template <class _Ty> inline int_precision operator+( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) += rhs;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator unary +
//	@return 	int_precision	-	a
//	@param		"a"	-	operand
//
// Description:
//  Unary + operator
//  Do nothing
//
inline int_precision operator+( const int_precision& a )
	{
	return a;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator++ Prefix
//	@return 	int_precision	-	return the incremented a
//	@param		"a"	-	operand
//
// Description:
//  Increment operator
//
inline int_precision operator++( int_precision& a )
	{
	a += int_precision( 1 );
	return a;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/14/2005
//	@brief 		operator++ Postfix
//	@return 	int_precision	-	return the a before incrementation
//	@param		"a"	-	operand
//
// Description:
//  Postfix Increment operator
//
inline int_precision operator++( int_precision& a, int )
	{
	int_precision postfix_a(a);

	a += int_precision( 1 );
	return postfix_a;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/19/2006
//	@brief 		operator-
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision - <any other type>
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator-( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) -= int_precision(rhs);
	}


// @author Henrik Vestermark (hve@hvks.com)
// @date		2/19/2006
//	@brief 		operator-
//	@return 	int_precision	-	return addition of lhs - rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for <any other type> - int_precision
//
template <class _Ty> inline int_precision operator-( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) -= rhs;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator unary -
//	@return 	int_precision	-	-a
//	@param		"a"	-	operand for sign change
//
// Description:
//  Unary - operator
//  Change sign
//
inline int_precision operator-( const int_precision& a )
	{
	int_precision b(a);
	b.change_sign();
	return b;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator-- prefix
//	@return 	int_precision	-	return the decremented a
//	@param		"a"	-	operand
//
// Description:
//  Decrement operator
//
inline int_precision operator--( int_precision& a )
	{
	a -= int_precision( 1 );
	return a;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/14/2005
//	@brief 		operator-- postfix
//	@return 	int_precision	-	return the a before decrementation
//	@param		"a"	-	operand
//
// Description:
//  Postfix Decrement operator
//
int_precision operator--( int_precision& a, int )
	{
	int_precision postfix_a(a);
	a -= int_precision( 1 );
	return postfix_a;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/19/2006
//	@brief 		operator*
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision * <any other type>
//
template <class _Ty> inline int_precision operator*( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) *= int_precision(rhs);
	}


// @author Henrik Vestermark (hve@hvks.com)
// @date		2/19/2006
//	@brief 		operator*
//	@return 	int_precision	-	return addition of lhs - rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for <any other type> * int_precision
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator*( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) *= rhs;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/19/2006
//	@brief 		operator/
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision / <any other type>
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator/( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) /= int_precision(rhs);
	}


// @author Henrik Vestermark (hve@hvks.com)
// @date		2/19/2006
//	@brief 		operator*
//	@return 	int_precision	-	return addition of lhs - rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for <any other type> / int_precision
//
template <class _Ty> inline int_precision operator/( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) /= rhs;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/19/2006
//	@brief 		operator%
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision % <any other type>
//
template <class _Ty> inline int_precision operator%( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) %= int_precision(rhs);
	}


// @author Henrik Vestermark (hve@hvks.com)
// @date		2/19/2006
//	@brief 		operator%
//	@return 	int_precision	-	return addition of lhs - rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for <any other type> % int_precision
//
template <class _Ty> inline int_precision operator%( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) %= rhs;
	}

// @author Henrik Vestermark (hve@hvks.com)
// @date		5/sep/2021
//	@brief 		operator<<
//	@return 	int_precision	-	return addition of lhs - rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision << int_precision
//
inline int_precision operator<<(const int_precision& lhs, const int_precision& rhs)
	{
	return int_precision(lhs) <<= rhs;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/20/2006
//	@brief 		operator<<
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision << <any other type>
//
template <class _Ty> inline int_precision operator<<( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) <<= int_precision(rhs);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/20/2006
//	@brief 		operator>>
//	@return 	int_precision	-	return addition of lhs + rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for int_precision >> <any other type>
//
template <class _Ty> inline int_precision operator>>( int_precision& lhs, const _Ty& rhs )
	{
	return int_precision(lhs) >>= int_precision(rhs);
	}


// @author Henrik Vestermark (hve@hvks.com)
// @date		2/20/2006
//	@brief 		operator>>
//	@return 	int_precision	-	return addition of lhs - rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  Add operator for <any other type> >> int_precision
//
template <class _Ty> inline int_precision operator>>( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) >>= rhs;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator&
//	@return 	int_precision	-	return lhs & rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  And operator for int_precision & <any other type>
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator&( int_precision& lhs, const _Ty& rhs )
   {
   return int_precision(lhs) &= int_precision(rhs);
   }

// @author Henrik Vestermark (hve@hvks.com)
// @date		11/Aug/2021
//	@brief 		operator&
//	@return 	int_precision	-	return lhs & rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  | operator for <any other type> & int_precision
//
template <class _Ty> inline int_precision operator&( const _Ty& lhs, const int_precision& rhs )
   {
   return int_precision(lhs) &= rhs;
   }

// @author Henrik Vestermark (hve@hvks.com)
// @date		2/Sep/2021
//	@brief 		operator|
//	@return 	int_precision	-	return lhs | rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  | operator for <any other type> | int_precision
//
inline int_precision operator|(const int_precision& lhs, const int_precision& rhs)
	{
	return int_precision(lhs) |= rhs;
	}

// @author Henrik Vestermark (hve@hvks.com)
// @date		2/Sep/2021
//	@brief 		operator|
//	@return 	int_precision	-	return lhs | rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  | operator for <any other type> | int_precision
//
//template <class _Ty> inline int_precision operator|(const _Ty& lhs, const int_precision& rhs)
//	{
//	return int_precision(lhs) |= rhs;
//	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		2/Sep/2021
//	@brief 		operator&
//	@return 	int_precision	-	return lhs | rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  And operator for int_precision | <any other type>
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator|(int_precision& lhs, const _Ty& rhs)
	{
	return int_precision(lhs) |= int_precision(rhs);
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/Sep/2021
//	@brief 		operator&
//	@return 	int_precision	-	return lhs ^ rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  And operator for int_precision ^ <any other type>
//  no const on the lhs parameter to prevent ambigous overload
//
template <class _Ty> inline int_precision operator^(int_precision& lhs, const _Ty& rhs)
	{	
	return int_precision(lhs) ^= int_precision(rhs);
	}

// @author Henrik Vestermark (hve@hvks.com)
// @date		11/Aug/2021
//	@brief 		operator^
//	@return 	int_precision	-	return lhs ^ rhs
//	@param		"lhs"	-	First operand
//	@param		"rhs"	-	Second operand
//
// Description:
//  ^  operator for <any other type> ^ int_precision
//
template <class _Ty> inline int_precision operator^( const _Ty& lhs, const int_precision& rhs )
	{
	return int_precision(lhs) ^= rhs;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/Sep/2021
//	@brief 		operator unary ~(negate)
//	@return 	int_precision	-	-a
//	@param		"a"	-	operand for ~ (negate) operator
//
// Description:
//  Unary ~ operator
//  Negate Integer
//
inline int_precision operator~(const int_precision& a)
	{
	int_precision lhs;
	std::vector<iptype> des = a.number();
	std::vector<iptype>::iterator d_pos;

	for (d_pos = des.begin(); d_pos != des.end(); ++d_pos)
		{ // negating element of the number
		*d_pos = ~*d_pos;
		}
	lhs.number(des);
	return lhs; 
	}

//////////////////////////////////////////////////////////////////////
//
//
//    Comparison
//
//
//////////////////////////////////////////////////////////////////////

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator==
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean equal of two precision numbers. Early out algorithm
//  1) if sign different then result is false. We actual don't do this test because of -0==+0 we should not ocuured but just in case
//  2) if length is different then the result is false
//  3) use core compare to determine boolean value
//
template <class _Ty> inline bool operator==( int_precision& a, const _Ty& b )
	{
	int_precision c(b);
	if( a.sign()==c.sign() &&  _int_precision_compare(const_cast<int_precision&>(a).pointer(), const_cast<int_precision&>(c).pointer())==0)// Same return true
		return true;
	return false;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator==
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean equal of two precision numbers. Early out algorithm
//  1) if sign different then result is false. We actual don't do this test because of -0==+0 we should not ocuured but just in case
//  2) if length is different then the result is false
//  3) use core compare to determine boolean value
//
template <class _Ty> inline bool operator==( const _Ty& a, const int_precision& b )
	{
	int_precision c(a);
	if( c.sign()==b.sign() && _int_precision_compare(const_cast<int_precision&>(c).pointer(), const_cast<int_precision&>(b).pointer()) == 0 )    return true;
	return false;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator<
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean less of two precision numbers. Early out algorithm for higher performance
//  1) If sign different determine boolean result based on sign
//  2) Otherwise determine boolean result based length of number amnd the sign
//  3) Same sign and same length. Do a core comparison and return the result
//
template <class _Ty> inline bool operator<( int_precision& a, const _Ty& c )
	{
	int sign1, sign2, cmp;
	int_precision b(c);

	sign1 = a.sign();
	sign2 = b.sign();

	// Different signs
	if( sign1 > sign2 )
		return false;
	if( sign1 < sign2 )
		return true;

	// Same sign
	if( sign1 == 1 && a.size() < b.size() ) // Different therefore true
		return true;
	if( sign1 == 1 && a.size() > b.size() ) // Different therefore false
		return false;
	if( sign1 == -1 && a.size() > b.size() )
		return true;
	if( sign1 == -1 && a.size() < b.size() )
		return false;

	// Same sign and same length
	cmp = _int_precision_compare(const_cast<int_precision&>(a).pointer(), const_cast<int_precision&>(b).pointer());
	if( cmp < 0 && sign1 == 1 )
		return true;
	else
		if( cmp > 0 && sign1 == -1 )
			return true;

	return false;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		11/Aug/2021
//	@brief 		operator<
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean less of two precision numbers. Early out algorithm for higher performance
//  1) If sign different determine boolean result based on sign
//  2) Otherwise determine boolean result based length of number amnd the sign
//  3) Same sign and same length. Do a core comparison and return the result
//
template <class _Ty> inline bool operator<( const _Ty& c, const int_precision& b )
	{
	int sign1, sign2, cmp;
	int_precision a(c);

	sign1 = a.sign();
	sign2 = b.sign();

	// Different signs
	if( sign1 > sign2 )
		return false;
	if( sign1 < sign2 )
		return true;

	// Same sign
	if( sign1 == 1 && a.size() < b.size() ) // Different therefore true
		return true;
	if( sign1 == 1 && a.size() > b.size() ) // Different therefore false
		return false;
	if( sign1 == -1 && a.size() > b.size() )
		return true;
	if( sign1 == -1 && a.size() < b.size() )
		return false;

	// Same sign and same length
	cmp = _int_precision_compare(const_cast<int_precision&>(a).pointer(), const_cast<int_precision&>(b).pointer());
	if( cmp < 0 && sign1 == 1 )
		return true;
	else
		if( cmp > 0 && sign1 == -1 )
			return true;

	return false;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator!=
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean not equal of two precision numbers
//
template <class _Ty> inline bool operator!=( int_precision& a, const _Ty& b )
	{
	return a == b ? false : true;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator!=
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean not equal of two precision numbers
//
template <class _Ty> inline bool operator!=( const _Ty& a, const int_precision& b )
	{
	return a == b ? false : true;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator>
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean greater of two precision numbers
//
template <class _Ty> inline bool operator>( int_precision& a, const _Ty& b )
	{
	return b < a ? true : false;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator>
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean greater of two precision numbers
//
template <class _Ty> inline bool operator>( const _Ty& a, const int_precision& b )
	{
	int_precision c(a);
	return b < c ? true : false;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator<=
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean less or equal of two precision numbers
//
template <class _Ty> inline bool operator<=( int_precision& a, const _Ty& b )
	{
	return b < a ? false : true;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator<=
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean less or equal of two precision numbers
//
template <class _Ty> inline bool operator<=( const _Ty& a, const int_precision& b )
	{
	int_precision c(a);
	return b < c ? false : true;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator>=
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean greater or equal of two precision numbers
//
template <class _Ty> inline bool operator>=( int_precision& a, const _Ty& b )
	{
	return a < b ? false: true;
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		1/19/2005
//	@brief 		operator>=
//	@return 	bool	- 	the boolean value of the operator
//	@param		"a"	-	First operand number to compare
//	@param		"b"	-	Second operand number to
//
// Description:
//  Boolean less or equal of two precision numbers
//
template <class _Ty> inline bool operator>=( const _Ty& a, const int_precision& b )
	{
	return a < b ? false: true;
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/Feb/2017, revised 20/JUL/2019
//	@brief 		gcd - Greatest Common Divisor
//	@return 	The greates common divisor or a & b
//	@param		"a"	-	First operand number 
//	@param		"b"	-	Second operand number
//
// Description:
//  gcd of two integer. Tis should work for both signed and unsigned operands
//  change the while loop while(b>0) to while(b!=0) to accomodate negative b
//	use "slow" Euclid algorithm. in precisioncore.cpp is a dedicated gcd for int_precisions argument
//
template<class _Ty> inline _Ty gcd(const _Ty lhs, const _Ty rhs)
	{
	_Ty tmp, a = lhs, b = rhs;
	// GCD(0,rhs)==rhs; GCD(lhs,0)==0; GCD(0,0)==0
	if (a == (_Ty)0) return b;
	if (b == (_Ty)0) return a;
	while (b !=(_Ty)0) { tmp = b; b = a%b; a = tmp; }
	return a;
	}

//////////////////////////////////////////////////////////////////////
//
//
//    Logical !,&&,||
//
//
//////////////////////////////////////////////////////////////////////

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/Nov/2021
//	@brief 		operator unary !(Logical NOT)
//	@return 	int_precision	-	-a
//	@param		"a"	-	operand for ! operator
//
// Description:
//  Unary ~ operator
//  Negate Integer
//
inline bool operator!(const int_precision& a)
	{
	return a.iszero();
	}


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/Nov/2021
//	@brief 		operator unary && (Logical and)
//	@return 	int_precision	-	-a
//	@param		"a"	-	operand for && operator
//
// Description:
//  Unary ~ operator
//  Negate Integer
//
inline bool operator&&(const int_precision& a, const int_precision& b)
	{
	return !a.iszero() && !b.iszero();
	}

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		3/Nov/2021
//	@brief 		operator unary || (Logical or)
//	@return 	int_precision	-	-a
//	@param		"a"	-	operand for || operator
//
// Description:
//  Unary ~ operator
//  Negate Integer
//
inline bool operator||(const int_precision& a, const int_precision& b)
	{
	return !a.iszero() || !b.iszero();
	}

//////////////////////////////////////////////////////////////////////////////////
//
//	Random precision class
//		with supporting other random generators
//		ChacHa20
//		Xoshiro256pp
//		Xoshiro256ss
//		Xoshiro512pp
//		Xoshiro512ss
//
//////////////////////////////////////////////////////////////////////////////////

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		10/Jun/2023
//	@brief 		random_precision template class
//	@param		"_TY"	-	The underlying random number generator class
//  @param		"_type"	-	The return type of the operator() and min() and max()
// 
// Description:
//		Implement the arbitrary precision version from the underlying (32-64-bit random class)
// 
// 
template<class PrngType, class ReturnType> class random_precision
{
	PrngType generator;

	static inline unsigned long long splitmix64(unsigned long long x) {
		x += 0x9E3779B97F4A7C15;
		x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9;
		x = (x ^ (x >> 27)) * 0x94D049BB133111EB;
		return x ^ (x >> 31);
	}

public:
	random_precision(const int_precision& val = std::random_device{}())
	{   // Initialization
		seed(val);
	}

	random_precision(const std::seed_seq& seeds)
	{   // Initialization through seed_seq seed
		seed(seeds);
	}

	void seed(const int_precision& seed_value)
	{   // Seed value is either a uint32_t or uint64_t
		generator.seed(ReturnType(seed_value));
	}

	void seed(std::seed_seq& seeds)	// No const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		std::array<unsigned, 1> sequence;
		seeds.generate(sequence.begin(), sequence.end());
		generator.seed(splitmix64(static_cast<unsigned long long>(sequence[0])));
	}

	static int_precision min()
	{
		return int_precision(0ull);
	}

	static int_precision max(const std::uintmax_t bitcnt = 64)
	{
		if (bitcnt <= 64)
			return int_precision(~0ull);
		int_precision m;
		m.setbit(bitcnt);           // 2^bitcnt
		m -= int_precision(1);      // 2^bitcnt-1
		return m;
	}

	int_precision operator()(const std::uintmax_t bitcnt = 64)
	{
		int_precision result(0);
		iptype a;
		size_t bcnt = bitcnt % 64;
		// Ensure uniform distribution of the random numbers
		std::uniform_int_distribution<std::uintmax_t> disbits(0, bitcnt);
		std::uniform_int_distribution<std::uintmax_t> dis(0, (~0ull));  // Full 64bit range

		bcnt = disbits(generator);
		a = bcnt % 64;
		// Build int_precision random number
		// Set most significant 64bits segment
		if (a != 0)
		{
			if (bcnt < 64)
				a = dis(generator);
			else
			{
				if (a == 63)
					a = ~(0ull);
				else
				{
					a = 1ull << (a + 1);
					a -= 1;
				}
				std::uniform_int_distribution<std::uintmax_t> distop(0, a);      // Most significant range
				a = distop(generator);
			}
			result = int_precision(a);
		}
		for (; bcnt > 64; bcnt -= 64)
		{
			result <<= Bitsiptype;
			result += dis(generator);
		}

		return result;
	}

	bool operator==(const random_precision<PrngType, ReturnType>& rhs) const
	{
		return this->generator == rhs.generator;
	}

	bool operator!=(const random_precision<PrngType, ReturnType>& rhs) const
	{
		return this->generator != rhs.generator;
	}

	void discard(const unsigned long long z)
	{
		for (unsigned long long i = z; i > 0; --i)
			(void)operator()();
	}
};


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		10/Jun/2023
//	@brief 		chacha20 class for PRNG
// 
// Description:
//		Implement the 32-bit chacha20 random number generator 
// 
// ChaCha20 PRNG class
//
class chacha20
{
	// ChaCh20 output 32 bit unsigned integers
	// The 3 inialization key, nonce & counter
	std::vector<uint8_t> key_;
	std::vector<uint8_t> nonce_;
	uint32_t counter_;          // Number of 16 block generated
	std::array<uint32_t, 16> block_; // Holds the next 16 random numbers
	int position_;              // Current position into the block generated

	// ChaCha20 constants
	const std::array<uint32_t, 4> kInitialState = { 0x61707865, 0x3320646e, 0x79622d32, 0x6b206574 };
	const std::array<uint8_t, 16> kSigma = { 'e', 'x', 'p', 'a', 'n', 'd', ' ', '3', '2', '-', 'b', 'y', 't', 'e', ' ', 'k' };

	// ChaCha20 quarter round operation
	static void QuarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d)
	{
		a += b; d ^= a; d = (d << 16) | (d >> 16);
		c += d; b ^= c; b = (b << 12) | (b >> 20);
		a += b; d ^= a; d = (d << 8) | (d >> 24);
		c += d; b ^= c; b = (b << 7) | (b >> 25);
	}

	// ChaCha20 core function
	static void ChaCha20Core(const std::array<uint32_t, 16>& input, std::array<uint32_t, 16>& output)
	{
		std::array<uint32_t, 16> state = input;

		for (int i = 0; i < 10; ++i) {
			// Column rounds
			QuarterRound(state[0], state[4], state[8], state[12]);
			QuarterRound(state[1], state[5], state[9], state[13]);
			QuarterRound(state[2], state[6], state[10], state[14]);
			QuarterRound(state[3], state[7], state[11], state[15]);

			// Diagonal rounds
			QuarterRound(state[0], state[5], state[10], state[15]);
			QuarterRound(state[1], state[6], state[11], state[12]);
			QuarterRound(state[2], state[7], state[8], state[13]);
			QuarterRound(state[3], state[4], state[9], state[14]);
		}

		for (int i = 0; i < 16; ++i) {
			output[i] = state[i] + input[i];
		}
	}

	// Generate the next 16 random numbers
	void generateNewBlock()
	{
		std::array<uint32_t, 16> input;
		std::array<uint32_t, 16> output;

		// Set the ChaCha20 initial state
		input[0] = kInitialState[0];
		input[1] = kInitialState[1];
		input[2] = kInitialState[2];
		input[3] = kInitialState[3];

		// Set the key, nonce, and counter
		std::copy(kSigma.begin(), kSigma.end(), reinterpret_cast<uint8_t*>(&input[4]));
		std::copy(key_.begin(), key_.end(), reinterpret_cast<uint8_t*>(&input[8]));
		std::copy(nonce_.begin(), nonce_.end(), reinterpret_cast<uint8_t*>(&input[12]));
		input[14] = counter_;

		ChaCha20Core(input, output);

		// Copy the output to the block
		std::copy(output.begin(), output.end(), block_.begin());
		++counter_;
	}

public:
	// Constructor
	chacha20(const std::vector<uint8_t>& key, const std::vector<uint8_t>& nonce, const uint32_t counter)
		: key_(key), nonce_(nonce), counter_(counter), position_(0) {
		block_.fill(0);
	}
	chacha20() { seed(); };

	// Seed with 3 parameters
	void seed(const std::vector<uint8_t>& key, const std::vector<uint8_t>& nonce, const uint32_t counter)
	{
		//key_ = key;
		//nonce_ = nonce;
		key_.assign(key.begin(), key.end());
		nonce_.assign(nonce.begin(), nonce.end());
		counter_ = counter;
		position_ = 0;
	}

	// Seed with a single value or no value at all
	void seed(const uint32_t s = std::random_device{}())
	{
		std::mt19937 gen(s);  // use the build in mt19937 PRNG for random values
		std::uniform_int_distribution<uint32_t> dis(1, 0xfe);

		key_.clear();
		for (int i = 0; i < 16; ++i)
			key_.push_back(static_cast<uint8_t>(dis(gen)));
		nonce_.clear();
		for (int i = 0; i < 8; ++i)
			nonce_.push_back(static_cast<uint8_t>(dis(gen)));
		counter_ = gen();
		position_ = 0;
	}

	void seed(std::seed_seq& seeds)	// No const qualifier for gcc comptability
	{// Initialization through seed_seq seed
		std::array<uint32_t, 16> sequencekey;
		std::array<uint32_t, 16> sequencenonce;
		std::array<uint32_t, 1> sequencecounter;

		seeds.generate(sequencekey.begin(), sequencekey.end());
		key_.clear();
		for (int i = 0; i < 16; ++i)
			key_.push_back(static_cast<uint8_t>(sequencekey[i]));
		seeds.generate(sequencenonce.begin(), sequencenonce.end());
		nonce_.clear();
		for (int i = 0; i < 8; ++i)
			nonce_.push_back(static_cast<uint8_t>(sequencenonce[i]));

		seeds.generate(sequencecounter.begin(), sequencecounter.end());
		counter_ = sequencecounter[0];
		position_ = 0;
	}

	// Get next random number
	uint32_t operator()()
	{
		if (position_ == 0 || position_ >= 16)
		{
			generateNewBlock();
			position_ = 0;
		}

		uint32_t randomNumber = block_[position_];
		++position_;
		return randomNumber;
	}
	// Return min value
	static constexpr uint32_t min()
	{
		return uint32_t(0ul);
	}
	// Return max value
	static constexpr uint32_t max()
	{
		return std::numeric_limits<uint32_t>::max();
	}
	// Comparison for == or !=
	bool operator==(const chacha20& rhs) const
	{
		return this->key_ == rhs.key_ && this->nonce_ == rhs.nonce_ && this->counter_ == rhs.counter_;
	}
	bool operator!=(const chacha20& rhs) const
	{
		return this->key_ != rhs.key_ || this->nonce_ != rhs.nonce_ || this->counter_ != rhs.counter_;
	}
	// Discard the next z random generatoed numbers
	void discard(const unsigned long z) {
		for (unsigned long long i = z; i > 0; --i)
			(void)operator()();
	}
};

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		19/Jun/2023
//	@brief 		xoshiro256pp class for PRNG
// 
// Description:
//		Implement the 64-bit xoshiro256pp random number generator 
// 
// xoshiro256++ random number generator implementation
// This is xoshiro256++ 1.0, one of our all-purpose, rock-solid generators.
//   It has excellent (sub-ns) speed, a state (256 bits) that is large
//   enough for any parallel application, and it passes all tests we are
//   aware of.
//
//   For generating just floating-point numbers, xoshiro256+ is even faster.
//
//   The state must be seeded so that it is not everywhere zero. If you have
//   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
//   output to fill s. 
class xoshiro256pp {
	using result_type = uint64_t;
	std::array<result_type, 4> s;   // Internal state 256 bits

	static result_type rotl(const result_type x, const int k)
	{
		return (x << k) | (x >> (64 - k));
	}

	static result_type splitmix64(result_type x)
	{
		x += 0x9E3779B97F4A7C15;
		x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9;
		x = (x ^ (x >> 27)) * 0x94D049BB133111EB;
		return x ^ (x >> 31);
	}

public:

	xoshiro256pp(const result_type val = std::random_device{}() /* 5489u*/)
	{// Initialization
		seed(val);
	}

	xoshiro256pp(std::seed_seq& seeds)	// No const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		seed(seeds);
	}

	void seed(const result_type seed_value)
	{
		for (int i = 0; i < 4; ++i)
			s[i] = splitmix64(seed_value + i);
	}

	void seed(std::seed_seq& seeds)	// No const qualifier got gcc compatability
	{   // Initialization through seed_seq seed
		std::array<unsigned, 4> sequence;
		seeds.generate(sequence.begin(), sequence.end());
		for (int i = 0; i < 4; ++i)
			s[i] = splitmix64(static_cast<result_type>(sequence[i]));
	}

	static result_type min()
	{
		return result_type(0u);
	}

	static result_type max()
	{
		return  std::numeric_limits<result_type>::max();
	}

	result_type operator()()
	{  /// 256++
		const result_type result = rotl(s[0] + s[3], 23) + s[0];
		const result_type t = s[1] << 17;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];
		s[2] ^= t;
		s[3] = rotl(s[3], 45);

		return result;
	}

	bool operator==(const xoshiro256pp& rhs) const
	{
		return this->s == rhs.s;
	}

	bool operator!=(const xoshiro256pp& rhs) const
	{
		return this->s != rhs.s;
	}

	void discard(const unsigned long long z)
	{
		for (unsigned long long i = z; i > 0; --i)
			(void)operator()();
	}
};


//	@author Henrik Vestermark (hve@hvks.com)
//	@date		19/Jun/2023
//	@brief 		xoshiro256ss class for PRNG
// 
// Description:
//		Implement the 64-bit xoshiro256ss random number generator 
// 
// xoshiro256** random number generator implementation
// This is xoshiro256** 1.0, one of our all-purpose, rock-solid
//   generators. It has excellent (sub-ns) speed, a state (256 bits) that is
//   large enough for any parallel application, and it passes all tests we
//   are aware of.
//
//   For generating just floating-point numbers, xoshiro256+ is even faster.
//
//   The state must be seeded so that it is not everywhere zero. If you have
//   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
//   output to fill s.
class xoshiro256ss
{// Private
	using result_type = uint64_t;
	std::array<result_type, 4> s;   // Internal state 256 bits

	static inline result_type rotl(const result_type x, const int k)
	{
		return (x << k) | (x >> (64 - k));
	}

	static inline result_type splitmix64(result_type x)
	{
		x += 0x9E3779B97F4A7C15;
		x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9;
		x = (x ^ (x >> 27)) * 0x94D049BB133111EB;
		return x ^ (x >> 31);
	}

public:
	inline xoshiro256ss(const result_type val = std::random_device{}())
	{// Initialization
		seed(val);
	}

	inline xoshiro256ss(std::seed_seq& seeds)	// no const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		seed(seeds);
	}

	inline void seed(const result_type seed_value)	
	{
		for (int i = 0; i < 4; ++i)
			s[i] = splitmix64(seed_value + i);
	}

	inline void seed(std::seed_seq& seeds)	// no const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		std::array<unsigned, 4> sequence;
		seeds.generate(sequence.begin(), sequence.end());
		for (size_t i = 0; i < sequence.size(); ++i)
			s[i] = splitmix64(static_cast<result_type>(sequence[i]));
	}

	static result_type min()
	{
		return result_type(0ull);
	}

	static result_type max()
	{
		return  std::numeric_limits<result_type>::max();
	}

	inline result_type operator()()
	{   // 256**
		const uint64_t result = rotl(s[1] * 5, 7) * 9;
		const uint64_t t = s[1] << 17;

		s[2] ^= s[0];
		s[3] ^= s[1];
		s[1] ^= s[2];
		s[0] ^= s[3];
		s[2] ^= t;
		s[3] = rotl(s[3], 45);

		return result;
	}

	inline bool operator==(const xoshiro256ss& rhs) const
	{
		return this->s == rhs.s;
	}

	inline bool operator!=(const xoshiro256ss& rhs) const
	{
		return this->s != rhs.s;
	}

	inline void discard(const unsigned long long z)
	{
		for (unsigned long long i = z; i > 0; --i)
			(void)operator()();
	}
};

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		19/Jun/2023
//	@brief 		xoshiro512pp class for PRNG
// 
// Description:
//		Implement the 64-bit xoshiro512pp random number generator 
// 
// xoshiro512++ random number generator implementation
// This is xoshiro512++ 1.0, one of our all-purpose, rock-solid
//   generators. It has excellent (about 1ns) speed, a state (512 bits) that
//   is large enough for any parallel application, and it passes all tests
//   we are aware of.
//
//   For generating just floating-point numbers, xoshiro512+ is even faster.
//
//   The state must be seeded so that it is not everywhere zero. If you have
//   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
//   output to fill s.
class xoshiro512pp {
	using result_type = uint64_t;
	std::array<result_type, 8> s;   // Internal state 512 bits

	static inline result_type rotl(const result_type x, const int k) {
		return (x << k) | (x >> (64 - k));
	}

	static inline result_type splitmix64(result_type x) {
		x += 0x9E3779B97F4A7C15;
		x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9;
		x = (x ^ (x >> 27)) * 0x94D049BB133111EB;
		return x ^ (x >> 31);
	}

public:
	inline xoshiro512pp(const result_type val = std::random_device{}() /*5489u*/)
	{// Initialization
		seed(val);
	}

	inline xoshiro512pp(std::seed_seq& seeds)	// No const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		seed(seeds);
	}

	inline void seed(const result_type seed_value) {
		for (int i = 0; i < 8; ++i) {
			s[i] = splitmix64(seed_value + i);
		}
	}

	inline void seed(std::seed_seq& seeds)	// No const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		std::array<unsigned, 8> sequence;
		seeds.generate(sequence.begin(), sequence.end());
		for (size_t i = 0; i < sequence.size(); ++i)
			s[i] = splitmix64(static_cast<result_type>(sequence[i]));
	}

	static result_type min()
	{
		return result_type(0ull);
	}

	static result_type max()
	{
		return  std::numeric_limits<result_type>::max();
	}

	inline result_type operator()()
	{//512++
		const result_type result = rotl(s[0] + s[2], 17) + s[2];
		const result_type t = s[1] << 11;

		s[2] ^= s[0];
		s[5] ^= s[1];
		s[1] ^= s[2];
		s[7] ^= s[3];
		s[3] ^= s[4];
		s[4] ^= s[5];
		s[0] ^= s[6];
		s[6] ^= s[7];
		s[6] ^= t;
		s[7] = rotl(s[7], 21);

		return result;
	}

	inline bool operator==(const xoshiro512pp& rhs) const
	{
		return this->s == rhs.s;
	}

	inline bool operator!=(const xoshiro512pp& rhs) const
	{
		return this->s != rhs.s;
	}

	inline void discard(const unsigned long long z)
	{
		for (unsigned long long i = z; i > 0; --i)
			(void)operator()();
	}
};

//	@author Henrik Vestermark (hve@hvks.com)
//	@date		19/Jun/2023
//	@brief 		xoshiro512ss class for PRNG
// 
// Description:
//		Implement the 64-bit xoshiro512ss random number generator 
// 
// This is xoshiro512** 1.0, one of our all-purpose, rock-solid generators
//   with increased state size. It has excellent (about 1ns) speed, a state
//   (512 bits) that is large enough for any parallel application, and it
//   passes all tests we are aware of.
//
//   For generating just floating-point numbers, xoshiro512+ is even faster.
//
//   The state must be seeded so that it is not everywhere zero. If you have
//   a 64-bit seed, we suggest to seed a splitmix64 generator and use its
//   output to fill s.
class xoshiro512ss {
	using result_type = uint64_t;
	std::array<result_type, 8> s;       // Internal state 512 bits

	static inline result_type rotl(const result_type x, const int k) {
		return (x << k) | (x >> (64 - k));
	}

	static inline result_type splitmix64(result_type x) {
		x += 0x9E3779B97F4A7C15;
		x = (x ^ (x >> 30)) * 0xBF58476D1CE4E5B9;
		x = (x ^ (x >> 27)) * 0x94D049BB133111EB;
		return x ^ (x >> 31);
	}

public:
	inline xoshiro512ss(const result_type val = std::random_device{}())
	{   // Initialization
		seed(val);
	}

	inline xoshiro512ss(std::seed_seq& seeds) // No const qualifier for gcc compatability 
	{   // Initialization through seed_seq seed
		seed(seeds);
	}

	inline void seed(const result_type seed_value)
	{   // Regular seed
		for (int i = 0; i < 8; ++i) {
			s[i] = splitmix64(seed_value + i);
		}
	}

	inline void seed(std::seed_seq& seeds)	// No const qualifier for gcc compatability
	{   // Initialization through seed_seq seed
		std::array<unsigned, 8> sequence;
		seeds.generate(sequence.begin(), sequence.end());
		for (size_t i = 0; i < sequence.size(); ++i)
			s[i] = splitmix64(static_cast<result_type>(sequence[i]));
	}

	static result_type min()
	{
		return result_type(0ull);
	}

	static result_type max()
	{
		return std::numeric_limits<result_type>::max();
	}

	inline result_type operator()()
	{//512**
		const result_type result = rotl(s[1] * 5, 7) * 9;
		const result_type t = s[1] << 11;

		s[2] ^= s[0];
		s[5] ^= s[1];
		s[1] ^= s[2];
		s[7] ^= s[3];
		s[3] ^= s[4];
		s[4] ^= s[5];
		s[0] ^= s[6];
		s[6] ^= s[7];
		s[6] ^= t;
		s[7] = rotl(s[7], 21);

		return result;
	}

	inline bool operator==(const xoshiro512ss& rhs) const
	{
		return this->s == rhs.s;
	}

	inline bool operator!=(const xoshiro512ss& rhs) const
	{
		return this->s != rhs.s;
	}

	inline void discard(const unsigned long long z)
	{
		for (unsigned long long i = z; i > 0; --i)
			(void)operator()();
	}
};

/////////////////////////////////////////////////////////////////////////////
//
//	END random precision
//
/////////////////////////////////////////////////////////////////////////////

#endif