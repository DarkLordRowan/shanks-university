#ifndef SERIES_BASE_HPP
#define SERIES_BASE_HPP

#include "series_iterators/incomplete_Gamma_func_iterator.hpp"
#include "series_iterators/m_fact_1mx_mp1_inverse_iterator.hpp"
#pragma once

#include <stdexcept>
#include <string>
#include <memory>

#include "custom_concepts.hpp"
#include "series_iterators/series_base_iterator.hpp"
#include "series_iterators/series_iterators.hpp"
#include "utils.hpp"

/**
 * @brief Helper function to get series by ID
 */
template <AcceptedLike T, std::unsigned_integral K>
inline std::unique_ptr<series_base_iterator<T, K>> create_iterator_by_id(series_iterator_id_t id) {

	switch (id) {
	case series_iterator_id_t::exp_iterator_id: return std::make_unique<exp_iterator<T, K>>();
	case series_iterator_id_t::cos_iterator_id: return std::make_unique<cos_iterator<T, K>>();
	case series_iterator_id_t::sin_iterator_id: return std::make_unique<sin_iterator<T, K>>();
	case series_iterator_id_t::cosh_iterator_id: return std::make_unique<cosh_iterator<T, K>>();
	case series_iterator_id_t::sinh_iterator_id: return std::make_unique<sinh_iterator<T, K>>();
	case series_iterator_id_t::bin_iterator_id: return std::make_unique<bin_iterator<T, K>>();
	case series_iterator_id_t::four_arctan_iterator_id: return std::make_unique<four_arctan_iterator<T, K>>();
	case series_iterator_id_t::ln1mx_iterator_id: return std::make_unique<ln1mx_iterator<T, K>>();
	case series_iterator_id_t::mean_sinh_sin_iterator_id: return std::make_unique<mean_sinh_sin_iterator<T, K>>();
	case series_iterator_id_t::exp_squared_erf_iterator_id: return std::make_unique<exp_squared_erf_iterator<T, K>>();
	case series_iterator_id_t::half_asin_two_x_iterator_id: return std::make_unique<half_asin_two_x_iterator<T, K>>();
	case series_iterator_id_t::inverse_1mx_iterator_id: return std::make_unique<inverse_1mx_iterator<T, K>>();
	case series_iterator_id_t::x_1mx_squared_iterator_id: return std::make_unique<x_1mx_squared_iterator<T, K>>();
	case series_iterator_id_t::erf_iterator_id: return std::make_unique<erf_iterator<T, K>>();
	case series_iterator_id_t::m_fact_1mx_mp1_inverse_iterator_id: return std::make_unique<m_fact_1mx_mp1_inverse_iterator<T, K>>();
	case series_iterator_id_t::inverse_sqrt_1m4x_iterator_id: return std::make_unique<inverse_sqrt_1m4x_iterator<T, K>>();
	case series_iterator_id_t::one_twelfth_3x2_pi2_iterator_id: return std::make_unique<one_twelfth_3x2_pi2_iterator<T, K>>();
	case series_iterator_id_t::one_twelfth_x2_pi2_iterator_id: return std::make_unique<one_twelfth_x2_pi2_iterator<T, K>>();
	case series_iterator_id_t::ln2_iterator_id: return std::make_unique<ln2_iterator<T, K>>();
	case series_iterator_id_t::one_iterator_id: return std::make_unique<one_iterator<T, K>>();
	case series_iterator_id_t::minus_one_quarter_iterator_id: return std::make_unique<minus_one_quarter_iterator<T, K>>();
	case series_iterator_id_t::pi_3_iterator_id: return std::make_unique<pi_3_iterator<T, K>>();
	case series_iterator_id_t::pi_4_iterator_id: return std::make_unique<pi_4_iterator<T, K>>();
	case series_iterator_id_t::pi_squared_6_minus_one_iterator_id: return std::make_unique<pi_squared_6_minus_one_iterator<T, K>>();
	case series_iterator_id_t::three_minus_pi_iterator_id: return std::make_unique<three_minus_pi_iterator<T, K>>();
	case series_iterator_id_t::one_twelfth_iterator_id: return std::make_unique<one_twelfth_iterator<T, K>>();
	case series_iterator_id_t::eighth_pi_m_one_third_iterator_id: return std::make_unique<eighth_pi_m_one_third_iterator<T, K>>();
	case series_iterator_id_t::one_third_pi_squared_m_nine_iterator_id: return std::make_unique<one_third_pi_squared_m_nine_iterator<T, K>>();
	case series_iterator_id_t::four_ln2_m_3_iterator_id: return std::make_unique<four_ln2_m_3_iterator<T, K>>();
	case series_iterator_id_t::exp_m_cos_x_sinsin_x_iterator_id: return std::make_unique<exp_m_cos_x_sinsin_x_iterator<T, K>>();
	case series_iterator_id_t::pi_four_minus_ln2_halfed_iterator_id: return std::make_unique<pi_four_minus_ln2_halfed_iterator<T, K>>();
	case series_iterator_id_t::five_pi_twelve_iterator_id: return std::make_unique<five_pi_twelve_iterator<T, K>>();
	case series_iterator_id_t::x_two_iterator_id: return std::make_unique<x_two_iterator<T, K>>();
	case series_iterator_id_t::pi_six_min_half_iterator_id: return std::make_unique<pi_six_min_half_iterator<T, K>>();
	case series_iterator_id_t::x_two_throught_squares_iterator_id: return std::make_unique<x_two_throught_squares_iterator<T, K>>();
	case series_iterator_id_t::minus_one_ned_in_n_iterator_id: return std::make_unique<minus_one_ned_in_n_iterator<T, K>>();
	case series_iterator_id_t::minus_one_n_fact_n_in_n_iterator_id: return std::make_unique<minus_one_n_fact_n_in_n_iterator<T, K>>();
	case series_iterator_id_t::ln_x_plus_one_x_minus_one_halfed_iterator_id: return std::make_unique<ln_x_plus_one_x_minus_one_halfed_iterator<T, K>>();
	case series_iterator_id_t::two_arcsin_square_x_halfed_iterator_id: return std::make_unique<two_arcsin_square_x_halfed_iterator<T, K>>();
	case series_iterator_id_t::pi_squared_twelve_iterator_id: return std::make_unique<pi_squared_twelve_iterator<T, K>>();
	case series_iterator_id_t::pi_cubed_32_iterator_id: return std::make_unique<pi_cubed_32_iterator<T, K>>();
	case series_iterator_id_t::minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator_id: return std::make_unique<minus_three_plus_ln3_three_devided_two_plus_two_ln2_iterator<T, K>>();
	case series_iterator_id_t::two_ln2_iterator_id: return std::make_unique<two_ln2_iterator<T, K>>();
	case series_iterator_id_t::pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator_id: return std::make_unique<pi_x_multi_e_xpi_plus_e_minusxpi_divided_e_xpi_minus_e_minusxpi_iterator<T, K>>();
	case series_iterator_id_t::pi_minus_x_2_iterator_id: return std::make_unique<pi_minus_x_2_iterator<T, K>>();
	case series_iterator_id_t::half_multi_ln_1div2multi1minuscosx_iterator_id: return std::make_unique<half_multi_ln_1div2multi1minuscosx_iterator<T, K>>();
	case series_iterator_id_t::half_minus_sinx_multi_pi_4_iterator_id: return std::make_unique<half_minus_sinx_multi_pi_4_iterator<T, K>>();
	case series_iterator_id_t::ln_1plussqrt1plusxsquare_minus_ln_2_iterator_id: return std::make_unique<ln_1plussqrt1plusxsquare_minus_ln_2_iterator<T, K>>();
	case series_iterator_id_t::ln_cosx_iterator_id: return std::make_unique<ln_cosx_iterator<T, K>>();
	case series_iterator_id_t::ln_sinx_minus_ln_x_iterator_id: return std::make_unique<ln_sinx_minus_ln_x_iterator<T, K>>();
	case series_iterator_id_t::pi_8_cosx_square_minus_1_div_3_cosx_iterator_id: return std::make_unique<pi_8_cosx_square_minus_1_div_3_cosx_iterator<T, K>>();
	case series_iterator_id_t::sqrt_oneminussqrtoneminusx_div_x_iterator_id: return std::make_unique<sqrt_oneminussqrtoneminusx_div_x_iterator<T, K>>();
	case series_iterator_id_t::one_minus_sqrt_1minus4x_div_2x_iterator_id: return std::make_unique<one_minus_sqrt_1minus4x_div_2x_iterator<T, K>>();
	case series_iterator_id_t::arcsin_x_minus_x_iterator_id: return std::make_unique<arcsin_x_minus_x_iterator<T, K>>();
	case series_iterator_id_t::pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator_id: return std::make_unique<pi_x_minus_x_square_square_minus_three_pi_x_plus_two_pi_square_iterator<T, K>>();
	case series_iterator_id_t::abs_sin_x_minus_2_div_pi_iterator_id: return std::make_unique<abs_sin_x_minus_2_div_pi_iterator<T, K>>();
	case series_iterator_id_t::pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator_id: return std::make_unique<pi_minus_3pi_4_and_pi_minus_x_minus_3pi_4_iterator<T, K>>();
	case series_iterator_id_t::minus_3_div_4_or_x_minus_3_div_4_iterator_id: return std::make_unique<minus_3_div_4_or_x_minus_3_div_4_iterator<T, K>>();
	case series_iterator_id_t::ten_minus_x_iterator_id: return std::make_unique<ten_minus_x_iterator<T, K>>();
	case series_iterator_id_t::x_iterator_id: return std::make_unique<x_iterator<T, K>>();
	case series_iterator_id_t::minus_x_minus_pi_4_or_minus_pi_4_iterator_id: return std::make_unique<minus_x_minus_pi_4_or_minus_pi_4_iterator<T, K>>();
	case series_iterator_id_t::one_div_two_minus_x_multi_three_plus_x_iterator_id: return std::make_unique<one_div_two_minus_x_multi_three_plus_x_iterator<T, K>>();
	case series_iterator_id_t::si_x_iterator_id: return std::make_unique<si_x_iterator<T, K>>();
	case series_iterator_id_t::ci_x_iterator_id: return std::make_unique<ci_x_iterator<T, K>>();
	case series_iterator_id_t::riemann_zeta_func_iterator_id: return std::make_unique<riemann_zeta_func_iterator<T, K>>();
	case series_iterator_id_t::riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator_id: return std::make_unique<riemann_zeta_func_xmin1_div_Riemann_zeta_func_x_iterator<T, K>>();
	case series_iterator_id_t::xsquareplus3_div_xsquareplus2multix_minus_1_iterator_id: return std::make_unique<xsquareplus3_div_xsquareplus2multix_minus_1_iterator<T, K>>();
	case series_iterator_id_t::arcsin_x_iterator_id: return std::make_unique<arcsin_x_iterator<T, K>>();
	case series_iterator_id_t::arctg_x_iterator_id: return std::make_unique<arctan_x_iterator<T, K>>();
	case series_iterator_id_t::k_x_iterator_id: return std::make_unique<k_x_iterator<T, K>>();
	case series_iterator_id_t::e_x_iterator_id: return std::make_unique<e_x_iterator<T, K>>();
	case series_iterator_id_t::sqrt_1plusx_iterator_id: return std::make_unique<sqrt_1plusx_iterator<T, K>>();
	case series_iterator_id_t::lambert_W_func_iterator_id: return std::make_unique<lambert_W_func_iterator<T, K>>();
	case series_iterator_id_t::incomplete_Gamma_func_iterator_id: return std::make_unique<incomplete_Gamma_func_iterator<T, K>>();
	case series_iterator_id_t::series_with_ln_number1_iterator_id: return std::make_unique<series_with_ln_number1_iterator<T, K>>();
	case series_iterator_id_t::series_with_ln_number2_iterator_id: return std::make_unique<series_with_ln_number2_iterator<T, K>>();
	case series_iterator_id_t::pi_iterator_id: return std::make_unique<pi_iterator<T, K>>();
	case series_iterator_id_t::x_min_sqrt_x_iterator_id: return std::make_unique<x_min_sqrt_x_iterator<T, K>>();
	case series_iterator_id_t::arctg_x2_iterator_id: return std::make_unique<arctan_x2_iterator<T, K>>();
	case series_iterator_id_t::ln1px4_iterator_id: return std::make_unique<ln1px4_iterator<T, K>>();
	case series_iterator_id_t::sin_x2_iterator_id: return std::make_unique<sin_x2_iterator<T, K>>();
	case series_iterator_id_t::arctg_x3_iterator_id: return std::make_unique<arctan_x3_iterator<T, K>>();
	case series_iterator_id_t::arcsin_x2_iterator_id: return std::make_unique<arcsin_x2_iterator<T, K>>();
	case series_iterator_id_t::ln1_m_x2_iterator_id: return std::make_unique<ln1_m_x2_iterator<T, K>>();
	case series_iterator_id_t::arctanh_x_iterator_id: return std::make_unique<arctanh_x_iterator<T, K>>();
	case series_iterator_id_t::arcsinh_x_iterator_id: return std::make_unique<arcsinh_x_iterator<T, K>>();
	case series_iterator_id_t::cos_x2_iterator_id: return std::make_unique<cos_x2_iterator<T, K>>();
	case series_iterator_id_t::sinh_x2_iterator_id: return std::make_unique<sinh_x2_iterator<T, K>>();
	case series_iterator_id_t::arctanh_x2_iterator_id: return std::make_unique<arctanh_x2_iterator<T, K>>();
	case series_iterator_id_t::cos3xmin1_div_xsquare_iterator_id: return std::make_unique<cos3xmin1_div_xsquare_iterator<T, K>>();
	case series_iterator_id_t::two_degree_x_iterator_id: return std::make_unique<two_degree_x_iterator<T, K>>();
	case series_iterator_id_t::sqrt_1plusx_min_1_min_x_div_2_iterator_id: return std::make_unique<sqrt_1plusx_min_1_min_x_div_2_iterator<T, K>>();
	case series_iterator_id_t::ln13_min_ln7_div_7_iterator_id: return std::make_unique<ln13_min_ln7_div_7_iterator<T, K>>();
	case series_iterator_id_t::one_div_sqrt2_sin_xdivsqrt2_iterator_id: return std::make_unique<one_div_sqrt2_sin_xdivsqrt2_iterator<T, K>>();
	case series_iterator_id_t::ln_1plusx_div_1plusx2_iterator_id: return std::make_unique<ln_1plusx_div_1plusx2_iterator<T, K>>();
	case series_iterator_id_t::cos_sqrt_x_iterator_id: return std::make_unique<cos_sqrt_x_iterator<T, K>>();
	case series_iterator_id_t::ln_1_plus_x3_iterator_id: return std::make_unique<ln_1_plus_x3_iterator<T, K>>();
	case series_iterator_id_t::x_div_1minx_iterator_id: return std::make_unique<x_div_1minx_iterator<T, K>>();
	case series_iterator_id_t::x_div_1minx2_iterator_id: return std::make_unique<x_div_1minx2_iterator<T, K>>();

	default:
		throw std::domain_error("Series not implemented");
	}
}


 /**
 * @brief Abstract class for series
 * @authors Bolshakov M.P.
 * @tparam T The type of the elements in the series, K The type of enumerating integer
 */
