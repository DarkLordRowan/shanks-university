#ifndef UTILS_HPP
#define UTILS_HPP

#include "custom_concepts.hpp"

template<AcceptedLike T>
struct series_result{
	std::vector<T> Sn;
	std::vector<T> an;
};

struct utils {

	template<AcceptedLike T, UnsignedIntLike K>
	constexpr static const T phi(K n);

	template<UnsignedIntLike K>
	constexpr static const K fact(const K n);

	template<UnsignedIntLike K>
	constexpr static const K double_fact(const K n);

	template<UnsignedIntLike K>
	constexpr static const K binomial_coefficient(const K n, const K k);

	template<AcceptedLike T, UnsignedIntLike K>
	constexpr static const T minus_one_raised_to_power_n(const K j);

	template<typename Arg, typename... Args>
	requires (is_precisable<Args>::value && ...)
	static void set_precision(const size_t precision, Arg& precisable_arg, Args& ...precisable_args);
	static void set_precision(const size_t precision) {}

	template<AcceptedLike T, UnsignedIntLike K>
	static void set_vec_precision(std::vector<T>& vec, size_t precision);

	template<AcceptedLike T>
	static size_t get_precision(const T& x){
		if constexpr (is_standart_types<T>::value){
			return static_cast<size_t>(0);
		}
		#ifdef INC_FPRECISION
		else if constexpr (std::is_same<T, float_precision>::value){
			return x.precision();
		}
		#ifdef INC_COMPLEXPRECISION
		else if constexpr (std::is_same<T, complex_precision<float_precision>>::value){
			return std::max(x.real().precision(), x.imag().precision());
		}
		#endif
		#endif
		else return static_cast<size_t>(0);
		
	}

};

template <AcceptedLike T, UnsignedIntLike K>
constexpr const T utils::phi(K n)
{
	K result = n;
	for (K i = 2; i * i <= n; ++i)
		if (n % i == 0) {
			while (n % i == 0)
				n /= i;
			result -= result / i;
		}

	result -= n > 1 ? result / n : 0;
	return static_cast<T>(result);
}

template<UnsignedIntLike K>
constexpr const K utils::fact(const K n) {
	K fact = static_cast<K>(1);
	for(K j = static_cast<K>(2); j <= n; ++j){
		fact *= j;
	}
	return fact;
}

template<UnsignedIntLike K>
constexpr const K utils::double_fact(const K n) {

	K double_fact = static_cast<K>(1);

	for (K j = n & static_cast<K>(1) + static_cast<K>(2); j <= n; j+=2){
		double_fact *= j;
	}

	return double_fact;
}

template<UnsignedIntLike K>
constexpr const K utils::binomial_coefficient(const K n, const K k) {

	if(n<k)
		throw std::invalid_argument("n>k");

	if (n==k || k==static_cast<K>(0))
		return static_cast<K>(1);

	K productNminusK = n; //n(n-1)(n-2)...(n-k)
	K factK = k;

	for(K j = static_cast<K>(1); j < k; j++){
		factK*=j;
		productNminusK*=(n-j);
	}

	return productNminusK/factK;

}

template<AcceptedLike T, UnsignedIntLike K>
constexpr const T utils::minus_one_raised_to_power_n(const K j){
    return static_cast<T>(j & 1 ? -1 : 1);
}

template<typename Arg, typename... Args>
requires (is_precisable<Args>::value && ...)
void utils::set_precision(const size_t precision, Arg& precisable_arg, Args& ...precisable_args){
	if constexpr (std::is_same<Arg, float_precision>::value){
		precisable_arg.precision(precision);
	}
	else if constexpr (std::is_same<Arg, complex_precision<float_precision>>::value){
		precisable_arg.ref_real()->precision(precision); precisable_arg.ref_imag()->precision(precision);
	}
	utils::set_precision(precision, precisable_args...);

}

template<AcceptedLike T, UnsignedIntLike K>
void utils::set_vec_precision(std::vector<T>& vec, size_t precision){
	for(size_t j = 0; j < vec.size(); ++j)
		utils::set_precision(precision, vec[j]);
}

#ifdef INC_COMPLEXPRECISION
//CUSTOM DEFINITIONS FOR ISFINITE, FMA, JUST FOR EVERYTHING TO WORK
template<class _Ty> inline complex_precision<_Ty> hypot(const complex_precision<_Ty>& x, const complex_precision<_Ty>& y){ return sqrt(x * x + y * y ); }
template<class _Ty> inline bool isfinite(const complex_precision<_Ty>& x){ 
   using std::isfinite;

   return isfinite(x.real()) && isfinite(x.imag()); 
}
template<class _Ty> inline complex_precision<_Ty> fma(complex_precision<_Ty> x, complex_precision<_Ty> y, complex_precision<_Ty> z){
   _Ty real_part = x.real() * y.real() - x.imag() * y.imag() + z.real();
   _Ty imag_part = x.real() * y.imag() + y.real() * x.imag() + z.imag() ;
   complex_precision<_Ty> res(real_part, imag_part);
   return res;
}
template<class _Ty> inline std::string to_string(const complex_precision<_Ty>& x){ 
   using std::to_string;
   return to_string(x.real()) + " + " +to_string(x.imag()) + " * i";
}
#endif

#endif