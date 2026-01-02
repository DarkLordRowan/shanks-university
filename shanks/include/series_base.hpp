#ifndef SERIES_BASE_HPP
#define SERIES_BASE_HPP

#pragma once

#include <stdexcept>
#include <string>
#include <memory>
#include <cassert>
#include <iostream>

#include "custom_concepts.hpp"
#include "series_iterators/series_base_iterator.hpp"
#include "series_iterators/series_iterators.hpp"
#include "utils.hpp"


/**
 * @file series_base.hpp
 * @brief This file contains the base class for all series and helper functions for iterator creation.
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 */

namespace shanks { namespace series{

/**
 * @brief Factory function to create a series iterator based on its unique ID.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (std::unsigned_integral).
 * @param id The ID of the series iterator to instantiate.
 * @return std::unique_ptr<shanks::iters::series_base_iterator<T, K>> A pointer to the newly created iterator.
 * @throws std::domain_error if the requested series ID is not implemented.
 */
template <AcceptedLike T, std::unsigned_integral K>
inline std::unique_ptr<shanks::iters::series_base_iterator<T, K>> create_iterator_by_id(shanks::iters::series_iterator_id_t id) {

	switch (id) {
	case shanks::iters::series_iterator_id_t::exp_iterator_id: return std::make_unique<shanks::iters::exp_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::cos_iterator_id: return std::make_unique<shanks::iters::cos_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sin_iterator_id: return std::make_unique<shanks::iters::sin_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::cosh_iterator_id: return std::make_unique<shanks::iters::cosh_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sinh_iterator_id: return std::make_unique<shanks::iters::sinh_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::bin_iterator_id: return std::make_unique<shanks::iters::bin_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::four_arctan_iterator_id: return std::make_unique<shanks::iters::four_arctan_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln1mx_iterator_id: return std::make_unique<shanks::iters::ln1mx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::mean_sinh_sin_iterator_id: return std::make_unique<shanks::iters::mean_sinh_sin_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::exp_squared_erf_iterator_id: return std::make_unique<shanks::iters::exp_squared_erf_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::half_asin_two_x_iterator_id: return std::make_unique<shanks::iters::half_asin_two_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::inverse_1mx_iterator_id: return std::make_unique<shanks::iters::inverse_1mx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_1mx_squared_iterator_id: return std::make_unique<shanks::iters::x_1mx_squared_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::erf_iterator_id: return std::make_unique<shanks::iters::erf_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::m_fact_1mx_mp1_inverse_iterator_id: return std::make_unique<shanks::iters::m_fact_1mx_mp1_inverse_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::inverse_sqrt_1m4x_iterator_id: return std::make_unique<shanks::iters::inverse_sqrt_1m4x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_twelfth_3x2_pi2_iterator_id: return std::make_unique<shanks::iters::one_twelfth_3x2_pi2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_twelfth_x2_pi2_iterator_id: return std::make_unique<shanks::iters::one_twelfth_x2_pi2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln2_iterator_id: return std::make_unique<shanks::iters::ln2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_iterator_id: return std::make_unique<shanks::iters::one_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::minus_one_quarter_iterator_id: return std::make_unique<shanks::iters::minus_one_quarter_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_3_iterator_id: return std::make_unique<shanks::iters::pi_3_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_4_iterator_id: return std::make_unique<shanks::iters::pi_4_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_squared_6_minus_one_iterator_id: return std::make_unique<shanks::iters::pi_squared_6_minus_one_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::three_minus_pi_iterator_id: return std::make_unique<shanks::iters::three_minus_pi_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_twelfth_iterator_id: return std::make_unique<shanks::iters::one_twelfth_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::eighth_pi_m_one_third_iterator_id: return std::make_unique<shanks::iters::eighth_pi_m_one_third_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_third_pi_squared_m_nine_iterator_id: return std::make_unique<shanks::iters::one_third_pi_squared_m_nine_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::four_ln2_m_3_iterator_id: return std::make_unique<shanks::iters::four_ln2_m_3_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::exp_m_cos_x_sinsin_x_iterator_id: return std::make_unique<shanks::iters::exp_m_cos_x_sinsin_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_four_minus_ln2_halfed_iterator_id: return std::make_unique<shanks::iters::pi_four_minus_ln2_halfed_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::five_pi_twelve_iterator_id: return std::make_unique<shanks::iters::five_pi_twelve_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_two_iterator_id: return std::make_unique<shanks::iters::x_two_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_six_min_half_iterator_id: return std::make_unique<shanks::iters::pi_six_min_half_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_two_throught_squares_iterator_id: return std::make_unique<shanks::iters::x_two_throught_squares_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::minus_one_ned_in_n_iterator_id: return std::make_unique<shanks::iters::minus_one_ned_in_n_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::minus_one_n_fact_n_in_n_iterator_id: return std::make_unique<shanks::iters::minus_one_n_fact_n_in_n_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln_x_plus_one_x_minus_one_halfed_iterator_id: return std::make_unique<shanks::iters::ln_x_plus_one_x_minus_one_halfed_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::two_arcsin_square_x_halfed_iterator_id: return std::make_unique<shanks::iters::two_arcsin_square_x_halfed_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_squared_twelve_iterator_id: return std::make_unique<shanks::iters::pi_squared_twelve_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_cubed_32_iterator_id: return std::make_unique<shanks::iters::pi_cubed_32_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator_id: return std::make_unique<shanks::iters::minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::two_ln2_iterator_id: return std::make_unique<shanks::iters::two_ln2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator_id: return std::make_unique<shanks::iters::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_minus_x_2_iterator_id: return std::make_unique<shanks::iters::pi_minus_x_2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::half_multi_ln_1div2multi1minuscosx_iterator_id: return std::make_unique<shanks::iters::half_multi_ln_1div2multi1minuscosx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::half_minus_sinx_multi_pi_4_iterator_id: return std::make_unique<shanks::iters::half_minus_sinx_multi_pi_4_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_iterator_id: return std::make_unique<shanks::iters::ln_1plussqrt1plusxsquare_minus_ln_2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln_cosx_iterator_id: return std::make_unique<shanks::iters::ln_cosx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln_sinx_minus_ln_x_iterator_id: return std::make_unique<shanks::iters::ln_sinx_minus_ln_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_8_cosx_square_minus_1_div_3_cosx_iterator_id: return std::make_unique<shanks::iters::pi_8_cosx_square_minus_1_div_3_cosx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sqrt_oneminussqrtoneminusx_div_x_iterator_id: return std::make_unique<shanks::iters::sqrt_oneminussqrtoneminusx_div_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_minus_sqrt_1minus4x_div_2x_iterator_id: return std::make_unique<shanks::iters::one_minus_sqrt_1minus4x_div_2x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arcsin_x_minus_x_iterator_id: return std::make_unique<shanks::iters::arcsin_x_minus_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator_id: return std::make_unique<shanks::iters::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::abs_sin_x_minus_2_div_pi_iterator_id: return std::make_unique<shanks::iters::abs_sin_x_minus_2_div_pi_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator_id: return std::make_unique<shanks::iters::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::minus_3_div_4_or_x_minus_3_div_4_iterator_id: return std::make_unique<shanks::iters::minus_3_div_4_or_x_minus_3_div_4_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ten_minus_x_iterator_id: return std::make_unique<shanks::iters::ten_minus_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_iterator_id: return std::make_unique<shanks::iters::x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::minus_x_minus_pi_4_or_minus_pi_4_iterator_id: return std::make_unique<shanks::iters::minus_x_minus_pi_4_or_minus_pi_4_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_div_two_minus_x_multi_three_plus_x_iterator_id: return std::make_unique<shanks::iters::one_div_two_minus_x_multi_three_plus_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::si_x_iterator_id: return std::make_unique<shanks::iters::si_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ci_x_iterator_id: return std::make_unique<shanks::iters::ci_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::riemann_zeta_func_iterator_id: return std::make_unique<shanks::iters::riemann_zeta_func_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator_id: return std::make_unique<shanks::iters::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_iterator_id: return std::make_unique<shanks::iters::xsquareplus3_div_xsquareplus2multix_minus_1_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arcsin_x_iterator_id: return std::make_unique<shanks::iters::arcsin_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arctg_x_iterator_id: return std::make_unique<shanks::iters::arctan_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::k_x_iterator_id: return std::make_unique<shanks::iters::k_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::e_x_iterator_id: return std::make_unique<shanks::iters::e_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sqrt_1plusx_iterator_id: return std::make_unique<shanks::iters::sqrt_1plusx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::lambert_W_func_iterator_id: return std::make_unique<shanks::iters::lambert_W_func_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::incomplete_Gamma_func_iterator_id: return std::make_unique<shanks::iters::incomplete_Gamma_func_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::series_with_ln_number1_iterator_id: return std::make_unique<shanks::iters::series_with_ln_number1_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::series_with_ln_number2_iterator_id: return std::make_unique<shanks::iters::series_with_ln_number2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::pi_iterator_id: return std::make_unique<shanks::iters::pi_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_min_sqrt_x_iterator_id: return std::make_unique<shanks::iters::x_min_sqrt_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arctg_x2_iterator_id: return std::make_unique<shanks::iters::arctan_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln1px4_iterator_id: return std::make_unique<shanks::iters::ln1px4_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sin_x2_iterator_id: return std::make_unique<shanks::iters::sin_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arctg_x3_iterator_id: return std::make_unique<shanks::iters::arctan_x3_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arcsin_x2_iterator_id: return std::make_unique<shanks::iters::arcsin_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln1_m_x2_iterator_id: return std::make_unique<shanks::iters::ln1_m_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arctanh_x_iterator_id: return std::make_unique<shanks::iters::arctanh_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arcsinh_x_iterator_id: return std::make_unique<shanks::iters::arcsinh_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::cos_x2_iterator_id: return std::make_unique<shanks::iters::cos_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sinh_x2_iterator_id: return std::make_unique<shanks::iters::sinh_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::arctanh_x2_iterator_id: return std::make_unique<shanks::iters::arctanh_x2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::cos3xmin1_div_xsquare_iterator_id: return std::make_unique<shanks::iters::cos3xmin1_div_xsquare_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::two_degree_x_iterator_id: return std::make_unique<shanks::iters::two_degree_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::sqrt_1plusx_min_1_min_x_div_2_iterator_id: return std::make_unique<shanks::iters::sqrt_1plusx_min_1_min_x_div_2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln13_min_ln7_div_7_iterator_id: return std::make_unique<shanks::iters::ln13_min_ln7_div_7_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::one_div_sqrt2_sin_xdivsqrt2_iterator_id: return std::make_unique<shanks::iters::one_div_sqrt2_sin_xdivsqrt2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln_1plusx_div_1plusx2_iterator_id: return std::make_unique<shanks::iters::ln_1plusx_div_1plusx2_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::cos_sqrt_x_iterator_id: return std::make_unique<shanks::iters::cos_sqrt_x_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::ln_1_plus_x3_iterator_id: return std::make_unique<shanks::iters::ln_1_plus_x3_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_div_1minx_iterator_id: return std::make_unique<shanks::iters::x_div_1minx_iterator<T, K>>();
	case shanks::iters::series_iterator_id_t::x_div_1minx2_iterator_id: return std::make_unique<shanks::iters::x_div_1minx2_iterator<T, K>>();

	default:
		throw std::domain_error("Series not implemented");
	}
}


