#ifndef INTERVALPRECISION_FWD_HPP
#define INTERVALPRECISION_FWD_HPP
#pragma once

namespace intprec {

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
// enum round_mode { ROUND_NEAR, ROUND_UP, ROUND_DOWN, ROUND_ZERO };

// The five different interval types
// # Close   a<=x<=b	[a,b]
// # Left open a<x<=b	(a,b]	same as Right close
// # Right open a<=x<b	[a,b)	same as Left close
// # Open a<x<b			(a,b)
// # EMPTY interval
enum interval_type { EMPTY, CLOSE, LEFT_OPEN, RIGHT_OPEN, OPEN, RIGHT_CLOSE = LEFT_OPEN, LEFT_CLOSE = RIGHT_OPEN };

// There are 5 decorations values in the IEEE1788 standard
// Common:	COM, bounded, nonempty and the computed interval(fx) is bounded
// Defined & Continuous: DAC, nonempty and continuous
// Defined: DEF, nonempty
// Trivial: TRV, always true, no information is given
// Ill-formed: ILL, Not an Interval (NAI)
// Compute: COMPUTE is just a trigger for that the interval decoration has to be re-computed for the interval
//
enum interval_decoration { COMPUTE, ILL, TRV, DEF, DAC, COM };

template <FloatLike IT>
class interval {
private:
    IT left;                  // Left interval
    IT right;                 // Right interval
    enum interval_type type;  // Interval type CLOSE, OPEN, LEFT_OPEN, RIGHT_OPEN, (RIGHT_CLOSE is synonym for LEFT_OPEN
                              // and LEFT_CLOSE same as RIGHT_OPEN
    enum interval_decoration decoration;  // Decoration type COM,DAC,DEF,TRV, ILL

    std::pair<IT, IT> two_sum(const IT& a, const IT& b);
    std::pair<IT, IT> two_prod(const IT& a, const IT& b);
    IT split(const IT& a);
    static std::pair<IT, IT> fasttwo_sum(const IT& a, const IT& b);
    std::pair<IT, IT> fasttwo_prod(const IT& a, const IT& b);

public:
    using value_type = IT;

    // constructor. zero, one or two arguments for type IT
    explicit interval();           // Empty interval
    explicit interval(const IT&);  // Singleton interval
    // Regular interval with interval_type (default CLOSE)
    explicit interval(const IT&, const IT&, const enum interval_type t = CLOSE);

    // Implicit promotion constructors from integer types to allow Eigen traits to instantiate Scalar(1) or Scalar(0)
    interval(int val) : left(IT(val)), right(IT(val)), type(CLOSE), decoration(COM) {}
    interval(long val) : left(IT(val)), right(IT(val)), type(CLOSE), decoration(COM) {}
    interval(long long val) : left(IT(val)), right(IT(val)), type(CLOSE), decoration(COM) {}

    // Constructor for mixed type IT != _X (base types). Allows auto construction of e.g. interval<double> x(float)
    // Notice that the phase one constructor above is still valid when both the interval type IT and the argument
    // is also of the same type
    template <typename _X>
    explicit interval(const _X&);  // Singleton interval
    // Regular interval with interval_type (default CLOSE)
    template <typename _X, typename _Y>
    explicit interval(const _X&, const _Y&, const enum interval_type t = CLOSE);
    // constructor for any other type to IT. Both up and down conversions are possible
    // constructor for an interval<_X> argument
    template <typename _X>
    explicit interval(const interval<_X>& a);

    // Coordinate functions.
    IT rightinterval() const;                                   // Return rightinterval bound
    IT leftinterval() const;                                    // Return leftinterval bound
    IT rightinterval(const IT&);                                // Set and return rightinterval bound
    IT leftinterval(const IT&);                                 // Set and return leftinterval bound
    enum interval_type intervaltype() const;                    // Return interval type
    enum interval_type intervaltype(const enum interval_type);  // Set and return interval type
    enum interval_decoration intervaldecoration() const;        // Return the decoration information
    enum interval_decoration intervaldecoration(
        const enum interval_decoration);  // Set and return decoration information

    // IEEE1788 standard functions
    IT inf(bool toclose = false) const;  // Return infimum of interval
    IT sup(bool toclose = false) const;  // Return supremum of interval
    IT mid() const;                      // Return midpoint of interval
    IT rad() const;                      // Return radius of interval
    IT wid() const;                      // Return width of interval
    IT mig() const;                      // Return Mignitude. inf(|x|)
    IT mag() const;                      // Return Magnitude. sup(|x|)