template <AcceptedLike T, UnsignedIntLike K>
class series_base
{
public:

	/**
	* @brief Parameterized constructor to initialize the series with function argument
	* @authors Bolshakov M.P.
	* @param x The argument for function series
	*/
	series_base(
		std::string name = "unknown", 
		std::string condition = "unknown", 
		series_iterator_id_t iterator_id = series_iterator_id_t::null_iterator_id
	) : series_name(name), condition(condition) {
		assert(iterator_id != series_iterator_id_t::null_iterator_id);
		series_iterator = create_iterator_by_id<T,K>(iterator_id);
		this->iterator_id = iterator_id;
	}

	/**
	* @brief x getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_x() const { return series_iterator->x; }

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const T get_sum() const { return series_iterator->sum(); }

	/**
	* @brief sum getter
	* @authors Bolshakov M.P.
	*/
	[[nodiscard]] constexpr const std::string get_name() const { return series_name; }

	/**
	 * @brief 
	 * 
	 * @param vecSize 
	 * @return std::vector<T> 
	 */
	series_result<T> generate_series(
		const T& x , 
		const K vecSize, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
	);
	
	void init_iterator(
		const T& x, 
		const T& addTParameter = static_cast<T>(1),
		const K addKParameter = static_cast<K>(1)
	);