 /**
 * @brief Abstract class for mathematical series.
 *
 * Defines the common structure and operations for all series in the library,
 * managing their names, convergence conditions, and internal iterators.
 *
 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
 * @tparam T Floating-point type for series elements (AcceptedLike).
 * @tparam K Unsigned integral type for indexing (UnsignedIntLike).
 */
template <AcceptedLike T, UnsignedIntLike K>
class series_base
{
public:

	/**
	* @brief Parameterized constructor to initialize the series object.
	*
	* @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	* @param name Descriptive name of the series.
	* @param condition Mathematical condition for series convergence.
	* @param iterator_id ID of the iterator to be used for term generation.
	*/
	series_base(
		std::string name = "unknown",
		std::string condition = "unknown",
		shanks::iters::series_iterator_id_t iterator_id = shanks::iters::series_iterator_id_t::null_iterator_id
	) : series_name(name), condition(condition) {
		// Ensure that a valid iterator ID is provided
		assert((iterator_id != shanks::iters::series_iterator_id_t::null_iterator_id));
		series_iterator = create_iterator_by_id<T,K>(iterator_id);
		this->iterator_id = iterator_id;
	}

	/**
	* @brief Retrieves the current evaluation point x.
	* @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	* @return const T The value of x.
	*/
	[[nodiscard]] constexpr const T get_x() const { return series_iterator->x; }

