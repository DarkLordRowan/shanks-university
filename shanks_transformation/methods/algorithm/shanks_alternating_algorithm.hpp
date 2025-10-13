#pragma once

#include "../series_acceleration.hpp"
#include <vector>	// Include the vector library
#include <cmath>	//Include for fma, isfinite


/**
 * @brief Shanks transformation for alternating series class template.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all arithmetic operations and intermediate calculations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 *           Series terms should alternate in sign for optimal performance
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_transform_alternating : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should alternate in sign for optimal performance
	 */
	explicit shanks_transform_alternating() : series_acceleration<T, K>("shanks alternating") {};

	/**
	 * @brief Shanks transformation for alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	T operator()(
        const K n, 
    	const K order,
		const SeriesResult<T>& data
	) const override;

};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_transform_alternating<T, K>::operator()(
	const K n, 
    const K order,
	const SeriesResult<T>& data
) const {

    K required_size = order + n + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate theta_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;

	if (order == static_cast<K>(1)) [[unlikely]]
	{

		T result = static_cast<T>(0.0);

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// For alternating series: e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		result += fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			static_cast<T>(1) / (data.an.at(n) - data.an.at(n + static_cast<K>(1))),
			data.Sn.at(n)
		);

		if constexpr (isComplexLike<T>::value){
       		if (!isfinite(result.real()) || !isfinite(result.imag())){
        	    throw std::overflow_error("division by zero");
        	}
    	} else {
        	if(!isfinite(result)){
        	    throw std::overflow_error("division by zero");
        	}
    	}

		return result;
	}
	//n > order >= 1
	if (n <= order){
		throw std::invalid_argument("n is less or equal than order");
	}

	std::vector<T> T_n(
		n + order, 
		static_cast<T>(0.0)
	);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
		T_n[i] = fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			static_cast<T>(1) / (data.an.at(i) - data.an.at(i + static_cast<K>(1))),
			data.Sn.at(n)
		);
	}

	std::vector<T> T_n_plus_1(
		n + order, 
		static_cast<T>(0.0)
	);

	T a = static_cast<T>(0.0);
	T b = static_cast<T>(0.0);
	T c = static_cast<T>(0.0);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			// For theory, see: Brezinski et al. (2010), Section 3 - Multistep transformation
			// Higher order transformation for alternating series
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(a, c + b - a, -b * c),
				static_cast<T>(1) / (static_cast<T>(2) * a - b - c),
				a
			);
		}
		T_n = T_n_plus_1;
	}

	if constexpr (isComplexLike<T>::value){
        if (!isfinite(T_n[n].real()) || !isfinite(T_n[n].imag())){
            throw std::overflow_error("division by zero");
        }
    } else {
        if(!isfinite(T_n[n])){
            throw std::overflow_error("division by zero");
        }
    }

	return T_n[n];

}

#ifdef INC_FPRECISION

template <UnsignedIntLike K>
class shanks_transform_alternating<float_precision, K> : public series_acceleration<float_precision, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should alternate in sign for optimal performance
	 */
	explicit shanks_transform_alternating() : series_acceleration<float_precision, K>("shanks alternating") {};

	/**
	 * @brief Shanks transformation for alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	float_precision operator()(
        const K n, 
    	const K order,
		const SeriesResult<float_precision>& data
	) const override;

};

template <UnsignedIntLike K>
float_precision shanks_transform_alternating<float_precision, K>::operator()(
	const K n, 
    const K order,
	const SeriesResult<float_precision>& data
) const {

    K required_size = order + n + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate theta_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;

	const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());

	if (order == static_cast<K>(1)) [[unlikely]]
	{

		float_precision result = float_precision(0.0, precision);

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// For alternating series: e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		result += fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			float_precision(1) / (data.an.at(n) - data.an.at(n + static_cast<K>(1))),
			data.Sn.at(n)
		);

        if(!isfinite(result)){
            throw std::overflow_error("division by zero");
    	}

		return result;
	}
	//n > order >= 1
	if (n <= order){
		throw std::invalid_argument("n is less or equal than order");
	}

	std::vector<float_precision> T_n(
		n + order, 
		float_precision(0.0, precision)
	);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
		T_n[i] = fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			float_precision(1) / (data.an.at(i) - data.an.at(i + static_cast<K>(1))),
			data.Sn.at(n)
		);
	}

	std::vector<float_precision> T_n_plus_1(
		n + order, 
		float_precision(0.0, precision)
	);

	float_precision a = float_precision(0.0, precision);
	float_precision b = float_precision(0.0, precision);
	float_precision c = float_precision(0.0, precision);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			// For theory, see: Brezinski et al. (2010), Section 3 - Multistep transformation
			// Higher order transformation for alternating series
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(a, c + b - a, -b * c),
				float_precision(1) / (float_precision(2) * a - b - c),
				a
			);
		}
		T_n = T_n_plus_1;
	}

    if(!isfinite(T_n[n])){
        throw std::overflow_error("division by zero");
    }

	return T_n[n];

}

#ifdef INC_COMPLEXPRECISION

template <UnsignedIntLike K>
class shanks_transform_alternating<complex_precision<float_precision>, K> : public series_acceleration<complex_precision<float_precision>, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should alternate in sign for optimal performance
	 */
	explicit shanks_transform_alternating() : series_acceleration<complex_precision<float_precision>, K>("shanks alternating") {};

	/**
	 * @brief Shanks transformation for alternating series function.
	 * @authors Bolshakov M.P., Pashkov B.B.
	 * @param n The number of terms in the partial sum to use for transformation
	 *        Valid values: n >= order > 0, n > 0
	 *        Higher values use more terms but may provide better acceleration
	 * @param order The order of transformation (number of recursive applications)
	 *        Valid values: order >= 0 (order=0 returns untransformed partial sum)
	 *        Higher orders provide more aggressive acceleration but may be less stable
	 * @return The accelerated partial sum after Shanks transformation
	 * @throws std::overflow_error if division by zero or numerical instability occurs
	 */
	complex_precision<float_precision> operator()(
        const K n, 
    	const K order,
		const SeriesResult<complex_precision<float_precision>>& data
	) const override;

};