	std::pair<T, T> next(){
		if (iterator_initialized){
			std::cout << "\n" << series_iterator->n << "\n";
			const T an = series_iterator->next(); sum+=an;
			return std::pair<T,T>(an, sum);
		}
		throw std::domain_error("iterator not initialized");
	}


protected:

	/**
    * @brief Throws domain error with unified message format
    * @authors Maximov A.K.
    * @param condition Description of the divergence condition
    * @throws std::domain_error with formatted message containing series name, x value and condition
    */
	void throw_domain_error() const {

		using std::to_string;

		throw std::domain_error(series_name + " series diverges at x = " + to_string(series_iterator->x) + " (" + condition + ")");
	}

	std::unique_ptr<series_base_iterator<T,K>> series_iterator;

	/**
    * @brief Name of the series for unified error messages
    * Used to generate consistent error output format
    * @authors Maximov A.K.
    */
	std::string series_name = "series base";

	std::string condition = "no conditions";
	
	series_iterator_id_t iterator_id = series_iterator_id_t::null_iterator_id;

	bool iterator_initialized = false;

	T sum = static_cast<T>(0);
};

template <AcceptedLike T, UnsignedIntLike K>
void series_base<T, K>::init_iterator(
	const T& x, 
	const T& addTParameter,
	const K addKParameter
){
	series_iterator->reset();
	sum = static_cast<T>(0);
	if constexpr (!is_standart_types<T>::value){
            const size_t precision = utils::get_precision<T>(x);
            utils::set_precision(precision, series_iterator->x, sum);
    }
	series_iterator->x = x;
	if(series_iterator->check_validity()) throw_domain_error();
	iterator_initialized = true;
	

	switch(iterator_id){
		case series_iterator_id_t::bin_iterator_id:{
			bin_iterator<T,K>* ptr = static_cast<bin_iterator<T,K>*>(series_iterator.get());
			if constexpr (!is_standart_types<T>::value){
            	const size_t precision = utils::get_precision<T>(x);
           		utils::set_precision(precision, series_iterator->x);
    		}
			ptr->alpha = addTParameter;
		}
		case series_iterator_id_t::incomplete_Gamma_func_iterator_id:{
			incomplete_Gamma_func_iterator<T,K>* ptr = static_cast<incomplete_Gamma_func_iterator<T,K>*>(series_iterator.get());
			if constexpr (!is_standart_types<T>::value){
				const size_t precision = utils::get_precision<T>(x);
				utils::set_precision(precision, series_iterator->x);			
			}
			ptr->alpha = addTParameter;
		}
		case series_iterator_id_t::m_fact_1mx_mp1_inverse_iterator_id:{
			m_fact_1mx_mp1_inverse_iterator<T,K>* ptr = static_cast<m_fact_1mx_mp1_inverse_iterator<T,K>*>(series_iterator.get());
			ptr->m = addKParameter;
		}
	}
}

template <AcceptedLike T, UnsignedIntLike K>
series_result<T> series_base<T, K>::generate_series(
	const T& x, 
	const K vecSize, 
	const T& addTParameter,
	const K addKParameter
){

	init_iterator(x, addTParameter, addKParameter);
	std::vector<T> vecAn(vecSize, static_cast<T>(0));
	std::vector<T> vecSn(vecSize, static_cast<T>(0));

	if constexpr (is_precisable<T>::value){
		utils::set_vec_precision<T>(vecAn, utils::get_precision(series_iterator->x));
		utils::set_vec_precision<T>(vecSn, utils::get_precision(series_iterator->x));
	}

	for (K j = 0; j < vecSize; ++j){
		try{
			vecAn[j] = series_iterator->next();
		} catch (...) {
			vecAn[j] = (j == 0 ? static_cast<T>(0) : vecAn[j-1]);
		}
		sum += vecAn[j];
		vecSn[j] = vecAn[j] + (j == 0 ? static_cast<T>(0) : vecSn[j-1]);
	}

	return series_result<T>{.Sn = vecSn, .an = vecAn};

}
#endif