	/**
	* @brief Retrieves the current cumulative sum of the series.
	* @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	* @return const T The current sum.
	*/
	[[nodiscard]] constexpr const T get_sum() const { return series_iterator->sum(); }

	/**
	* @brief Retrieves the name of the series.
	* @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	* @return const std::string The series name.
	*/
	[[nodiscard]] constexpr const std::string get_name() const { return series_name; }

	/**
	 * @brief Generates vectors of terms and partial sums for the series.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x Evaluation point.
	 * @param vecSize Number of terms to generate.
	 * @param addTParameter Additional floating-point parameter for specialized iterators.
	 * @param addKParameter Additional integral parameter for specialized iterators.
	 * @return series_result<T> Structure containing terms (an) and partial sums (Sn).
	 */
	series_result<T> generate_series(
		const T& x ,
		const K vecSize,
		const T& addTParameter = utils::cast<T>(1.0),
		const K addKParameter = static_cast<K>(1)
	);

	/**
	 * @brief Initializes the internal iterator with specific evaluation parameters.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @param x Evaluation point.
	 * @param addTParameter Additional floating-point parameter.
	 * @param addKParameter Additional integral parameter.
	 */
	void init_iterator(
		const T& x,
		const T& addTParameter = utils::cast<T>(1.0),
		const K addKParameter = static_cast<K>(1)
	);

