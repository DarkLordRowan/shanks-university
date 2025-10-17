/**
 * @file shanks_algorithm.hpp
 * @brief This file contains the definition of the Shanks transformation class
 *        and specialization of this transformation for alternating series.
 */

 // For theory, see:
 // Shanks, D. (1955). Non-linear transformations of divergent and slowly convergent sequences.
 // Journal of Mathematics and Physics, 34(1-4), 1-42.
 // Senhadji, M.N. (2001). On condition numbers of the Shanks transformation.
 // Journal of Computational and Applied Mathematics, 135(1), 41-61.
 // Brezinski, C., He, Y., Hu, X.B., Redivo-Zaglia, M., & Sun, J.Q. (2010).
 // Multistep epsilon-algorithm, Shanks' transformation, and Lotka-Volterra system by Hirota's method.
 // Mathematics of Computation.

#pragma once

#include "../series_acceleration.hpp"
#include <stdexcept>
#include <vector>	// Include the vector library
#include <cmath>	//Include for fma, isfinite

/**
 * @brief Shanks transformation for non-alternating series class template.
 *
 * @tparam T Floating-point type for series elements (must satisfy Accepted)
 *           Represents numerical precision (float, double, long double)
 *           Used for all arithmetic operations and intermediate calculations
 * @tparam K Unsigned integral type for indices and order (must satisfy std::unsigned_integral)
 *           Used for counting terms, indexing operations, and transformation order
 * @tparam series_templ Type of series object to accelerate. Must provide:
 *           - T operator()(K n) const: returns the n-th series term a_n
 *           - T S_n(K n) const: returns the n-th partial sum s_n = a_0 + ... + a_n
 */