template <UnsignedIntLike K>
complex_precision<float_precision> shanks_transform_alternating<complex_precision<float_precision>, K>::operator()(
	const K n, 
    const K order,
	const SeriesResult<complex_precision<float_precision>>& data
) const {

    K required_size = order + n + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("Sn or an is smaller than required to calculate theta_{" + to_string(order) + "}^{" + to_string(n) + "}");
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;

	const size_t precision = std::max(
		std::max(data.Sn[0].real().precision(), data.Sn[0].imag().precision()), 
		std::max(data.an[0].real().precision(), data.an[0].imag().precision())
	);

	if (order == static_cast<K>(1)) [[unlikely]]
	{

		complex_precision<float_precision> result = complex_precision<float_precision> (
			float_precision(0.0, precision),
			float_precision(0.0, precision)
		);

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// For alternating series: e₁(Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		result += fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			complex_precision<float_precision>(1) / (data.an.at(n) - data.an.at(n + static_cast<K>(1))),
			data.Sn.at(n)
		);

        if(!isfinite(result)){
            throw std::overflow_error("division by zero");
    	}

		return result;
	}
	//n > order >= 1
	if (n <= order){
		throw std::invalid_argument("n is less or equal than order");
	}

	std::vector<complex_precision<float_precision>> T_n(
		n + order, 
		complex_precision<float_precision> (
			float_precision(0.0, precision),
			float_precision(0.0, precision)
		)
	);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches int->K
	{

		// For theory, see: Senhadji (2001), Section 3.2 - Alternating series case
		// e₁(Sᵢ) = Sᵢ + (aᵢaᵢ₊₁)/(aᵢ - aᵢ₊₁)
		T_n[i] = fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			complex_precision<float_precision>(1) / (data.an.at(i) - data.an.at(i + static_cast<K>(1))),
			data.Sn.at(n)
		);
	}

	std::vector<complex_precision<float_precision>> T_n_plus_1(
		n + order, 
		complex_precision<float_precision> (
			float_precision(0.0, precision),
			float_precision(0.0, precision)
		)
	);

	complex_precision<float_precision> a = complex_precision<float_precision> (
		float_precision(0.0, precision),
		float_precision(0.0, precision)
	);
	complex_precision<float_precision> b = complex_precision<float_precision> (
		float_precision(0.0, precision),
		float_precision(0.0, precision)
	);
	complex_precision<float_precision> c = complex_precision<float_precision> (
		float_precision(0.0, precision),
		float_precision(0.0, precision)
	);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			a = T_n[i];
			b = T_n[i - static_cast<K>(1)];
			c = T_n[i + static_cast<K>(1)];

			// For theory, see: Brezinski et al. (2010), Section 3 - Multistep transformation
			// Higher order transformation for alternating series
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(a, c + b - a, -b * c),
				complex_precision<float_precision>(1) / (complex_precision<float_precision>(2) * a - b - c),
				a
			);
		}
		T_n = T_n_plus_1;
	}

    if(!isfinite(T_n[n])){
        throw std::overflow_error("division by zero");
    }

	return T_n[n];

}

#endif
#endif