	/**
	 * @brief Computes the next term in the series and updates the sum.
	 *
	 * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
	 * @return std::pair<T, T> A pair containing the next term and the updated total sum.
	 * @throws std::domain_error if the iterator has not been initialized.
	 */
	std::pair<T, T> next(){
		if (iterator_initialized){
			// Log current step for debugging purposes
			std::cout << "\n" << series_iterator->n << "\n";
			const T an = series_iterator->next(); sum+=an;
			return std::pair<T,T>(an, sum);
		}
		throw std::domain_error("iterator not initialized");
	}


protected:

	/**
    * @brief Helper to throw a formatted domain error for divergent series.
    * @authors Naumov A.U., Lykov D.S., Kreynin R.G.
    * @throws std::domain_error with detailed context about the divergence point and conditions.
    */
	void throw_domain_error() const {
		throw std::domain_error(series_name + " series diverges at x = " + utils::to_string(series_iterator->x) + " (" + condition + ")");
	}

	/// Unique pointer to the underlying iterator implementation.
	std::unique_ptr<shanks::iters::series_base_iterator<T,K>> series_iterator;

	/// Internal storage for the series name.
	std::string series_name = "series base";

	/// Description of convergence conditions.
	std::string condition = "no conditions";

	/// Identifier for the current iterator type.
	shanks::iters::series_iterator_id_t iterator_id = shanks::iters::series_iterator_id_t::null_iterator_id;

	/// State flag tracking iterator initialization.
	bool iterator_initialized = false;

	/// The current total sum of the series.
	T sum = utils::cast<T>(0.0);
};

/**
 * @brief Initialization logic for the series iterator.
 */
template <AcceptedLike T, UnsignedIntLike K>
void series_base<T, K>::init_iterator(
	const T& x,
	const T& addTParameter,
	const K addKParameter
){
	series_iterator->reset();
	sum = utils::cast<T>(0.0);
	#ifdef SET_PRECISION_SET
	if constexpr (!is_standard_types<T>::value) utils::set_precision(utils::get_precision<T>(x), series_iterator->x, sum);
	#endif

	series_iterator->x = x;

	// Validate point before starting calculation
	if(series_iterator->check_validity()) throw_domain_error();
	iterator_initialized = true;


	// Apply specialized parameters based on the iterator implementation
	switch(iterator_id){
		case shanks::iters::series_iterator_id_t::bin_iterator_id:{
			shanks::iters::bin_iterator<T,K>* ptr = static_cast<shanks::iters::bin_iterator<T,K>*>(series_iterator.get());
			#ifdef SET_PRECISION_SET
			if constexpr (!is_standard_types<T>::value) utils::set_precision(utils::get_precision<T>(x), series_iterator->x);
			#endif
			ptr->alpha = addTParameter;
		}
		case shanks::iters::series_iterator_id_t::incomplete_Gamma_func_iterator_id:{
			shanks::iters::incomplete_Gamma_func_iterator<T,K>* ptr = static_cast<shanks::iters::incomplete_Gamma_func_iterator<T,K>*>(series_iterator.get());
			#ifdef SET_PRECISION_SET
			if constexpr (!is_standard_types<T>::value) utils::set_precision(utils::get_precision<T>(x), series_iterator->x);
			#endif
			ptr->alpha = addTParameter;
		}
		case shanks::iters::series_iterator_id_t::m_fact_1mx_mp1_inverse_iterator_id:{
			shanks::iters::m_fact_1mx_mp1_inverse_iterator<T,K>* ptr = static_cast<shanks::iters::m_fact_1mx_mp1_inverse_iterator<T,K>*>(series_iterator.get());
			ptr->m = addKParameter;
		}
	}
}

/**
 * @brief Generation logic for series data vectors.
 */
template <AcceptedLike T, UnsignedIntLike K>
series_result<T> series_base<T, K>::generate_series(
	const T& x,
	const K vecSize,
	const T& addTParameter,
	const K addKParameter
){

	init_iterator(x, addTParameter, addKParameter);
	std::vector<T> vecAn(vecSize, utils::cast<T>(0.0));
	std::vector<T> vecSn(vecSize, utils::cast<T>(0.0));

	#ifdef SET_PRECISION_SET
	if constexpr (is_precisable<T>::value){
		utils::set_vec_precision<T>(vecAn, utils::get_precision(x));
		utils::set_vec_precision<T>(vecSn, utils::get_precision(x));
	}
	#endif

	// Populate the vectors term-by-term
	for (K j = 0; j < vecSize; ++j){
		try{
			vecAn[j] += series_iterator->next();
		} catch (...) {
			// Fallback strategy: repeat last term if generation fails
			vecAn[j] += (j == 0 ? utils::cast<T>(0.0) : vecAn[j-1]);
		}
		sum += vecAn[j];
		vecSn[j] += vecAn[j] + (j == 0 ? utils::cast<T>(0.0) : vecSn[j-1]);
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn};
}

} //namespace shanks::series
} //namespace shanks

#endif