template <AcceptedLike T, UnsignedIntLike K>
class shanks_algorithm final : public series_acceleration<T, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should be non-alternating for optimal performance
	 */
	explicit shanks_algorithm() : series_acceleration<T, K>("shanks original") {};

	/**
	 * @brief Shanks transformation for non-alternating series function.
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
        const series_result<T>& data
	) const;
};

template <AcceptedLike T, UnsignedIntLike K>
T shanks_algorithm<T, K>::operator()(
	const K n, 
    const K order, 
    const series_result<T>& data
) const{

    const K required_size = order + n + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for shanks_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;


	if (order == static_cast<K>(1)) [[unlikely]]
	{
		T tmp = static_cast<T>(0.0);

		tmp -= data.an.at(n + static_cast<K>(1)) * data.an.at(n);

		// For theory, see: Shanks (1955), Eq. (6) - Aitken's Δ² process
		// e₁(Sₙ) = (SₙSₙ₊₂ - Sₙ₊₁²)/(Sₙ₊₂ - 2Sₙ₊₁ + Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		const T result = fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			(data.an.at(n) + data.an.at(n + static_cast<K>(1))) / 
			(fma(data.an.at(n), data.an.at(n), tmp) - fma(data.an.at(n + static_cast<K>(1)), data.an.at(n + static_cast<K>(1)), tmp)),
			data.Sn.at(n)
		);
		//n > order >= 1

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
	if (n < order){
		throw std::invalid_argument("n is less than order");
	}

	std::vector<T> T_n(
		n + order, 
		static_cast<T>(0.0)
	);

	T tmp = static_cast<T>(0.0);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches  int -> K
	{
		tmp = -data.an.at(i + static_cast<K>(1)) * data.an.at(i + static_cast<K>(1));

		// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
		// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
		T_n[i] = fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			(data.an.at(i) + data.an.at(i + static_cast<K>(1))) / 
			(fma(data.an.at(i), data.an.at(i), tmp) - fma(data.an.at(i + static_cast<K>(1)), data.an.at(i + static_cast<K>(1)), tmp)),
			data.Sn.at(i)
		);
	}

	std::vector<T> T_n_plus_1(
		n + order, 
		static_cast<T>(0.0)
	);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(T_n[i], T_n[i + static_cast<K>(1)] + T_n[i - static_cast<K>(1)] - T_n[i], -T_n[i - static_cast<K>(1)] * T_n[i + static_cast<K>(1)]),
				static_cast<T>(1) / (fma(static_cast<T>(2), T_n[i], -T_n[i - static_cast<K>(1)] - T_n[i + static_cast<K>(1)])),
				T_n[i]
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
class shanks_algorithm<float_precision, K> final : public series_acceleration<float_precision, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should be non-alternating for optimal performance
	 */
	explicit shanks_algorithm() : series_acceleration<float_precision, K>("shanks original") {};

	/**
	 * @brief Shanks transformation for non-alternating series function.
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
        const series_result<float_precision>& data
	) const;
};

template <UnsignedIntLike K>
float_precision shanks_algorithm<float_precision, K>::operator()(
	const K n, 
    const K order, 
    const series_result<float_precision>& data
) const{

    const K required_size = n + order + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for shanks_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

    using std::isfinite;
	using std::fma;

	const size_t precision = std::max(data.Sn[0].precision(), data.an[0].precision());


	if (order == static_cast<K>(1)) [[unlikely]]
	{
		float_precision tmp = float_precision(0.0, precision);

		tmp -= data.an.at(n + static_cast<K>(1)) * data.an.at(n);

		// For theory, see: Shanks (1955), Eq. (6) - Aitken's Δ² process
		// e₁(Sₙ) = (SₙSₙ₊₂ - Sₙ₊₁²)/(Sₙ₊₂ - 2Sₙ₊₁ + Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		const float_precision result = fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			(data.an.at(n) + data.an.at(n + static_cast<K>(1))) / 
			(fma(data.an.at(n), data.an.at(n), tmp) - fma(data.an.at(n + static_cast<K>(1)), data.an.at(n + static_cast<K>(1)), tmp)),
			data.Sn.at(n)
		);
		//n > order >= 1

        if(!isfinite(result)){
            throw std::overflow_error("division by zero");
    	}

		return result;
	}
	//n > order >= 1

	if (n < order){
		throw std::invalid_argument("n is less than order");
	}

	std::vector<float_precision> T_n(
		n + order, 
		float_precision(0.0, precision)
	);

	float_precision tmp = float_precision(0.0, precision);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches  int -> K
	{
		tmp = -data.an.at(i + static_cast<K>(1)) * data.an.at(i + static_cast<K>(1));

		// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
		// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
		T_n[i] = fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			(data.an.at(i) + data.an.at(i + static_cast<K>(1))) / 
			(fma(data.an.at(i), data.an.at(i), tmp) - fma(data.an.at(i + static_cast<K>(1)), data.an.at(i + static_cast<K>(1)), tmp)),
			data.Sn.at(i)
		);
	}

	std::vector<float_precision> T_n_plus_1(
		n + order, 
		float_precision(0.0, precision)
	);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(T_n[i], T_n[i + static_cast<K>(1)] + T_n[i - static_cast<K>(1)] - T_n[i], -T_n[i - static_cast<K>(1)] * T_n[i + static_cast<K>(1)]),
				static_cast<float_precision>(1) / (fma(static_cast<float_precision>(2), T_n[i], -T_n[i - static_cast<K>(1)] - T_n[i + static_cast<K>(1)])),
				T_n[i]
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
class shanks_algorithm<complex_precision<float_precision>, K> final : public series_acceleration<complex_precision<float_precision>, K>
{
public:

	/**
	 * @brief Parameterized constructor to initialize the Shanks transformation for non-alternating series.
	 * @authors Bolshakov M.P.
	 * @param series The series class object to be accelerated
	 *        Must be a valid object implementing the required series interface
	 *        Series terms should be non-alternating for optimal performance
	 */
	explicit shanks_algorithm() : series_acceleration<complex_precision<float_precision>, K>("shanks original") {};

	/**
	 * @brief Shanks transformation for non-alternating series function.
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
        const series_result<complex_precision<float_precision>>& data
	) const;
};

template <UnsignedIntLike K>
complex_precision<float_precision> shanks_algorithm<complex_precision<float_precision>, K>::operator()(
	const K n, 
    const K order, 
    const series_result<complex_precision<float_precision>>& data
) const{

    const K required_size = n + order + static_cast<K>(1);

    if (data.Sn.size() < required_size || data.an.size() < required_size){
        throw std::out_of_range("The Sn or an smaller then required for shanks_{" + to_string(order) + "}^{" + to_string(n) + "}\n" +
        "the size of Sn and an must be at least " + to_string(required_size));
	}

    if (order == static_cast<K>(0)) {
        return data.Sn.at(n);
    }

	if (n < order){
		throw std::invalid_argument("n is less than order");
	}

    using std::isfinite;
	using std::fma;

	const size_t precision = std::max(
		std::max(data.Sn[0].real().precision(),data.Sn[0].imag().precision()), 
		std::max(data.an[0].real().precision(),data.an[0].imag().precision())
	);


	if (order == static_cast<K>(1)) [[unlikely]]
	{
		complex_precision<float_precision> tmp = complex_precision<float_precision>(
			float_precision(0.0, precision),
			float_precision(0.0, precision)
		);

		tmp -= data.an.at(n + static_cast<K>(1)) * data.an.at(n);

		// For theory, see: Shanks (1955), Eq. (6) - Aitken's Δ² process
		// e₁(Sₙ) = (SₙSₙ₊₂ - Sₙ₊₁²)/(Sₙ₊₂ - 2Sₙ₊₁ + Sₙ) = Sₙ + (aₙaₙ₊₁)/(aₙ - aₙ₊₁)
		const complex_precision<float_precision> result = fma(
			data.an.at(n) * data.an.at(n + static_cast<K>(1)),
			(data.an.at(n) + data.an.at(n + static_cast<K>(1))) / 
			(fma(data.an.at(n), data.an.at(n), tmp) - fma(data.an.at(n + static_cast<K>(1)), data.an.at(n + static_cast<K>(1)), tmp)),
			data.Sn.at(n)
		);
		//n > order >= 1

        if (!isfinite(result.real()) || !isfinite(result.imag())){
            throw std::overflow_error("division by zero");
        }

		return result;
	}
	//n > order >= 1

	std::vector<complex_precision<float_precision>> T_n(
		n + order, 
		complex_precision<float_precision>(
			float_precision(0.0, precision),
			float_precision(0.0, precision)
		)
	);

	complex_precision<float_precision> tmp = complex_precision<float_precision>(
		float_precision(0.0, precision),
		float_precision(0.0, precision)
	);

	for (K i = n - order + static_cast<K>(1); i <= n + order - static_cast<K>(1); ++i) // if we got to this branch then we know that n >= order - see previous branches  int -> K
	{
		tmp = -data.an.at(i + static_cast<K>(1)) * data.an.at(i + static_cast<K>(1));

		// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
		// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
		T_n[i] = fma(
			data.an.at(i) * data.an.at(i + static_cast<K>(1)),
			(data.an.at(i) + data.an.at(i + static_cast<K>(1))) / 
			(fma(data.an.at(i), data.an.at(i), tmp) - fma(data.an.at(i + static_cast<K>(1)), data.an.at(i + static_cast<K>(1)), tmp)),
			data.Sn.at(i)
		);
	}

	std::vector<complex_precision<float_precision>> T_n_plus_1(
		n + order,
		complex_precision<float_precision>(
			float_precision(0.0, precision),
			float_precision(0.0, precision)
		)
	);

	for (K j = static_cast<K>(2); j <= order; ++j) {
		for (K i = n - order + j; i <= n + order - j; ++i) {

			// For theory, see: Shanks (1955), Eq. (12) - Higher order transformation
			// eₖ(Sₙ) = eₖ₋₁(Sₙ) + [eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ)] / [1 - (eₖ₋₁(Sₙ₊₁) - eₖ₋₁(Sₙ))/(eₖ₋₁(Sₙ₊₂) - eₖ₋₁(Sₙ₊₁))]
			T_n_plus_1[i] = fma(
				fma(T_n[i], T_n[i + static_cast<K>(1)] + T_n[i - static_cast<K>(1)] - T_n[i], -T_n[i - static_cast<K>(1)] * T_n[i + static_cast<K>(1)]),
				static_cast<complex_precision<float_precision>>(1) / (fma(static_cast<complex_precision<float_precision>>(2), T_n[i], -T_n[i - static_cast<K>(1)] - T_n[i + static_cast<K>(1)])),
				T_n[i]
			);
		}
		T_n = T_n_plus_1;
	}

    if (!isfinite(T_n[n].real()) || !isfinite(T_n[n].imag())){
        throw std::overflow_error("division by zero");
    }

	return T_n[n];
	
}

#endif
#endif