    // Is methods as required per IEEE 1788 standard
    bool isEmpty() const;
    bool isEntire() const;
    bool isPoint() const;
    bool isImproper() const;
    bool isProper() const;

    bool in(const IT& i);  // Check if an pointis within the interval

    // Miscellaneous but usefull coordinate functions
    enum int_class isClass() const;
    std::string toString() const;  // Convert interval to a string

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
    // operator float_precision() const;

    // Essential operators
    interval<IT>& operator=(const interval<IT>&);
    interval<IT>& operator+=(const interval<IT>&);
    interval<IT>& operator-=(const interval<IT>&);
    interval<IT>& operator*=(const interval<IT>&);
    interval<IT>& operator/=(const interval<IT>&);
    interval<IT>& operator&=(const interval<IT>&);  // Intersection
    interval<IT>& operator|=(const interval<IT>&);  // Union
    interval<IT>& operator^=(const interval<IT>&);  // minus intersection

    // Check that this class is only for float or double
    static_assert(isFloatLike<IT>::value, "IT must be one of the float types: float, double, or long double");

    // Exception class. Not used
    class out_of_range : public std::logic_error {
    public:
        explicit out_of_range(const std::string& message) : std::logic_error(message) {}
    };

    class divide_by_zero : public std::logic_error {
    public:
        explicit divide_by_zero(const std::string& message) : std::logic_error(message) {}
    };
    class domain_error : public std::logic_error {
    public:
        explicit domain_error(const std::string& message) : std::logic_error(message) {}
    };
};

// Unary and Binary arithmetic
// Arithmetic +,-,*,/ Binary and Unary
template <FloatLike IT>
interval<IT> operator+(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
interval<IT> operator+(const interval<IT>&);  // Unary
template <FloatLike IT>
interval<IT> operator-(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
interval<IT> operator-(const interval<IT>&);  // Unary
template <FloatLike IT>
interval<IT> operator*(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
interval<IT> operator/(const interval<IT>&, const interval<IT>&);

// Boolean Comparison Operators
template <FloatLike IT>
bool operator==(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool operator!=(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool operator>=(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool operator<=(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool operator>(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool operator<(const interval<IT>&, const interval<IT>&);

// Other functions
template <FloatLike IT>
interval<IT> abs(const interval<IT>&);

// Elementary functions
template <FloatLike IT>
interval<IT> sqr(const interval<IT>&);  // x^2
template <FloatLike IT>
interval<IT> sqrt(const interval<IT>&);  // sqrt(x)

template <FloatLike IT>
interval<IT> atan2(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
interval<IT> hypot(const interval<IT>&, const interval<IT>&);

// Arithmetic binary and monadic operators for mixed arithmetic
template <FloatLike IT, FloatLike _X>
interval<IT> operator+(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
interval<IT> operator+(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
interval<IT> operator-(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
interval<IT> operator-(const _X&, const interval<IT>&);
// template <FloatLike IT, FloatLike _X> interval<IT> operator*(const interval<IT>&, const _X&);
// template <FloatLike IT, FloatLike _X> interval<IT> operator*(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
interval<IT> operator/(const _X&, const interval<IT>&);

// Arithmetic operators for integral types mixed arithmetic
template <FloatLike IT, std::integral _X>
interval<IT> operator+(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
interval<IT> operator+(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
interval<IT> operator-(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
interval<IT> operator-(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
interval<IT> operator*(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
interval<IT> operator*(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
interval<IT> operator/(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
interval<IT> operator/(const _X&, const interval<IT>&);

// Boolean operators for mixed arithmetic
template <FloatLike IT, FloatLike _X>
bool operator==(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
bool operator==(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator==(const interval<IT>&, const interval<_X>&);
template <FloatLike IT, FloatLike _X>
bool operator!=(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
bool operator!=(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator!=(const interval<IT>&, const interval<_X>&);
template <FloatLike IT, FloatLike _X>
bool operator>=(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
bool operator>=(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator>=(const interval<IT>&, const interval<_X>&);
template <FloatLike IT, FloatLike _X>
bool operator<=(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
bool operator<=(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator<=(const interval<IT>&, const interval<_X>&);
template <FloatLike IT, FloatLike _X>
bool operator>(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
bool operator>(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator>(const interval<IT>&, const interval<_X>&);

// Boolean operators for integral types mixed arithmetic
template <FloatLike IT, std::integral _X>
bool operator==(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
bool operator==(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
bool operator!=(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
bool operator!=(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
bool operator>=(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
bool operator>=(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
bool operator<=(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
bool operator<=(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
bool operator>(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
bool operator>(const _X&, const interval<IT>&);
template <FloatLike IT, std::integral _X>
bool operator<(const interval<IT>&, const _X&);
template <FloatLike IT, std::integral _X>
bool operator<(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator<(const interval<IT>&, const _X&);
template <FloatLike IT, FloatLike _X>
bool operator<(const _X&, const interval<IT>&);
template <FloatLike IT, FloatLike _X>
bool operator<(const interval<IT>&, const interval<_X>&);

template <FloatLike IT>
bool subset(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool interior(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
bool precedes(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
int inclusion(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
std::pair<interval<IT>, interval<IT>> join(const interval<IT>&, const interval<IT>&);
template <FloatLike IT>
interval<IT> intersection(const interval<IT>&, const interval<IT>&);

template <FloatLike IT>
bool empty(const interval<IT>&);
template <FloatLike IT>
bool entire(const interval<IT>&);

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

// Get PI at the precision for IT (float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> pi_interval(const size_t precision = 0);
// Get e at the precision for IT.(float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> e_interval(const size_t precision = 0);
// Get ln2 at the precision for IT.(float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> ln2_interval(const size_t precision = 0);
// Get ln10 at the precision for IT.(float_precision also based on the precision)
template <typename IT>
constexpr interval<IT> ln10_interval(const size_t precision = 0);

// Inifinity declaration for the various types
template <FloatLike T>
T infinity_interval();

// Underflow declaration for the various types
template <FloatLike T>
T underflow_interval();

template <FloatLike IT>
interval<IT> floor(const interval<IT>&);  // floor(x)
template <FloatLike IT>
interval<IT> ceil(const interval<IT>&);  // ceil(x)
template <FloatLike IT>
interval<IT> sgn(const interval<IT>&);  // sqn(x)
template <FloatLike IT>
interval<IT> log(const interval<IT>&);  // log(x)
template <FloatLike IT>
interval<IT> log10(const interval<IT>&);  // log10(x)
template <FloatLike IT>
interval<IT> exp(const interval<IT>&);  // exp(x)
template <FloatLike IT>
interval<IT> pow(const interval<IT>&, const interval<IT>&);  // pow(x,y)==x^y

// Trigonometric functions
template <FloatLike IT>
interval<IT> sin(const interval<IT>&);  // sin(x)
template <FloatLike IT>
interval<IT> cos(const interval<IT>&);  // cos(x)
template <FloatLike IT>
interval<IT> tan(const interval<IT>&);  // tan(x)
template <FloatLike IT>
interval<IT> asin(const interval<IT>&);  // asin(x)
template <FloatLike IT>
interval<IT> acos(const interval<IT>&);  // acos(x)
template <FloatLike IT>
interval<IT> atan(const interval<IT>&);  // atan(x)

// Hyperbolic functions
template <FloatLike IT>
interval<IT> sinh(const interval<IT>&);  // sin(x)
template <FloatLike IT>
interval<IT> cosh(const interval<IT>&);  // cos(x)
template <FloatLike IT>
interval<IT> tanh(const interval<IT>&);  // tan(x)
template <FloatLike IT>
interval<IT> asinh(const interval<IT>&);  // asin(x)
template <FloatLike IT>
interval<IT> acosh(const interval<IT>&);  // acos(x)
template <FloatLike IT>
interval<IT> atanh(const interval<IT>&);  // atan(x)

template <FloatLike IT>
IT intervaldistance(const interval<IT>&, const interval<IT>&);
enum interval_type compute_interval_type(const enum interval_type, const enum interval_type);

template <class _Ty>
std::ostream& operator<<(std::ostream& strm, const interval<_Ty>& a);
template <class _Ty>
std::istream& operator>>(std::istream& strm, interval<_Ty>& c);
}  // namespace intprec

#endif